#ifndef __GUICONTROLLABEL_H__
#define __GUICONTROLLABEL_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

class GuiControlLabel : public GuiControl
{

public:

	GuiControlLabel(uint32_t id, SDL_Rect bounds, const char* text);
	virtual ~GuiControlLabel();

	// Called each loop iteration
	bool Update(float dt);

private:

	bool drawBasic = false;
};

#endif // __GUICONTROLLABEL_H__