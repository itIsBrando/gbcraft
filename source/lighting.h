#ifndef LIGHTING_H
#define LIGHTING_H

#include "types.h"

void lt_init();
void lt_show(level_t *lvl);
void lt_hide();

void lt_create_source(uint tx, uint ty);

#endif