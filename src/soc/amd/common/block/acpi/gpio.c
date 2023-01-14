/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <gpio.h>

static int acpigen_soc_gpio_op(const char *op, unsigned int gpio_num)
{
	if (gpio_num >= SOC_GPIO_TOTAL_PINS) {
		printk(BIOS_WARNING, "Pin %d should be smaller than"
					" %d\n", gpio_num, SOC_GPIO_TOTAL_PINS);
		return -1;
	}
	if (SOC_GPIO_TOTAL_PINS >= AMD_GPIO_FIRST_REMOTE_GPIO_NUMBER &&
			gpio_num >= SOC_GPIO_TOTAL_PINS) {
		printk(BIOS_WARNING, "Pin %d is a remote GPIO which isn't supported"
			" yet.\n", gpio_num);
		return -1;
	}
	/* op (gpio_num) */
	acpigen_emit_namestring(op);
	acpigen_write_integer(gpio_num);
	return 0;
}

static int acpigen_soc_get_gpio_state(const char *op, unsigned int gpio_num)
{
	if (gpio_num >= SOC_GPIO_TOTAL_PINS) {
		printk(BIOS_WARNING, "Pin %d should be smaller than"
					" %d\n", gpio_num, SOC_GPIO_TOTAL_PINS);
		return -1;
	}
	if (SOC_GPIO_TOTAL_PINS >= AMD_GPIO_FIRST_REMOTE_GPIO_NUMBER &&
			gpio_num >= SOC_GPIO_TOTAL_PINS) {
		printk(BIOS_WARNING, "Pin %d is a remote GPIO which isn't supported"
			" yet.\n", gpio_num);
		return -1;
	}
	/* Store (op (gpio_num), Local0) */
	acpigen_write_store();
	acpigen_soc_gpio_op(op, gpio_num);
	acpigen_emit_byte(LOCAL0_OP);
	return 0;
}

int acpigen_soc_read_rx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_get_gpio_state("\\_SB.GRXS", gpio_num);
}

int acpigen_soc_get_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_get_gpio_state("\\_SB.GTXS", gpio_num);
}

int acpigen_soc_set_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_gpio_op("\\_SB.STXS", gpio_num);
}

int acpigen_soc_clear_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_gpio_op("\\_SB.CTXS", gpio_num);
}
