#ifndef __DMA_H__
#define __DMA_H__

int dma_setup(void);
void *dma_buffer_get(unsigned long *size);
void dma_buffer_put(void *buf);

#endif
