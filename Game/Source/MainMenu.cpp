#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "MainMenu.h"

#include "Defs.h"
#include "Log.h"
#include "GuiControl.h"
#include "GuiManager.h"

#ifdef __linux__
#include "External/Optick/include/optick.h"
#elif _MSC_VER
#include "Optick/include/optick.h"
#endif

MainMenu::MainMenu() : Module()
{
	name.Create("mainmenu");
}

MainMenu::MainMenu(bool startEnabled) : Module(startEnabled)
{
	name.Create("mainmenu");
}

// Destructor
MainMenu::~MainMenu()
{}

// Called before render is available
bool MainMenu::Awake(pugi::xml_node& config)
{
	LOG("Loading MainMenu");
	bool ret = true;
	
	return ret;
}

// Called before the first frame
bool MainMenu::Start()
{
	SDL_Rect btPos = { static_cast<int>(30), static_cast<int>(30), 120,20};
	gcButton = (GuiControlButton*) app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "MyButton", btPos, this);

	return true;
}

// Called each loop iteration
bool MainMenu::PreUpdate()
{
	// OPTICK PROFILIN
	OPTICK_EVENT();

	return true;
}

// Called each loop iteration
bool MainMenu::Update(float dt)
{
	// OPTICK PROFILIN
	OPTICK_EVENT();

	return true;
}

// Called each loop iteration
bool MainMenu::PostUpdate()
{
	// OPTICK PROFILIN
	OPTICK_EVENT();

	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool MainMenu::CleanUp()
{
	LOG("Freeing mainmenu");
	app->guiManager->RemoveGuiControl(gcButton);
	return true;
}

bool MainMenu::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	app->fade->Fade(this, (Module*)app->scene, 60);

	return true;
}

void MainMenu::RenderGUI()
{
	
}
