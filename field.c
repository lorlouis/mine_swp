#include <stdlib.h>
#include <time.h>
#include "field.h"

/* Returns
 *  0 on error
 *      or
 *  A pointer to the tile at postition x, y */
struct tile_s* field_get_tile(struct field_s* field, int x, int y) {
    if(x < 0 || x > field->width ) return 0;
    if(y < 0 || y > field->height) return 0;
    return field->tiles + (field->width * y) + x;
}

/* Returns:
 *  -1 on error
 *      or
 *  The number of bombs around x,y */
int field_count_surronding_bombs(struct field_s* field, int x, int y) {
    if(x < 0 || x > field->width) return -1;
    if(y < 0 || y > field->height) return -1;

    int nb_bombs = 0;

    /* counting the number of bombs
     * starting in the top left and going down */
    for(int r_y = -1; r_y < 2; r_y++) {
        for(int r_x = -1; r_x < 2; r_x++) {
            /* do not count bombs before or after a line wrap */
            if(x+r_x >= field->width) continue;
            if(x+r_x < 0) continue;
            if(y+r_y >= field->height) continue;
            if(y+r_y < 0) continue;
            /* skip center tile */
            if(r_x == 0 && r_y == 0) continue;
            struct tile_s* tmp_tile = field_get_tile(field, x+r_x, y+r_y);
                /* skip tiles outside of the field */
                if(tmp_tile == 0) continue;
            if(tmp_tile->has_bomb) nb_bombs++;
        }
    }
    return nb_bombs;
}


/* Seeds rand with the current time the first time it is called
 * places the bombs and returns error if nb_bombs > number of tiles
 *
 * Return
 *  -1 on error
 *      or
 *  0 on success
 *
 * WARNING
 * Might take a while to place bombs if the number of bombs is
 * close to the number of tiles because the placement is entirely random
 *  */
int field_init(struct field_s* field) {
    static int rand_seeded = 0;
    if(!rand_seeded) {
        srand(time(0));
    }
    if(field->nb_bombs > field->width * field->height) return -1;

    /* TODO fix to lower to O(n) time in worst case senario */
    /* place the bombs */
    int nb_bombs = field->nb_bombs;
    int rand_x, rand_y;
    struct tile_s *tmp_tile;
    while(nb_bombs) {
        rand_x = rand() % field->width;
        rand_y = rand() % field->height;
        tmp_tile = field_get_tile(field, rand_x, rand_y);
        if(tmp_tile && !tmp_tile->has_bomb) {
            tmp_tile->has_bomb = 1;
            nb_bombs--;
            field->bombs[nb_bombs] = tmp_tile;
        }
    }

    /* update the nb_bombs of each tile in the field */
    for(int i = 0; i < field->width * field->height; i++) {
        tmp_tile = field_get_tile(field, i % field->width, i / field->width);
        if(!tmp_tile) return -1;
        tmp_tile->nb_mines = field_count_surronding_bombs(
                field, i % field->width, i / field->width);
    }
    return 0;
}

/* Mallocs a field struct but it does not call field_init */
struct field_s* field_new(int width, int height, int nb_bombs) {
    struct field_s *new_field = malloc(sizeof(struct field_s));

    if(new_field == 0) return 0;

    new_field->tiles = calloc(width * height, sizeof(struct tile_s));

    if(new_field->tiles == 0) return 0;

    new_field->bombs = malloc(sizeof(struct tile_s*) * nb_bombs);

    if(new_field->tiles == 0) return 0;

    new_field->height = height;
    new_field->width = width;
    new_field->nb_bombs = nb_bombs;
    return new_field;
}

/* Frees a field created by field_new, otherwise crashed the program */
void field_free(struct field_s* field) {
    free(field->bombs);
    free(field->tiles);
    free(field);
}

/* Recursively opens a tile and its surrounding tiles */
int field_open_tile(struct field_s* field, int x, int y) {
    struct tile_s *tmp_tile;
    tmp_tile = field_get_tile(field, x, y);
    if(!tmp_tile) return -1;
    if(!tmp_tile->opened) {
        tmp_tile->opened = 1;
        if(!tmp_tile->has_bomb && !tmp_tile->nb_mines) {
            /* opening the tiles
             * starting in the top left and going down */
            for(int r_y = -1; r_y < 2; r_y++) {
                for(int r_x = -1; r_x < 2; r_x++) {
                    /* do not count bombs before or after a line wrap */
                    if(x+r_x >= field->width) continue;
                    if(x+r_x < 0) continue;
                    if(y+r_y >= field->height) continue;
                    if(y+r_y < 0) continue;
                    /* skip center tile */
                    if(r_x == 0 && r_y == 0) continue;
                    field_open_tile(field, x+r_x, y+r_y);
                }
            }
        }
    }
    return 0;
}


/* Returns
 *  -1 on game lost
 *   0 on game continuing
 *   1 on game won
 *
 * WARNING
 * takes more time the closer the player is to victory */
int field_check_game_state(struct field_s* field) {
    /* if one of the bombs is opened then the game is lost */
    for(int i = 0; i < field->nb_bombs; i++) {
        if(field->bombs[i]->opened) {
            return -1;
        }
    }
    /* check if the game is won
     * (takes more time the closer the player is to victory) */
    for(int i =0; i < field->height * field->width; i++) {
        if(!field->tiles[i].opened && !field->tiles[i].has_bomb) return 0;
    }
    return 1;
}

/* returns the state on the flag once toggled */
int field_toggle_flag_tile(struct field_s* field, int x, int y) {
    return field_get_tile(field, x, y)->flaged ^= 1;
}
