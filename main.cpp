// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"

// Functions in this file
int get_action (GameInputs inputs);
void move_dragon();
void move_goblin();
int update_game (int action);
void draw_game (int init);
void init_main_map ();
void init_next_map();
void init_next_map_advanced();
int main ();

void game_over(int next_state);
int action_button();
int go_up();
int go_down();
int go_right();
int go_left();

/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_key;
    // Additional properties
    int spell;  // If spell is cast
    int talk;   // If talked to Merlin
    int exit;   // If exit the dungeon 
    int enter;  // If entered the dugeon (but did not cast spell)
    // Advanced properties
    int mode_select;    // To select b/w BASELINE and ADVANCED
    int health;  
    int phealth;    
} Player;

/**
 * Moving Dragon.
 */
struct {
    int x, y;
    int px, py;
    int dir;
    int dead;
} MobileDragon;

/**
 * Moving Goblin.
 */
struct {
    int x, y;
    int px, py;
    int dir;
    int dead;
} MobileGoblin;

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1     // B1
#define MENU_BUTTON 2       // Menu
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
#define GAME 7              // For mode selection
#define BASELINE 8
#define ADVANCED 9

DigitalOut test_led(LED1);  // Turns on when omnipotent is ON

// Return coordinates (from dungeon to main)
int return_cox;
int return_coy;

// Object coordinates
int spell_cox, spell_coy;
int spell_goblin_cox, spell_goblin_coy;
int elixir_cox, elixir_coy;

int mode_select;    // switch between modes

static int gameState = MENU_BUTTON;

int get_action(GameInputs inputs)
{   
    switch(gameState) {
        case MENU_BUTTON:
            if (inputs.b1 == 0 && inputs.b2 == 1 && inputs.b3 == 1) {
                mode_select = 0;
                return BASELINE;
            }
            else if (inputs.b1 == 1 && inputs.b2 == 0 && inputs.b3 == 1) {
                mode_select = 1; 
                return ADVANCED; 
            }
            else return NO_ACTION;
        case GAME:
            if(inputs.b2 == 0) { 
                omnipotent = !omnipotent;
                test_led = !test_led;
                wait(1.0);
            }
            
            // B1 is default action button
            if (inputs.b1 == 0) return ACTION_BUTTON;
            
            // ACCELEROMETER INPUTS
            if (inputs.ay > 0.30) return GO_UP;
            if (inputs.ay < -0.30) return GO_DOWN;
            if (inputs.ax > 0.30) return GO_RIGHT;
            if (inputs.ax < -0.30) return GO_LEFT;
            return NO_ACTION;
        default:
            return NO_ACTION;
    }
}

/**
 * Moves the moving DRAGON every time a player moves 2 blocks.
 */
void move_dragon() {
    MapItem* east = get_east(MobileDragon.x, MobileDragon.y);
    MapItem* west = get_west(MobileDragon.x, MobileDragon.y);
    switch(MobileDragon.dir) {
        case 0:     // RIGHT
            if (east->walkable == 0) {
                MobileDragon.dir = 1;  // change to left
            }
            else {
                map_erase(MobileDragon.x, MobileDragon.y);
                MobileDragon.x++;
                add_dragon(MobileDragon.x, MobileDragon.y);
            }
            break;
        case 1:     // LEFT
            if (west->walkable == 0) {
                MobileDragon.dir = 0;  // change to right
            }
            else {
                map_erase(MobileDragon.x, MobileDragon.y);
                MobileDragon.x--;
                add_dragon(MobileDragon.x, MobileDragon.y);
            }
            break;
        default: 
            break;
    }
}

/**
 * Moves the moving GOBLIN every time a player moves 2 blocks.
 */
void move_goblin() {
    MapItem* north = get_north(MobileGoblin.x, MobileGoblin.y);
    MapItem* south = get_south(MobileGoblin.x, MobileGoblin.y);
    switch(MobileGoblin.dir) {
        case 0:     // UP
            if (north->walkable == 0) {
                MobileGoblin.dir = 1;  // change to down
            }
            else {
                map_erase(MobileGoblin.x, MobileGoblin.y);
                MobileGoblin.y--;
                add_goblin(MobileGoblin.x, MobileGoblin.y);
            }
            break;
        case 1:     // DOWN
            if (south->walkable == 0) {
                MobileGoblin.dir = 0;  // change to up
            }
            else {
                map_erase(MobileGoblin.x, MobileGoblin.y);
                MobileGoblin.y++;
                add_goblin(MobileGoblin.x, MobileGoblin.y);
            }
            break;
        default: 
            break;
    }
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
#define HURT      3
#define WIN       4

int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;
    Player.phealth = Player.health;

    MobileDragon.px = MobileDragon.x;
    MobileDragon.py = MobileDragon.y;
    
    MobileGoblin.px = MobileGoblin.x;
    MobileGoblin.py = MobileGoblin.y;
    
    
    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {   
        case BASELINE:
            gameState = GAME;
            return BASELINE;
        case ADVANCED:
            gameState = GAME;
            return ADVANCED;
        case GO_UP:
            if (gameState == MENU_BUTTON) return NO_ACTION;
            if (mode_select) {  // only for ADVANCED
                if ((Player.x + Player.y)%2) {
                    if (!MobileDragon.dead) move_dragon();
                    if (!MobileGoblin.dead) move_goblin();
                }
            }
            return go_up();
        case GO_LEFT:
            if (gameState == MENU_BUTTON) return NO_ACTION;
            if (mode_select) {
                if ((Player.x + Player.y)%2) {
                    if (!MobileDragon.dead) move_dragon();
                    if (!MobileGoblin.dead) move_goblin();
                }
            }
            return go_left();
        case GO_DOWN:
            if (gameState == MENU_BUTTON) return NO_ACTION;
            if (mode_select) {
                if ((Player.x + Player.y)%2) {
                    if (!MobileDragon.dead) move_dragon();
                    if (!MobileGoblin.dead) move_goblin();
                }
            }
            return go_down();
        case GO_RIGHT:
            if (gameState == MENU_BUTTON) return NO_ACTION;
            if (mode_select) {
                if ((Player.x + Player.y)%2) {
                    if (!MobileDragon.dead) move_dragon();
                    if (!MobileGoblin.dead) move_goblin();
                }
            }
            return go_right();
        case ACTION_BUTTON:
            if (gameState == MENU_BUTTON) return NO_ACTION;
            return action_button();
//        case MENU_BUTTON:
//            break;
        default:        
            break;
    }
    return NO_RESULT;
}

/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    // Draw game border first
    if(init) draw_border();
    
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (init && i == 0 && j == 0) // Only draw the player on init
            {
                draw_player(u, v, Player.has_key);
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
            }
            else if (x == MobileDragon.x && y == MobileDragon.y) {
                if(Player.px != Player.x || Player.py != Player.y || init == 2){
                    MapItem* curr_item = get_here(x, y);
                    draw = (curr_item)?curr_item->draw:draw_nothing;
                    draw(u,v);
                    draw_dragon(u, v);
                }
                continue;
            }
            else if (x == MobileGoblin.x && y == MobileGoblin.y) {
                if(Player.px != Player.x || Player.py != Player.y || init == 2){
                    MapItem* curr_item = get_here(x, y);
                    draw = (curr_item)?curr_item->draw:draw_nothing;
                    draw(u,v);
                    draw_goblin(u, v);
                }
                continue;
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars    
    draw_upper_status(Player.x, Player.y, Player.px, Player.py);
    if (mode_select) draw_lower_status(Player.health, Player.phealth);  // Only for ADVANCED mode
}

// Dsiplays "Game Over" when the quest is completed
void game_over(int next_state)
{
    if (next_state == GAME_OVER) {
        uLCD.color(RED);      
        uLCD.cls();
        uLCD.text_width(1);     
        uLCD.text_height(1);    
        uLCD.printf("***Game Over***");
        uLCD.color(BLACK);
        while (true) {
            // Inifinite loop
        }
    }
    
    if (next_state == WIN) {
        uLCD.color(GREEN);      
        uLCD.cls();
        uLCD.text_width(1);     
        uLCD.text_height(1);    
        uLCD.printf("Yay! You WON :)");
        uLCD.color(BLACK);
        while (true) {
            // Inifinite loop
        }
    }
}

int action_button() 
{
    // Interaction with WIZARD
    if (get_north(Player.x, Player.y) -> type == WIZARD || get_south(Player.x, Player.y) -> type == WIZARD || get_east(Player.x, Player.y) -> type == WIZARD || get_west(Player.x, Player.y) -> type == WIZARD) {
    
        const char* line1 = "I have a job for you! You have to kill the dragon for a nice reward. Here's the laddar to the dungeon where it lives.\n";

        const char* line2 = "Good job! Here's the key to the treasure chest.\n";
        
        if (!Player.talk) {
            speech(line1);
            Player.talk = !Player.talk;
            add_laddar(Player.x + 3, Player.y + 3);
        } 
        if (Player.spell) {
            speech(line2);
            add_key(Player.x + 3, Player.y - 3);
        }

        return FULL_DRAW;
    }
    
    // Interaction with LADDAR
    if (get_north(Player.x, Player.y) -> type == LADDAR || get_south(Player.x, Player.y) -> type == LADDAR || get_east(Player.x, Player.y) -> type == LADDAR || get_west(Player.x, Player.y) -> type == LADDAR) {
        if (Player.spell && Player.exit) {
            speech("Now that the dragon is dead, the dungeon is sealed. Go talk to Merlin again.\n");
            return FULL_DRAW;
        } 
        else if (Player.spell && !Player.exit) {
            Player.exit = 1;
            Player.x = return_cox;
            Player.y = return_coy;
            init_main_map();
            return FULL_DRAW;
        }
        else if (!Player.enter) {
            return_cox = Player.x;    // save initial coordinates to ruturn to the same position
            return_coy = Player.y;
            Player.enter = 1;
            if (!mode_select) {
                speech("This is the laddar to the dugneon. Be careful! Seek the GOOD spell to kill the dragon.\n");
                init_next_map();
                Player.x = 2;
                Player.y = 4;
            }
            else if (mode_select) {
                speech("This is the laddar to the dugneon. Be careful! Kill BOTH Dragon and Goblin or DIE.\n");
                init_next_map_advanced();
                Player.x = 15;
                Player.y = 13;
            }
            draw_game(1);
            return FULL_DRAW;
        }
    }
    
    // Interaction with GOOD (Dragon) spell
    if (get_north(Player.x, Player.y) -> type == SPELL || get_south(Player.x, Player.y) -> type == SPELL || get_east(Player.x, Player.y) -> type == SPELL || get_west(Player.x, Player.y) -> type == SPELL) {
        if (!mode_select) {     // BASELINE interaction
            speech("You cast the good spell! Now the dragon is dead. Go back to Merlin and talk to him again.\n");
            Player.spell = 1;
        }
        else if (mode_select) { // ADVANCED interaction
            speech("You cast the final spell! Now the dragon is (also) dead. Go back to Merlin and talk to him again.\n");
            MobileDragon.dead = 1;
            map_erase(spell_cox, spell_coy);
            map_erase(MobileDragon.x, MobileDragon.y);
            add_grave(MobileDragon.x, MobileDragon.y);
            Player.spell = 1;
        }
        return FULL_DRAW;
    }

    // Interaction with DARK (Goblin) spell
    if (get_north(Player.x, Player.y) -> type == SPELL_DARK || get_south(Player.x, Player.y) -> type == SPELL_DARK || get_east(Player.x, Player.y) -> type == SPELL_DARK || get_west(Player.x, Player.y) -> type == SPELL_DARK) {
        if (!mode_select) {     // BASELINE interaction
            speech("This is the dark spell... You're gonna be cursed if you use it! Try the other one.\n");
        }
        else if (mode_select) { // ADVANCED interaction
            speech("You cast the spell! Now the goblin is dead. Go finish off the dragon!\n");
            MobileGoblin.dead = 1;
            map_erase(spell_goblin_cox, spell_goblin_coy);
            map_erase(MobileGoblin.x, MobileGoblin.y);
            add_grave(MobileGoblin.x, MobileGoblin.y); 
            add_elixir(elixir_cox, elixir_coy);     // Drop elixir
        }
        return FULL_DRAW;
    }
    
    // Interaction with KEY
    if (get_north(Player.x, Player.y) -> type == KEY || get_south(Player.x, Player.y) -> type == KEY || get_east(Player.x, Player.y) -> type == KEY || get_west(Player.x, Player.y) -> type == KEY) {
        
        speech("You got a key. Now open the treasure chest and you'll be finally rewarded.\n");

        Player.has_key = true;

        if      (get_north(Player.x, Player.y) -> type == KEY)  map_erase(Player.x, Player.y - 1);
        else if (get_south(Player.x, Player.y) -> type == KEY)  map_erase(Player.x, Player.y + 1);
        else if (get_east(Player.x, Player.y) -> type == KEY)   map_erase(Player.x + 1, Player.y);
        else if (get_west(Player.x, Player.y) -> type == KEY)   map_erase(Player.x - 1, Player.y);
        
        add_chest(Player.x - 3, Player.y - 3);
        
        return FULL_DRAW;
    }
    
    // Interaction with CHEST
    if (get_north(Player.x, Player.y) -> type == CHEST || get_south(Player.x, Player.y) -> type == CHEST || get_east(Player.x, Player.y) -> type == CHEST || get_west(Player.x, Player.y) -> type == CHEST) {
        if (Player.has_key) {
            speech("Congratulations! You're a hero and rich!\n");
            game_over(WIN);
            return FULL_DRAW;
        }
    }
    
    // Interaction with ELIXIR
    if (get_north(Player.x, Player.y) -> type == ELIXIR || get_south(Player.x, Player.y) -> type == ELIXIR || get_east(Player.x, Player.y) -> type == ELIXIR || get_west(Player.x, Player.y) -> type == ELIXIR) {
        
        speech("This is Life Elixir. You're health is now boosted up.\n");
        
        Player.phealth = Player.health;
        Player.health++;

        if      (get_north(Player.x, Player.y) -> type == ELIXIR)  map_erase(Player.x, Player.y - 1);
        else if (get_south(Player.x, Player.y) -> type == ELIXIR)  map_erase(Player.x, Player.y + 1);
        else if (get_east(Player.x, Player.y) -> type == ELIXIR)   map_erase(Player.x + 1, Player.y);
        else if (get_west(Player.x, Player.y) -> type == ELIXIR)   map_erase(Player.x - 1, Player.y);
    
        
        return FULL_DRAW;
    }
    
    return FULL_DRAW;

}

int go_up()
{
    MapItem *next = get_north(Player.x, Player.y);
    MapItem *here = get_here(Player.x, Player.y);

    if (omnipotent) {
        Player.y = Player.y - 1;
        if (!(next->walkable) || !(here->walkable))
            return FULL_DRAW;

        return NO_RESULT;
    }

    if (next->walkable) {
        Player.y = Player.y - 1;
        return NO_RESULT;
    } 
    else {
        speaker = 0.3;
        speaker.period(1.0/75.0);
        wait(0.05);
        speaker = 0;
    }
    
    if (next->type == DANGER)
    {
        Player.phealth = Player.health;
        Player.health = Player.health - 1;
        
        if (Player.health <= 0)
            return GAME_OVER;
        
        return HURT;
    }

    
    return NO_RESULT;
}

int go_down()
{
    MapItem *next = get_south(Player.x, Player.y);
    MapItem *here = get_here(Player.x, Player.y);

    if (omnipotent) {
        Player.y = Player.y + 1;
        if (!(next->walkable) || !(here->walkable))
            return FULL_DRAW;

        return NO_RESULT;
    }

    if (next->walkable) {
        Player.y = Player.y + 1;
        return NO_RESULT;
    } 
    else {
        speaker = 0.3;
        speaker.period(1.0/75.0);
        wait(0.05);
        speaker = 0;
    }
    
    if (next->type == DANGER)
    {
        Player.phealth = Player.health;
        Player.health = Player.health - 1;
        
        if (Player.health <= 0)
            return GAME_OVER;
        
        return HURT;
    }
    
    return NO_RESULT;
}

int go_right()
{
    MapItem *next = get_east(Player.x, Player.y);
    MapItem *here = get_here(Player.x, Player.y);

    if (omnipotent) {
        Player.x = Player.x + 1;
        if (!(next->walkable) || !(here->walkable))
            return FULL_DRAW;

        return NO_RESULT;
    }

    if (next->walkable) {
        Player.x = Player.x + 1;
        return NO_RESULT;
    } 
    else {
        speaker = 0.3;
        speaker.period(1.0/75.0);
        wait(0.05);
        speaker = 0;
    }
    
    if (next->type == DANGER)
    {
        Player.phealth = Player.health;
        Player.health = Player.health - 1;
        
        if (Player.health <= 0)
            return GAME_OVER;
        
        return HURT;
    }
    
    return NO_RESULT;
}

int go_left() 
{
    MapItem *next = get_west(Player.x, Player.y);
    MapItem *here = get_here(Player.x, Player.y);

    if (omnipotent) {
        Player.x = Player.x - 1;
        if (!(next->walkable) || !(here->walkable))
            return FULL_DRAW;

        return NO_RESULT;
    }

    if (next->walkable) {
        Player.x = Player.x - 1;
        return NO_RESULT;
    } 
    else {
        speaker = 0.3;
        speaker.period(1.0/75.0);
        wait(0.05);
        speaker = 0;
    }
    
    if (next->type == DANGER)
    {
        Player.phealth = Player.health;
        Player.health = Player.health - 1;
        
        if (Player.health <= 0)
            return GAME_OVER;
        
        return HURT;
    }
    
    return NO_RESULT;
}

/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */
void init_main_map()
{
    // "Random" plants
    Map* map = set_active_map(0);
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_plant(i % map_width(), i / map_width());
    }
    pc.printf("plants\r\n");
        
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");

    // Populate with the wizard
    add_npc_wizard(43, 39);

    print_map();
}

void init_next_map()
{
     
    Map *map = set_active_map(1);
     // "Random" plants <-- UNCOMMENT IF NEED PLANTS
//    for (int i = map_width() + 3; i < map_area(); i += 39) {
//        add_plant(i % map_width(), i / map_width());
//    }
//    pc.printf("plants\r\n");

    pc.printf("Adding walls!\r\n");
    add_wall(0, 0, HORIZONTAL, map_width());
    add_wall(0, map_height() - 1, HORIZONTAL, map_width());
    add_wall(0, 0, VERTICAL, map_height());
    add_wall(map_width() - 1, 0, VERTICAL, map_height());

    add_wall(1, 3, HORIZONTAL, 6);
    add_wall(3, 6, HORIZONTAL, 6);
    pc.printf("Walls done!\r\n");
    
    // Add laddar
    add_laddar(1, 4); 

    // Populate with spells
    add_spell(1, 1);
    add_spell_dark(8, 8); 

    print_map();
}


void init_next_map_advanced() 
{
    Map *map = set_active_map(2);
    char nextMap[20][20] = {
        {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', 'G', ' ', ' ', 'A', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', 'W', 'W', 'W', ' ', 'D', ' ', 'W', 'W', 'W', 'W', 'W', 'W', 'W', ' ', ' ', ' ', 'W', 'W', 'W'},
        {'W', 'W', 'W', 'W', ' ', ' ', ' ', 'W', 'W', 'W', 'W', 'W', 'W', 'W', ' ', ' ', ' ', 'W', 'W', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', 'S', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', 'L', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W'},
        {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'}
    };
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            if (nextMap[i][j] == 'W') {
                add_wall(j, i, VERTICAL, 1);
            } else if (nextMap[i][j] == 'A') {      // GOBLIN spell
                add_spell_dark(j, i);
                spell_goblin_cox = j;
                spell_goblin_coy = i;
            } 
            else if (nextMap[i][j] == 'G') {
                add_goblin(j, i);
                MobileGoblin.x = j;
                MobileGoblin.y = i;
            }
            else if (nextMap[i][j] == 'D') {
                add_dragon(j, i);
                MobileDragon.x = j;
                MobileDragon.y = i;
            }
            else if (nextMap[i][j] == 'B') {      // DRAGON spell
                add_spell(j, i);
                spell_cox = j;
                spell_coy = i;
            }
            else if (nextMap[i][j] == 'L') {
                add_laddar(j, i);
            }
            else if (nextMap[i][j] == 'S') {
                add_sign(j, i);
            }
            else if (nextMap[i][j] == 'E') {
                elixir_cox = j;
                elixir_coy = i;
            }
        }
    }
    
    print_map();
}

/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{
    GameInputs inputs;
    int action;
    int next_state;

    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    // uLCD.filled_rectangle(64, 64, 74, 74, RED); //DELETE OR COMMENT THIS LINE  

    maps_init();

    while(1)
    {
        switch (gameState) {
            case MENU_BUTTON:
                // START PAGE
                uLCD.cls();
                uLCD.text_width(3);
                uLCD.text_height(3);
                uLCD.color(RED);
                uLCD.printf("DRAGON\nSLAYER");
                wait(3);
            
                // IN-GAME MENU
                uLCD.cls();
                uLCD.color(GREEN);
                uLCD.printf("Select Mode: \n\n");
                uLCD.printf("1 - BASELINE\n");
                uLCD.printf("2 - ADVANCED");
        
                next_state = NO_ACTION;
                while (next_state == NO_ACTION) 
                {
                    inputs = read_inputs();
                    action = get_action(inputs);
                    next_state = update_game(action);
                    if (next_state == BASELINE || next_state == ADVANCED) {
                        init_main_map();        // main map is the same -- dungeon map different
                        set_active_map(0);
                        Player.x = Player.y = 40;
                        if (next_state == ADVANCED) {
                            Player.health = 2;      // HEALTH is only for ADVANCED
                        }
                        break;
                    }
                }
                uLCD.cls();
                wait(.5);
                gameState = GAME;
            case GAME:
                // Initial drawing
                draw_game(true);
                // Main game loop
                while(1)
                {
                    // Timer to measure game update speed
                    Timer t; 
                    t.start();
                    // Actually do the game update:
                    // 1. Read inputs        
                    inputs = read_inputs();
                    // 2. Determine action (get_action) 
                    action = get_action(inputs);
                    // 3. Update game (update_game)
                    next_state = update_game(action);
                    // 3b. Check for game over
                    game_over(next_state);
                    // 4. Draw frame (draw_game)
                    draw_game(true);
                    // 5. Frame delay
                    t.stop();
                    int dt = t.read_ms();
                    if (dt < 100) wait_ms(100 - dt);
                }
                // break; // unreachable
            default:
                break;
        }
    }
}
