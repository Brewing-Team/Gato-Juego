#include "App.h"
#include "Input.h"
#include "RopeEntity.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
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
	
	app->render->camera.target = player;
	app->render->camera.useInterpolation = true;
	app->render->camera.lerpSpeed = 4.0f;
	app->render->camera.offset = { 0,0 };
	//app->LoadRequest(); Mario, de momento lo dejo comentado porque rompe cosas

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file
	//img = app->tex->Load("Assets/Textures/test.png");
	
	//Music is commented so that you can add your own music
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(img, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	SDL_Rect btPos = { static_cast<int>(30), static_cast<int>(30), 120,20};
	gcButton = (GuiControlButton*) app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "MyButton", btPos, this);

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

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	// OPTICK PROFILIN
	OPTICK_EVENT();

	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	return true;
}

void RenderGUI()
{
	
}
