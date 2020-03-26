#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/cortex.h>
#include <debug.h>
#include <string.h>

/* must 512 aligned */
#define SAMPLE_HALF_NUM		(1024 + 256)
#define SAMPLE_NUM		(SAMPLE_HALF_NUM * 2)
#define SAMPLE_DEPTH		(1)
#define CHANNEL_NUM		(6)
#define SAMPLE_TOTAL		(CHANNEL_NUM * SAMPLE_NUM)

#define ADC_DMA_CHANNEL		DMA_CHANNEL1

static uint8_t __attribute__((aligned(16))) sig_buf[SAMPLE_TOTAL + 1024];
static uint8_t *buf0, *buf1;

static int b0_valid;
static int b1_valid;

#define dma_assert(msg)						\
	do {							\
		error("DMA-ASSERT LINE %d: ", __LINE__);	\
		error(msg);					\
	} while (0)

int dma_setup(void)
{
	rcc_periph_clock_enable(RCC_DMA);
	dma_disable_channel(DMA1, ADC_DMA_CHANNEL);

	dma_enable_circular_mode(DMA1, ADC_DMA_CHANNEL);
	dma_enable_memory_increment_mode(DMA1, ADC_DMA_CHANNEL);

	dma_set_peripheral_size(DMA1, ADC_DMA_CHANNEL, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, ADC_DMA_CHANNEL, DMA_CCR_MSIZE_8BIT);

	dma_set_read_from_peripheral(DMA1, ADC_DMA_CHANNEL);
	dma_set_peripheral_address(DMA1, ADC_DMA_CHANNEL,
				   (uint32_t)&ADC_DR(ADC1));

	dma_set_memory_address(DMA1, ADC_DMA_CHANNEL, (uint32_t)sig_buf);
	dma_set_number_of_data(DMA1, ADC_DMA_CHANNEL, SAMPLE_TOTAL);
	dma_enable_half_transfer_interrupt(DMA1, ADC_DMA_CHANNEL);
	dma_enable_transfer_complete_interrupt(DMA1, ADC_DMA_CHANNEL);
	dma_enable_channel(DMA1, ADC_DMA_CHANNEL);
	nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);

	buf0 = sig_buf;
	buf1 = sig_buf + SAMPLE_HALF_NUM * CHANNEL_NUM;

	return 0;
}

void dma_isr(void)
{
	uint32_t isr;
	uint32_t teif, htif, tcif;

	// printf("dma isr\r\n");

	isr = DMA_ISR(DMA1);

	tcif = isr & (1 << 1);
	htif = isr & (1 << 2);
	teif = isr & (1 << 3);
	if (teif)
		dma_assert("transmit error\r\n");
	if (tcif && htif)
		dma_assert("overflow\r\n");

	if (htif) {
		/* half transmission complete */
		if (b0_valid)
			dma_assert("overflow\r\n");
		b0_valid = 1;
	} else {
		/* transmission complete */
		if (b1_valid)
			dma_assert("overflow\r\n");
		b1_valid = 1;
	}

	DMA_IFCR(DMA1) = isr;
}

void *dma_buffer_get(unsigned long *sector_num)
{
	void *buf;
	cm_disable_interrupts();
	if (b0_valid && b1_valid)
		dma_assert("overflow\r\n");
	if (!b0_valid && !b1_valid) {
		*sector_num = 0;
		buf = NULL;
	} else {
		*sector_num = (SAMPLE_DEPTH * CHANNEL_NUM) *
			(SAMPLE_HALF_NUM / 512);
		buf = b0_valid ? buf0 : buf1;
	}
	cm_enable_interrupts();
	return buf;
}

void dma_buffer_put(void *buf)
{
	if (buf == buf0)
		b0_valid = 0;
	else
		b1_valid = 0;
}

void dma_destroy(void)
{
	cm_disable_interrupts();
	dma_disable_channel(DMA1, ADC_DMA_CHANNEL);
	cm_enable_interrupts();
}
