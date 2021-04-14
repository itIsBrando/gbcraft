#include <gba_video.h>

#include "window.h"
#include "bg.h"


/**
 * @param win window object to fill
 * @param bg must outlive `win`
 * @param num enables either win0 or win1
 */
void win_init(WIN_REGULAR *win, BG_REGULAR *bg, bool num)
{
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
 */
void win_move(WIN_REGULAR *win, u16 x, u16 y, u8 w, u8 h)
{
    REG_WIN0H = (u16)(x << 8) | (x + w);
    REG_WIN0V = (u16)(y << 8) | (y + h);
    bg_move(win->background, x, y - 16);
    win->x = x;
    win->y = y;
    win->w = w;
    win->h = h;
}