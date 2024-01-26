#include "App.h"
#include "FadeToBlack.h"
#include "Input.h"
#include "RopeEntity.h"
#include "SString.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Timer.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"

#include "Defs.h"
#include "Log.h"
#include "GuiControl.h"
#include "GuiManager.h"

#ifdef __linux__
#include "External/Optick/include/optick.h"
#include <Box2D/Dynamics/b2Body.h>

#elif _MSC_VER
#include "Optick/include/optick.h"
#endif

Scene::Scene() : Module()
{
	name.Create("scene");
}

Scene::Scene(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access

	for (pugi::xml_node ropeNode = config.child("rope"); ropeNode; ropeNode = ropeNode.next_sibling("rope"))
	{
		RopeEntity* rope = new RopeEntity();
		app->entityManager->AddEntity(rope);
		rope->parameters = ropeNode;
	}

	if (config.child("player")) {
		player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
		player->parameters = config.child("player");
	}
	
	if (config.child("enemies"))
	{
		pugi::xml_node enemies = config.child("enemies");
		for (pugi::xml_node enemyNode = enemies.child("owl_enemy"); enemyNode; enemyNode = enemyNode.next_sibling("owl_enemy"))
		{
			OwlEnemy* owlEnemy = (OwlEnemy*)app->entityManager->CreateEntity(EntityType::OWLENEMY);
			owlEnemy->parameters = enemyNode;
		}

		for (pugi::xml_node enemyNode = enemies.child("dog_enemy"); enemyNode; enemyNode = enemyNode.next_sibling("dog_enemy"))
		{
			DogEnemy* dogEnemy = (DogEnemy*)app->entityManager->CreateEntity(EntityType::DOGENEMY);
			dogEnemy->parameters = enemyNode;
		}
	}
	
	if (config.child("map")) {
		//Get the map name from the config file and assigns the value in the module
		app->map->name = config.child("map").attribute("name").as_string();
		app->map->path = config.child("map").attribute("path").as_string();
	}

	if (config.child("camera")) {
		//Get the map name from the config file and assigns the value in the module
		app->render->camera.x = config.child("camera").attribute("x").as_int();
		app->render->camera.y = config.child("camera").attribute("y").as_int();
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	app->physics->Enable();
	app->map->Enable();
	app->entityManager->Enable();
	app->guiManager->Enable();

	app->render->camera.target = player;
	app->render->camera.useInterpolation = true;
	app->render->camera.lerpSpeed = 4.0f;
	app->render->camera.offset = { 0,0 };

	playingTime = new Timer();

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(img, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	//Create a Label
	std::string scoreString = std::to_string(player->score);
	gcScore = (GuiControlLabel*)app->guiManager->CreateGuiControl(GuiControlType::LABEL, 4, scoreString.c_str(), { (int)windowW - 200,20,150,25 }, this);

	gcLives = (GuiControlLabel*)app->guiManager->CreateGuiControl(GuiControlType::LABEL, 5, "", { 5,5,50,25 }, this);
	gcLives->SetTexture(app->map->GetAnimByName("livesAnimation")->texture);
	gcLives->section = app->map->GetAnimByName("livesAnimation")->GetCurrentFrame();

	std::string timeString = std::to_string(playingTime->ReadSec());
	gcTime = (GuiControlLabel*)app->guiManager->CreateGuiControl(GuiControlType::LABEL, 4, scoreString.c_str(), { (int)windowW / 2 + 100,20,50,25 }, this);

	//Pause Menu UI
	gcResume = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "Resume", { (int)windowW / 2 - 50, (int)windowH / 2 - 100, 150, 50 }, this);
	gcResume->SetObserver(this);
	gcResume->state = GuiControlState::DISABLED;

	gcSettings = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "Settings", { (int)windowW / 2 - 50, (int)windowH / 2 - 50, 150, 50 }, this);
	gcSettings->SetObserver(this);
	gcSettings->state = GuiControlState::DISABLED;

	gcBackToTitle = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "Back to Title", { (int)windowW / 2 - 50, (int)windowH / 2, 150, 50 }, this);
	gcBackToTitle->SetObserver(this);
	gcBackToTitle->state = GuiControlState::DISABLED;

	gcExit = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "Exit", { (int)windowW / 2 - 50, (int)windowH / 2 + 50, 150, 50 }, this);
	gcExit->SetObserver(this);
	gcExit->state = GuiControlState::DISABLED;

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	// OPTICK PROFILIN
	OPTICK_EVENT();

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	// OPTICK PROFILIN
	OPTICK_EVENT();

	if(freeCam)
	{
		float camSpeed = 1;

		if(app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			app->render->camera.y -= (int)ceil(camSpeed * dt);

		if(app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			app->render->camera.y += (int)ceil(camSpeed * dt);

		if(app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			app->render->camera.x -= (int)ceil(camSpeed * dt);

		if(app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			app->render->camera.x += (int)ceil(camSpeed * dt);
	}

	// Renders the image in the center of the screen 
	//app->render->DrawTexture(img, (int)textPosX, (int)textPosY);

	/* for (int i = 0; rope1[i + 1] != NULL; i++)
	{
		app->render->DrawLine(rope1[i].body->GetPosition().x * 100, rope1[i].body->GetPosition().y * 100, rope1[i + 1].body->GetPosition().x * 100, rope1[i + 1].body->GetPosition().y * 100, 255, 255, 255);
	}  */

	RenderGUI();

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	// OPTICK PROFILIN
	OPTICK_EVENT();

	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		if(paused){
			paused = false;
			gcResume->state = GuiControlState::NORMAL;
			gcSettings->state = GuiControlState::NORMAL;
			gcBackToTitle->state = GuiControlState::NORMAL;
			gcExit->state = GuiControlState::NORMAL;
			}
		else{
			paused = true;
			gcResume->state = GuiControlState::DISABLED;
			gcSettings->state = GuiControlState::DISABLED;
			gcBackToTitle->state = GuiControlState::DISABLED;
			gcExit->state = GuiControlState::DISABLED;
		}

	if(exitPressed)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	app->guiManager->RemoveGuiControl(gcScore);
	app->guiManager->RemoveGuiControl(gcLives);
	app->guiManager->RemoveGuiControl(gcResume);
	app->guiManager->RemoveGuiControl(gcSettings);
	app->guiManager->RemoveGuiControl(gcBackToTitle);
	app->guiManager->RemoveGuiControl(gcExit);

	return true;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	switch (control->id)
	{
	case 6:
		paused = true;
		gcResume->state = GuiControlState::DISABLED;
		gcSettings->state = GuiControlState::DISABLED;
		gcBackToTitle->state = GuiControlState::DISABLED;
		gcExit->state = GuiControlState::DISABLED;
	break;
	case 7:
		break;
	case 8:
		app->fade->Fade(this, (Module*)app->mainMenu, 60);
		app->physics->Disable();
		app->map->Disable();
		app->entityManager->Disable();
	break;
	case 9:
		exitPressed = true;
	break;
	}

	return true;
}

void Scene::RenderGUI()
{
	std::string scoreString = "Points:" + std::to_string(player->score);
	gcScore->text = scoreString.c_str();

	gcLives->section = app->map->GetAnimByName("livesAnimation")->GetCurrentFrame();
	app->map->GetAnimByName("livesAnimation")->loop = true;

	gcTime->text = std::to_string(playingTime->ReadSec()).c_str();
}
