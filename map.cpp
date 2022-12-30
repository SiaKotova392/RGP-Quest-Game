#include "map.h"

#include "globals.h"
#include "graphics.h"

/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 * In this file you need to define how the map will be structured. IE how will
 * you put values into the map, pull them from the map. Remember a "Map" variable
 * is a hashtable plus two ints (see below) 
 * You will have more than one map variable, one will be the main map with it's own hashtable.
 * Then you'll have a second map with another hashtable
 * You should store objects into the hashtable with different properties (spells
 * etc)
 */
struct Map {
    HashTable* items;
    int w, h;
};

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map map[3];
static int active_map;

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    return (X + Y) * (X + Y + 1) / 2 + X;
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{
    return key % NUM_BUCKETS;
}

void maps_init()
{
    map[0].items = createHashTable(map_hash,  NUM_BUCKETS);
    map[0].h = HEIGHT;
    map[0].w = WIDTH;

    map[1].items = createHashTable(map_hash,  NUM_BUCKETS);
    map[1].h = 10;
    map[1].w = 10;
    
    map[2].items = createHashTable(map_hash,  NUM_BUCKETS);
    map[2].h = 20;
    map[2].w = 20;
}

Map* get_active_map()
{
    // There's only one map
    return &map[active_map];
}

Map* set_active_map(int m)
{
    active_map = m;
    return &map[active_map];
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}

int map_width()
{
    Map* map = get_active_map();
    return map->w;
}

int map_height()
{
    Map* map = get_active_map();
    return map->h;
}

int map_area()
{
    Map* map = get_active_map();
    return map_width() * map_height();
}

MapItem* get_north(int x, int y)
{
    Map* map = get_active_map();
    return (MapItem *)getItem(map->items, XY_KEY(x, y - 1));
}

MapItem* get_south(int x, int y)
{
    Map* map = get_active_map();
    return (MapItem *)getItem(map->items, XY_KEY(x, y + 1));
}

MapItem* get_east(int x, int y)
{
    Map* map = get_active_map();
    return (MapItem *)getItem(map->items, XY_KEY(x + 1, y));
}

MapItem* get_west(int x, int y)
{
    Map* map = get_active_map();
    return (MapItem *)getItem(map->items, XY_KEY(x - 1, y));
}

MapItem* get_here(int x, int y)
{
    Map* map = get_active_map();
    return (MapItem *)getItem(map->items, XY_KEY(x, y));
}


void map_erase(int x, int y)
{
    Map* map = get_active_map();
    removeItem(map->items, XY_KEY(x, y));
}

void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_plant(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PLANT;
    w1->draw = draw_plant;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_npc_wizard(int x, int y)
{
    MapItem* w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = WIZARD;
    w1->draw = draw_npc_wizard;
    w1->walkable = true;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

void add_key(int x, int y)
{
    MapItem *w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = KEY;
    w1->draw = draw_key;
    w1->walkable = true;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

void add_spell(int x, int y)
{
    MapItem *w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = SPELL;
    w1->draw = draw_spell;
    w1->walkable = true;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

void add_spell_dark(int x, int y)
{
    MapItem *w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = SPELL_DARK;
    w1->draw = draw_spell_dark;
    w1->walkable = true;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

void add_chest(int x, int y)
{
    MapItem *w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = CHEST;
    w1->draw = draw_chest;
    w1->walkable = true;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

void add_laddar(int x, int y)
{
    MapItem *w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = LADDAR;
    w1->draw = draw_laddar;
    w1->walkable = true;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

void add_dragon(int x, int y)
{
    MapItem *w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = DANGER;
    w1->draw = draw_dragon;
    w1->walkable = false;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

void add_goblin(int x, int y)
{
    MapItem *w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = DANGER;
    w1->draw = draw_goblin;
    w1->walkable = false;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

void add_grave(int x, int y)
{
    MapItem *w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = GRAVE;
    w1->draw = draw_grave;
    w1->walkable = false;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

void add_elixir(int x, int y)
{
    MapItem *w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = ELIXIR;
    w1->draw = draw_elixir;
    w1->walkable = true;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

void add_sign(int x, int y)
{
    MapItem *w1 = (MapItem *)malloc(sizeof(MapItem));
    w1->type = SIGN;
    w1->draw = draw_sign;
    w1->walkable = true;
    w1->data = NULL;
    void *val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val)
        free(val); // If something is already there, free it
}

