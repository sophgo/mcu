/* THIS IS AUTO GENERATED CODE */

#include <pin.h>

void pin_init(void)
{
	/* port A configuration */
	/* output data, default output state */
	GPIO_ODR(GPIOA) = 0;
	/* output type, push-pull or open-drain */
	GPIO_OTYPER(GPIOA) = 0;
	/* output speed, low, medium, high, very high */
	GPIO_OSPEEDR(GPIOA) =
	    (GPIO_OSPEEDR(GPIOA) & ~0xffffffff) | (GPIO_OSPEED_LOW << (0 * 2)) |
	    (GPIO_OSPEED_LOW << (1 * 2)) | (GPIO_OSPEED_HIGH << (2 * 2)) |
	    (GPIO_OSPEED_HIGH << (3 * 2)) | (GPIO_OSPEED_LOW << (4 * 2)) |
	    (GPIO_OSPEED_LOW << (5 * 2)) | (GPIO_OSPEED_LOW << (6 * 2)) |
	    (GPIO_OSPEED_LOW << (7 * 2)) | (GPIO_OSPEED_LOW << (8 * 2)) |
	    (GPIO_OSPEED_HIGH << (9 * 2)) | (GPIO_OSPEED_HIGH << (10 * 2)) |
	    (GPIO_OSPEED_LOW << (11 * 2)) | (GPIO_OSPEED_LOW << (12 * 2)) |
	    (GPIO_OSPEED_LOW << (13 * 2)) | (GPIO_OSPEED_LOW << (14 * 2)) |
	    (GPIO_OSPEED_LOW << (15 * 2));
	/* pull-up pull-down */
	GPIO_PUPDR(GPIOA) =
	    (GPIO_PUPDR(GPIOA) & ~0xffffffff) | (GPIO_PUPD_NONE << (0 * 2)) |
	    (GPIO_PUPD_NONE << (1 * 2)) | (GPIO_PUPD_NONE << (2 * 2)) |
	    (GPIO_PUPD_NONE << (3 * 2)) | (GPIO_PUPD_NONE << (4 * 2)) |
	    (GPIO_PUPD_NONE << (5 * 2)) | (GPIO_PUPD_NONE << (6 * 2)) |
	    (GPIO_PUPD_NONE << (7 * 2)) | (GPIO_PUPD_NONE << (8 * 2)) |
	    (GPIO_PUPD_NONE << (9 * 2)) | (GPIO_PUPD_NONE << (10 * 2)) |
	    (GPIO_PUPD_NONE << (11 * 2)) | (GPIO_PUPD_NONE << (12 * 2)) |
	    (GPIO_PUPD_NONE << (13 * 2)) | (GPIO_PUPD_NONE << (14 * 2)) |
	    (GPIO_PUPD_NONE << (15 * 2));
	/* af, alternative function selection */
	GPIO_AFRL(GPIOA) =
	    (GPIO_AFRL(GPIOA) & ~0x00007700) | (GPIO_AF4 << (2 * 4)) | (GPIO_AF4
									<< (3 *
									    4));
	GPIO_AFRH(GPIOA) =
	    (GPIO_AFRH(GPIOA) & ~0x07700000) | (GPIO_AF4 << ((9 - 4) * 4)) |
	    (GPIO_AF4 << ((10 - 4) * 4));
	/* mode, input, output, alternate function or analog */
	GPIO_MODER(GPIOA) =
	    (GPIO_MODER(GPIOA) & ~0xc3ffffff) | (GPIO_MODE_OUTPUT << (0 * 2)) |
	    (GPIO_MODE_OUTPUT << (1 * 2)) | (GPIO_MODE_AF << (2 * 2)) |
	    (GPIO_MODE_AF << (3 * 2)) | (GPIO_MODE_OUTPUT << (4 * 2)) |
	    (GPIO_MODE_OUTPUT << (5 * 2)) | (GPIO_MODE_OUTPUT << (6 * 2)) |
	    (GPIO_MODE_OUTPUT << (7 * 2)) | (GPIO_MODE_INPUT << (8 * 2)) |
	    (GPIO_MODE_AF << (9 * 2)) | (GPIO_MODE_AF << (10 * 2)) |
	    (GPIO_MODE_OUTPUT << (11 * 2)) | (GPIO_MODE_OUTPUT << (12 * 2)) |
	    (GPIO_MODE_OUTPUT << (15 * 2));

	/* port B configuration */
	/* output data, default output state */
	GPIO_ODR(GPIOB) = 0;
	/* output type, push-pull or open-drain */
	GPIO_OTYPER(GPIOB) =
	    (GPIO_OTYPE_OD << 8) | (GPIO_OTYPE_OD << 9) | (GPIO_OTYPE_OD << 10)
	    | (GPIO_OTYPE_OD << 11);
	/* output speed, low, medium, high, very high */
	GPIO_OSPEEDR(GPIOB) =
	    (GPIO_OSPEEDR(GPIOB) & ~0xffffffff) | (GPIO_OSPEED_LOW << (0 * 2)) |
	    (GPIO_OSPEED_LOW << (1 * 2)) | (GPIO_OSPEED_LOW << (2 * 2)) |
	    (GPIO_OSPEED_LOW << (3 * 2)) | (GPIO_OSPEED_LOW << (4 * 2)) |
	    (GPIO_OSPEED_LOW << (5 * 2)) | (GPIO_OSPEED_LOW << (6 * 2)) |
	    (GPIO_OSPEED_LOW << (7 * 2)) | (GPIO_OSPEED_HIGH << (8 * 2)) |
	    (GPIO_OSPEED_HIGH << (9 * 2)) | (GPIO_OSPEED_HIGH << (10 * 2)) |
	    (GPIO_OSPEED_HIGH << (11 * 2)) | (GPIO_OSPEED_LOW << (12 * 2)) |
	    (GPIO_OSPEED_LOW << (13 * 2)) | (GPIO_OSPEED_LOW << (14 * 2)) |
	    (GPIO_OSPEED_LOW << (15 * 2));
	/* pull-up pull-down */
	GPIO_PUPDR(GPIOB) =
	    (GPIO_PUPDR(GPIOB) & ~0xffffffff) | (GPIO_PUPD_NONE << (0 * 2)) |
	    (GPIO_PUPD_NONE << (1 * 2)) | (GPIO_PUPD_NONE << (2 * 2)) |
	    (GPIO_PUPD_NONE << (3 * 2)) | (GPIO_PUPD_NONE << (4 * 2)) |
	    (GPIO_PUPD_NONE << (5 * 2)) | (GPIO_PUPD_NONE << (6 * 2)) |
	    (GPIO_PUPD_NONE << (7 * 2)) | (GPIO_PUPD_NONE << (8 * 2)) |
	    (GPIO_PUPD_NONE << (9 * 2)) | (GPIO_PUPD_NONE << (10 * 2)) |
	    (GPIO_PUPD_NONE << (11 * 2)) | (GPIO_PUPD_NONE << (12 * 2)) |
	    (GPIO_PUPD_NONE << (13 * 2)) | (GPIO_PUPD_NONE << (14 * 2)) |
	    (GPIO_PUPD_NONE << (15 * 2));
	/* af, alternative function selection */
	/* AFRL reset state */
	GPIO_AFRH(GPIOB) =
	    (GPIO_AFRH(GPIOB) & ~0x77770000) | (GPIO_AF4 << ((8 - 4) * 4)) |
	    (GPIO_AF4 << ((9 - 4) * 4)) | (GPIO_AF6 << ((10 - 4) * 4)) |
	    (GPIO_AF6 << ((11 - 4) * 4));
	/* mode, input, output, alternate function or analog */
	GPIO_MODER(GPIOB) =
	    (GPIO_MODER(GPIOB) & ~0xffffffff) | (GPIO_MODE_OUTPUT << (0 * 2)) |
	    (GPIO_MODE_OUTPUT << (1 * 2)) | (GPIO_MODE_OUTPUT << (2 * 2)) |
	    (GPIO_MODE_OUTPUT << (3 * 2)) | (GPIO_MODE_OUTPUT << (4 * 2)) |
	    (GPIO_MODE_OUTPUT << (5 * 2)) | (GPIO_MODE_OUTPUT << (6 * 2)) |
	    (GPIO_MODE_OUTPUT << (7 * 2)) | (GPIO_MODE_AF << (8 * 2)) |
	    (GPIO_MODE_AF << (9 * 2)) | (GPIO_MODE_AF << (10 * 2)) |
	    (GPIO_MODE_AF << (11 * 2)) | (GPIO_MODE_OUTPUT << (12 * 2)) |
	    (GPIO_MODE_OUTPUT << (13 * 2)) | (GPIO_MODE_OUTPUT << (14 * 2)) |
	    (GPIO_MODE_OUTPUT << (15 * 2));

	/* port C configuration */
	/* output data, default output state */
	GPIO_ODR(GPIOC) = 0;
	/* output type, push-pull or open-drain */
	GPIO_OTYPER(GPIOC) = (GPIO_OTYPE_OD << 0) | (GPIO_OTYPE_OD << 1);
	/* output speed, low, medium, high, very high */
	GPIO_OSPEEDR(GPIOC) =
	    (GPIO_OSPEEDR(GPIOC) & ~0xffffffff) | (GPIO_OSPEED_HIGH << (0 * 2))
	    | (GPIO_OSPEED_HIGH << (1 * 2)) | (GPIO_OSPEED_LOW << (2 * 2)) |
	    (GPIO_OSPEED_LOW << (3 * 2)) | (GPIO_OSPEED_LOW << (4 * 2)) |
	    (GPIO_OSPEED_LOW << (5 * 2)) | (GPIO_OSPEED_LOW << (6 * 2)) |
	    (GPIO_OSPEED_LOW << (7 * 2)) | (GPIO_OSPEED_LOW << (8 * 2)) |
	    (GPIO_OSPEED_LOW << (9 * 2)) | (GPIO_OSPEED_LOW << (10 * 2)) |
	    (GPIO_OSPEED_LOW << (11 * 2)) | (GPIO_OSPEED_LOW << (12 * 2)) |
	    (GPIO_OSPEED_LOW << (13 * 2)) | (GPIO_OSPEED_LOW << (14 * 2)) |
	    (GPIO_OSPEED_LOW << (15 * 2));
	/* pull-up pull-down */
	GPIO_PUPDR(GPIOC) =
	    (GPIO_PUPDR(GPIOC) & ~0xffffffff) | (GPIO_PUPD_NONE << (0 * 2)) |
	    (GPIO_PUPD_NONE << (1 * 2)) | (GPIO_PUPD_NONE << (2 * 2)) |
	    (GPIO_PUPD_NONE << (3 * 2)) | (GPIO_PUPD_NONE << (4 * 2)) |
	    (GPIO_PUPD_NONE << (5 * 2)) | (GPIO_PUPD_NONE << (6 * 2)) |
	    (GPIO_PUPD_NONE << (7 * 2)) | (GPIO_PUPD_NONE << (8 * 2)) |
	    (GPIO_PUPD_NONE << (9 * 2)) | (GPIO_PUPD_NONE << (10 * 2)) |
	    (GPIO_PUPD_NONE << (11 * 2)) | (GPIO_PUPD_NONE << (12 * 2)) |
	    (GPIO_PUPD_NONE << (13 * 2)) | (GPIO_PUPD_NONE << (14 * 2)) |
	    (GPIO_PUPD_NONE << (15 * 2));
	/* af, alternative function selection */
	GPIO_AFRL(GPIOC) =
	    (GPIO_AFRL(GPIOC) & ~0x00000077) | (GPIO_AF7 << (0 * 4)) | (GPIO_AF7
									<< (1 *
									    4));
	/* AFRH reset state */
	/* mode, input, output, alternate function or analog */
	GPIO_MODER(GPIOC) =
	    (GPIO_MODER(GPIOC) & ~0xffffffff) | (GPIO_MODE_AF << (0 * 2)) |
	    (GPIO_MODE_AF << (1 * 2)) | (GPIO_MODE_INPUT << (2 * 2)) |
	    (GPIO_MODE_ANALOG << (3 * 2)) | (GPIO_MODE_OUTPUT << (4 * 2)) |
	    (GPIO_MODE_ANALOG << (5 * 2)) | (GPIO_MODE_OUTPUT << (6 * 2)) |
	    (GPIO_MODE_OUTPUT << (7 * 2)) | (GPIO_MODE_OUTPUT << (8 * 2)) |
	    (GPIO_MODE_OUTPUT << (9 * 2)) | (GPIO_MODE_OUTPUT << (10 * 2)) |
	    (GPIO_MODE_OUTPUT << (11 * 2)) | (GPIO_MODE_OUTPUT << (12 * 2)) |
	    (GPIO_MODE_OUTPUT << (13 * 2)) | (GPIO_MODE_OUTPUT << (14 * 2)) |
	    (GPIO_MODE_OUTPUT << (15 * 2));

	/* port D configuration */
	/* output data, default output state */
	GPIO_ODR(GPIOD) = 0;
	/* output type, push-pull or open-drain */
	GPIO_OTYPER(GPIOD) = 0;
	/* output speed, low, medium, high, very high */
	GPIO_OSPEEDR(GPIOD) =
	    (GPIO_OSPEEDR(GPIOD) & ~0xffffcfff) | (GPIO_OSPEED_LOW << (0 * 2)) |
	    (GPIO_OSPEED_LOW << (1 * 2)) | (GPIO_OSPEED_LOW << (2 * 2)) |
	    (GPIO_OSPEED_LOW << (3 * 2)) | (GPIO_OSPEED_LOW << (4 * 2)) |
	    (GPIO_OSPEED_LOW << (5 * 2)) | (GPIO_OSPEED_LOW << (7 * 2)) |
	    (GPIO_OSPEED_LOW << (8 * 2)) | (GPIO_OSPEED_LOW << (9 * 2)) |
	    (GPIO_OSPEED_LOW << (10 * 2)) | (GPIO_OSPEED_LOW << (11 * 2)) |
	    (GPIO_OSPEED_LOW << (12 * 2)) | (GPIO_OSPEED_LOW << (13 * 2)) |
	    (GPIO_OSPEED_LOW << (14 * 2)) | (GPIO_OSPEED_LOW << (15 * 2));
	/* pull-up pull-down */
	GPIO_PUPDR(GPIOD) =
	    (GPIO_PUPDR(GPIOD) & ~0xffffcfff) | (GPIO_PUPD_NONE << (0 * 2)) |
	    (GPIO_PUPD_NONE << (1 * 2)) | (GPIO_PUPD_NONE << (2 * 2)) |
	    (GPIO_PUPD_NONE << (3 * 2)) | (GPIO_PUPD_NONE << (4 * 2)) |
	    (GPIO_PUPD_NONE << (5 * 2)) | (GPIO_PUPD_NONE << (7 * 2)) |
	    (GPIO_PUPD_NONE << (8 * 2)) | (GPIO_PUPD_NONE << (9 * 2)) |
	    (GPIO_PUPD_NONE << (10 * 2)) | (GPIO_PUPD_NONE << (11 * 2)) |
	    (GPIO_PUPD_NONE << (12 * 2)) | (GPIO_PUPD_NONE << (13 * 2)) |
	    (GPIO_PUPD_NONE << (14 * 2)) | (GPIO_PUPD_NONE << (15 * 2));
	/* af, alternative function selection */
	/* AFRL reset state */
	/* AFRH reset state */
	/* mode, input, output, alternate function or analog */
	GPIO_MODER(GPIOD) =
	    (GPIO_MODER(GPIOD) & ~0xffffcfff) | (GPIO_MODE_OUTPUT << (0 * 2)) |
	    (GPIO_MODE_OUTPUT << (1 * 2)) | (GPIO_MODE_OUTPUT << (2 * 2)) |
	    (GPIO_MODE_OUTPUT << (3 * 2)) | (GPIO_MODE_OUTPUT << (4 * 2)) |
	    (GPIO_MODE_OUTPUT << (5 * 2)) | (GPIO_MODE_OUTPUT << (7 * 2)) |
	    (GPIO_MODE_OUTPUT << (8 * 2)) | (GPIO_MODE_OUTPUT << (9 * 2)) |
	    (GPIO_MODE_OUTPUT << (10 * 2)) | (GPIO_MODE_OUTPUT << (11 * 2)) |
	    (GPIO_MODE_OUTPUT << (12 * 2)) | (GPIO_MODE_OUTPUT << (13 * 2)) |
	    (GPIO_MODE_OUTPUT << (14 * 2)) | (GPIO_MODE_OUTPUT << (15 * 2));

	/* port E configuration */
	/* output data, default output state */
	GPIO_ODR(GPIOE) = 0;
	/* output type, push-pull or open-drain */
	GPIO_OTYPER(GPIOE) = 0;
	/* output speed, low, medium, high, very high */
	GPIO_OSPEEDR(GPIOE) =
	    (GPIO_OSPEEDR(GPIOE) & ~0xffffffff) | (GPIO_OSPEED_LOW << (0 * 2)) |
	    (GPIO_OSPEED_LOW << (1 * 2)) | (GPIO_OSPEED_LOW << (2 * 2)) |
	    (GPIO_OSPEED_LOW << (3 * 2)) | (GPIO_OSPEED_LOW << (4 * 2)) |
	    (GPIO_OSPEED_LOW << (5 * 2)) | (GPIO_OSPEED_LOW << (6 * 2)) |
	    (GPIO_OSPEED_LOW << (7 * 2)) | (GPIO_OSPEED_HIGH << (8 * 2)) |
	    (GPIO_OSPEED_HIGH << (9 * 2)) | (GPIO_OSPEED_LOW << (10 * 2)) |
	    (GPIO_OSPEED_LOW << (11 * 2)) | (GPIO_OSPEED_LOW << (12 * 2)) |
	    (GPIO_OSPEED_LOW << (13 * 2)) | (GPIO_OSPEED_LOW << (14 * 2)) |
	    (GPIO_OSPEED_LOW << (15 * 2));
	/* pull-up pull-down */
	GPIO_PUPDR(GPIOE) =
	    (GPIO_PUPDR(GPIOE) & ~0xffffffff) | (GPIO_PUPD_NONE << (0 * 2)) |
	    (GPIO_PUPD_NONE << (1 * 2)) | (GPIO_PUPD_NONE << (2 * 2)) |
	    (GPIO_PUPD_NONE << (3 * 2)) | (GPIO_PUPD_NONE << (4 * 2)) |
	    (GPIO_PUPD_NONE << (5 * 2)) | (GPIO_PUPD_NONE << (6 * 2)) |
	    (GPIO_PUPD_NONE << (7 * 2)) | (GPIO_PUPD_NONE << (8 * 2)) |
	    (GPIO_PUPD_NONE << (9 * 2)) | (GPIO_PUPD_NONE << (10 * 2)) |
	    (GPIO_PUPD_NONE << (11 * 2)) | (GPIO_PUPD_NONE << (12 * 2)) |
	    (GPIO_PUPD_NONE << (13 * 2)) | (GPIO_PUPD_NONE << (14 * 2)) |
	    (GPIO_PUPD_NONE << (15 * 2));
	/* af, alternative function selection */
	/* AFRL reset state */
	GPIO_AFRH(GPIOE) =
	    (GPIO_AFRH(GPIOE) & ~0x00770000) | (GPIO_AF6 << ((8 - 4) * 4)) |
	    (GPIO_AF6 << ((9 - 4) * 4));
	/* mode, input, output, alternate function or analog */
	GPIO_MODER(GPIOE) =
	    (GPIO_MODER(GPIOE) & ~0xffffffff) | (GPIO_MODE_OUTPUT << (0 * 2)) |
	    (GPIO_MODE_OUTPUT << (1 * 2)) | (GPIO_MODE_OUTPUT << (2 * 2)) |
	    (GPIO_MODE_OUTPUT << (3 * 2)) | (GPIO_MODE_OUTPUT << (4 * 2)) |
	    (GPIO_MODE_OUTPUT << (5 * 2)) | (GPIO_MODE_OUTPUT << (6 * 2)) |
	    (GPIO_MODE_OUTPUT << (7 * 2)) | (GPIO_MODE_AF << (8 * 2)) |
	    (GPIO_MODE_AF << (9 * 2)) | (GPIO_MODE_OUTPUT << (10 * 2)) |
	    (GPIO_MODE_OUTPUT << (11 * 2)) | (GPIO_MODE_OUTPUT << (12 * 2)) |
	    (GPIO_MODE_OUTPUT << (13 * 2)) | (GPIO_MODE_OUTPUT << (14 * 2)) |
	    (GPIO_MODE_OUTPUT << (15 * 2));

	/* port H configuration */
	/* output data, default output state */
	GPIO_ODR(GPIOH) = 0;
	/* output type, push-pull or open-drain */
	GPIO_OTYPER(GPIOH) = 0;
	/* output speed, low, medium, high, very high */
	GPIO_OSPEEDR(GPIOH) =
	    (GPIO_OSPEEDR(GPIOH) & ~0x0030000f) | (GPIO_OSPEED_LOW << (0 * 2)) |
	    (GPIO_OSPEED_LOW << (1 * 2)) | (GPIO_OSPEED_LOW << (10 * 2));
	/* pull-up pull-down */
	GPIO_PUPDR(GPIOH) =
	    (GPIO_PUPDR(GPIOH) & ~0x0030000f) | (GPIO_PUPD_NONE << (0 * 2)) |
	    (GPIO_PUPD_NONE << (1 * 2)) | (GPIO_PUPD_NONE << (10 * 2));
	/* af, alternative function selection */
	/* AFRL reset state */
	/* AFRH reset state */
	/* mode, input, output, alternate function or analog */
	GPIO_MODER(GPIOH) =
	    (GPIO_MODER(GPIOH) & ~0x0030000f) | (GPIO_MODE_OUTPUT << (0 * 2)) |
	    (GPIO_MODE_OUTPUT << (1 * 2)) | (GPIO_MODE_OUTPUT << (10 * 2));

}

/* AUTO GENERATED CODE END */
