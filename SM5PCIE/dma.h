#ifndef __DMA_H__
#define __DMA_H__

int dma_setup(void);
int dma_destroy(void);
void *dma_buffer_get(void);
void dma_buffer_put(void *buf);

#endif
