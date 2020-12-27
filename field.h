#ifndef FIELD_H
#define FIELD_H 1

/* packed to fit within an 8 bit unsighed char */
struct tile_s {
    unsigned int nb_mines: 4;
    unsigned int flaged :1;
    unsigned int opened :1;
    unsigned int has_bomb :1;
};


struct field_s {
    int width;
    int height;
    int nb_bombs;
    struct tile_s* tiles;
    struct tile_s** bombs;
};

/* Returns
 *  0 on error
 *      or
 *  A pointer to the tile at postition x, y */
struct tile_s* field_get_tile(struct field_s* field, int x, int y);

/* Returns:
 *  -1 on error
 *      or
 *  The number of bombs around x,y */
int field_count_surronding_bombs(struct field_s* field, int x, int y);

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
int field_init(struct field_s* field);

/* Mallocs a field struct but it does not call field_init */
struct field_s* field_new(int width, int height, int nb_bombs);

/* Frees a field created by field_new, otherwise crashed the program */
void field_free(struct field_s* field);

/* Recursively opens a tile and its surrounding tiles */
int field_open_tile(struct field_s* field, int x, int y);

/* Returns
 *  -1 on game lost
 *   0 on game continuing
 *   1 on game won
 *
 * WARNING
 * takes more time the closer the player is to victory */
int field_check_game_state(struct field_s* field);

/* returns the state on the flag once toggled */
int field_toggle_flag_tile(struct field_s* field, int x, int y);

#endif
