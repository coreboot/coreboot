/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <stdint.h>
#include <assert.h>
#include <device/mmio.h>
#include <gpio.h>

#define SIFIVE_GPIO_INPUT_VAL  (FU740_GPIO + 0x00)
#define SIFIVE_GPIO_INPUT_EN   (FU740_GPIO + 0x04)
#define SIFIVE_GPIO_OUTPUT_EN  (FU740_GPIO + 0x08)
#define SIFIVE_GPIO_OUTPUT_VAL (FU740_GPIO + 0x0C)
#define SIFIVE_GPIO_PUE        (FU740_GPIO + 0x10)
#define SIFIVE_GPIO_DS         (FU740_GPIO + 0x14)
#define SIFIVE_GPIO_RISE_IE    (FU740_GPIO + 0x18)
#define SIFIVE_GPIO_RISE_IP    (FU740_GPIO + 0x1C)
#define SIFIVE_GPIO_FALL_IE    (FU740_GPIO + 0x20)
#define SIFIVE_GPIO_FALL_IP    (FU740_GPIO + 0x24)
#define SIFIVE_GPIO_HIGH_IE    (FU740_GPIO + 0x28)
#define SIFIVE_GPIO_HIGH_IP    (FU740_GPIO + 0x2C)
#define SIFIVE_GPIO_LOW_IE     (FU740_GPIO + 0x30)
#define SIFIVE_GPIO_LOW_IP     (FU740_GPIO + 0x34)
#define SIFIVE_GPIO_IOF_EN     (FU740_GPIO + 0x38)
#define SIFIVE_GPIO_IOF_SEL    (FU740_GPIO + 0x3C)
#define SIFIVE_GPIO_OUT_XOR    (FU740_GPIO + 0x40)

void gpio_set(gpio_t gpio, int value)
{
	uint32_t output_val = read32((void *)SIFIVE_GPIO_OUTPUT_VAL);

	if (value)
		output_val |= (1 << gpio);
	else
		output_val &= ~(1 << gpio);

	write32((void *)SIFIVE_GPIO_OUTPUT_VAL, output_val);
}

int gpio_get(gpio_t gpio)
{
	uint32_t input_val = read32((void *)SIFIVE_GPIO_INPUT_VAL);
	return !!(input_val & (1 << gpio));
}

void gpio_set_direction(gpio_t gpio, enum gpio_direction gpio_dir)
{
	uint32_t input_en  = read32((void *)SIFIVE_GPIO_INPUT_EN);
	uint32_t output_en = read32((void *)SIFIVE_GPIO_OUTPUT_EN);

	if (gpio_dir == GPIO_OUTPUT) {
		input_en  &= ~(1 << gpio);
		output_en |=  (1 << gpio);
	} else if (gpio_dir == GPIO_INPUT) {
		input_en  |=  (1 << gpio);
		output_en &= ~(1 << gpio);
	}

	write32((void *)SIFIVE_GPIO_INPUT_EN, input_en);
	write32((void *)SIFIVE_GPIO_OUTPUT_EN, output_en);
}
