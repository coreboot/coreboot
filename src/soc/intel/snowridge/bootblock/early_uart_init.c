/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <stdint.h>

#include "../common/uart8250mem.h"
#include "bootblock.h"

static void pci_early_hsuart_device_enable(uint8_t func, uint16_t io_base)
{
	register uint16_t cmd_stat_reg;

	pci_or_config32(PCH_DEV_UART(func), UART_IOBA, io_base);

	/**
	 * Enable memory/io space and allow to initiate a transaction as a master.
	 */
	cmd_stat_reg = pci_read_config16(PCH_DEV_UART(func), PCI_COMMAND);
	cmd_stat_reg |= PCI_COMMAND_MASTER | PCI_COMMAND_IO;

#if CONFIG_CONSOLE_UART_BASE_ADDRESS != 0
	/* Decode MMIO at MEMBA (BAR1). */
	pci_write_config32(PCH_DEV_UART(func), UART_MEMBA,
			   CONFIG_CONSOLE_UART_BASE_ADDRESS + SIZE_OF_HSUART_RES * func);
	cmd_stat_reg |= PCI_COMMAND_MEMORY;
#endif

	pci_write_config16(PCH_DEV_UART(func), PCI_COMMAND, cmd_stat_reg);

#if CONFIG_TTYS0_BAUD > 115200
#if CONFIG_CONSOLE_UART_BASE_ADDRESS && CONFIG(ECAM_MMCONF_SUPPORT)
#define UCMR_OFFSET 0x34
	/**
	 * Change UART baseclock to 24 x 1.8432MHz -> 44.2368MHz, use
	 * `HIGH_SPEED_CLK_MULT` (24) times faster base clock.
	 */
	write32p(CONFIG_CONSOLE_UART_BASE_ADDRESS + UCMR_OFFSET,
		 read32p(CONFIG_CONSOLE_UART_BASE_ADDRESS + UCMR_OFFSET) * HIGH_SPEED_CLK_MULT);
#else
#error MMIO access is required for baudrates above 115200.
#endif
#endif
}

void early_uart_init(void)
{
	static const uint16_t legacy_uart_io_port_tab[] = {0x3F8, 0x2F8, 0x3E8, 0x2E8};
	register int i;

	for (i = SNOWRIDGE_UARTS_TO_INIT - 1; i >= 0; --i) {
		pci_early_hsuart_device_enable(i, legacy_uart_io_port_tab[i]);
	}
}
