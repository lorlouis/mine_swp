#include <stdio.h>

#include "interface.h"
#include "field.h"

void field_draw(struct field_s* field, int cursor_x, int cursor_y) {
    printf("  ");
    for(int i = 0; i < field->width; i++) {
        printf("%2x", i);
    }
    printf("\n");
    for(int i = 0; i < field->width * field->height; i++) {
        if(i % field->width == 0) {
            printf("%2x", (i/field->width));
        }
        if(i % field->width == cursor_x && i / field->width == cursor_y) {
            printf("\e[7m\e[5m");
        }
        if(field->tiles[i].flaged) {
            printf(FLAG_STR);
        }
        else if(!field->tiles[i].opened) {
            printf(CLOSED_STR);
        }
        else if(field->tiles[i].has_bomb) {
            printf(BOMB_STR);
        }
        else if(field->tiles[i].nb_mines != 0) {
            printf("%2d", field->tiles[i].nb_mines);
        }
        else{
            printf(OPENED_STR);
        }
        if(i % field->width == cursor_x && i / field->width == cursor_y) {
            printf("\e[0m");
        }
        if(i != 0 && (i+1) % field->width == 0) {
            printf("\n");
        }
    }
}

void field_reveal_bombs(struct field_s* field) {
    for(int i = 0; i < field->width * field->height; i++) {
        field->tiles[i].opened = 1;
    }
    field_draw(field, -1, -1);
}
