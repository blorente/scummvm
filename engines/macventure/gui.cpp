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

#include "common/file.h"
#include "image/bmp.h"

#include "macventure/macventure.h"
#include "macventure/gui.h"

namespace MacVenture {

enum MenuAction;
struct WindowData;
enum WindowReference;

const bool kLoadStaticMenus = true;

enum {
	kMenuHighLevel = -1,
	kMenuAbout = 0,
	kMenuFile = 1,
	kMenuEdit = 2,
	kMenuSpecial = 3
};

enum {
	kCommandNum = 8
};

static const Graphics::MenuData menuSubItems[] = {
	{ kMenuHighLevel,	"File",				0, 0, false },
	{ kMenuHighLevel,	"Edit",				0, 0, false },		
	{ kMenuHighLevel,	"Special",			0, 0, false },
	{ kMenuHighLevel,	"Font",				0, 0, false },
	{ kMenuHighLevel,	"FontSize",			0, 0, false },

	//{ kMenuAbout,		"About",			kMenuActionAbout, 0, true},

	{ kMenuFile,		"New",				kMenuActionNew, 0, true },
	{ kMenuFile,		NULL,				0, 0, false },
	{ kMenuFile,		"Open...",			kMenuActionOpen, 0, true },
	{ kMenuFile,		"Save",				kMenuActionSave, 0, true },
	{ kMenuFile,		"Save as...",		kMenuActionSaveAs, 0, true },
	{ kMenuFile,		NULL,				0, 0, false },
	{ kMenuFile,		"Quit",				kMenuActionQuit, 0, true },

	{ kMenuEdit,		"Undo",				kMenuActionUndo, 'Z', true },
	{ kMenuEdit,		NULL,				0, 0, false },
	{ kMenuEdit,		"Cut",				kMenuActionCut, 'K', true },
	{ kMenuEdit,		"Copy",				kMenuActionCopy, 'C', true },
	{ kMenuEdit,		"Paste",			kMenuActionPaste, 'V', true },
	{ kMenuEdit,		"Clear",			kMenuActionClear, 'B', true },

	{ kMenuSpecial,		"Clean Up",			kMenuActionCleanUp, 0, true },
	{ kMenuSpecial,		"Mess Up",			kMenuActionMessUp, 0, true },

	{ 0,				NULL,				0, 0, false }
};

bool outConsoleWindowCallback(Graphics::WindowClick, Common::Event &event, void *gui);
bool controlsWindowCallback(Graphics::WindowClick, Common::Event &event, void *gui);
void menuCommandsCallback(int action, Common::String &text, void *data);

Gui::Gui(MacVentureEngine *engine, Common::MacResManager *resman) {
	_engine = engine;
	_resourceManager = resman;
	_windowData = nullptr;
	_controlData = nullptr;

	initGUI();
}

Gui::~Gui() {

	if (_windowData)
		delete _windowData;

	if (_controlData)
		delete _controlData;
}

void Gui::draw() {
	_wm.draw();
}

bool Gui::processEvent(Common::Event &event) {
	return _wm.processEvent(event);
}

const WindowData& Gui::getWindowData(WindowReference reference) {
	assert(_windowData);
	Common::List<WindowData>::const_iterator iter = _windowData->begin();
	while (iter->refcon != reference && iter != _windowData->end()) {
		iter++;
	}

	if (iter->refcon == reference)
		return *iter;

	error("Could not locate the desired window data");
}

void Gui::initGUI() {
	_screen.create(kScreenWidth, kScreenHeight, Graphics::PixelFormat::createFormatCLUT8());
	_wm.setScreen(&_screen);

	// Menu
	_menu = _wm.addMenu();
	if (!loadMenus())
		error("Could not load menus");
	_menu->setCommandsCallback(menuCommandsCallback, this);
	_menu->calcDimensions();

	if (!loadWindows())
		error("Could not load windows");
	
	initWindows();

	if (!loadControls())
		error("Could not load controls");
	
}

void Gui::initWindows() {
	
	// In-game Output Console
	_outConsoleWindow = _wm.addWindow(false, true, true);
	_outConsoleWindow->setDimensions(getWindowData(kOutConsoleWindow).bounds);
	_outConsoleWindow->setActive(false);
	_outConsoleWindow->setCallback(outConsoleWindowCallback, this);
	loadBorder(_outConsoleWindow, "border_inac.bmp", false);
	
	// Game Controls Window
	_controlsWindow = _wm.addWindow(false, false, false);
	_controlsWindow->setDimensions(getWindowData(kCommandsWindow).bounds);
	_controlsWindow->setActive(false);
	_controlsWindow->setCallback(controlsWindowCallback, this);
	loadBorder(_controlsWindow, "border_inac.bmp", false);
	
}

void Gui::loadBorder(Graphics::MacWindow *target, Common::String filename, bool active) {
	Common::File borderfile;

	if (!borderfile.open(filename)) {
		debug(1, "Cannot open border file");
		return;
	}

	Common::SeekableReadStream *stream = borderfile.readStream(borderfile.size());

	if (stream) {
		debug(4, "Loading %s border from %s", (active ? "active" : "inactive"), filename.c_str());
		target->loadBorder(*stream, active);

		borderfile.close();

		delete stream;
	}
}

bool Gui::loadMenus() {

	if (kLoadStaticMenus) {
		// We assume that, if there are static menus, we don't need dynamic ones	
		_menu->addStaticMenus(menuSubItems);
		return true;
	}

	Common::MacResIDArray resArray;
	Common::SeekableReadStream *res;
	Common::MacResIDArray::const_iterator iter;

	if ((resArray = _resourceManager->getResIDArray(MKTAG('M', 'E', 'N', 'U'))).size() == 0)
		return false;	

	_menu->addMenuSubItem(0, "Abb", kMenuActionAbout, 0, 'A', true);

	int i = 1;
	for (iter = resArray.begin(); iter != resArray.end(); ++iter) {
		res = _resourceManager->getResource(MKTAG('M', 'E', 'N', 'U'), *iter);
		bool enabled;
		uint16 key;
		uint16 style;
		uint8 titleLength;
		char* title;

		int menunum = -1; // High level menus have level -1
		/* Skip menuID, width, height, resourceID, placeholder */
		for (int skip = 0; skip < 5; skip++) { res->readUint16BE(); }
		enabled = res->readUint32BE();
		titleLength = res->readByte();
		title = new char[titleLength + 1];
		res->read(title, titleLength);
		title[titleLength] = '\0';

		if (titleLength > 1) {
			_menu->addMenuItem(title);

			// Read submenu items
			while ((titleLength = res->readByte())) {
				title = new char[titleLength + 1];
				res->read(title, titleLength);
				title[titleLength] = '\0';
				// Skip icon
				res->readUint16BE();
				// Read key
				key = res->readUint16BE();
				// Skip mark
				res->readUint16BE();
				// Read style
				style = res->readUint16BE();
				_menu->addMenuSubItem(i, title, 0, style, key, enabled);
			}
		}	

		i++;
	}

	return true;
}

bool Gui::loadWindows() {
	Common::MacResIDArray resArray;
	Common::SeekableReadStream *res;
	Common::MacResIDArray::const_iterator iter;

	_windowData = new Common::List<WindowData>();

	if ((resArray = _resourceManager->getResIDArray(MKTAG('W', 'I', 'N', 'D'))).size() == 0)
		return false;

	uint32 id = kCommandsWindow;
	for (iter = resArray.begin(); iter != resArray.end(); ++iter) {
		res = _resourceManager->getResource(MKTAG('W', 'I', 'N', 'D'), *iter);
		WindowData data;
		uint16 top, left, bottom, right;
		top = res->readUint16BE();
		left = res->readUint16BE();
		bottom = res->readUint16BE();
		right = res->readUint16BE();
		data.bounds = Common::Rect(left, top, right, bottom);
		data.type = res->readUint16BE();
		data.visible = res->readUint16BE();
		data.hasCloseBox = res->readUint16BE();
		data.refcon = (WindowReference)id; id++;
		res->readUint32BE(); // Skip the true id. For some reason it's reading 0
		data.titleLength = res->readByte();
		if (data.titleLength) {
			data.title = new char[data.titleLength + 1];
			res->read(data.title, data.titleLength);
			data.title[data.titleLength] = '\0';
		}

		_windowData->push_back(data);
	}

	return true;
}

bool Gui::loadControls() {
	Common::MacResIDArray resArray;
	Common::SeekableReadStream *res;
	Common::MacResIDArray::const_iterator iter;

	_controlData = new Common::List<ControlData>();

	if ((resArray = _resourceManager->getResIDArray(MKTAG('C', 'N', 'T', 'L'))).size() == 0)
		return false;

	uint32 id = kControlExitBox;
	for (iter = resArray.begin(); iter != resArray.end(); ++iter) {
		res = _resourceManager->getResource(MKTAG('C', 'N', 'T', 'L'), *iter);
		ControlData data;
		uint16 top, left, bottom, right;
		top = res->readUint16BE();
		left = res->readUint16BE();
		bottom = res->readUint16BE();
		right = res->readUint16BE();
		data.bounds = Common::Rect(left, top, right, bottom);
		data.scrollValue = res->readUint16BE();
		data.visible = res->readByte();
		res->readByte(); // Unused
		data.scrollMax = res->readUint16BE();
		data.scrollMin = res->readUint16BE();
		data.cdef = res->readUint16BE();
		data.refcon = (ControlReference)id; id++;
		res->readUint32BE();
		data.titleLength = res->readByte();
		if (data.titleLength) {
			data.title = new char[data.titleLength + 1];
			res->read(data.title, data.titleLength);
			data.title[data.titleLength] = '\0';
		}

		_controlData->push_back(data);
	}

	return true;
}

/* CALLBACKS */
bool outConsoleWindowCallback(Graphics::WindowClick, Common::Event &event, void *gui) {
	return true;
}

bool controlsWindowCallback(Graphics::WindowClick, Common::Event &event, void *gui) {
	debug("Controls window");
	return true;
}

void menuCommandsCallback(int action, Common::String &text, void *data) {
	Gui *g = (Gui *)data;

	g->handleMenuAction((MenuAction)action);
}

/* HANDLERS */
void Gui::handleMenuAction(MenuAction action) {
	switch (action)	{
	case MacVenture::kMenuActionAbout:
		debug("MacVenture Menu Action: About");
		break;
	case MacVenture::kMenuActionNew:
		debug("MacVenture Menu Action: New");
		break;
	case MacVenture::kMenuActionOpen:
		debug("MacVenture Menu Action: Open");
		break;
	case MacVenture::kMenuActionSave:
		debug("MacVenture Menu Action: Save");
		break;
	case MacVenture::kMenuActionSaveAs:
		debug("MacVenture Menu Action: Save As");
		break;
	case MacVenture::kMenuActionQuit:
		_engine->requestQuit();
		break;
	case MacVenture::kMenuActionUndo:
		debug("MacVenture Menu Action: Undo");
		break;
	case MacVenture::kMenuActionCut:
		debug("MacVenture Menu Action: Cut");
		break;
	case MacVenture::kMenuActionCopy:
		debug("MacVenture Menu Action: Copy");
		break;
	case MacVenture::kMenuActionPaste:
		debug("MacVenture Menu Action: Paste");
		break;
	case MacVenture::kMenuActionClear:
		debug("MacVenture Menu Action: Clear");
		break;
	case MacVenture::kMenuActionCleanUp:
		debug("MacVenture Menu Action: Clean Up");
		break;
	case MacVenture::kMenuActionMessUp:
		debug("MacVenture Menu Action: Mess Up");
		break;
	case MacVenture::kMenuActionCommand:
		debug("MacVenture Menu Action: GENERIC");
		break;
	default:
		break;
	}
}

} // End of namespace MacVenture
