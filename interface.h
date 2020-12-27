#ifndef INTERFACE_H
#define INTERFACE_H 1

#include <stdio.h>

#include "field.h"

#define CLOSED_STR "▓▓"
#define OPENED_STR "░░"
#define FLAG_STR "▐▀"
#define BOMB_STR "@@"

void field_draw(struct field_s* field, int cursor_x, int cursor_y);

void field_reveal_bombs(struct field_s* field);
#endif
