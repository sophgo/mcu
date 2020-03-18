#include <spi_io.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <tick.h>

#define NSS_PORT	GPIOA
#define NSS_PIN		GPIO15
#define SPI		SPI1
#define RCC_SPI		RCC_SPI1

/******************************************************************************
 Module Public Functions - Low level SPI control functions
******************************************************************************/

void SPI_Init (void)
{
	rcc_periph_clock_enable(RCC_SPI);
	spi_reset(SPI);
	spi_init_master(SPI,
			SPI_CR1_BAUDRATE_FPCLK_DIV_2,
			SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
			SPI_CR1_CPHA_CLK_TRANSITION_1,
			SPI_CR1_DFF_8BIT,
			SPI_CR1_MSBFIRST);
	spi_enable_software_slave_management(SPI);
	spi_set_nss_high(SPI);
	spi_enable(SPI);
}

BYTE SPI_RW (BYTE d) {
	uint8_t dr;

	while(SPI_SR(SPI) & SPI_SR_BSY);
	SPI_DR(SPI) = d;
	while(!(SPI_SR(SPI) & SPI_SR_RXNE));
	dr = SPI_DR(SPI);

	return dr;
}

void SPI_Release (void) {
}

inline void SPI_CS_Low (void)
{
	gpio_clear(NSS_PORT, NSS_PIN);
}

inline void SPI_CS_High (void)
{
	gpio_set(NSS_PORT, NSS_PIN);
}

inline void SPI_Freq_High (void)
{
}

inline void SPI_Freq_Low (void)
{
}

static unsigned long start_time;
static unsigned long timeout;

void SPI_Timer_On (WORD ms)
{
	timeout = ms;
	start_time = tick_get();
}

inline BOOL SPI_Timer_Status (void)
{
	return tick_get() - start_time <= timeout;
}

inline void SPI_Timer_Off (void)
{
}

