#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include "Module.h"
#include "GuiControl.h"
#include "GuiControlButton.h"

struct SDL_Texture;

class MainMenu : public Module
{
public:

	MainMenu();

	MainMenu(bool startEnabled);

	// Destructor
	virtual ~MainMenu();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Handles multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

	void RenderGUI();

public:

private:
	SDL_Texture* img;
	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;
	GuiControlButton* playButton;
	GuiControlButton* optionsButton;
	GuiControlButton* exitButton;
};

#endif // __MAINMENU_H__