#include "hotbar.h"
#include "window.h"


#include "bg.h"

void bar_draw_stamina(ent_t *player)
{
    for(uint i = 0; i < player->player.max_stamina >> 1; i++)
    {
        bg_write_tile(
            win_get_0()->background, 20 + i, 1,
            (i < (player->player.stamina >> 1)) ? 515 : 516
        );
    }
}


void bar_draw_health(ent_t *player)
{
    for(uint i = 0; i < player->player.max_health >> 1; i++)
    {
        bg_write_tile(win_get_0()->background, 20 + i, 0,
            (i < (player->player.health >> 1)) ? 513 : 514
        );
    }
}
