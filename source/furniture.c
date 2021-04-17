#include "entity.h"

#include "text.h"
#include "obj.h"
#include "memory.h"


void ent_furniture_update(ent_t *e)
{

    ent_draw(e);
}



void ent_furniture_onhurt(ent_t *f, ent_t *plr)
{
    if(plr->type != ENT_TYPE_PLAYER)
        return;


    text_error("INTERACTED WITH BENCH");
}