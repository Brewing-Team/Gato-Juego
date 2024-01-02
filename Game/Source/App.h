#ifndef __APP_H__
#define __APP_H__

#include "GuiManager.h"
#include "Module.h"
#include "List.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "EntityManager.h"

#ifdef __linux__
#include <pugixml.hpp>
#elif _MSC_VER
#include "PugiXml/src/pugixml.hpp"
#endif

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene;
class EntityManager;
class Map;
class Physics;
class GuiManager;

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	// Request a save data in an XML file 
	bool LoadRequest();

	// Request to load data from XML file 
	bool SaveRequest();

private:

	// Load config file
	bool LoadConfig();

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Reads XML file and loads the data
	bool LoadFromFile();

	// Save XML file
	bool SaveFromFile();

public:

	// Modules
	Window* win;
	Input* input;
	Render* render;
	Textures* tex;
	Audio* audio;
	Scene* scene;
	EntityManager* entityManager;
	Map* map;
	Physics* physics;
	GuiManager* guiManager;

private:

	int argc;
	char** args;
	SString gameTitle;
	SString organization;

	List<Module *> modules;

	// xml_document to store the config file and
	// xml_node(s) to read specific branches of the xml
	pugi::xml_document configFile;
	pugi::xml_node configNode;

	uint frames;
	float dt;

    // required variables are provided:
	Timer startupTime;
	PerfTimer frameTime;
	PerfTimer lastSecFrameTime;

	uint64_t frameCount = 0;
	uint32_t framesPerSecond = 0;
	uint32_t lastSecFrameCount = 0;

	float averageFps = 0.0f;
	uint32_t secondsSinceStartup = 0;

	uint32_t maxFrameDuration = 16;

	float maxFps = 30; // TODO hacer esto variable

	bool loadRequest = false;
	bool saveRequest = false;

};

extern App* app;

extern bool debug;
extern bool fpsLimiter;
extern bool freeCam;

#endif	// __APP_H__