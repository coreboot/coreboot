/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>

static int acpigen_soc_gpio_op(const char *op, unsigned int gpio_num)
{
	/* op (gpio_num) */
	acpigen_emit_namestring(op);
	acpigen_write_integer(gpio_num);
	return 0;
}

static int acpigen_soc_get_gpio_state(const char *op, unsigned int gpio_num)
{
	/* Store (op (gpio_num), Local0) */
	acpigen_write_store();
	acpigen_soc_gpio_op(op, gpio_num);
	acpigen_emit_byte(LOCAL0_OP);
	return 0;
}

int acpigen_soc_read_rx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_get_gpio_state("\\_SB.PCI0.GRXS", gpio_num);
}

int acpigen_soc_get_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_get_gpio_state("\\_SB.PCI0.GTXS", gpio_num);
}

int acpigen_soc_set_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_gpio_op("\\_SB.PCI0.STXS", gpio_num);
}

int acpigen_soc_clear_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_gpio_op("\\_SB.PCI0.CTXS", gpio_num);
}
