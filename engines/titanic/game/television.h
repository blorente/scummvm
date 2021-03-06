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

#ifndef TITANIC_TELEVISION_H
#define TITANIC_TELEVISION_H

#include "titanic/core/background.h"
#include "titanic/messages/messages.h"
#include "titanic/messages/pet_messages.h"

namespace Titanic {

class CTelevision : public CBackground {
	DECLARE_MESSAGE_MAP;
	bool LeaveViewMsg(CLeaveViewMsg *msg);
	bool ChangeSeasonMsg(CChangeSeasonMsg *msg);
	bool EnterViewMsg(CEnterViewMsg *msg);
	bool PETUpMsg(CPETUpMsg *msg);
	bool PETDownMsg(CPETDownMsg *msg);
	bool StatusChangeMsg(CStatusChangeMsg *msg);
	bool ActMsg(CActMsg *msg);
	bool PETActivateMsg(CPETActivateMsg *msg);
	bool MovieEndMsg(CMovieEndMsg *msg);
	bool ShipSettingMsg(CShipSettingMsg *msg);
	bool TurnOff(CTurnOff *msg);
	bool TurnOn(CTurnOn *msg);
	bool LightsMsg(CLightsMsg *msg);
private:
	static int _v1;
	static bool _turnOn;
	static int _v3;
	static int _v4;
	static int _v5;
	static int _v6;
private:
	int _fieldE0;
	int _fieldE4;
	bool _isOn;
	int _fieldEC;
	int _soundHandle;
public:
	CLASSDEF;
	CTelevision();
	static void init();
	static void deinit();

	/**
	 * Save the data for the class to file
	 */
	virtual void save(SimpleFile *file, int indent);

	/**
	 * Load the data for the class from file
	 */
	virtual void load(SimpleFile *file);
};

} // End of namespace Titanic

#endif /* TITANIC_TELEVISION_H */
