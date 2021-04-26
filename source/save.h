#ifndef SAVE_H
#define SAVE_H

#include "types.h"

#include "level.h"
#include "entity.h"


typedef struct {
    char test[9];
    ent_t entities[30];
    uint ent_size;
    tile_type_t map[LEVEL_SIZE];
    uint layer;
    uint scx, scy;
    uint checksum; // must be last in struct
} save_t;

uint sve_get_id();

void sve_read_data(u8 *data, uint size, uint sector);
void sve_write_data(void *data, uint size, uint sector);
void sve_write_byte(u8 byte, u16 address);
u8 sve_read_byte(u16 address);
void sve_erase_sector(uint sector);

void sve_save_level(level_t *level);

bool sve_is_entity_essential(const ent_t *e);
uint sve_compute_checksum(const save_t *save);
bool sve_validate_checksum(const save_t *save);

void sve_load_save(save_t *save, level_t *lvl);
void sve_load_from_persistant(level_t *lvl);
void sve_write_to_persistant(const save_t *sav);

#endif