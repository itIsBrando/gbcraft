#ifndef DMA_H
#define DMA_H

#include <gba_types.h>
#include <stdio.h>

// #include <tonc_core.h>

typedef enum {
    DMA_TIMING_NOW      = 0x00000000, // starts immediately
    DMA_TIMING_VBLANK   = 0x10000000, // starts at VBlank
    DMA_TIMING_HBLANK   = 0x20000000, // starts at HBlank
    DMA_TIMING_REFRESH  = 0x30000000, // who know what this does \__(-_-)__/
} dma_timing_t;


typedef enum {
    DMA_SRC_INC = (0),      // increase pointer after each transfer
    DMA_SRC_DEC = (0x00800000),      // decrease pointer after each transfer
    DMA_SRC_FIXED = (0x01000000),    // address does not change
} dma_source_adjustment_t;

typedef enum {
    DMA_DST_INC = (0),      // increase pointer after each transfer
    DMA_DST_DEC = (0x00200000),      // decrease pointer after each transfer
    DMA_DST_FIXED = (0x00400000),    // address does not change
    DMA_DST_RELOAD = (0x00600000),   // increment the destination but reset afterwards so repeated DMA starts at same address
} dma_destination_adjustment_t;

typedef enum {
    DMA_SIZE_16 = 0,         // use 16-bits
    DMA_SIZE_32 = 0x04000000, // use 32-bits
} dma_chunk_size_t;


typedef enum {
    DMA_CHANNEL_0,
    DMA_CHANNEL_1,
    DMA_CHANNEL_2,
    DMA_CHANNEL_3,
    DMA_ENABLE = 1 << 31,
} dma_channel_t;



/**
 * Sets up a DMA control register. Does not start a DMA transfer
 * @param channel DMA_CHANNEL_0-3
 * @param dstAdjustment DMA_DST_...
 * @param srcAdjustment DMA_SRC_...
 * @param size DMA_SIZE_16 for 16-bits, or DMA_SIZE_32 for 32-bits
 * @param timing DMA_TIMING_...
 * @param repeat true to repeat for VBlank or HBlank timing modes
 */
void dma_init(
    dma_channel_t channel,
    dma_destination_adjustment_t dstAdjustment,
    dma_source_adjustment_t srcAdjustment,
    dma_chunk_size_t size, dma_timing_t timing,
    bool repeat
);


/**
 * Starts a DMA transfer at VBLANK. Do not call `dma_init` first
 * @param size size in bytes
 */
void dma_copy(dma_channel_t channel, void *destination, const void *source, uint size);


/**
 * Starts a DMA transfer NOW. Do not call `dma_init` first
 * @param source source address (holds data to fill `destination` with)
 * @param destination destination address
 * @param size size in bytes
 */
void dma_fill(dma_channel_t channel, void *destination, const void *source, uint size);


/**
 * Starts a DMA transfer. Call `dma_init` first
 * @param source
 * @param destination 
 * @param size size in bytes
 */
void dma_start(dma_channel_t channel, const void *source, void *destination, uint size);

#endif