/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/gpio.h>
#include <gpio.h>
#include <types.h>

static const struct soc_amd_event gpio_event_table[] = {
	{ GPIO_0, GEVENT_21 }, /* GPIO0 may only be used as PWR_BTN_L in ACPI */
	{ GPIO_1, GEVENT_19 },
	{ GPIO_2, GEVENT_8 },
	{ GPIO_3, GEVENT_2 },
	{ GPIO_4, GEVENT_4 },
	{ GPIO_5, GEVENT_7 },
	{ GPIO_6, GEVENT_10 },
	{ GPIO_7, GEVENT_11 },
	{ GPIO_8, GEVENT_23 },
	{ GPIO_9, GEVENT_22 },
	{ GPIO_16, GEVENT_12 },
	{ GPIO_17, GEVENT_13 },
	{ GPIO_18, GEVENT_14 },
	{ GPIO_21, GEVENT_5 },
	{ GPIO_22, GEVENT_3 },
	{ GPIO_23, GEVENT_16 },
	{ GPIO_24, GEVENT_15 },
	{ GPIO_40, GEVENT_20 },
	{ GPIO_84, GEVENT_18 },
	{ GPIO_86, GEVENT_9 },
	{ GPIO_89, GEVENT_0 },
	{ GPIO_90, GEVENT_1 },
	{ GPIO_91, GEVENT_6 },
	{ GPIO_129, GEVENT_17 },
};

void soc_get_gpio_event_table(const struct soc_amd_event **table, size_t *items)
{
	*table = gpio_event_table;
	*items = ARRAY_SIZE(gpio_event_table);
}
