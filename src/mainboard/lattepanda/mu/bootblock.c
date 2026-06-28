/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <baseboard/variants.h>
#include <device/pci_ops.h>
#include <intelpch/espi.h>
#include <soc/pci_devs.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8613e/it8613e.h>

#define it8613e_index	0x2e

#define UART_DEV	PNP_DEV(it8613e_index, IT8613E_SP1)

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *early_pads;
	size_t early_num;

	/* Fixed IO decode for eSPI */
	pci_write_config32(PCH_DEV_ESPI, ESPI_IO_DEC, 0x3c030070);

	early_pads = variant_early_gpio_table(&early_num);
	gpio_configure_pads(early_pads, early_num);

	ite_enable_serial(UART_DEV, CONFIG_TTYS0_BASE);
}
