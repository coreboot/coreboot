/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SRC_INCLUDE_GPIO_H__
#define __SRC_INCLUDE_GPIO_H__

#include <soc/gpio.h> /* IWYU pragma: export */

#ifndef __ASSEMBLER__ /* __ASSEMBLER__ also covers __ACPI__ case */

#include <types.h>

/* <soc/gpio.h> must typedef a gpio_t that fits in 32 bits. */
_Static_assert(sizeof(gpio_t) <= sizeof(u32), "gpio_t doesn't fit in lb_gpio");

/* The following functions must be implemented by SoC/board code. */
int gpio_get(gpio_t gpio);
int gpio_tx_get(gpio_t gpio);
void gpio_set(gpio_t gpio, int value);
void gpio_input_pulldown(gpio_t gpio);
void gpio_input_pullup(gpio_t gpio);
void gpio_input(gpio_t gpio);
void gpio_output(gpio_t gpio, int value);
uint32_t _gpio_base3_value(const gpio_t gpio[], int num_gpio, int binary_first);

/*
 * This function may be implemented by SoC/board code to provide
 * a mapping from a GPIO pin to controller by returning the ACPI
 * path for the controller that owns this GPIO.
 *
 * If not implemented the default handler will return NULL.
 */
const char *gpio_acpi_path(gpio_t gpio);

/*
 * This function may be implemented by SoC/board code to provide
 * a mapping from the internal representation of a GPIO to the 16bit
 * value used in an ACPI GPIO pin table entry.
 *
 * If not implemented by the SOC the default handler will return 0
 * because the underlying type of gpio_t is unknown.
 */
uint16_t gpio_acpi_pin(gpio_t gpio);

/*
 * Read the value presented by the set of GPIOs, when each pin is interpreted
 * as a base-2 digit (LOW = 0, HIGH = 1).
 *
 * gpio[]: pin positions to read. gpio[0] is less significant than gpio[1].
 * num_gpio: number of pins to read.
 *
 * There are also pulldown and pullup variants which default each gpio to
 * be configured with an internal pulldown and pullup, respectively.
 */
uint32_t gpio_base2_value(const gpio_t gpio[], int num_gpio);
uint32_t gpio_pulldown_base2_value(const gpio_t gpio[], int num_gpio);
uint32_t gpio_pullup_base2_value(const gpio_t gpio[], int num_gpio);

/*
 * Read the value presented by the set of GPIOs, when each pin is interpreted
 * as a base-3 digit (LOW = 0, HIGH = 1, Z/floating = 2).
 * Example: X1 = Z, X2 = 1 -> gpio_base3_value({GPIO(X1), GPIO(X2)}) = 5
 * BASE3() from <base3.h> can generate numbers to compare the result to.
 *
 * gpio[]: pin positions to read. gpio[0] is less significant than gpio[1].
 * num_gpio: number of pins to read.
 */
static inline uint32_t gpio_base3_value(const gpio_t gpio[], int num_gpio)
{
	return _gpio_base3_value(gpio, num_gpio, 0);
}

/*
 * Read the value presented by the set of GPIOs, when each pin is interpreted
 * as a base-3 digit (LOW = 0, HIGH = 1, Z/floating = 2) in a non-standard
 * ternary number system where the first 2^n natural numbers are represented
 * as they would be in a binary system (without any Z digits), and the following
 * 3^n-2^n numbers use the remaining ternary representations in the normal
 * ternary system order (skipping the values that were already used up).
 * This is useful for boards which initially used a binary board ID and later
 * decided to switch to tri-state after some revisions have already been built.
 * Example: For num_gpio = 2 we get the following representation:
 *
 *   Number      X1     X0
 *     0          0      0
 *     1          0      1
 *     2          1      0
 *     3          1      1	// Start counting ternaries back at 0 after this
 *     4          0      2	// Skipping 00 and 01 which are already used up
 *     5          1      2	// Skipping 10 and 11 which are already used up
 *     6          2      0
 *     7          2      1
 *     8          2      2
 *
 * gpio[]: pin positions to read. gpio[0] is less significant than gpio[1].
 * num_gpio: number of pins to read.
 */
static inline uint32_t gpio_binary_first_base3_value(const gpio_t gpio[],
						     int num_gpio)
{
	return _gpio_base3_value(gpio, num_gpio, 1);
}

#endif /* !__ASSEMBLER__ */

#endif /* __SRC_INCLUDE_GPIO_H__ */
