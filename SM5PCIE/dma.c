#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/adc.h>
#include <debug.h>

#define SAMPLE_HALF_NUM		(512)
#define SAMPLE_NUM		(SAMPLE_HALF_NUM * 2)
#define SAMPLE_DEPTH		(2)
#define CHANNEL_NUM		(5)
#define SAMPLE_TOTAL		(CHANNEL_NUM * SAMPLE_NUM)

#define ADC_DMA_CHANNEL		DMA_CHANNEL1

uint16_t sig_buf[SAMPLE_TOTAL];

void dma_assert(const char *msg)
{
	error("%s", msg);
	while (1)
		;
}

int dma_setup(void)
{
	dma_disable_channel(DMA1, ADC_DMA_CHANNEL);

	dma_enable_circular_mode(DMA1, ADC_DMA_CHANNEL);
	dma_enable_memory_increment_mode(DMA1, ADC_DMA_CHANNEL);

	dma_set_peripheral_size(DMA1, ADC_DMA_CHANNEL, DMA_CCR_PSIZE_16BIT);
	dma_set_memory_size(DMA1, ADC_DMA_CHANNEL, DMA_CCR_MSIZE_16BIT);

	dma_set_read_from_peripheral(DMA1, ADC_DMA_CHANNEL);
	dma_set_peripheral_address(DMA1, ADC_DMA_CHANNEL,
				   (uint32_t)&ADC_DR(ADC1));

	dma_set_memory_address(DMA1, ADC_DMA_CHANNEL, (uint32_t)sig_buf);
	dma_set_number_of_data(DMA1, ADC_DMA_CHANNEL, SAMPLE_TOTAL);

	dma_enable_half_transfer_interrupt(DMA1, ADC_DMA_CHANNEL);
	dma_enable_transfer_complete_interrupt(DMA1, ADC_DMA_CHANNEL);
	dma_enable_channel(DMA1, ADC_DMA_CHANNEL);
	return 0;
}

/* half transmission complete */
void dma_ht_isr(void)
{}

/* transmission complete */
void dma_ct_isr(void)
{}

void dma_isr(void)
{
	uint32_t isr;
	uint32_t teif, htif, tcif;

	isr = DMA_ISR(DMA1);

	tcif = isr & (1 << 1);
	htif = isr & (1 << 2);
	teif = isr & (1 << 3);
	if (teif)
		dma_assert("transmit error\n");
	if (tcif && htif)
		dma_assert("overflow\n");

	if (htif)
		dma_ht_isr();
	else
		dma_ct_isr();

	DMA_IFCR(isr);
}


