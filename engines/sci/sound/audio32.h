/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef SCI_AUDIO32_H
#define SCI_AUDIO32_H
#include "audio/audiostream.h"     // for AudioStream, SeekableAudioStream (...
#include "audio/mixer.h"           // for Mixer, SoundHandle
#include "audio/rate.h"            // for Audio::st_volume_t, RateConverter
#include "common/array.h"          // for Array
#include "common/mutex.h"          // for StackLock, Mutex
#include "common/scummsys.h"       // for int16, uint8, uint32, uint16
#include "engines/sci/resource.h"  // for ResourceId
#include "sci/engine/vm_types.h"   // for reg_t, NULL_REG

namespace Sci {

/**
 * An audio channel used by the software SCI mixer.
 */
struct AudioChannel {
	/**
	 * The ID of the resource loaded into this channel.
	 */
	ResourceId id;

	/**
	 * The resource loaded into this channel.
	 */
	Resource *resource;

	/**
	 * Data stream containing the raw audio for the channel.
	 */
	Common::SeekableReadStream *resourceStream;

	/**
	 * The audio stream loaded into this channel.
	 * `SeekableAudioStream` is used here instead of
	 * `RewindableAudioStream` because
	 * `RewindableAudioStream` does not include the
	 * `getLength` function, which is needed to tell the
	 * game engine the duration of audio streams.
	 */
	Audio::SeekableAudioStream *stream;

	/**
	 * The converter used to transform and merge the input
	 * stream into the mixer's output buffer.
	 */
	Audio::RateConverter *converter;

	/**
	 * Duration of the channel, in ticks.
	 */
	uint32 duration;

	/**
	 * The tick when the channel was started.
	 */
	uint32 startedAtTick;

	/**
	 * The tick when the channel was paused.
	 */
	uint32 pausedAtTick;

	/**
	 * Whether or not the audio in this channel should loop
	 * infinitely.
	 */
	bool loop;

	/**
	 * The time, in ticks, that the channel fade began.
	 * If 0, the channel is not being faded.
	 */
	uint32 fadeStartTick;

	/**
	 * The start volume of a fade.
	 */
	int fadeStartVolume;

	/**
	 * The total length of the fade, in ticks.
	 */
	uint32 fadeDuration;

	/**
	 * The end volume of a fade.
	 */
	int fadeTargetVolume;

	/**
	 * Whether or not the channel should be stopped and
	 * freed when the fade is complete.
	 */
	bool stopChannelOnFade;

	/**
	 * Whether or not this channel contains a Robot
	 * audio block.
	 */
	bool robot;

	/**
	 * For digital sound effects, the related VM
	 * Sound::nodePtr object for the sound.
	 */
	reg_t soundNode;

	/**
	 * The playback volume, from 1 to 127 inclusive.
	 */
	int volume;

	/**
	 * The amount to pan to the right, from 0 to 100.
	 * 50 is centered, -1 is not panned.
	 */
	int pan;
};

/**
 * Special audio channel indexes used to select a channel
 * for digital audio playback.
 */
enum AudioChannelIndex {
	kRobotChannel = -3,
	kNoExistingChannel = -2,
	kAllChannels = -1
};

/**
 * Audio32 acts as a permanent audio stream into the system
 * mixer and provides digital audio services for the SCI32
 * engine, since the system mixer does not support all the
 * features of SCI.
 */
class Audio32 : public Audio::AudioStream {
public:
	Audio32(ResourceManager *resMan);
	~Audio32();

private:
	ResourceManager *_resMan;
	Audio::Mixer *_mixer;
	Audio::SoundHandle _handle;
	Common::Mutex _mutex;

	enum {
		/**
		 * The maximum channel volume.
		 */
		kMaxVolume = 127
	};

#pragma mark -
#pragma mark AudioStream implementation
public:
	int readBuffer(Audio::st_sample_t *buffer, const int numSamples);
	bool isStereo() const { return true; }
	int getRate() const { return _mixer->getOutputRate(); }
	bool endOfData() const { return _numActiveChannels == 0; }
	bool endOfStream() const { return false; }

private:
	/**
	 * Mixes audio from the given source stream into the
	 * target buffer using the given rate converter.
	 */
	int writeAudioInternal(Audio::RewindableAudioStream *const sourceStream, Audio::RateConverter *const converter, Audio::st_sample_t *targetBuffer, const int numSamples, const Audio::st_volume_t leftVolume, const Audio::st_volume_t rightVolume, const bool loop);

#pragma mark -
#pragma mark Channel management
public:
	/**
	 * Gets the number of currently active channels.
	 */
	inline uint8 getNumActiveChannels() const {
		Common::StackLock lock(_mutex);
		return _numActiveChannels;
	}

	/**
	 * Finds a channel that is already configured for the
	 * given audio sample.
	 *
	 * @param startIndex The location of the audio resource
	 * information in the arguments list.
	 */
	int16 findChannelByArgs(int argc, const reg_t *argv, const int startIndex, const reg_t soundNode) const;

	/**
	 * Finds a channel that is already configured for the
	 * given audio sample.
	 */
	int16 findChannelById(const ResourceId resourceId, const reg_t soundNode = NULL_REG) const;

private:
	typedef Common::Array<Resource *> UnlockList;

	/**
	 * The audio channels.
	 */
	Common::Array<AudioChannel> _channels;

	/**
	 * The number of active audio channels in the mixer.
	 * Being active is not the same as playing; active
	 * channels may be paused.
	 */
	uint8 _numActiveChannels;

	/**
	 * Whether or not we are in the audio thread.
	 *
	 * This flag is used instead of passing a parameter to
	 * `freeUnusedChannels` because a parameter would
	 * require forwarding through the public method `stop`,
	 * and there is not currently any reason for this
	 * implementation detail to be exposed.
	 */
	bool _inAudioThread;

	/**
	 * The list of resources from freed channels that need
	 * to be unlocked from the main thread.
	 */
	UnlockList _resourcesToUnlock;

	/**
	 * Gets the audio channel at the given index.
	 */
	inline AudioChannel &getChannel(const int16 channelIndex) {
		Common::StackLock lock(_mutex);
		assert(channelIndex >= 0 && channelIndex < _numActiveChannels);
		return _channels[channelIndex];
	}

	/**
	 * Gets the audio channel at the given index.
	 */
	inline const AudioChannel &getChannel(const int16 channelIndex) const {
		Common::StackLock lock(_mutex);
		assert(channelIndex >= 0 && channelIndex < _numActiveChannels);
		return _channels[channelIndex];
	}

	/**
	 * Frees all non-looping channels that have reached the
	 * end of their stream.
	 */
	void freeUnusedChannels();

	/**
	 * Frees resources allocated to the given channel.
	 */
	void freeChannel(const int16 channelIndex);

	/**
	 * Unlocks all resources that were freed by the audio
	 * thread.
	 */
	void unlockResources();

#pragma mark -
#pragma mark Script compatibility
public:
	/**
	 * Gets the (fake) sample rate of the hardware DAC.
	 * For script compatibility only.
	 */
	inline uint16 getSampleRate() const {
		return _globalSampleRate;
	}

	/**
	 * Sets the (fake) sample rate of the hardware DAC.
	 * For script compatibility only.
	 */
	void setSampleRate(uint16 rate);

	/**
	 * Gets the (fake) bit depth of the hardware DAC.
	 * For script compatibility only.
	 */
	inline uint8 getBitDepth() const {
		return _globalBitDepth;
	}

	/**
	 * Sets the (fake) sample rate of the hardware DAC.
	 * For script compatibility only.
	 */
	void setBitDepth(uint8 depth);

	/**
	 * Gets the (fake) number of output (speaker) channels
	 * of the hardware DAC. For script compatibility only.
	 */
	inline uint8 getNumOutputChannels() const {
		return _globalNumOutputChannels;
	}

	/**
	 * Sets the (fake) number of output (speaker) channels
	 * of the hardware DAC. For script compatibility only.
	 */
	void setNumOutputChannels(int16 numChannels);

	/**
	 * Gets the (fake) number of preloaded channels.
	 * For script compatibility only.
	 */
	inline uint8 getPreload() const {
		return _preload;
	}

	/**
	 * Sets the (fake) number of preloaded channels.
	 * For script compatibility only.
	 */
	inline void setPreload(uint8 preload) {
		_preload = preload;
	}

private:
	/**
	 * The hardware DAC sample rate. Stored only for script
	 * compatibility.
	 */
	uint16 _globalSampleRate;

	/**
	 * The maximum allowed sample rate of the system mixer.
	 * Stored only for script compatibility.
	 */
	uint16 _maxAllowedSampleRate;

	/**
	 * The hardware DAC bit depth. Stored only for script
	 * compatibility.
	 */
	uint8 _globalBitDepth;

	/**
	 * The maximum allowed bit depth of the system mixer.
	 * Stored only for script compatibility.
	 */
	uint8 _maxAllowedBitDepth;

	/**
	 * The hardware DAC output (speaker) channel
	 * configuration. Stored only for script compatibility.
	 */
	uint8 _globalNumOutputChannels;

	/**
	 * The maximum allowed number of output (speaker)
	 * channels of the system mixer. Stored only for script
	 * compatibility.
	 */
	uint8 _maxAllowedOutputChannels;

	/**
	 * The number of audio channels that should have their
	 * data preloaded into memory instead of streaming from
	 * disk.
	 * 1 = all channels, 2 = 2nd active channel and above,
	 * etc.
	 * Stored only for script compatibility.
	 */
	uint8 _preload;

#pragma mark -
#pragma mark Robot
public:

private:
	/**
	 * When true, channels marked as robot audio will not be
	 * played.
	 */
	bool _robotAudioPaused;

#pragma mark -
#pragma mark Playback
public:
	/**
	 * Starts or resumes playback of an audio channel.
	 */
	uint16 play(int16 channelIndex, const ResourceId resourceId, const bool autoPlay, const bool loop, const int16 volume, const reg_t soundNode, const bool monitor);

	/**
	 * Resumes playback of a paused audio channel, or of
	 * the entire audio player.
	 */
	bool resume(const int16 channelIndex);
	bool resume(const ResourceId resourceId, const reg_t soundNode = NULL_REG) {
		Common::StackLock lock(_mutex);
		return resume(findChannelById(resourceId, soundNode));
	}

	/**
	 * Pauses an audio channel, or the entire audio player.
	 */
	bool pause(const int16 channelIndex);
	bool pause(const ResourceId resourceId, const reg_t soundNode = NULL_REG) {
		Common::StackLock lock(_mutex);
		return pause(findChannelById(resourceId, soundNode));
	}

	/**
	 * Stops and unloads an audio channel, or the entire
	 * audio player.
	 */
	int16 stop(const int16 channelIndex);
	int16 stop(const ResourceId resourceId, const reg_t soundNode = NULL_REG) {
		Common::StackLock lock(_mutex);
		return stop(findChannelById(resourceId, soundNode));
	}

	/**
	 * Returns the playback position for the given channel
	 * number, in ticks.
	 */
	int16 getPosition(const int16 channelIndex) const;
	int16 getPosition(const ResourceId resourceId, const reg_t soundNode = NULL_REG) {
		Common::StackLock lock(_mutex);
		return getPosition(findChannelById(resourceId, soundNode));
	}

	/**
	 * Sets whether or not the given channel should loop.
	 */
	void setLoop(const int16 channelIndex, const bool loop);
	void setLoop(const ResourceId resourceId, const reg_t soundNode, const bool loop) {
		Common::StackLock lock(_mutex);
		setLoop(findChannelById(resourceId, soundNode), loop);
	}

	reg_t kernelPlay(const bool autoPlay, const int argc, const reg_t *const argv);

private:
	/**
	 * The tick when audio was globally paused.
	 */
	uint32 _pausedAtTick;

	/**
	 * The tick when audio was globally started.
	 */
	uint32 _startedAtTick;

#pragma mark -
#pragma mark Effects
public:
	/**
	 * Gets the volume for a given channel. Passing
	 * `kAllChannels` will get the global volume.
	 */
	int16 getVolume(const int16 channelIndex) const;
	int16 getVolume(const ResourceId resourceId, const reg_t soundNode) const {
		Common::StackLock lock(_mutex);
		return getVolume(findChannelById(resourceId, soundNode));
	}

	/**
	 * Sets the volume of an audio channel. Passing
	 * `kAllChannels` will set the global volume.
	 */
	void setVolume(const int16 channelIndex, int16 volume);
	void setVolume(const ResourceId resourceId, const reg_t soundNode, const int16 volume) {
		Common::StackLock lock(_mutex);
		setVolume(findChannelById(resourceId, soundNode), volume);
	}

	/**
	 * Initiate an immediate fade of the given channel.
	 */
	bool fadeChannel(const int16 channelIndex, const int16 targetVolume, const int16 speed, const int16 steps, const bool stopAfterFade);
	bool fadeChannel(const ResourceId resourceId, const reg_t soundNode, const int16 targetVolume, const int16 speed, const int16 steps, const bool stopAfterFade) {
		Common::StackLock lock(_mutex);
		return fadeChannel(findChannelById(resourceId, soundNode), targetVolume, speed, steps, stopAfterFade);
	}

	/**
	 * Gets whether attenuated mixing mode is active.
	 */
	inline bool getAttenuatedMixing() const {
		return _attenuatedMixing;
	}

	/**
	 * Sets the attenuated mixing mode.
	 */
	void setAttenuatedMixing(bool attenuated) {
		Common::StackLock lock(_mutex);
		_attenuatedMixing = attenuated;
	}

private:
	/**
	 * If true, audio will be mixed by reducing the target
	 * buffer by half every time a new channel is mixed in.
	 * The final channel is not attenuated.
	 */
	bool _attenuatedMixing;

	/**
	 * When true, a modified attenuation algorithm is used
	 * (`A/4 + B`) instead of standard linear attenuation
	 * (`A/2 + B/2`).
	 */
	bool _useModifiedAttenuation;

	/**
	 * Processes an audio fade for the given channel.
	 *
	 * @returns true if the fade was completed and the
	 * channel was stopped.
	 */
	bool processFade(const int16 channelIndex);

#pragma mark -
#pragma mark Signal monitoring
public:
	/**
	 * Returns whether the currently monitored audio channel
	 * contains any signal within the next audio frame.
	 */
	bool hasSignal() const;

private:
	/**
	 * The index of the channel being monitored for signal,
	 * or -1 if no channel is monitored. When a channel is
	 * monitored, it also causes the engine to play only the
	 * monitored channel.
	 */
	int16 _monitoredChannelIndex;

	/**
	 * The data buffer holding decompressed audio data for
	 * the channel that will be monitored for an audio
	 * signal.
	 */
	Audio::st_sample_t *_monitoredBuffer;

	/**
	 * The size of the buffer, in bytes.
	 */
	size_t _monitoredBufferSize;

	/**
	 * The number of valid audio samples in the signal
	 * monitoring buffer.
	 */
	int _numMonitoredSamples;
};

} // End of namespace Sci
#endif
