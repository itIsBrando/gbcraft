#ifndef TYPES_H
#define TYPES_H

#include "defines.h"
#include "obj.h"

#define MAX_ENTITY_SIZE 20
#define INVENTORY_SIZE 20 // has to be 50????
#define MAX_INVENTORY_SIZE INVENTORY_SIZE /** @todo maintain constitent naming convention, remove INVENTORY_SIZE*/

enum {
    TILE_GRASS,
    TILE_WATER,
    TILE_WOOD,
    TILE_STONE,
    TILE_TREE,
    TILE_IRON,
    TILE_GOLD,
    TILE_STAIR_DOWN,
    TILE_MUD,
    TILE_STAIR_UP,
    TILE_DOOR_CLOSED,
    TILE_DOOR_OPEN,
    TILE_FLOOR_WOOD,
    TILE_SAPLING,
    TILE_SEEDED_MUD,
    TILE_WHEAT_MUD,

    TILE_NONE, // last tile
}; // order irrelevant

typedef uint8_t tile_type_t;

// order relevant
typedef enum {
    ENT_TYPE_PLAYER=0,
    ENT_TYPE_SLIME,
    ENT_TYPE_ZOMBIE,
    ENT_TYPE_FURNITURE,
    ENT_TYPE_ITEM_ENTITY,
} ent_type_t;


typedef enum {
    ITEM_TYPE_WOOD,
    ITEM_TYPE_STONE,
    ITEM_TYPE_SAPLING,
    ITEM_TYPE_TOOL,
    ITEM_TYPE_FURNITURE,
    ITEM_TYPE_COAL,
    ITEM_TYPE_IRON_ORE,
    ITEM_TYPE_GOLD_ORE,
    ITEM_TYPE_IRON,
    ITEM_TYPE_GOLD,
    ITEM_TYPE_GEM,
    ITEM_TYPE_DOOR,
    ITEM_TYPE_FLOOR,
    ITEM_TYPE_SEED,
    ITEM_TYPE_WHEAT,
    ITEM_TYPE_BREAD,
    ITEM_TYPE_APPLE,
    ITEM_TYPE_SLIME,
    ITEM_TYPE_GOLDEN_APPLE,
} item_type_t; // order relevant? (bk from future: "no")


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
    FURNITURE_TYPE_CRAFTING,
    FURNITURE_TYPE_CHEST,
    FURNITURE_TYPE_FURNACE,
    FURNITURE_TYPE_ANVIL,
    FURNITURE_TYPE_LANTERN,
} furniture_type_t;



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
    DIRECTION_NONE,
} direction_t;

typedef struct ent_t ent_t;
typedef struct item_t item_t;
typedef struct tile_t tile_t;
typedef struct inventory_t inventory_t;
typedef struct level_t level_t;
typedef struct furniture_t furniture_t;


typedef struct {
    bool (*onhurt)(ent_t *, ent_t *, s8); /** called when damage is received @param dmg amount of damage to subtract from ent's health @returns true if the entity dies, otherwises false*/
    void (*init)(ent_t *);    /** Called when entity is created. @param ent entity */
    void (*doDamage)(ent_t *, ent_t *);    // called when damage is done
    bool (*ontouch)(ent_t *, ent_t *, u16, u16);     /** called when entity collides with this @param x relative pixel x @param y relative pixel y @returns true to inhibit movement*/
    bool maypass;           /** Now just a bool ;) */
    void (*ondeath)(struct ent_t *);               // called when this dies
    void (*onupdate)(struct ent_t *);              // called every frame
    void (*onrelocate)(ent_t *eNew, level_t *lvl); /** called when the location of a pointer to an entity is about to change. @param eOld location of old pointer @param eNew location of the new pointer @param lvl new level pointer*/
} ent_event_t;


typedef struct {
    bool (*interact)(
        item_t *,   // reference to this item
        ent_t *,    // player entity
        const tile_t *,  // pointer to the tile in the map where the item that will be placed on
        uint x,      // absolute tile x of the tile 
        uint y      // absolute tile y of the tile
    ); // called when a player presses `A` with this item in their inventory
    bool canattack;
} item_event_t;



typedef struct tile_event_t {
    bool(*ontouch)(ent_t *, uint, uint);
    bool(*maypass)(ent_t *);
    bool (*interact)(ent_t *, item_t *item, uint x, uint y); /** @param item item_t used to interact with this tile @param x tile x @param y tile y*/
    void (*onrandomtick)(level_t *lvl, uint x, uint y); /** @param x absolute tile x @param y absolute tile y */
} tile_event_t;


/**
 * TOOL_TYPE_...
 * - used in `item_t`
 */
typedef enum {
    TOOL_TYPE_AXE,
    TOOL_TYPE_PICKAXE,
    TOOL_TYPE_SWORD,
    TOOL_TYPE_PICKUP,
    TOOL_TYPE_HOE,
} tooltype_t;


typedef struct item_t {
    u16 tile; // tile number of the inventory representation
    u8 palette; // palette number for item icon
    item_type_t type;
    union {
        tooltype_t tooltype;
        furniture_type_t furnituretype;
    };
    union {
        u8 level;   // used for tools
        s8 count;   // used for all other resources
    };
    item_event_t *event;
    inventory_t *parent;
    const char *name;
} item_t;


typedef struct inventory_t {
    uint size;
    item_t items[MAX_INVENTORY_SIZE];
    ent_t *parent;
} inventory_t;


typedef struct furniture_t {
    furniture_type_t type;
    inventory_t inventory;
} furniture_t;


typedef struct {
    item_type_t item_type;
    u8 required_amount;
} cost_t;

typedef struct {
    const item_t *result;
    u8 amount; // number of `result` to add to the player's inventory. 0 for tools
    u8 costs_num;
    cost_t costs[4];
} recipe_t;

/**
 * Used for collision
 */
typedef struct {
    uint sx, sy; // start x and start y
    uint w, h; // width and height
} bounding_rect_t;


typedef struct {
    s8 health;
    s8 max_health;
    s8 stamina;
    s8 max_stamina;
    uint on_stairs; // >0 after entering a staircase
    bool is_swimming;
    bool removed; // true when the player has changed levels. Used to interrupt flow inside functions because this entity pointer will be decayed soon
    bool dead;
    uint active_item; // index of the player's select item in its inventory
    inventory_t inventory;
    u8 invulnerability;
    u8 stamina_time;
} player_t;


typedef struct {
    s8 health;
    uint walk;
    s8 xAccel;
    s8 yAccel;
    uint invulernability;
} zombie_t;

typedef struct {
    s8 health;
    int jump_time;
    s8 xAccel;
    s8 yAccel;
    uint invulernability;
} slime_t;


typedef struct {
    s8 dx;
    s8 dy;
    uint frames_alive;
    uint count;
    item_t *item;
} itemEntity_t;


typedef struct ent_t {
    u16 x, y;          // absolute X and Y coordinates of map
    s8 xKnockback, yKnockback;
    ent_type_t type;    // refers to the type of entity this is
    union {
        player_t player;
        zombie_t zombie;
        slime_t slime;
        furniture_t furniture;
        itemEntity_t itemEntity;
    };
    obj_t *sprite;       // refers to OAM sprite representing this entity
    level_t *level;
    direction_t dir;
    const ent_event_t *events;
} ent_t;


typedef struct tile_t {
    tile_type_t type;
    union {
        u8 center;   // for 9pt indexing.
        u8 topRight; // for top-bottom indexing, single_8x8 & single_16x16
    } tiling;
    tile_indexing_mode_t indexing;
    const tile_event_t *event;
} tile_t;


#define LEVEL_WIDTH 64
#define LEVEL_HEIGHT 64
#define LEVEL_SIZE (LEVEL_HEIGHT * LEVEL_WIDTH)


typedef struct level_t {
    uint ent_size;  // number of entities in the `entities` table
    ent_t entities[MAX_ENTITY_SIZE]; // @todo allocate on heap
    ent_t *player;
    tile_type_t map[LEVEL_SIZE]; // needs to be adapted to level.size @todo
    u8 data[LEVEL_SIZE];
    u8 mob_density;
    uint layer;
    struct level_t *parent;
} level_t;



extern BG_REGULAR *main_background;

#endif