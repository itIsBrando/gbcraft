#include "DMA.h"

typedef struct dma_t {
    volatile const void *src;
    volatile void *dst;
    u32 control;
} dma_t;

#define dma_mem ((volatile dma_t*)0x040000B0)
#define DMA_ENABLE() dma->control |= (1 << 0x1F)
#define DMA_SET_SIZE(size) dma->control |= ((size) & 0xFFFF)


inline void dma_init(dma_channel_t channel,
    dma_destination_adjustment_t dstAdjustment,
    dma_source_adjustment_t srcAdjustment,
    dma_chunk_size_t size, dma_timing_t timing, bool repeat
)
{
    volatile dma_t *dma = &dma_mem[channel & 0x3];

    dma->control = (repeat << 0x19) |
    channel | dstAdjustment | srcAdjustment | size | timing;
}


inline void dma_copy(dma_channel_t channel, void *destination, const void *source, uint size)
{
    volatile dma_t *dma = &dma_mem[channel & 0x3];

    dma->control = 0;
    dma->src = source;
    dma->dst = destination;
    DMA_SET_SIZE(size >> 2);

    dma->control |= DMA_ENABLE | DMA_DST_INC | DMA_SRC_INC | DMA_SIZE_32 | DMA_TIMING_NOW;
}


void dma_fill(dma_channel_t channel, void *destination, const void *source, uint size)
{
    volatile dma_t *dma = &dma_mem[channel & 0x3];
    dma_init(channel, DMA_DST_INC, DMA_SRC_FIXED, DMA_SIZE_16, DMA_TIMING_NOW, false);

    dma->src = source;
    dma->dst = destination;
    DMA_SET_SIZE(size >> 1);
    DMA_ENABLE();
}


void dma_start(dma_channel_t channel, const void *source, void *destination, uint size)
{
    volatile dma_t *dma = &dma_mem[channel & 0x3];

    dma->src = source;
    dma->dst = destination;
    DMA_SET_SIZE(size);
    DMA_ENABLE();
}