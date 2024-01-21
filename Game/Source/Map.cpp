
#include "Animation.h"
#include "App.h"
#include "Render.h"
#include "SString.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"
#include "Window.h"
#include "Scene.h"
#include "ScoreItem.h"
#include "FoodItem.h"
#include "Checkpoint.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>

#ifdef __linux__
#include <SDL_image.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include "External/Optick/include/optick.h"
#elif _MSC_VER
#include "SDL_image/include/SDL_image.h"
#include "Optick/include/optick.h"
#endif

Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    return ret;
}

bool Map::Start() {

    //Calls the functon to load the map, make sure that the filename is assigned
    SString mapPath = path;
    mapPath += name;
    bool ret = Load(mapPath);

    //Initialize pathfinding 
    pathfinding = new PathFinding();

    //Initialize the navigation map
    uchar* navigationMap = NULL;
    CreateNavigationMap(mapData.width, mapData.height, &navigationMap);
    pathfinding->SetNavigationMap((uint)mapData.width, (uint)mapData.height, navigationMap);
    RELEASE_ARRAY(navigationMap);

    return ret;
}

bool Map::Update(float dt)
{
    // OPTICK PROFILIN
    OPTICK_EVENT();

    if(mapLoaded == false)
        return false;

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) {

        if (mapLayerItem->data->properties.GetProperty("Draw") != NULL && mapLayerItem->data->properties.GetProperty("Draw")->value) {

            iPoint cameraPosMap = WorldToMap((int)(-app->render->camera.x), (int)(-app->render->camera.y));
            //cameraPosMap = { 20,40 };

            int leftClipping = MAX(cameraPosMap.x, 0);
            int rightClipping = MIN(((cameraPosMap.x + (int)(app->render->camera.w / mapData.tileWidth) / app->win->GetScale()) + clippingMargin), mapLayerItem->data->width);
            
            int topClipping = MAX(cameraPosMap.y, 0);
            int bottomClipping = MIN(((cameraPosMap.y + (int)(app->render->camera.h / mapData.tileHeight) / app->win->GetScale()) + clippingMargin), mapLayerItem->data->height);


            // NO CLIPPING (DEBUG PURPOUSE)

            /*
            leftClipping = 0;
            rightClipping = mapLayerItem->data->width;
            topClipping = 0;
            bottomClipping = mapLayerItem->data->height;
            */
            


            for (int x = leftClipping; x < rightClipping; x++)
            {
                for (int y = topClipping; y < bottomClipping; y++)
                {
                    int gid = mapLayerItem->data->Get(x, y);
                    TileSet* tileset = GetTilesetFromTileId(gid);

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToWorld(x, y);


                    // TODO 
                    // HACER QUE EL PARALAX NO TENGA DESFASE DE COORDENADAS POR LA ESCALA
                    /*
                    
                    if (mapLayerItem->data->parallaxFactor == 1.0f)
                    {
                        app->render->DrawTexture(tileset->texture,
                            pos.x,
                            pos.y,
                            &r,
                            mapLayerItem->data->parallaxFactor);
                    }
                    else
                    {
                        app->render->DrawTexture(tileset->texture,
                            pos.x * mapLayerItem->data->parallaxFactor,
                            pos.y * mapLayerItem->data->parallaxFactor,
                            &r,
                            mapLayerItem->data->parallaxFactor);
                    }
                    
                    */

                    app->render->DrawTexture(tileset->texture,
                        pos.x,
                        pos.y,
                        &r,
                        1.0f);
                      
                        
                }
            }
        }
        mapLayerItem = mapLayerItem->next;

    }

    return true;
}

iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tileWidth;
    ret.y = y * mapData.tileHeight;

    return ret;
}

iPoint Map::WorldToMap(int x, int y) 
{
    iPoint ret;

    ret.x = x / mapData.tileWidth;
    ret.y = y / mapData.tileHeight;

    return ret;
}

Animation* Map::GetAnimByName(SString name)
{
    ListItem<Animation*>* item = mapData.animations.start;
    Animation* set = NULL;

    while (item)
    {
        set = item->data;
        if (item->data->name == name)
        {
            return set;
        }
        item = item->next;
    }

    return set;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int gid) const
{
    SDL_Rect rect = { 0 };
    int relativeIndex = gid - firstgid;

    rect.w = tileWidth;
    rect.h = tileHeight;
    rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
    rect.y = margin + (tileWidth + spacing) * (relativeIndex / columns);

    return rect;
}

TileSet* Map::GetTilesetFromTileId(int gid) const
{
    ListItem<TileSet*>* item = mapData.tilesets.start;
    TileSet* set = NULL;

    while (item)
    {
        set = item->data;
        if (gid < (item->data->firstgid + item->data->tilecount))
        {
            break;
        }
        item = item->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    //Clean up pathfing class 
    pathfinding->CleanUp();

    // L05: DONE 2: Make sure you clean up any memory allocated from tilesets/map
    ListItem<TileSet*>* tileset;
    tileset = mapData.tilesets.start;

    while (tileset != NULL) {
        RELEASE(tileset->data);
        tileset = tileset->next;
    }

    mapData.tilesets.Clear();

    // L06: DONE 2: clean up all layer data
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.maplayers.start;

    while (layerItem != NULL) {
        RELEASE(layerItem->data->data);
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }

    return true;
}

// Load new map
bool Map::Load(SString mapFileName)
{
    bool ret = true;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if(result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName.GetString(), result.description());
        ret = false;
    }

    if(ret == true)
    {
        ret = LoadMap(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadTileSet(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadAllLayers(mapFileXML.child("map"));
    }
    
    // NOTE: Later you have to create a function here to load and create the colliders from the map

    if (ret == true)
    {
        ret = LoadColliders(mapFileXML);
    }

    PhysBody* c1 = app->physics->CreateRectangle(238, 632, 480 * 50, 16, STATIC);
    c1->ctype = ColliderType::PLATFORM;

   /* PhysBody* c2 = app->physics->CreateRectangle(352 + 64, 384 + 32, 128, 64, STATIC);
    c2->ctype = ColliderType::PLATFORM;

    PhysBody* c3 = app->physics->CreateRectangle(256, 704 + 32, 576, 64, STATIC);
    c3->ctype = ColliderType::PLATFORM;*/
    
    if(ret == true)
    {
        LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
        LOG("width : %d height : %d",mapData.width,mapData.height);
        LOG("tile_width : %d tile_height : %d",mapData.tileWidth, mapData.tileHeight);
        
        LOG("Tilesets----");

        ListItem<TileSet*>* tileset;
        tileset = mapData.tilesets.start;

        while (tileset != NULL) {
            LOG("name : %s firstgid : %d",tileset->data->name.GetString(), tileset->data->firstgid);
            LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
            LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
            tileset = tileset->next;
        }

        LOG("Layers----");

        ListItem<MapLayer*>* mapLayer;
        mapLayer = mapData.maplayers.start;

        while (mapLayer != NULL) {
            LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
            LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
            mapLayer = mapLayer->next;
        }
    }

    // Find the navigation layer
        ListItem<MapLayer*>* mapLayerItem;
        mapLayerItem = mapData.maplayers.start;
        navigationLayer = mapLayerItem->data;

        //Search the layer in the map that contains information for navigation
        while (mapLayerItem != NULL) {
            if (mapLayerItem->data->properties.GetProperty("Navigation") != NULL && mapLayerItem->data->properties.GetProperty("Navigation")->value) {
                navigationLayer = mapLayerItem->data;
                break;
            }
            mapLayerItem = mapLayerItem->next;
        }

    if(mapFileXML) mapFileXML.reset();

    mapLoaded = ret;

    return ret;
}

bool Map::LoadMap(pugi::xml_node mapFile)
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        //Load map general properties
        mapData.height = map.attribute("height").as_int();
        mapData.width = map.attribute("width").as_int();
        mapData.tileHeight = map.attribute("tileheight").as_int();
        mapData.tileWidth = map.attribute("tilewidth").as_int();
        mapData.type = MAPTYPE_UNKNOWN;
    }

    return ret;
}

bool Map::LoadTileSet(pugi::xml_node mapFile){

    bool ret = true; 

    pugi::xml_node tileset;
    for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();

        set->name = tileset.attribute("name").as_string();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->margin = tileset.attribute("margin").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->tileWidth = tileset.attribute("tilewidth").as_int();
        set->tileHeight = tileset.attribute("tileheight").as_int();
        set->columns = tileset.attribute("columns").as_int();
        set->tilecount = tileset.attribute("tilecount").as_int();

        SString texPath = path; 
        texPath += tileset.child("image").attribute("source").as_string();
        set->texture = app->tex->Load(texPath.GetString());

        if(tileset.child("tile")) //check if the tileset is an Animation (no se si seria mejor checkear si tiene un animation child)
        {
            LoadAnimation(tileset.child("tile"), set);
        }
        else
        {
            mapData.tilesets.Add(set);
        }
    }

    return ret;
}

bool Map::LoadAnimation(pugi::xml_node node, TileSet* tileset)
{
    bool ret = true;

    Animation* anim = new Animation();
    anim->name = tileset->name;
    anim->texture = tileset->texture;

    for (pugi::xml_node frameNode = node.child("animation").child("frame"); frameNode && ret; frameNode = frameNode.next_sibling("frame"))
    {
        int id = frameNode.attribute("tileid").as_int();
        int tilesPerRow = tileset->columns;
        int x = (id % tilesPerRow) * tileset->tileWidth;
        int y = (id / tilesPerRow) * tileset->tileHeight;
        anim->PushBack({x, y, tileset->tileWidth, tileset->tileHeight});
    }

    mapData.animations.Add(anim);

    return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    //Load the attributes
    layer->id = node.attribute("id").as_int();
    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();
    if (node.attribute("parallaxx"))
    {
        layer->parallaxFactor = node.attribute("parallaxx").as_float();
    }
    else
    {
        layer->parallaxFactor = 1.0f;
    }

    LoadProperties(node, layer->properties);

    //Reserve the memory for the data 
    layer->data = new uint[layer->width * layer->height];
    memset(layer->data, 0, layer->width * layer->height);

    //Iterate over all the tiles and assign the values
    pugi::xml_node tile;
    int i = 0;
    for (tile = node.child("data").child("tile"); tile && ret; tile = tile.next_sibling("tile"))
    {
        layer->data[i] = tile.attribute("gid").as_int();
        i++;
    }

    return ret;
}

bool Map::LoadAllLayers(pugi::xml_node mapNode) {
    bool ret = true;

    for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode && ret; layerNode = layerNode.next_sibling("layer"))
    {
        //Load the layer
        MapLayer* mapLayer = new MapLayer();
        ret = LoadLayer(layerNode, mapLayer);

        //add the layer to the map
        mapData.maplayers.Add(mapLayer);
    }

    return ret;
}

bool Map::LoadColliders(pugi::xml_node mapFile)
{
    bool ret = true; 
    uint scale = app->win->GetScale();

    //TODO!! check if the objectgroup's class is collider
    pugi::xml_node objectGroup;
    for (objectGroup = mapFile.child("map").child("objectgroup"); objectGroup && ret; objectGroup = objectGroup.next_sibling("objectgroup"))
    {
        pugi::xml_node properties = objectGroup.child("properties");
        pugi::xml_node property = properties.child("property");
        std::string objectType = property.attribute("value").as_string();

        if (objectType == "collider") {
            pugi::xml_node collider;
            for (collider = objectGroup.child("object"); collider && ret; collider = collider.next_sibling("object"))
            {

                if (SString(collider.attribute("type").as_string()) == "rectangle")
                {
                    Colliders* c = new Colliders();

                    c->x = collider.attribute("x").as_int();
                    c->y = collider.attribute("y").as_int();
                    c->width = collider.attribute("width").as_int();
                    c->height = collider.attribute("height").as_int();

                    PhysBody* c1 = app->physics->CreateRectangle(c->x + c->width / 2, c->y + c->height / 2, c->width, c->height, STATIC);

                    if (SString(objectGroup.attribute("class").as_string()) == "platforms") {
                        c1->ctype = ColliderType::PLATFORM;
                    }
                    else if (SString(objectGroup.attribute("class").as_string()) == "death")
                    {
                        c1->ctype = ColliderType::DEATH;
                        c1->body->GetFixtureList()->SetSensor(true);
                    }
                    else if (SString(objectGroup.attribute("class").as_string()) == "limits")
                    {
                        c1->ctype = ColliderType::LIMITS;
                    }
                    else if (SString(objectGroup.attribute("class").as_string()) == "win")
                    {
                        c1->ctype = ColliderType::WIN;
                    }
                    else
                    {
                        c1->ctype = ColliderType::UNKNOWN;
                    }

                }
                else if (SString(collider.attribute("type").as_string()) == "polygon")
                {
                    /* int* points = new int[collider.child("polygon").attribute("points").as_int() * sizeof(int)];

                        app->physics->CreateChain(collider.attribute("x").as_int(),
                                                collider.attribute("y").as_int(),
                                                points,
                                                collider.child("polygon").attribute("points").as_int(),
                                                    STATIC);*/
                }
            }
        }
        
        if (objectType == "item") {
            pugi::xml_node item;
            for (item = objectGroup.child("object"); item && ret; item = item.next_sibling("object"))
            { 

               if (SString(item.attribute("type").as_string()) == "coin") {
                   ScoreItem* itemEntity = (ScoreItem*)app->entityManager->CreateEntity(EntityType::SCOREITEM);
                   
                   itemEntity->position.x = item.attribute("x").as_int();
                   itemEntity->position.y = item.attribute("y").as_int();
                   itemEntity->texturePath = "Assets/Textures/bridge.png";
                   //itemEntity->parameters = item;
               } else if (SString(item.attribute("type").as_string()) == "food") {
                   FoodItem* foodItem = (FoodItem*)app->entityManager->CreateEntity(EntityType::FOODITEM);

                   foodItem->position.x = item.attribute("x").as_int();
                   foodItem->position.y = item.attribute("y").as_int();
               }
               
            }
        }

        if (objectType == "checkpoint") {
            pugi::xml_node checkpoint;
            for (checkpoint = objectGroup.child("object"); checkpoint && ret; checkpoint = checkpoint.next_sibling("object"))
            {
                Checkpoint* checkpointEntity = (Checkpoint*)app->entityManager->CreateEntity(EntityType::CHECKPOINT);

                checkpointEntity->position.x = checkpoint.attribute("x").as_int();
                checkpointEntity->position.y = checkpoint.attribute("y").as_int();
                checkpointEntity->size.x = checkpoint.attribute("width").as_int();
                checkpointEntity->size.y = checkpoint.attribute("height").as_int();
                checkpointEntity->texturePath = "Assets/Textures/bridge.png";
            }
        }
    }

    return ret;
}

// L13: Create navigationMap map for pathfinding
void Map::CreateNavigationMap(int& width, int& height, uchar** buffer) const
{
    bool ret = false;

    //Sets the size of the map. The navigation map is a unidimensional array 
    uchar* navigationMap = new uchar[navigationLayer->width * navigationLayer->height];
    //reserves the memory for the navigation map
    memset(navigationMap, 1, navigationLayer->width * navigationLayer->height);

    for (int x = 0; x < mapData.width; x++)
    {
        for (int y = 0; y < mapData.height; y++)
        {
            //i is the index of x,y coordinate in a unidimensional array that represents the navigation map
            int i = (y * navigationLayer->width) + x;

            //Gets the gid of the map in the navigation layer
            int gid = navigationLayer->Get(x, y);
            
            //If the gid is a blockedGid is an area that I cannot navigate, so is set in the navigation map as 0, all the other areas can be navigated
            //!!!! make sure that you assign blockedGid according to your map
            if (gid == blockedGid) navigationMap[i] = 0;
            else navigationMap[i] = 1;
        }
    }

    *buffer = navigationMap;
    width = mapData.width;
    height = mapData.height;

}

bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.list.Add(p);
    }

    return ret;
}

Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* item = list.start;
    Property* p = NULL;

    while (item)
    {
        if (item->data->name == name) {
            p = item->data;
            break;
        }
        item = item->next;
    }

    return p;
}


