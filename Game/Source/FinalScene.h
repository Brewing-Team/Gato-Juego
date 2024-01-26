#ifndef __FINALSCENE_H__
#define __FINALSCENE_H__

#include "Defs.h"
#include "Module.h"
#include "Physics.h"
#include "Player.h"
#include "OwlEnemy.h"
#include "DogEnemy.h"
#include "GuiControl.h"
#include "GuiControlButton.h"

struct SDL_Texture;

class FinalScene : public Module
{
public:

	FinalScene();

	FinalScene(bool startEnabled);

	// Destructor
	virtual ~FinalScene();

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
	bool winCondition = false;
	Player* player;

private:
	SDL_Texture* img;
	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;
	GuiControlButton* gcButton;

	//pugi::xml_node& config;
};

#endif // __FINALSCENE_H__