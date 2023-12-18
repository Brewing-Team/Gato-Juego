#ifndef __MAP_H__
#define __MAP_H__

#include "Module.h"
#include "Pathfinding.h"
#include "List.h"
#include "Point.h"

#ifdef __linux__
#include <pugixml.hpp>
#include <SDL.h>
#elif _MSC_VER
#include "PugiXml\src\pugixml.hpp"
#endif

#include "Defs.h"
#include "Log.h"

enum MapOrientation
{
	ORTOGRAPHIC = 0,
	ISOMETRIC
};

class Animation;

// Ignore Terrain Types and Tile Types for now, but we want the image!
struct TileSet
{
	SString	name;
	int	firstgid;
	int margin;
	int	spacing;
	int	tileWidth;
	int	tileHeight;
	int columns;
	int tilecount;

	SDL_Texture* texture;
	SDL_Rect GetTileRect(int gid) const;
};

struct Colliders
{
	int x;
	int y;
	int width;
	int height;
};

//  We create an enum for map type, just for convenience,
// NOTE: Platformer game will be of type ORTHOGONAL
enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

struct Properties
{
	struct Property
	{
		SString name;
		bool value;
	};

	~Properties()
	{
		//...
		ListItem<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.Clear();
	}

	Property* GetProperty(const char* name);

	List<Property*> list;
};

struct MapLayer
{
	SString	name;
	int id;
	int width;
	int height;
	float parallaxFactor;
	uint* data;

	Properties properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y * width) + x];
	}
};

struct MapData
{
	int width;
	int	height;
	int	tileWidth;
	int	tileHeight;
	List<TileSet*> tilesets;
	MapTypes type;

	List<MapLayer*> maplayers;
	List<Animation*> animations;
};

class Map : public Module {

public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(SString mapFileName);

	fPoint MapToWorld(int x, int y) const;
	fPoint WorldToMap(int x, int y);

	Animation* GetAnimByName(SString name);

	// L13: Create navigation map for pathfinding
    void CreateNavigationMap(int& width, int& height, uchar** buffer) const;

private:

	// clipping margin to prevent seeing the tiles generating in the border of the screen
	const int clippingMargin = 1;

	bool LoadMap(pugi::xml_node mapFile);
	bool LoadTileSet(pugi::xml_node mapFile);
	bool LoadAnimation(pugi::xml_node node, TileSet* tileset);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadAllLayers(pugi::xml_node mapNode);
	bool LoadColliders(pugi::xml_node mapFile);
	TileSet* GetTilesetFromTileId(int gid) const;
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

public:

	MapData mapData;
	SString name;
	SString path;
	PathFinding* pathfinding;

private:
	bool mapLoaded;
	MapLayer* navigationLayer;
	int blockedGid = 287; //!!!! make sure that you assign blockedGid according to your map
};

#endif // __MAP_H__