#include "save.h"
#include "item.h"

#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "text.h"
#include "lib/DMA.h"

/**
 * Checks to see if the entity is one that should be saved or not
 */
bool sve_is_entity_essential(const ent_t *e)
{
    const uint type = e->type;

    if(type == ENT_TYPE_PLAYER || type == ENT_TYPE_FURNITURE)
        return true;

    return false;
}


/**
 * Computes the checksum for the data in `save`
 * @param save save_t filled with savedata
 */
uint sve_compute_checksum(const save_t *save)
{
    u8 *ptr = (u8*)save->lvls;
    uint checksum = 0;

    for(uint i = 0; i < sizeof(save_level_t) * 2; i++)
        checksum += *ptr++;

    return checksum;
}


/**
 * @returns true if the checksum matches the checksum in `save`
 */
bool sve_validate_checksum(const save_t *save)
{
    return save->checksum == sve_compute_checksum(save);
}


/**
 * @param lvl can be NULL
 * @param layer layer of the level
 */
void sve_save_level(save_t *save, level_t *lvl, uint layer)
{
    save_level_t *saveLvl = &save->lvls[layer];
    uint size = 0;
    
    if(lvl == NULL)
    {
        saveLvl->is_generated = false;
        return;
    }

    saveLvl->is_generated = true;
    
    for(uint i = 0; i < lvl->ent_size; i++)
    {
        if(sve_is_entity_essential(&lvl->entities[i])) {
            saveLvl->entities[size++] = lvl->entities[i];
        }
    }
    
    saveLvl->ent_size = size;
    memcpy(saveLvl->map, lvl->map, sizeof(lvl->map));
}


/**
 * Saves a level for powering off
 */
void sve_save_game(level_t **world)
{
    save_t *save = calloc(sizeof(save_t), sizeof(uint8_t));

    memcpy(save->name, "TSTWORLD", 8);

    for(uint i = 0; i < 3; i++)
        sve_save_level(save, world[i], i);

    save->layer = lvl_get_current()->layer;
    save->scx = bg_get_scx(main_background);
    save->scy = bg_get_scy(main_background);

    save->checksum = sve_compute_checksum(save);
    sve_write_to_persistant(save);

    free(save);
}


#define REG_EEPROM_CMD1 *((vu8 *)0x0E005555)
#define REG_EEPROM_CMD2 *((vu8 *)0x0E002AAA)


inline void sve_send_command(u8 cmd)
{
    REG_EEPROM_CMD1 = 0xAA;
    REG_EEPROM_CMD2 = 0x55;
    REG_EEPROM_CMD1 = cmd;
}


/**
 * Sets bank number for EEPROMs larger than 64KBytes
 * @param bank [0-1]
 */
void sve_set_bank(bool bank)
{
    sve_send_command(0xB0);
    *(vu8*)(0x0E000000) = bank;
}


void sve_write_byte(u8 byte, u16 address)
{
    sve_send_command(0xa0);
    vu8 *write_address = (vu8*)(0x0E000000 | address);
    *write_address = byte;
    while(*write_address != byte);
}


inline u8 sve_read_byte(u16 address) {
    return *(vu8*)(0x0E000000 | address);
}


/**
 * @returns the ID number of the flash chip on board
 */
uint sve_get_id()
{
    sve_send_command(0x90);

    uint id = (sve_read_byte(1) << 8) | sve_read_byte(0);

    sve_send_command(0xF0);
    return id;
}


/**
 * @param data u8 data pointer
 * @param size size of data block in bytes
 * @param sector starting sector to write data. [0-F]
 */
void sve_write_data(void *data, uint size, uint sector)
{
    sve_erase_sector(sector);
    sve_set_bank(0);

    if(size + (sector << 12) > 0xFFFF) {
        text_error("DATA TOO LARGE FOR STORAGE");
        size = 0xFFFF;
    }

    for(uint i = 0; i < size; i++)
    {
        sve_write_byte(*(vu8*)data++, i);
        // check to see if we need to switch banks
        if((i & 0xFFF) == 0xFFF)
            sve_erase_sector(++sector);
    }
}


/**
 * @param data u8 buffer to fill
 * @param size size of data in bytes
 * @param sector starting sector to read data. [0-F]
 */
void sve_read_data(u8 *data, uint size, uint sector)
{
    uint addr = sector << 12;
    sve_set_bank(0);

    if((size + addr) > 0xFFFF)
    {
        text_error("SIZE TOO LARGE TO READ");
        size = 0xffff;
    }
    
    for(uint i = 0; i < size; i++)
    {
        *data++ = sve_read_byte(addr++);
    }
}


/**
 * Erases a 4KByte sector. Necessary before writing
 * @param sector [0x0-0xF]
 * @note incompatible with ATMEL devices
 */
void sve_erase_sector(uint sector)
{
    sector &= 0xF;
    sve_send_command(0x80);

    REG_EEPROM_CMD1 = 0xAA;
    REG_EEPROM_CMD2 = 0x55;
    vu8 *sector_address = (vu8*)(0x0E000000 | (sector << 12));
    *sector_address = 0x30;

    // while(*sector_address != 0xff);
}

#define REG_WAITCNT *((vu16*)0x4000204)


void sve_write_to_persistant(const save_t *sav)
{
    text_print("DO NOT REMOVE GAMEPAK.", 0, 0);
    REG_WAITCNT = 0x4317; // set waitstates based on GBAtek

    sve_write_data((u8*)sav, sizeof(save_t), 0);

    text_print("IT IS NOW SAFE TO POWER OFF.", 0, 0);
}


void sve_load_level(save_t *save, level_t **world, uint layer)
{
    level_t *lvl = world[layer];
    save_level_t *saveLvl = &save->lvls[layer];

    if(!saveLvl->is_generated) {
        world[layer] = NULL;
        return;
    }

    if(!lvl) {
        world[layer] = lvl = lvl_new(layer, NULL);
    }

    memcpy(lvl->map, saveLvl->map, sizeof(lvl->map));

    for(uint i = 0; i < saveLvl->ent_size; i++) {
        ent_t *e = &lvl->entities[i];
        *e = saveLvl->entities[i];
        e->sprite = NULL;
        ent_load_events(e);

        if(e->type == ENT_TYPE_PLAYER)
            lvl->player = e;

        // must reorganize all pointers for items
        if(e->type == ENT_TYPE_PLAYER || e->type == ENT_TYPE_FURNITURE)
        {
            inventory_t *inv = e->type == ENT_TYPE_PLAYER ? &e->player.inventory : &e->furniture.inventory;
            item_t *item = inv->items;

            inv->parent = e;
            
            for(uint j = 0; j < inv->size; j++)
            {
                item->parent = inv;
                item->event = item_get_from_type(item->type, item->tooltype)->event;
                item->name = item_lookup_name(&inv->items[j]);
                item++;
            }
        }

    }
    
    lvl->ent_size = saveLvl->ent_size;
    lvl->layer = layer;
}


/**
 * Call from outside
 * @param lvl level_t to fill
 * @returns level that is currently active in savefile, NULL if failed
 */
level_t *sve_load_from_persistant(level_t **world)
{
    save_t *save = malloc(sizeof(save_t));
    uint layer = save->layer;

    REG_WAITCNT = 0x4317; // set waitstates based on GBAtek

    sve_read_data((u8*)save, sizeof(save_t), 0);

    if(!sve_validate_checksum(save)) {
        text_print("CANNOT LOAD", 0, 1);
        text_error("CHECKSUM FAILED");
        return NULL;
    }

    text_print("LOADING WORLD", 0, 1);
    text_print(save->name, 14, 1);

    for(uint i = 0; i < 3; i++)
    {
        sve_load_level(save, world, i);

        if(i && world[i])
            world[i]->parent = world[i-1]->parent;
    }

    bg_move(main_background, save->scx, save->scy);

    free(save);

    return world[layer];
}