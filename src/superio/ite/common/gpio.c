/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pnp_ops.h>
#include <device/pnp.h>
#include <stdint.h>

#include "ite.h"
#include "ite_gpio.h"

/* Catch ITE SIOs that enable the driver but do not configure the number of sets */
#if CONFIG_SUPERIO_ITE_COMMON_NUM_GPIO_SETS == 0
#error "Maximum number of ITE SIO GPIO sets not provided"
#endif

#if CONFIG_SUPERIO_ITE_COMMON_NUM_GPIO_SETS > 10
#error "ITE SIO GPIO drivers only support up to 10 GPIO sets"
#endif

/* GPIO Polarity Select: 1: Inverting, 0: Non-inverting */
#define ITE_GPIO_REG_POLARITY(x)			\
	(((x) > 8) ? (0xd1 + ((x) - 9) * 5)	\
		   : (0xb0 + ((x) - 1))		\
	)

/* GPIO Internal Pull-up: 1: Enable, 0: Disable */
#define ITE_GPIO_REG_PULLUP(x)			\
	(((x) > 8) ? (0xd4 + ((x) - 9) * 5)	\
		   : (0xb8 + ((x) - 1))		\
	)

/* GPIO Function Select: 1: Simple I/O, 0: Alternate function */
#define ITE_GPIO_REG_FN_SELECT(x)			\
	(((x) > 8) ? (0xd3 + ((x) - 9) * 5)	\
		   : (0xc0 + ((x) - 1))		\
	)

/* GPIO Mode: 0: input mode, 1: output mode */
#define ITE_GPIO_REG_OUTPUT(x)			\
	(((x) > 8) ? (0xd2 + ((x) - 9) * 5)	\
		   : (0xc8 + ((x) - 1))		\
	)

/* GPIO LED pin mapping register */
#define ITE_GPIO_REG_LED_PINMAP(x)		(0xf8 + ((x) & 1) * 2)
#define   ITE_GPIO_LED_PIN_LOC(set, pin)	((((set) & 7) << 3) | ((pin) & 7))
#define   ITE_GPIO_LED_PIN_LOC_MASK		0x3f
/* GPIO LED control register */
#define ITE_GPIO_REG_LED_CONTROL(x)		(0xf9 + ((x) & 1) * 2)
#define   ITE_GPIO_LED_OUTPUT_LOW		(1 << 0)
#define   ITE_GPIO_LED_PINMAP_CLEAR		(1 << 4)
#define   ITE_GPIO_LED_SHORT_LOW_PULSE					\
	(CONFIG(SUPERIO_ITE_COMMON_GPIO_LED_FREQ_5BIT) ? (1 << 5)	\
						       : (1 << 3)	\
	)
#define   ITE_GPIO_LED_FREQ_SEL(x)					\
	(CONFIG(SUPERIO_ITE_COMMON_GPIO_LED_FREQ_5BIT)		\
		? ((((x) & 0x18) << 3) | (((x) & 0x7) << 1))	\
		: (((x) & 0x3) << 1)				\
	)
#define   ITE_GPIO_LED_FREQ_SEL_MASK \
	(CONFIG(SUPERIO_ITE_COMMON_GPIO_LED_FREQ_5BIT) ? 0xce : 0x06)

static bool ite_has_gpio_fn_select_reg(u8 set)
{
	/* IT8718F has all registers for all sets. */
	if (CONFIG(SUPERIO_ITE_IT8718F))
		return true;

	/* Typically ITE GPIO sets 6 to 8 don't have enable and polarity registers. */
	if (set < 6 || set > 8)
		return true;

	return false;
}

static bool ite_has_gpio_polarity_reg(u8 set)
{
	/* IT8718F has all registers for all sets. */
	if (CONFIG(SUPERIO_ITE_IT8718F))
		return true;

	/* IT8720F/IT8721F has polarity register for all GPIO sets */
	if (CONFIG(SUPERIO_ITE_IT8720F) || CONFIG(SUPERIO_ITE_IT8721F))
		return true;

	/* Typically ITE GPIO sets 6 to 8 don't have enable and polarity registers. */
	if (set < 6 || set > 8)
		return true;

	return false;
}

static bool ite_has_gpio_pullup_reg(u8 set)
{
	/* IT8718F/IT8720F does not have pull-up register for set 2 */
	if ((CONFIG(SUPERIO_ITE_IT8718F) || CONFIG(SUPERIO_ITE_IT8720F)) && (set == 2))
		return false;

	/* IT8783E/F does not have pull-up register for set 6 */
	if (CONFIG(SUPERIO_ITE_IT8783EF) && (set == 6))
		return false;

	/*
	 * ITE GPIO Sets 7 and 8 don't have a pullup register.
	 * See IT8786/IT8625 datasheet section 8.10.10.
	 * Also applies to IT8728F.
	 */
	if (set != 7 && set != 8)
		return true;

	return false;
}

/*
 * Configures a single GPIO given its number as gpio_num, direction ("in_out"
 * parameter) and properties, such as polarity and pull ("gpio_ctrl"
 * parameter). The "enable" parameter can configure the GPIO in Simple I/O
 * mode when set or Alternate function mode when clear. Some chips may also
 * not support configuring all properties for a particular GPIO. It is left to
 * the implementer to check if GPIO settings are valid for given gpio_num.
 */
void ite_gpio_setup(pnp_devfn_t gpiodev, u8 gpio_num, enum ite_gpio_direction in_out,
		    enum ite_gpio_mode enable, u8 gpio_ctrl)
{
	u8 set = (gpio_num / 10);
	u8 pin = (gpio_num % 10);

	/* Number of configurable sets is chip dependent, 8 pins each */
	if (gpio_num < 10 || set > CONFIG_SUPERIO_ITE_COMMON_NUM_GPIO_SETS || pin > 7)
		return;

	pnp_enter_conf_state(gpiodev);
	pnp_set_logical_device(gpiodev);

	if (ite_has_gpio_fn_select_reg(set))
		pnp_unset_and_set_config(gpiodev, ITE_GPIO_REG_FN_SELECT(set),
					 1 << pin, (enable & 1) << pin);

	if (ite_has_gpio_polarity_reg(set))
		pnp_unset_and_set_config(gpiodev, ITE_GPIO_REG_POLARITY(set),
					 1 << pin,
					 (gpio_ctrl & ITE_GPIO_POL_INVERT) ? 1 << pin : 0);


	pnp_unset_and_set_config(gpiodev, ITE_GPIO_REG_OUTPUT(set), 1 << pin, (in_out & 1) << pin);

	if (ite_has_gpio_pullup_reg(set))
		pnp_unset_and_set_config(gpiodev, ITE_GPIO_REG_PULLUP(set), 1 << pin,
					 (gpio_ctrl & ITE_GPIO_PULLUP_ENABLE) ? 1 << pin : 0);

	pnp_exit_conf_state(gpiodev);
}

void ite_gpio_setup_led(pnp_devfn_t gpiodev, u8 gpio_num,
			enum ite_gpio_led led_no,
			enum ite_led_frequency freq,
			u8 led_ctrl)
{
	u8 set = (gpio_num / 10);
	u8 pin = (gpio_num % 10);
	u8 reg = 0;

	/* Number of configurable sets is chip dependent, 8 pins each */
	if (gpio_num < 10 || set > CONFIG_SUPERIO_ITE_COMMON_NUM_GPIO_SETS || pin > 7)
		return;

	/* LED is available only for GPIO sets 1-5 */
	if (set > 5)
		return;

	pnp_enter_conf_state(gpiodev);
	pnp_set_logical_device(gpiodev);

	/* Pinmap clear bit is only available when frequency is controlled with 5 bits */
	if (CONFIG(SUPERIO_ITE_COMMON_GPIO_LED_FREQ_5BIT) && (led_ctrl & ITE_LED_PINMAP_CLEAR))
		reg |= ITE_GPIO_LED_PINMAP_CLEAR;

	if (led_ctrl & ITE_LED_OUTPUT_LOW)
		reg |= ITE_GPIO_LED_OUTPUT_LOW;

	if (led_ctrl & ITE_LED_SHORT_LOW_PULSE)
		reg |= ITE_GPIO_LED_SHORT_LOW_PULSE;

	reg |= ITE_GPIO_LED_FREQ_SEL(freq);
	pnp_write_config(gpiodev, ITE_GPIO_REG_LED_CONTROL(led_no), reg);

	reg = ITE_GPIO_LED_PIN_LOC(set, pin);
	pnp_write_config(gpiodev, ITE_GPIO_REG_LED_PINMAP(led_no), reg);

	pnp_exit_conf_state(gpiodev);
}
