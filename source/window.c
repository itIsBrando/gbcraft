#include <gba_video.h>

#include "window.h"
#include "bg.h"


WIN_REGULAR *_win_0; // pointer to first window


/**
 * @param win window object to fill
 * @param bg must outlive `win`
 * @param num enables either win0 or win1
 */
void win_init(WIN_REGULAR *win, BG_REGULAR *bg, bool num)
{
    _win_0 = win;
    win->background = bg;
    win->window_number = num;
    REG_DISPCNT &= 0x1FFF;
    REG_DISPCNT |= 1 << (0xD + num); // enable either win0 or win1

    REG_WINOUT = BIT(4) | BIT(
        2/* map number to include outside window*/
    );
    // enable background and sprites
    REG_WININ = BIT(bg->map_number) | BIT(4);
}


/**
 * Moves the window to (x, y) and sets its width and height too
 * @param x pixel x coordinate [0, 240)
 * @param y pixel y coordinate [0, 160)
 */
void win_move(WIN_REGULAR *win, uint x, uint y, uint w, uint h)
{
    REG_WIN0H = (u16)(x << 8) | (x + w);
    REG_WIN0V = (u16)(y << 8) | (y + h);
    bg_move(win->background, -x, -y);
    win->x = x;
    win->y = y;
    win->w = w;
    win->h = h;
}


/**
 * Moves the window in a direction
 */
void win_move_by(WIN_REGULAR *win, direction_t direction)
{
    win_move(win,
        win->x + dir_get_x(direction),
        win->y + dir_get_y(direction),
        win->w, win->h
    );
}


inline void win_set_size(WIN_REGULAR *win, uint w, uint h)
{
    win_move(win, win->x, win->y, w, h);
}



/**
 * Gets a pointer to the structure that refers to window 0.
 */
inline WIN_REGULAR *win_get_0()
{
    return _win_0;
}