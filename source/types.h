#ifndef TYPES_H
#define TYPES_H

#include "defines.h"
#include "obj.h"

typedef enum
{
    TILE_GRASS,
    TILE_WATER,
    TILE_WOOD,
    TILE_ROCK,
    TILE_TREE,
} tile_type_t;


typedef enum {
    ENT_TYPE_PLAYER=0,
    ENT_TYPE_SLIME,
    ENT_TYPE_SKELETON,
    ENT_TYPE_ZOMBIE,
} ent_type_t;


typedef enum {
    /**
     * Uses 9 unique tiles for this object
     * where: g=grass, s=stone
     * g s g
     * s s s
     * g s g
     * each stone has a unique tile number
     */
    TILE_INDEXING_9PT=0,
    /**
     * Uses two unique 16x16 tiles for this object
     * where: g=grass, w=wood
     * w1 w3
     * w2 g
     * g  g
     * Each tile is 16x16
     * w1= tile from topRight, topRight+1, topRight, topRight+1
     * w2= tile from topRight, topRight+1, topRight+nextRow,topRight+nextRow+1
     * w3 = w2
     */
    TILE_INDEXING_TOP_BOT,
    /**
     * All tiles look the same. Uses one, 8x8px tile that covers 16x16px
     */
    TILE_INDEXING_SINGLE_8x8,
    /**
     * All tiles look the same. Uses four GB, 8x8 tiles to create a 16x16 tile
     */
    TILE_INDEXING_SINGLE_16x16,
} tile_indexing_mode_t;


typedef enum {
    DIRECTION_LEFT = 1 << 7,
    DIRECTION_RIGHT = 1 << 6,
    DIRECTION_UP = 1 << 5,
    DIRECTION_DOWN = 1 << 4,
    DIRECTION_LEFT_UP = 1 << 3, 
    DIRECTION_RIGHT_UP = 1 << 2,
    DIRECTION_LEFT_DOWN = 1 << 1,
    DIRECTION_RIGHT_DOWN = 1 << 0,
} tile_surround_mask;

typedef tile_surround_mask direction_t;

typedef struct {
    void (*onhurt)(struct ent_t *, struct ent_t *);      // called when damage is received
    void (*doDamage)(struct ent_t *, struct ent_t *);    // called when damage is done
    void (*ontouch)(struct ent_t *, struct ent_t *);     // called when entity collides with this
    void (*ondeath)(struct ent_t *);               // called when this dies
    void (*onupdate)(struct ent_t *);              // called every frame
} ent_event_t;


typedef struct ent_t {
    uint x, y;          // absolute X and Y coordinates of map
    ent_type_t type;    // refers to the type of entity this is
    // union {
    //     player_t;
    // } entity;
    obj_t *sprite;       // refers to OAM sprite representing this entity
    struct level_t *level;
    
    ent_event_t events;
} ent_t;


typedef struct {
    tile_type_t type;
    union {
        u16 center;   // for 9pt indexing.
        u16 topRight; // for top-bottom indexing, single_8x8 & single_16x16
    } tiling;
    tile_indexing_mode_t indexing;
    void(*onhurt)(ent_t *);
    void(*ontouch)(ent_t *);
    bool(*maypass)(ent_t *);
} tile_t;


#define LEVEL_WIDTH 32
#define LEVEL_HEIGHT 32
#define LEVEL_SIZE (LEVEL_HEIGHT * LEVEL_WIDTH)


typedef struct {
    uint ent_size;  // number of entities in the `entities` table
    ent_t entities[50]; // @todo allocate on heap
    tile_type_t map[LEVEL_SIZE];
    uint layer;
    struct level_t *parent;
} level_t;



extern BG_REGULAR main_background;

#endif