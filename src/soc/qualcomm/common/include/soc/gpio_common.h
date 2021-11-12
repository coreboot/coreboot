/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_COMMON_GPIO_H_
#define _SOC_QUALCOMM_COMMON_GPIO_H_

#include <soc/gpio.h>
#include <soc/addressmap.h>

#define GPIO_FUNC_GPIO	0
#define TLMM_GPIO_OFF_DELTA	0x1000

/* GPIO TLMM INTR: Mask */
enum gpio_tlmm_intr_bmsk {
	GPIO_INTR_STATUS_MASK = 0x1,
	GPIO_INTR_DECT_CTL_MASK = 0x3,
};

/* GPIO TLMM: Mask */
enum gpio_tlmm_bmsk {
	GPIO_BMSK = 0x1,
	GPIO_CFG_PULL_BMSK = 0x3,
	GPIO_CFG_FUNC_BMSK = 0xF,
	GPIO_CFG_DRV_BMSK = 0x7,
	GPIO_CFG_EGPIO_BMSK = 0x800,
};

/* GPIO TLMM INTR: Shift */
enum gpio_tlmm_intr_shft {
	GPIO_INTR_DECT_CTL_SHFT = 2,
	GPIO_INTR_RAW_STATUS_EN_SHFT = 4,
};

/* GPIO TLMM: Shift */
enum gpio_tlmm_shft {
	GPIO_CFG_PULL_SHFT = 0,
	GPIO_CFG_FUNC_SHFT = 2,
	GPIO_CFG_DRV_SHFT = 6,
	GPIO_CFG_OE_SHFT = 9,
	GPIO_CFG_EGPIO_SHFT = 1,
};

/* GPIO IO: Shift */
enum gpio_io_shft {
	GPIO_IO_IN_SHFT,
	GPIO_IO_OUT_SHFT,
};

/* GPIO INTR STATUS */
enum gpio_irq_status {
	GPIO_INTR_STATUS_DISABLE,
	GPIO_INTR_STATUS_ENABLE,
};

/* GPIO TLMM: Direction */
enum gpio_direction {
	GPIO_INPUT,
	GPIO_OUTPUT,
};

/* GPIO TLMM: Pullup/Pulldown */
enum gpio_pull {
	GPIO_NO_PULL,
	GPIO_PULL_DOWN,
	GPIO_KEEPER,
	GPIO_PULL_UP,
};

/* GPIO TLMM: Drive Strength */
enum gpio_drv_str {
	GPIO_2MA,
	GPIO_4MA,
	GPIO_6MA,
	GPIO_8MA,
	GPIO_10MA,
	GPIO_12MA,
	GPIO_14MA,
	GPIO_16MA,
};

enum gpio_irq_type {
	IRQ_TYPE_LEVEL,
	IRQ_TYPE_RISING_EDGE,
	IRQ_TYPE_FALLING_EDGE,
	IRQ_TYPE_DUAL_EDGE,
};

typedef struct {
	u32 addr;
} gpio_t;

struct tlmm_gpio {
	uint32_t cfg;
	uint32_t in_out;
	uint32_t intr_cfg;
	uint32_t intr_status;
};

#define GPIO(num) ((gpio_t){.addr = GPIO##num##_ADDR})

void gpio_configure(gpio_t gpio, uint32_t func, uint32_t pull,
				uint32_t drive_str, uint32_t enable);
void gpio_input_irq(gpio_t gpio, enum gpio_irq_type type, uint32_t pull);
int gpio_irq_status(gpio_t gpio);

#endif /* _SOC_QUALCOMM_COMMON_GPIO_H_ */
