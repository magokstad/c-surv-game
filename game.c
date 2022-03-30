// Standards
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Math and time etc
#include <math.h>
#include <time.h>

// Ncurses for terminal gui
#include <ncurses.h>

// Imports from other scripts in projects
#include "perlin_ctrlc_ctrlv.c"



//0.03 or 0.025 OG PERL_FREQ
#define PERL_FREQ   0.02

// 5 or 4 or 3 OG PERL_DEPTH
#define PERL_DEPTH  3

// WIP but plan is to have a seperate noise gen for mats on tiles. How i save the state if these, idk
#define MAT_FREQ    5
#define MAT_DEPTH   0.03




// COLOR AND SYMBOL OPTS

// Symbols for the materials
#define GRASS_CHAR  'M'
#define HGRASS_CHAR 'N'
#define SAND_CHAR   ')'
#define WATER_CHAR  '~'
#define DWATER_CHAR '-'
#define STONE_CHAR  'O'
#define MOUNT_CHAR  '^'
#define HMOUNT_CHAR 'A'
#define PLAYER_CHAR 'P'

#define MV_BLACK_L  {DWATER_CHAR, MOUNT_CHAR, HMOUNT_CHAR}
#define MV_BLACK_S  3
//#define EMPTY_CHAR  '.'

// Material color pair IDs (for some reason it didnt like 0...)
// These are used to assign a pair of colors to a char
#define GRASS_PAIR  1

#define SAND_PAIR   2
#define WATER_PAIR  3
#define STONE_PAIR  4 
#define TREE_PAIR   5
#define PLAYER_PAIR 6

#define HGRASS_PAIR 7
#define HMOUNT_PAIR 8
#define DWATER_PAIR 9

#define HUD1_PAIR   10

// New color IDs
#define COLOR_DGREEN    10 // Dark green
#define COLOR_GRAY      11
#define COLOR_DGRAY     12 // Dark gray
#define COLOR_BWHITE    13 // Bright white
#define COLOR_SAND      14
#define COLOR_SHORE     15
#define COLOR_DWATER    16 // Deep water

// Max and min heights for terrain
#define DWATER_MAX_H    0.5
#define WATER_MAX_H     0.62
#define SAND_MAX_H      0.65
#define GRASS_MAX_H     0.73
#define HGRASS_MAX_H    0.84
#define MOUNT_MAX_H     0.90
#define HMOUNT_MAX_H    1

// WIP
#define STONE_MIN_H     0.8
#define TREE_MIN_H      0.7


// PLAYER SETTINGS
#define B_UP    'w'
#define B_DOWN  's'
#define B_LEFT  'a'
#define B_RIGHT 'd'
#define B_CAM   'r'

// Speeds
#define PX_SPEED 1
#define PY_SPEED 1

// This one is called during setup in main
void color_init() {

    // Starts color mode
    start_color();

    // Initializes newly deifned colors
    init_color(COLOR_SHORE,     100,    500,    1000);
    init_color(COLOR_DWATER,    80,    400,    800);
    init_color(COLOR_DGREEN,    0,      500,    0);
    init_color(COLOR_DGRAY,     500,    500,    500);
    init_color(COLOR_BWHITE,    1000,   1000,   1000);
    init_color(COLOR_SAND,      1000,   860,    540);

    // Initializes color pairs
    init_pair(GRASS_PAIR,   COLOR_GREEN,    COLOR_GREEN);
    init_pair(HGRASS_PAIR,  COLOR_DGREEN,   COLOR_DGREEN);

    init_pair(SAND_PAIR,    COLOR_SAND,     COLOR_SAND);
    init_pair(WATER_PAIR,   COLOR_SHORE,    COLOR_SHORE);
    init_pair(DWATER_PAIR,  COLOR_DWATER,     COLOR_DWATER);
    
    init_pair(STONE_PAIR,   COLOR_WHITE,    COLOR_WHITE); // white is already quite gray
    init_pair(HMOUNT_PAIR,  COLOR_BWHITE,   COLOR_BWHITE);

    init_pair(TREE_PAIR,    COLOR_GREEN,    COLOR_BLACK); // i also need brown and diff shade of green
    init_pair(PLAYER_PAIR,  COLOR_WHITE,      COLOR_RED); // i dont fully know what to do w/ the player yet...

    init_pair(HUD1_PAIR,    COLOR_WHITE,    COLOR_BLACK);
}

// compat check
void check_compat() {
    if (has_colors() == 0) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
}



// STRUCTS

// Coord struct
typedef struct {
    int x,y;
} gm_vec2;

// Map struct
typedef struct {
    char * map_arr; 
    int maxx, maxy, area, curr_add_pos;
    gm_vec2 cam_pos;
} gm_map;

// Item struct
typedef struct {
    //WIP
} item;

// Inventory struct
typedef struct {
    // WIP
    item * item_arr;
} inventory;

// Player struct
typedef struct {
    int health;
    gm_vec2 pos;
    inventory inv;
} player;



// INITS

void init_map(gm_map * map, gm_vec2 * start) {
    map->maxx = getmaxx(stdscr); // screen max x
    map->maxy = getmaxy(stdscr); // screen max y
    map->area = map->maxx * map->maxy; // This can be handy to know the size of the map to be printed
    map->map_arr = malloc(map->maxx * map->maxy * sizeof(char)); // This should be the exact amount of memory needed 
    map->cam_pos = *start; // Position of camera / screen
    map->curr_add_pos = 0; // Slight hack for quick iteration through characters on screen (used in get_line)
}

void init_player(player * px, int start_x, int start_y) {
    px->pos.x = start_x;
    px->pos.y = start_y;
    
    // Inventory implementation WIP
}



// LOGIC

// Generates "height" for that tile using perlin noise
float get_perlin(int x,int y) {
    float ans = perlin2d(x,y,PERL_FREQ, PERL_DEPTH); // this function was copy pasted from somewhere i dont remmeber
    return ans;
}


// TODO make this and the attron func less repetitive
// Generates a line of terrain from perlin noise
void gen_line(gm_map * map, int x_start, int y_start) {
    //char a_line[map->maxx];
    for (int i = 0; i < map->maxx; i++) {
        float next_val = get_perlin(x_start+i, y_start);
        //int next_pos = ((y_start) * map->maxx) + i;

        // Terrain gen
        // TODO make less repetitive
        if (next_val < DWATER_MAX_H) {
            map->map_arr[map->curr_add_pos] = DWATER_CHAR;
        }
        else if (next_val < WATER_MAX_H) {
            map->map_arr[map->curr_add_pos] = WATER_CHAR;
        }
        else if (next_val < SAND_MAX_H) {
            map->map_arr[map->curr_add_pos] = SAND_CHAR;
        }
        else if (next_val < GRASS_MAX_H) {
            map->map_arr[map->curr_add_pos] = GRASS_CHAR;
        }
        else if (next_val < HGRASS_MAX_H) {
            map->map_arr[map->curr_add_pos] = HGRASS_CHAR;
        }
        else if (next_val <= MOUNT_MAX_H) {
            map->map_arr[map->curr_add_pos] = MOUNT_CHAR;
        }
        else if (next_val < HMOUNT_MAX_H) {
            map->map_arr[map->curr_add_pos] = HMOUNT_CHAR;
        }
        map->curr_add_pos++;
    }
    //strcat(map->map_arr, a_line);
}

// Seperate material generation like trees and stones. WIP
void gen_mats(gm_map * map) {

}

// Generates the map line by line
void gen_map(gm_map * map) {
    for (int i = 0; i < map->maxy; i++) {
        gen_line(map, map->cam_pos.x, map->cam_pos.y+i);
    }
}

//TODO make less repetitive
void choose_attri(char current){
    if (current == WATER_CHAR) {
        attron(COLOR_PAIR(WATER_PAIR));
    }
    if (current == DWATER_CHAR) {
        attron(COLOR_PAIR(DWATER_PAIR));
    }
    else if (current == SAND_CHAR) {
        attron(COLOR_PAIR(SAND_PAIR));
    }
    else if (current == GRASS_CHAR) {
        attron(COLOR_PAIR(GRASS_PAIR));
    }
    else if (current == HGRASS_CHAR) {
        attron(COLOR_PAIR(HGRASS_PAIR));
    }
    else if (current == MOUNT_CHAR) {
        attron(COLOR_PAIR(STONE_PAIR));
    }
    else if (current == HMOUNT_CHAR) {
        attron(COLOR_PAIR(HMOUNT_PAIR));
    }
}

// Main screen calculation
void printw_map(gm_map * map) {
    for (int i = 0; i < map->area; i++) {
        char current = map->map_arr[i];
        choose_attri(current);

        // Kinda proud of this line. Integer division results in row (aka line) pos, 
        // modulo results in column pos.
        // I cant see a faster way of implementing this (unlesss???).
        mvaddch(i / map->maxx, i % map->maxx, current);
    }
}

// This one is now less expandable than it should be 
// Checks if block is safe on player coords on map offsett by input x and y
int is_safe(player * p1, gm_map * map, int x , int y) {
    
    chtype res = mvinch(p1->pos.y - map->cam_pos.y + y, p1->pos.x - map->cam_pos.x + x) & A_CHARTEXT;
    if (res == GRASS_CHAR) {return 1;} // since grass is the block most stepped on, this should speed stuff up;
    char arr[] = MV_BLACK_L;
    for (int i = 0; i<MV_BLACK_S; i++) {
        if (res == arr[i]) {
            return 0;
        }
    }
    return 1;
}

// Moves the position of the camera to top left so player stays in middle
void reset_cam(player * px, gm_map * map) {

    // map->maxx / 2 would be amount to the left
    // Some of this code is a little cheeky and hacky. 
    // I'll have to find a solution to keeping both the player coords, and cam coords.
    map->cam_pos.x = px->pos.x - (map->maxx / 2);
    map->cam_pos.y = px->pos.y - (map->maxy / 2);
}


// Increases players position by speeds defined above
void handle_input(player * p1, gm_map * map, char thechar) {
        if (thechar == B_UP && is_safe(p1, map, 0, -1)) {
            p1->pos.y -= PY_SPEED;
        }
        else if (thechar == B_DOWN && is_safe(p1, map, 0, 1)) {
            p1->pos.y += PY_SPEED;
        }
        else if (thechar == B_LEFT && is_safe(p1, map, -1, 0)) {
            p1->pos.x -= PX_SPEED;
        }
        else if (thechar == B_RIGHT && is_safe(p1, map, 1, 0)){
            p1->pos.x += PX_SPEED;
        }
        else if (thechar == B_CAM) {
            reset_cam(p1, map);
        }
}

void check_auto_scroll(player * px, gm_map * map) {
    // Again, having this as a variable would be handy...
    int relative_x = px->pos.x - map->cam_pos.x;
    int relative_y = px->pos.y - map->cam_pos.y;
    
    // Doing this calculation every move is wastefull..
    // TODO store 90% and 10% screen coords as ariables
    if (relative_x < map->maxx/10 || relative_x > map->maxx*9 / 10) {
        reset_cam(px,map);
    }
    else if (relative_y < map->maxy/10 || relative_y > map->maxy*9 / 10) {
        reset_cam(px, map);
    }
}

void printw_hud(gm_map * map, player * p1) {
    attron(COLOR_PAIR(HUD1_PAIR));
    mvprintw(0, 0, " X: %d | Y: %d ", p1->pos.x, p1->pos.y);
}



// MAIN
// TODO unclutter main
int main (int argc, char *argv[]) {
    printf("argc: %d, argv: %s\n", argc, *argv);
    getchar();

    // SET_UP
    
    //check_compat(); // checks if terminal supports color etc. Funilly enough, this crashes my program
    srandom(clock()); // Sets the randomness seed to the local computer time


    // Sets seed of the map to a random number between 0 and 32767. Use INT32_MAX for greater variation
    const int rng_seed = random() % INT16_MAX;
    set_seed(rng_seed);

    // Position on map.
    int start_pos_x = 10240;//random() % INT32_MAX / 256;
    int start_pos_y = 10240;//random() % INT32_MAX / 256;


    // START
    initscr();
    cbreak();
    //nodelay(stdscr,1);
    curs_set(0);
    noecho();
    color_init();

    gm_vec2 cam_pos;
    cam_pos.x = start_pos_x;
    cam_pos.y = start_pos_y;

    gm_map map;
    init_map(&map, &cam_pos);

    player p1;
    init_player(&p1, (getmaxx(stdscr)/2) + start_pos_x, (getmaxy(stdscr)/2) + start_pos_y );

    char thechar = ' ';
    while (thechar != 'q') {
        
        // Generates map and prints it
        gen_map(&map);
        printw_map(&map);

        printw_hud(&map, &p1);
        printw("| S: %d ", rng_seed); // if the seed is saved in map, printwhud can print this too

        // This needs to be functionized
        attron(COLOR_PAIR(PLAYER_PAIR));
        mvprintw(p1.pos.y - map.cam_pos.y, p1.pos.x - map.cam_pos.x,"P");

        thechar = getch();
        handle_input(&p1, &map, thechar);
        handle_input(&p1, &map, thechar);
        check_auto_scroll(&p1, &map);

        map.curr_add_pos = 0;
        refresh();

    }
    free(map.map_arr);
    endwin();
}

/* 
Comments and thoughts:

- Chunks? I believe chunk saving/loading (like minecraft) is a good way to store data about the world. 
The world gen will act as a starter template so seeds work like normal, but cutting down a tree
would also then be saved if you were to reload the same chunk. This would also mean you could
create, save and load multiple worlds to save your progress, though i have to find out if this fits
for the type of game i want this to be.

- Roguelike, survival or both? The most important part of the game is replayability. I want every
playthrough to feel unique so even I who knows the ins and outs of the entire game can be surprised.

- Turn baseed gameplay? Im thinking about what type of gameplay would fit this game best. Ncurses has
quite limited functionality when it comes to realtime gameplay. I see this as an opportunity, rather
than a limitation, where i could try to make a decent turn based game. I could see this being harder to
implement than i think though, so other ideas are still welcome.

- Biomes? Adding multiple biomes would be fun, but it would also lead to another layer of perlin noise.
I have no idea if that amount of perlin noise could be handled by a computer. A fix for this is a 
"render distance" (again, like minecraft) which calculates this in advance before the player reaches
that location.

- Layered gameplay? I think a cool idea, is like a small chance of a "hole" spawning in a mountain, 
where if you step on it, get transported into a cave level. Or a shipwreck in the sea, or a dungeon 
in a forest.

- Pure C or C++? Introducing objects into the mix would make life slightly easier if i were to add mobs
etc. The question is if i can really bother now that a decent part of the foundation is already in pure C.

*/




// Old functions

/*
// Used for waiting a certain amt of time
struct timespec dab;
dab.tv_nsec = 30000000; // time in nanoseconds. "nanosleep(&dab, NULL);" can be used to wait that long
dab.tv_sec = 0; // time in seconds
*/

/*
void print_line(int pos_x,int pos_y, int size_x, int i) {
    for (int j = 0; j < size_x; j++) {
        float next = get_perlin(j+pos_x,i+pos_y);
        if ( next >= 0.72) {
            if (random() % 100 > 1) {printf("\033[42m\033[32mM"); continue;}
            printf("\033[100m\033[90mO");
        }
        else if (next >= 0.66) {
            printf("\033[42m\033[32mM");
        }
        else if (next >= 0.6) {
            printf("\033[103m\033[93m;");
        }
        else {
            printf("\033[44m\033[34m~");
        }
    }
}

void print_map(int pos_x, int pos_y, int size_x, int size_y) {
    for (int i = 0; i < size_y; i++) {
        print_line(pos_x,pos_y,size_x,i);
        printf("\n");
    }
}
*/