#ifndef __GUICONTROL_H__
#define __GUICONTROL_H__

#include "Input.h"
#include "Render.h"
#include "Module.h"

#include "Point.h"
#include "SString.h"

#ifdef __linux__
#include <SDL.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#endif

enum class GuiControlType
{
	BUTTON,
	LABEL,
	TOGGLE,
	CHECKBOX,
	SLIDER,
	SLIDERBAR,
	COMBOBOX,
	DROPDOWNBOX,
	INPUTBOX,
	VALUEBOX,
	SPINNER
};

enum class GuiControlState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
	SELECTED
};

class GuiControl
{
public:

	// Constructor
	GuiControl(GuiControlType type, uint32_t id) : type(type), id(id), state(GuiControlState::NORMAL) {}

	// Constructor
	GuiControl(GuiControlType type, SDL_Rect bounds, const char* text) :
		type(type),
		state(GuiControlState::NORMAL),
		bounds(bounds),
		text(text)
	{
		color.r = 255; color.g = 255; color.b = 255;
		texture = NULL;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// 
	void SetTexture(SDL_Texture* tex)
	{
		texture = tex;
		section = { 0, 0, 0, 0 };
	}

	// 
	void SetObserver(Module* module)
	{
		observer = module;
	}

	// 
	void NotifyObserver()
	{
		observer->OnGuiMouseClickEvent(this);
	}

public:

	uint32_t id;
	GuiControlType type;
	GuiControlState state;

	SString text;           // Control text (if required)
	SDL_Rect bounds;        // Position and size
	SDL_Color color;        // Tint color

	SDL_Texture* texture = nullptr;   // Texture atlas reference
	SDL_Rect section;       // Texture atlas base section

	Module* observer;        // Observer 
};

#endif // __GUICONTROL_H__