#include "GuiControlLabel.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Window.h"

#include "Map.h"
#include "Animation.h"

GuiControlLabel::GuiControlLabel(uint32_t id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;
}

GuiControlLabel::~GuiControlLabel()
{

}

bool GuiControlLabel::Update(float dt)
{
	//L15: DONE 4: Draw the button according the GuiControl State
	/* switch (state)
	{
	case GuiControlState::DISABLED:
		app->render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
		break;
	case GuiControlState::NORMAL:
		app->render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);
		break;
	case GuiControlState::FOCUSED:
		app->render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
		break;
	case GuiControlState::PRESSED:
		app->render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
		break;
	} */


	if(texture != nullptr)app->render->DrawTexture(texture, bounds.x,bounds.y, &section, 0.0f);
	app->render->DrawText(text.GetString(), bounds.x, bounds.y, bounds.w, bounds.h);

	return false;
}

