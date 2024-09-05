/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_GPIO_DEFS_H
#define SOC_MEDIATEK_COMMON_GPIO_DEFS_H

#include <types.h>

enum {
	MAX_GPIO_REG_BITS = 32,
	MAX_GPIO_MODE_PER_REG = 8,
	GPIO_MODE_BITS = 4,
};

#define GPIO(name) ((gpio_t){ \
		.id = PAD_##name##_ID, \
		.flag = PAD_##name##_FLAG, \
		.bit = PAD_##name##_BIT, \
		.base = PAD_##name##_BASE, \
		.offset = PAD_##name##_OFFSET \
	})

#define PIN(id, name, flag, bit, base, offset, \
	    func1, func2, func3, func4, func5, func6, func7) \
	PAD_##name##_ID = id, \
	PAD_##name##_FLAG = flag, \
	PAD_##name##_BIT = bit, \
	PAD_##name##_BASE = base, \
	PAD_##name##_OFFSET = offset, \
	PAD_##name##_FUNC_##func1 = 1, \
	PAD_##name##_FUNC_##func2 = 2, \
	PAD_##name##_FUNC_##func3 = 3, \
	PAD_##name##_FUNC_##func4 = 4, \
	PAD_##name##_FUNC_##func5 = 5, \
	PAD_##name##_FUNC_##func6 = 6, \
	PAD_##name##_FUNC_##func7 = 7

struct gpio_val_regs {
	uint32_t val;
	uint32_t set;
	uint32_t rst;
	uint32_t align;
};

#endif
