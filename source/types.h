#ifndef TYPES_H
#define TYPES_H

#include "defines.h"
#include "obj.h"

typedef enum
{
    TILE_GRASS,
    TILE_WATER,
    TILE_WOOD,
    TILE_STONE,
    TILE_TREE,

    TILE_NONE, // last tile
} tile_type_t; // order irrelevant


// order relevant
typedef enum {
    ENT_TYPE_PLAYER=0,
    ENT_TYPE_SLIME,
    ENT_TYPE_ZOMBIE,
    ENT_TYPE_FURNITURE,
} ent_type_t;


typedef enum {
    ITEM_TYPE_WOOD,
    ITEM_TYPE_STONE,
    ITEM_TYPE_SAPLING,
    ITEM_TYPE_TOOL,
    ITEM_TYPE_FURNITURE,
} item_type_t; // order relevant?


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



typedef struct tile_event_t {
    void(*onhurt)(ent_t *);
    void(*ontouch)(ent_t *);
    bool(*maypass)(ent_t *);
    void (*interact)(ent_t *, item_t *item, u16 x, u16 y);
} tile_event_t;


/**
 * TOOL_TYPE_...
 * - used in `item_t`
 */
typedef enum {
    TOOL_TYPE_AXE,
    TOOL_TYPE_PICKAXE,
    TOOL_TYPE_SWORD
} tooltype_t;

typedef struct item_t {
    u16 tile; // tile number of the inventory representation
    item_type_t type;
    tooltype_t tooltype;
    union {
        u8 level;   // used for tools
        s8 count;   // used for all other resources
    };
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
    s8 stamina;
    s8 max_stamina;
    item_t *activeItem;
    inventory_t inventory;
    u8 invulnerability;
    u8 stamina_time;
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
        u8 center;   // for 9pt indexing.
        u8 topRight; // for top-bottom indexing, single_8x8 & single_16x16
    } tiling;
    u8 connect_to;  // Another tile that connects visually (9pt indexing only)
    tile_indexing_mode_t indexing;
    const tile_event_t *event;
} tile_t;


#define LEVEL_WIDTH 128
#define LEVEL_HEIGHT 128
#define LEVEL_SIZE (LEVEL_HEIGHT * LEVEL_WIDTH)


typedef struct level_t {
    u16 ent_size;  // number of entities in the `entities` table
    ent_t entities[50]; // @todo allocate on heap
    u16 size; // must be a power of two. Rn it's only value is 64
    tile_type_t map[LEVEL_SIZE]; // needs to be adapted to level.size @todo
    u8 data[LEVEL_SIZE]; // holds tile damage @todo
    u16 layer;
    struct level_t *parent;
} level_t;



extern BG_REGULAR *main_background;

#endif