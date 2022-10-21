/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PICASSO_I2C_H
#define AMD_PICASSO_I2C_H

#include <types.h>
#include <soc/gpio.h>

#define GPIO_I2C2_SCL		BIT(2)
#define GPIO_I2C3_SCL		BIT(3)
#define GPIO_I2C_MASK		(GPIO_I2C2_SCL | GPIO_I2C3_SCL)

#define I2C2_SCL_PIN			GPIO_113
#define I2C3_SCL_PIN			GPIO_19

#define I2C2_SCL_PIN_IOMUX_GPIOxx	GPIO_113_IOMUX_GPIOxx
#define I2C3_SCL_PIN_IOMUX_GPIOxx	GPIO_19_IOMUX_GPIOxx

#define I2C4_USB_PD_CTRL_OFFSET	0x600
#define USB_PD_PORT_CONTROL	(APU_I2C4_BASE + I2C4_USB_PD_CTRL_OFFSET)
#define PD_PORT_MUX_OFFSET(x)	(0x10 * (x))
#define DP_REVERSE	BIT(4)
#define USB_PD_RFMUX_OVERRIDE	BIT(8)
#define USB_PD_DP_OVERRIDE	BIT(9)

/* Sets the base address for the specific I2C bus. */
void i2c_set_bar(unsigned int bus, uintptr_t bar);
void reset_i2c_peripherals(void);

#endif /* AMD_PICASSO_I2C_H */
