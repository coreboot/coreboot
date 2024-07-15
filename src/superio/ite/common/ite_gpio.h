/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_ITE_COMMON_GPIO_PRE_RAM_H
#define SUPERIO_ITE_COMMON_GPIO_PRE_RAM_H

#include <device/pnp_type.h>
#include <stdint.h>

#define ITE_GPIO_REG_SELECT(x)		(0x25 + (x))

enum ite_gpio_control {
	ITE_GPIO_CONTROL_DEFAULT = 0,
	ITE_GPIO_POL_INVERT = (1 << 0),
	ITE_GPIO_PULLUP_ENABLE = (1 << 1),
};

enum ite_gpio_direction {
	ITE_GPIO_INPUT,
	ITE_GPIO_OUTPUT
};

enum ite_gpio_mode {
	ITE_GPIO_ALT_FN_MODE,
	ITE_GPIO_SIMPLE_IO_MODE
};

/* There are two GP LED blink register sets */
enum ite_gpio_led {
	ITE_GPIO_LED_1,
	ITE_GPIO_LED_2
};

enum ite_led_control {
	ITE_LED_CONTROL_DEFAULT = 0,
	ITE_LED_SHORT_LOW_PULSE = (1 << 0),
	ITE_LED_OUTPUT_LOW = (1 << 1),
	/*
	 * Only for ITE SIOs with 5-bit frequency selection.
	 * When enabled, the LED pin mapping register is cleared when PANSWH# is low for over 4s.
	 */
	ITE_LED_PINMAP_CLEAR = (1 << 2),
};

enum ite_led_frequency {
	/* Most ITE SIOs have 2-bit frequency selection */
	ITE_LED_FREQ_4HZ = 0,
	ITE_LED_FREQ_1HZ = 1,
	ITE_LED_FREQ_0P25HZ = 2,
	ITE_LED_FREQ_0P125HZ = 3,
	/* ITE SIOs with 5-bit frequency selection: IT8625, IT8613 */
	ITE_LED_FREQ_4HZ_DUTY_50 = 0,
	ITE_LED_FREQ_1HZ_DUTY_50 = 1,
	ITE_LED_FREQ_0P25HZ_DUTY_50 = 2,
	ITE_LED_FREQ_2HZ_DUTY_50 = 3,
	ITE_LED_FREQ_0P25HZ_DUTY_25 = 4,
	ITE_LED_FREQ_0P25HZ_DUTY_75 = 5,
	ITE_LED_FREQ_0P125HZ_DUTY_25 = 6,
	ITE_LED_FREQ_0P125HZ_DUTY_75 = 7,
	ITE_LED_FREQ_0P4HZ_DUTY_20 = 8,
	ITE_LED_FREQ_0P5HZ_DUTY_50 = 16,
	ITE_LED_FREQ_0P125HZ_DUTY_50 = 24,
};

void ite_gpio_setup(pnp_devfn_t gpiodev, u8 gpio_num,
		    enum ite_gpio_direction output,
		    enum ite_gpio_mode enable,
		    u8 gpio_ctrl);

void ite_gpio_setup_led(pnp_devfn_t gpiodev, u8 gpio_num,
			enum ite_gpio_led led_no,
			enum ite_led_frequency freq,
			u8 led_ctrl);


#endif /* SUPERIO_ITE_COMMON_PRE_RAM_H */
