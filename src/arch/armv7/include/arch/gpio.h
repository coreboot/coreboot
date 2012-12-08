/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * Copyright (c) 2011, NVIDIA Corp. All rights reserved.
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _ASM_GENERIC_GPIO_H_
#define _ASM_GENERIC_GPIO_H_

/*
 * Generic GPIO API for U-Boot
 *
 * GPIOs are numbered from 0 to GPIO_COUNT-1 which value is defined
 * by the SOC/architecture.
 *
 * Each GPIO can be an input or output. If an input then its value can
 * be read as 0 or 1. If an output then its value can be set to 0 or 1.
 * If you try to write an input then the value is undefined. If you try
 * to read an output, barring something very unusual,  you will get
 * back the value of the output that you previously set.
 *
 * In some cases the operation may fail, for example if the GPIO number
 * is out of range, or the GPIO is not available because its pin is
 * being used by another function. In that case, functions may return
 * an error value of -1.
 */

/**
 * Stop using the GPIO.  This function should not alter pin configuration.
 *
 * @param gpio	GPIO number
 * @return 0 if ok, -1 on error
 */
int gpio_free(unsigned gpio);

/**
 * Make a GPIO an input.
 *
 * @param gpio	GPIO number
 * @return 0 if ok, -1 on error
 */
int gpio_direction_input(unsigned gpio);

/**
 * Make a GPIO an output, and set its value.
 *
 * @param gpio	GPIO number
 * @param value	GPIO value (0 for low or 1 for high)
 * @return 0 if ok, -1 on error
 */
int gpio_direction_output(unsigned gpio, int value);

/**
 * Get a GPIO's value. This will work whether the GPIO is an input
 * or an output.
 *
 * @param gpio	GPIO number
 * @return 0 if low, 1 if high, -1 on error
 */
int gpio_get_value(unsigned gpio);

/**
 * Set an output GPIO's value. The GPIO must already be an output or
 * this function may have no effect.
 *
 * @param gpio	GPIO number
 * @param value	GPIO value (0 for low or 1 for high)
 * @return 0 if ok, -1 on error
 */
int gpio_set_value(unsigned gpio, int value);

/**
 * Request ownership of a gpio. This should be called before any of the other
 * functions are used on this gpio.
 *
 * @param gp	GPIO number
 * @param label	User label for this GPIO
 * @return 0 if ok, -1 on error
 */
int gpio_request(unsigned gpio, const char *label);

#endif	/* _ASM_GENERIC_GPIO_H_ */
