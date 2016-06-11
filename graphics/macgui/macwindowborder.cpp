#include "macwindowborder.h"

#include "common/system.h"

#include "graphics/macgui/macwindowmanager.h"


namespace Graphics {

using namespace Graphics::MacGUIConstants;
	
MacWindowBorder::MacWindowBorder() : _activeInitialized(false), _inactiveInitialized(false) {
	_activeBorder = nullptr;
	_inactiveBorder = nullptr;	
}


MacWindowBorder::~MacWindowBorder() {
	if (_activeBorder)
		delete _activeBorder;
	if (_inactiveBorder)
		delete _inactiveBorder;
}

bool MacWindowBorder::hasBorder(bool active) { 
	return active ? _activeInitialized : _inactiveInitialized; 
}

void MacWindowBorder::addActiveBorder(TransparentSurface &source) {
	assert(!_activeBorder);
	_activeBorder = new NinePatchBitmap(&source, false);	
	_activeInitialized = true;
}

void MacWindowBorder::addInactiveBorder(TransparentSurface &source) {
	assert(!_inactiveBorder);
	_inactiveBorder = new NinePatchBitmap(&source, false);
	_inactiveInitialized = true;
}

void MacWindowBorder::blitBorderInto(ManagedSurface &destination, bool active) {
	
	TransparentSurface srf;
	NinePatchBitmap *src = active ? _activeBorder : _inactiveBorder;

	srf.create(destination.w, destination.h, destination.format);

	byte palette[kColorCount];
	g_system->getPaletteManager()->grabPalette(palette, 0, kColorCount);

	src->blit(srf, 0, 0, srf.w, srf.h, palette, kColorCount);
	destination.transBlitFrom(srf, destination.format.ARGBToColor(0, 255, 255, 255));
}

} // End of namespace Graphics
