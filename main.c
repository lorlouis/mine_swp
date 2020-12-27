#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <unistd.h>

#include "field.h"
#include "interface.h"

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif

#ifdef OS_WIN

#include <conio.h>

#else
#include <termios.h>
/* Shamelessly stolen from
 * https://www.includehelp.com/c-programs/gotoxy-clrscr-getch-getche-for-gcc-linux.aspx
 * */
static struct termios old, new;
void initTermios(int echo) {
  tcgetattr(0, &old); //grab old terminal i/o settings
  new = old; //make new settings same as old settings
  new.c_lflag &= ~ICANON; //disable buffered i/o
  new.c_lflag &= echo ? ECHO : ~ECHO; //set echo mode
  tcsetattr(0, TCSANOW, &new); //apply terminal io settings
}

/* Restore old terminal i/o settings */
void resetTermios(void) {
  tcsetattr(0, TCSANOW, &old);
}

char getch() {
  char ch;
  initTermios(0);
  ch = getchar();
  resetTermios();
  return ch;
}
#endif


int game_loop(struct field_s* field) {
    int cursor_x = 0;
    int cursor_y = 0;
    field_init(field);

    int c;
    int game_state;
    /* clearing the screen */
    fputs("\033[2J\033[1;1H", stdout);
    rewind(stdout);
    ftruncate(1,0);

    field_draw(field, cursor_x, cursor_y);
    puts("hjkl to move\nspace to toggle flags\nenter to open a tile");

    while((game_state = field_check_game_state(field)) == 0
            && (c = getch()) != 'q') {
        switch(c) {
            case 'h':
                if(cursor_x > 0) {
                    cursor_x--;
                }
                break;
            case 'l':
                if(cursor_x < field->width-1) {
                    cursor_x++;
                }
                break;
            case 'k':
                if(cursor_y > 0) {
                    cursor_y--;
                }
                break;
            case 'j':
                if(cursor_y < field->height-1) {
                    cursor_y++;
                }
                break;
            case 'o':
            case '\n':
                field_open_tile(field, cursor_x, cursor_y);
                break;
            case 'f':
            case 'p':
            case ' ':
                field_toggle_flag_tile(field, cursor_x, cursor_y);
                break;
        }
        /* clearing the screen */
        fputs("\033[2J\033[1;1H", stdout);
        rewind(stdout);
        ftruncate(1,0);

        field_draw(field, cursor_x, cursor_y);
        puts("hjkl to move\nspace to toggle flags\nenter to open a tile");
    }
    return game_state;
}


int is_number(char* str) {
    while(*str != '\0') {
        if(!isdigit((int)*str)) return 0;
        str++;
    }
    return 1;
}

int main(int argc, char **argv) {
    struct field_s field = {
        .width = 16,
        .height = 16,
        .nb_bombs = 32,
    };

    if(argc == 7) {
        for(int i = 1; i < argc; i++) {
            if(!strcmp(argv[i], "--width")) {
               if(i+1 > argc) {
                   fputs("--width expects a value\n", stderr);
                   return -1;
               }
               if(!is_number(argv[i+1])) {
                   fputs("--width expects a number\n", stderr);
                   return -1;
               }
               field.width = atoi(argv[i+1]);
               i++;
            }
            else if(!strcmp(argv[i], "--height")) {
               if(i+1 >= argc) {
                   fputs("--height expects a value\n", stderr);
                   return -1;
               }
               if(!is_number(argv[i+1])) {
                   fputs("--height expects a number\n", stderr);
                   return -1;
               }
               field.height = atoi(argv[i+1]);
               i++;
            }
            else if(!strcmp(argv[i], "--bombs")) {
               if(i+1 >= argc) {
                   fputs("--bombs expects a value\n", stderr);
                   return -1;
               }
               if(!is_number(argv[i+1])) {
                   fputs("--bombs expects a number\n", stderr);
                   return -1;
               }
               field.nb_bombs = atoi(argv[i+1]);
               i++;
            }
        }

        if(field.nb_bombs > field.height * field.width) {
            fputs("--bombs expects a number smalled than width x height\n", stderr);
            return -1;
        }
    }
    else if(argc != 1) {
        printf(
            "Usage: %s [--width <number> --height <number> --bombs <number>]\n",
            argv[0]);
        return -1;
    }

    field.tiles = calloc(field.width*field.height, sizeof(struct tile_s));
    field.bombs = malloc(sizeof(struct tile_s*) * field.nb_bombs);
    int game_state = game_loop(&field);
    if(game_state == -1) {
        fputs("\033[2J\033[1;1H", stdout);
        rewind(stdout);
        ftruncate(1,0);

        field_reveal_bombs(&field);
        puts("You lost");
    }
    else if(game_state == 1) {
        puts("You won");
    }
    else {
        puts("You quit the game");
    }
    free(field.tiles);
    free(field.bombs);
    return 0;
}
