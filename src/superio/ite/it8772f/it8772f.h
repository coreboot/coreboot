/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_ITE_IT8772F_H
#define SUPERIO_ITE_IT8772F_H

#define IT8772F_FDC  0x00 /* Floppy disk controller */
#define IT8772F_SP1  0x01 /* Com1 */
#define IT8772F_EC   0x04 /* Environment controller */
#define IT8772F_KBCK 0x05 /* Keyboard */
#define IT8772F_KBCM 0x06 /* Mouse */
#define IT8772F_GPIO 0x07 /* GPIO */
#define IT8772F_IR   0x0a /* Consumer IR */


/* GPIO interface */
#define IT8772F_GPIO_LED_BLINK1_PINMAP		0xf8

#define SIO_GPIO_BLINK_GPIO10 0x08
#define SIO_GPIO_BLINK_GPIO22 0x12
#define SIO_GPIO_BLINK_GPIO45 0x25

#define IT8772F_GPIO_LED_BLINK1_CONTROL		0xf9

#define IT8772F_GPIO_BLINK_FREQUENCY_4_HZ   (0<<1)
#define IT8772F_GPIO_BLINK_FREQUENCY_1_HZ   (1<<1)
#define IT8772F_GPIO_BLINK_FREQUENCY_1_4_HZ (2<<1)
#define IT8772F_GPIO_BLINK_FREQUENCY_1_8_HZ (3<<1)

#define GPIO_REG_SELECT(x)   (0x25 + (x))

/* GPIO Polarity Select: 1: Inverting, 0: Non-inverting */
#define GPIO_REG_POLARITY(x) (0xb0 + (x))

/* GPIO Internal Pull-up: 1: Enable, 0: Disable */
#define GPIO_REG_PULLUP(x)   (0xb8 + (x))

/* GPIO Function Select: 1: Simple I/O, 0: Alternate function */
#define GPIO_REG_ENABLE(x)   (0xc0 + (x))

/* GPIO Mode: 0: input mode, 1: output mode */
#define GPIO_REG_OUTPUT(x)   (0xc8 + (x))

#include <device/pnp_type.h>
#include <stdint.h>

void it8772f_gpio_setup(pnp_devfn_t dev, int set, u8 select, u8 polarity,
			u8 pullup, u8 output, u8 enable);

void it8772f_gpio_led(pnp_devfn_t dev, int set, u8 select, u8 polarity, u8 pullup,
			u8 output, u8 enable, u8 led_pin_map, u8 led_freq);

#endif /* SUPERIO_ITE_IT8772F_H */
