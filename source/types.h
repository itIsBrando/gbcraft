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
    ENT_TYPE_ZOMBIE,
    ENT_TYPE_SKELETON,
} ent_type_t;


typedef enum {
    ITEM_TYPE_WOOD,
    ITEM_TYPE_STONE,
    ITEM_TYPE_SAPLING,
    ITEM_TYPE_CRAFTING_TABLE,
} item_type_t;


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
    SURROUNDING_LEFT = 1 << 7,
    SURROUNDING_RIGHT = 1 << 6,
    SURROUNDING_UP = 1 << 5,
    SURROUNDING_DOWN = 1 << 4,
    SURROUNDING_LEFT_UP = 1 << 3, 
    SURROUNDING_RIGHT_UP = 1 << 2,
    SURROUNDING_LEFT_DOWN = 1 << 1,
    SURROUNDING_RIGHT_DOWN = 1 << 0,
} tile_surround_mask;

typedef enum {
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT_UP,
    DIRECTION_RIGHT_UP,
    DIRECTION_LEFT_DOWN,
    DIRECTION_RIGHT_DOWN,
} direction_t;

typedef struct ent_t ent_t;
typedef struct item_t item_t;
typedef struct tile_t tile_t;
typedef struct inventory_t inventory_t;
typedef struct level_t level_t;


typedef struct {
    void (*onhurt)(struct ent_t *, struct ent_t *);      // called when damage is received
    void (*doDamage)(struct ent_t *, struct ent_t *);    // called when damage is done
    void (*ontouch)(struct ent_t *, struct ent_t *);     // called when entity collides with this
    void (*ondeath)(struct ent_t *);               // called when this dies
    void (*onupdate)(struct ent_t *);              // called every frame
} ent_event_t;


typedef struct {
    bool (*interact)(
        item_t *,   // reference to this item
        ent_t *,    // player entity
        const tile_t *,  // pointer to the tile in the map where the item that will be placed on
        u16 x,      // absolute tile x of the tile 
        u16 y      // absolute tile y of the tile
    ); // called when a player presses `A` with this item in their inventory
    bool (*canattack)(item_t *, ent_t *);
} item_event_t;


typedef struct item_t {
    u16 tile; // tile number of the inventory representation
    item_type_t type;
    s8 count;
    item_event_t *event;
    inventory_t *parent;
    const char *name;
} item_t;


typedef struct inventory_t {
    u16 size;
    item_t items[30];
    ent_t *parent;
} inventory_t;


/**
 * Used for collision
 */
typedef struct {
    u16 sx, ex; // start and end X
    u16 sy, ey; // start and end Y
} bounding_rect_t;


typedef struct {
    s8 health;
    s8 max_health;
    item_t *activeItem;
    inventory_t inventory;
    u8 invulnerableTime;
} player_t;

typedef struct {
    s8 health;
} zombie_t;


typedef struct ent_t {
    u16 x, y;          // absolute X and Y coordinates of map
    s8 xKnockback, yKnockback;
    ent_type_t type;    // refers to the type of entity this is
    union {
        player_t player;
        zombie_t zombie;
    };
    obj_t *sprite;       // refers to OAM sprite representing this entity
    level_t *level;
    direction_t dir;
    
    ent_event_t *events;
} ent_t;


typedef struct tile_t {
    tile_type_t type;
    union {
        u16 center;   // for 9pt indexing.
        u16 topRight; // for top-bottom indexing, single_8x8 & single_16x16
    } tiling;
    tile_indexing_mode_t indexing;
    void(*onhurt)(ent_t *);
    void(*ontouch)(ent_t *);
    bool(*maypass)(ent_t *);
    void (*interact)(ent_t *, item_t *item, u16 x, u16 y);
} tile_t;


#define LEVEL_WIDTH 128
#define LEVEL_HEIGHT 128
#define LEVEL_SIZE (LEVEL_HEIGHT * LEVEL_WIDTH)


typedef struct level_t {
    u16 ent_size;  // number of entities in the `entities` table
    ent_t entities[50]; // @todo allocate on heap
    tile_type_t map[LEVEL_SIZE];
    u16 layer;
    struct level_t *parent;
} level_t;



extern BG_REGULAR *main_background;

#endif