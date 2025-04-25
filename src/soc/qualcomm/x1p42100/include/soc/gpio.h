/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_GPIO_H_
#define _SOC_QUALCOMM_X1P42100_GPIO_H_

#include <types.h>
#include <soc/addressmap.h>
#include <soc/gpio_common.h>

#define PIN(index, func1, func2, func3, func4) \
GPIO##index##_ADDR = TLMM_TILE_BASE + (index * TLMM_GPIO_OFF_DELTA), \
GPIO##index##_FUNC_##func1 = 1, \
GPIO##index##_FUNC_##func2 = 2, \
GPIO##index##_FUNC_##func3 = 3, \
GPIO##index##_FUNC_##func4 = 4

/* TODO: update as per datasheet */
enum {
	PIN(0, QUP0_L0, RES_2, RES_3, RES_4),
};

#endif /* _SOC_QUALCOMM_X1P42100_GPIO_H_ */
