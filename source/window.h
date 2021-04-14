#ifndef WINDOW_H
#define WINDOW_H

#include "defines.h"

void win_init(WIN_REGULAR *win, BG_REGULAR *bg, bool num);
void win_move(WIN_REGULAR *win, u16 x, u16 y, u8 w, u8 h);


#endif