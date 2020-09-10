/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <soc/gpio.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>

/*
 * return family number and internal pad number in that community
 * by pad number and which community it is in.
 */

	/* family number in high byte and inner pad number in lowest byte */

void bootblock_mainboard_early_init(void)
{
	uint32_t reg;
	uint32_t *pad_config_reg;

	/* Enable the UART hardware for COM1. */
	reg = 1;
	pci_write_config32(PCI_DEV(0, LPC_DEV, 0), UART_CONT, reg);

	/*
	 * Set up the pads to select the UART function
	 * AD12 SW16(UART1_DATAIN/UART0_DATAIN)   - Setting Mode 2 for UART0_RXD
	 * AD10 SW20(UART1_DATAOUT/UART0_DATAOUT) - Setting Mode 2 for UART0_TXD
	 */
	pad_config_reg = gpio_pad_config_reg(GP_SOUTHWEST, UART1_RXD_PAD);
	write32(pad_config_reg, SET_PAD_MODE_SELECTION(PAD_CONFIG0_DEFAULT0,
		M2));

	pad_config_reg = gpio_pad_config_reg(GP_SOUTHWEST, UART1_TXD_PAD);
	write32(pad_config_reg, SET_PAD_MODE_SELECTION(PAD_CONFIG0_DEFAULT0,
		M2));
}
