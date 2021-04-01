/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/mmio.h>
#include <soc/gpio.h>

#include "gpio.h"

struct pad_func {
	u8 pin_id;
	u8 func;
};

#define PAD_FUNC(name, func) {PAD_##name##_ID, PAD_##name##_FUNC_##func}

static void nor_set_gpio_pinmux(void)
{
	const struct pad_func *ptr = NULL;

	/* GPIO 140 ~ 143 */
	struct pad_func nor_pinmux[] = {
		PAD_FUNC(SPIM2_CSB, SPINOR_CS),
		PAD_FUNC(SPIM2_CLK, SPINOR_CK),
		PAD_FUNC(SPIM2_MO, SPINOR_IO0),
		PAD_FUNC(SPIM2_MI, SPINOR_IO1),
	};

	ptr = nor_pinmux;
	for (size_t i = 0; i < ARRAY_SIZE(nor_pinmux); i++) {
		gpio_set_pull((gpio_t){.id = ptr[i].pin_id},
				GPIO_PULL_ENABLE, GPIO_PULL_UP);
		gpio_set_mode((gpio_t){.id = ptr[i].pin_id}, ptr[i].func);
	}
}

void bootblock_mainboard_init(void)
{
	nor_set_gpio_pinmux();
	setup_chromeos_gpios();
}
