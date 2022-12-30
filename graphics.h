#ifndef GRAPHICS_H
#define GRAPHICS_H


/**
 * Takes a string image and draws it to the screen. The string is 121 characters
 * long, and represents an 11x11 tile in row-major ordering (across, then down,
 * like a regular multi-dimensional array). The available colors are:
 *      R = Red
 *      Y = Yellow
 *      G = Green
 *      D = Brown ("dirt")
 *      5 = Light grey (50%)
 *      3 = Dark grey (30%)
 *      Any other character is black
 * More colors can be easily added by following the pattern already given.
 */
void draw_img(int u, int v, const char* img);

/**
 * Draws the player. This depends on the player state, so it is not a DrawFunc.
 */
void draw_player(int u, int v, int key);

/**
 * DrawFunc functions. 
 * These can be used as the MapItem draw functions.
 */
void draw_nothing(int u, int v);
void draw_wall(int u, int v);
void draw_plant(int u, int v);

/**
 * Draw the upper status bar.
 */
void draw_upper_status(int x, int y, int px, int py);

/**
 * Draw the lower status bar.
 */ 
void draw_lower_status(int health, int phealth);

/**
 * Draw the border for the map.
 */
void draw_border();
 
 /**
 * Draw the wizard.
 */
void draw_npc_wizard(int u, int v);
 
/**
 * Draw the key.
 */
void draw_key(int u, int v);

/**
 * Draw the spell.
 */
void draw_spell(int u, int v);

/**
 * Draw the dark spell.
 */
void draw_spell_dark(int u, int v);

/**
 * Draw the chest.
 */ 
void draw_chest(int u, int v);
 
/**
 * Draw the laddar.
 */
void draw_laddar(int u, int v);

void draw_dragon(int u, int v);

void draw_goblin(int u, int v);

void draw_grave(int u, int v);

void draw_elixir(int u, int v);
 
void draw_sign(int u, int v);

#endif // GRAPHICS_H