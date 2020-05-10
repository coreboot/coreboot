/* Source : APQ8064 LK Boot */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __SOC_QUALCOMM_IPQ806X_GPIO_H_
#define __SOC_QUALCOMM_IPQ806X_GPIO_H_

#include <types.h>

#define GPIO_FUNC_ENABLE			1
#define GPIO_FUNC_DISABLE			0
#define FUNC_SEL_1				1
#define FUNC_SEL_3				3
#define FUNC_SEL_GPIO				0
#define GPIO_DRV_STR_10MA			0x4
#define GPIO_DRV_STR_11MA			0x7

/* GPIO TLMM: Direction */
#define GPIO_INPUT      0
#define GPIO_OUTPUT     1

/* GPIO TLMM: Pullup/Pulldown */
#define GPIO_NO_PULL    0
#define GPIO_PULL_DOWN  1
#define GPIO_KEEPER     2
#define GPIO_PULL_UP    3

/* GPIO TLMM: Drive Strength */
#define GPIO_2MA        0
#define GPIO_4MA        1
#define GPIO_6MA        2
#define GPIO_8MA        3
#define GPIO_10MA       4
#define GPIO_12MA       5
#define GPIO_14MA       6
#define GPIO_16MA       7

/* GPIO TLMM: Status */
#define GPIO_DISABLE    0
#define GPIO_ENABLE     1

/* GPIO MAX Valid # */
#define GPIO_MAX_NUM  68

/* GPIO TLMM: Mask */
#define GPIO_CFG_PULL_MASK    0x3
#define GPIO_CFG_FUNC_MASK    0xF
#define GPIO_CFG_DRV_MASK     0x7
#define GPIO_CFG_OE_MASK      0x1

/* GPIO TLMM: Shift */
#define GPIO_CFG_PULL_SHIFT   0
#define GPIO_CFG_FUNC_SHIFT   2
#define GPIO_CFG_DRV_SHIFT    6
#define GPIO_CFG_OE_SHIFT     9

/* GPIO IO: Mask */
#define GPIO_IO_IN_MASK       0x1
#define GPIO_IO_OUT_MASK      0x1

/* GPIO IO: Shift */
#define GPIO_IO_IN_SHIFT      0
#define GPIO_IO_OUT_SHIFT     1

typedef u32 gpio_t;

void gpio_tlmm_config_set(gpio_t gpio, unsigned int func,
			  unsigned int pull, unsigned int drvstr,
			  unsigned int enable);

void gpio_tlmm_config_get(gpio_t gpio, unsigned int *func,
			  unsigned int *pull, unsigned int *drvstr,
			  unsigned int *enable);

void gpio_io_config_set(gpio_t gpio, unsigned int out);

/* Keep this to maintain backwards compatibility with the vendor API. */
static inline void gpio_tlmm_config(unsigned int gpio, unsigned int func,
				    unsigned int dir, unsigned int pull,
				    unsigned int drvstr, unsigned int enable)
{
	gpio_tlmm_config_set(gpio, func, pull, drvstr, enable);
}
#endif // __SOC_QUALCOMM_IPQ806X_GPIO_H_
