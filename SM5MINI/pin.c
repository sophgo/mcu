/* THIS IS AUTO GENERATED CODE */

#include <pin.h>

void pin_init(void)
{
	/* port A configuration */
	/* output data, default output state */
	GPIO_ODR(GPIOA) = 0;
	/* output type, push-pull or open-drain */
	GPIO_OTYPER(GPIOA) = (GPIO_OTYPE_OD << 2) | (GPIO_OTYPE_OD << 3);
	/* output speed, low, medium, high, very high */
	GPIO_OSPEEDR(GPIOA) =
	    (GPIO_OSPEEDR(GPIOA) & ~0x000000f0) | (GPIO_OSPEED_HIGH << (2 * 2))
	    | (GPIO_OSPEED_HIGH << (3 * 2));
	/* pull-up pull-down */
	GPIO_PUPDR(GPIOA) =
	    (GPIO_PUPDR(GPIOA) & ~0xc3fff3f0) | (GPIO_PUPD_PULLUP << (2 * 2)) |
	    (GPIO_PUPD_PULLUP << (3 * 2)) | (GPIO_PUPD_NONE << (4 * 2)) |
	    (GPIO_PUPD_NONE << (6 * 2)) | (GPIO_PUPD_NONE << (7 * 2)) |
	    (GPIO_PUPD_NONE << (8 * 2)) | (GPIO_PUPD_NONE << (9 * 2)) |
	    (GPIO_PUPD_NONE << (10 * 2)) | (GPIO_PUPD_NONE << (11 * 2)) |
	    (GPIO_PUPD_NONE << (12 * 2)) | (GPIO_PUPD_NONE << (15 * 2));
	/* af, alternative function selection */
	GPIO_AFRL(GPIOA) =
	    (GPIO_AFRL(GPIOA) & ~0x0000ff00) | (GPIO_AF4 << (2 * 4)) | (GPIO_AF4
									<< (3 *
									    4));
	/* AFRH reset state */
	/* mode, input, output, alternate function or analog */
	GPIO_MODER(GPIOA) =
	    (GPIO_MODER(GPIOA) & ~0xc3ffffff) | (GPIO_MODE_ANALOG << (0 * 2)) |
	    (GPIO_MODE_ANALOG << (1 * 2)) | (GPIO_MODE_AF << (2 * 2)) |
	    (GPIO_MODE_AF << (3 * 2)) | (GPIO_MODE_INPUT << (4 * 2)) |
	    (GPIO_MODE_ANALOG << (5 * 2)) | (GPIO_MODE_INPUT << (6 * 2)) |
	    (GPIO_MODE_INPUT << (7 * 2)) | (GPIO_MODE_INPUT << (8 * 2)) |
	    (GPIO_MODE_OUTPUT << (9 * 2)) | (GPIO_MODE_OUTPUT << (10 * 2)) |
	    (GPIO_MODE_OUTPUT << (11 * 2)) | (GPIO_MODE_OUTPUT << (12 * 2)) |
	    (GPIO_MODE_OUTPUT << (15 * 2));

	/* port B configuration */
	/* output data, default output state */
	GPIO_ODR(GPIOB) = 0;
	/* output type, push-pull or open-drain */
	GPIO_OTYPER(GPIOB) =
	    (GPIO_OTYPE_OD << 6) | (GPIO_OTYPE_OD << 7) | (GPIO_OTYPE_OD << 10)
	    | (GPIO_OTYPE_OD << 11);
	/* output speed, low, medium, high, very high */
	GPIO_OSPEEDR(GPIOB) =
	    (GPIO_OSPEEDR(GPIOB) & ~0x00f0f000) | (GPIO_OSPEED_HIGH << (6 * 2))
	    | (GPIO_OSPEED_HIGH << (7 * 2)) | (GPIO_OSPEED_HIGH << (10 * 2)) |
	    (GPIO_OSPEED_HIGH << (11 * 2));
	/* pull-up pull-down */
	GPIO_PUPDR(GPIOB) =
	    (GPIO_PUPDR(GPIOB) & ~0x00f3ffff) | (GPIO_PUPD_NONE << (0 * 2)) |
	    (GPIO_PUPD_NONE << (1 * 2)) | (GPIO_PUPD_NONE << (2 * 2)) |
	    (GPIO_PUPD_NONE << (3 * 2)) | (GPIO_PUPD_NONE << (4 * 2)) |
	    (GPIO_PUPD_NONE << (5 * 2)) | (GPIO_PUPD_NONE << (6 * 2)) |
	    (GPIO_PUPD_NONE << (7 * 2)) | (GPIO_PUPD_NONE << (8 * 2)) |
	    (GPIO_PUPD_NONE << (10 * 2)) | (GPIO_PUPD_NONE << (11 * 2));
	/* af, alternative function selection */
	GPIO_AFRL(GPIOB) =
	    (GPIO_AFRL(GPIOB) & ~0xff000000) | (GPIO_AF1 << (6 * 4)) | (GPIO_AF1
									<< (7 *
									    4));
	GPIO_AFRH(GPIOB) =
	    (GPIO_AFRH(GPIOB) & ~0x0000ff00) | (GPIO_AF6 << ((10 - 8) * 4)) |
	    (GPIO_AF6 << ((11 - 8) * 4));
	/* mode, input, output, alternate function or analog */
	GPIO_MODER(GPIOB) =
	    (GPIO_MODER(GPIOB) & ~0x00f3ffff) | (GPIO_MODE_OUTPUT << (0 * 2)) |
	    (GPIO_MODE_OUTPUT << (1 * 2)) | (GPIO_MODE_OUTPUT << (2 * 2)) |
	    (GPIO_MODE_OUTPUT << (3 * 2)) | (GPIO_MODE_OUTPUT << (4 * 2)) |
	    (GPIO_MODE_OUTPUT << (5 * 2)) | (GPIO_MODE_AF << (6 * 2)) |
	    (GPIO_MODE_AF << (7 * 2)) | (GPIO_MODE_OUTPUT << (8 * 2)) |
	    (GPIO_MODE_AF << (10 * 2)) | (GPIO_MODE_AF << (11 * 2));

	/* port C configuration */
	/* output data, default output state */
	GPIO_ODR(GPIOC) = 0;
	/* output type, push-pull or open-drain */
	GPIO_OTYPER(GPIOC) = 0;
	/* output speed, low, medium, high, very high */
	/* reset state */
	/* pull-up pull-down */
	GPIO_PUPDR(GPIOC) =
	    (GPIO_PUPDR(GPIOC) & ~0xf0000000) | (GPIO_PUPD_NONE << (14 * 2)) |
	    (GPIO_PUPD_NONE << (15 * 2));
	/* af, alternative function selection */
	/* AFRL reset state */
	/* AFRH reset state */
	/* mode, input, output, alternate function or analog */
	GPIO_MODER(GPIOC) =
	    (GPIO_MODER(GPIOC) & ~0xf0000000) | (GPIO_MODE_OUTPUT << (14 * 2)) |
	    (GPIO_MODE_INPUT << (15 * 2));

}

/* AUTO GENERATED CODE END */
