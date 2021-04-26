#ifndef WINDOW_H
#define WINDOW_H

#include "types.h"

void win_init(WIN_REGULAR *win, BG_REGULAR *bg, bool num);
void win_move(WIN_REGULAR *win, uint x, uint y, uint w, uint h);
void win_move_by(WIN_REGULAR *win, direction_t direction);
void win_set_size(WIN_REGULAR *win, uint w, uint h);

WIN_REGULAR *win_get_0();

#endif