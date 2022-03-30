#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <math.h>
#include <time.h>
//#include "perlin_ctrlc_ctrlv.c"


typedef struct {
    int maxx;
    int maxy;
    char * array;
} rain_arr;

void init_rain_arr(rain_arr * arr) {
    arr->maxy = getmaxy(stdscr);
    arr->maxx = getmaxx(stdscr);

    char * gen_arr = malloc((arr->maxx * arr->maxy) * sizeof(char));
    arr->array = gen_arr;
}

void mov_arr_down(rain_arr * arr) {
    
    for (int i = 0; i < arr->maxy; i++) {
        
    }
}