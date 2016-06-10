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

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#ifndef MACVENTURE_GUI_H
#define MACVENTURE_GUI_H

#include "graphics/macgui/macwindowmanager.h"
#include "graphics/macgui/macwindow.h"
#include "graphics/macgui/macmenu.h"

namespace MacVenture {

using namespace Graphics::MacGUIConstants;
using namespace Graphics::MacWindowConstants;
class MacVentureEngine;

enum MenuAction {
	kMenuActionAbout,
	kMenuActionNew,
	kMenuActionOpen,
	kMenuActionSave,
	kMenuActionSaveAs,
	kMenuActionQuit,
	kMenuActionUndo,
	kMenuActionCut,
	kMenuActionCopy,
	kMenuActionPaste,
	kMenuActionClear,
	kMenuActionCleanUp,
	kMenuActionMessUp,

	kMenuActionCommand
};

enum WindowReference {
	kCommandsWindow = 0x80,
	kMainGameWindow = 0x81,
	kOutConsoleWindow = 0x82,
	kSelfWindow = 0x83,
	kExitsWindow = 0x84,
	kDiplomaWindow = 0x85
};

struct WindowData {
	Common::Rect bounds;
	uint16 type;
	uint16 visible;
	uint16 hasCloseBox;
	WindowReference refcon;
	uint8 titleLength;
	char* title;
};

enum ControlReference {
	kControlExitBox = 0,
	kControlExamine = 1,
	kControlOpen = 2,
	kControlClose = 3,
	kControlSpeak = 4,
	kControlOperate = 5,
	kControlGo = 6,
	kControlHit = 7,
	kControlConsume = 8
};

struct ControlData {
	Common::Rect bounds;
	uint16 scrollValue;
	uint8 visible;
	uint16 scrollMax;
	uint16 scrollMin;
	uint16 cdef;
	uint32 refcon;
	uint8 titleLength;
	char* title;
};


class Gui {	

public:
	Gui(MacVentureEngine *engine, Common::MacResManager *resman);
	~Gui();

	void draw();
	bool processEvent(Common::Event &event);
	void handleMenuAction(MenuAction action);

	const WindowData& getWindowData(WindowReference reference);

private: // Attributes

	MacVentureEngine *_engine;
	Common::MacResManager *_resourceManager;

	Graphics::ManagedSurface _screen;
	Graphics::MacWindowManager _wm;

	Common::List<WindowData> *_windowData;
	Common::List<ControlData> *_controlData;

	Graphics::MacWindow *_outConsoleWindow;
	Graphics::MacWindow *_controlsWindow;
	Graphics::Menu *_menu;

private: // Methods

	void initGUI();
	void initWindows();
	bool loadMenus();
	bool loadWindows();
	bool loadControls();
	void loadBorder(Graphics::MacWindow * target, Common::String filename, bool active);	

};

} // End of namespace MacVenture

#endif
