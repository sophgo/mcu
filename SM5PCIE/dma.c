#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/cortex.h>
#include <debug.h>
#include <string.h>
#include <config.h>

static uint8_t __attribute__((aligned(16))) sig_buf[SIG_BUF_SIZE];

#define ADC_DMA_CHANNEL		DMA_CHANNEL1

static uint8_t *buf0, *buf1;

static int buf0v;
static int buf1v;

#ifdef DEBUG

#define dma_error(msg)						\
	do {							\
		error("DMA-ASSERT LINE %d: ", __LINE__);	\
		error(msg);					\
	} while (0)
#else

#define dma_error(msg)						\
	do {							\
	} while (0)

#endif

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
	dma_set_number_of_data(DMA1, ADC_DMA_CHANNEL, SIG_BUF_SIZE);
	dma_enable_half_transfer_interrupt(DMA1, ADC_DMA_CHANNEL);
	dma_enable_transfer_complete_interrupt(DMA1, ADC_DMA_CHANNEL);
	dma_enable_channel(DMA1, ADC_DMA_CHANNEL);
	nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);

	buf0 = sig_buf;
	buf1 = sig_buf + SIG_BUF_SIZE / 2;

	return 0;
}

void dma_isr(void)
{
	uint32_t isr;
	uint32_t teif, htif;

	isr = DMA_ISR(DMA1);

	htif = isr & (1 << 2);
	teif = isr & (1 << 3);
	if (teif)
		dma_error("transmit error\r\n");
	if (htif)
		buf0v = 1;
	else
		buf1v = 1;

	DMA_IFCR(DMA1) = isr;
}

void *dma_buffer_get(void)
{
	void *buf;
	if (buf0v && buf1v)
		dma_error("overflow\r\n");
	if (!buf0v && !buf1v)
		buf = NULL;
	else
		buf = buf0v ? buf0 : buf1;
	return buf;
}

void dma_buffer_put(void *buf)
{
	cm_disable_interrupts();
	if (buf == buf0)
		buf0v = 0;
	else
		buf1v = 0;
	cm_enable_interrupts();
}

void dma_destroy(void)
{
	cm_disable_interrupts();
	dma_disable_channel(DMA1, ADC_DMA_CHANNEL);
	cm_enable_interrupts();
}
