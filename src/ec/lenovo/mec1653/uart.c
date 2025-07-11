/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <ec/lenovo/mec1653/mec1653.h>
#include "debug.h"
#include "uart.h"

static void pnp_write_config32(pnp_devfn_t dev, uint8_t offset, uint32_t value)
{
	pnp_write_config(dev, offset, value & 0xff);
	pnp_write_config(dev, offset + 1, (value >> 8) & 0xff);
	pnp_write_config(dev, offset + 2, (value >> 16) & 0xff);
	pnp_write_config(dev, offset + 3, (value >> 24) & 0xff);
}

void mec1653_configure_uart(void)
{
	pnp_devfn_t lpc_dev = PNP_DEV(EC_CFG_PORT, LDN_LPCIF);
	pnp_devfn_t uart_dev = PNP_DEV(EC_CFG_PORT, LDN_UART);

	// Enter PNP conf state
	outb(MEC_CFG_ENTRY_KEY, EC_CFG_PORT);

	// Select LPC I/F LDN
	pnp_set_logical_device(lpc_dev);

	// Write UART BAR
	pnp_write_config32(lpc_dev, LPCIF_BAR_UART, (uint32_t) UART_PORT << 16 | 0x8707);

	// Set SIRQ4 to UART
	pnp_write_config(lpc_dev, LPCIF_SIRQ(UART_IRQ), LDN_UART);

	// Enable and configure UART LDN
	pnp_set_logical_device(uart_dev);
	pnp_set_enable(uart_dev, 1);
	pnp_write_config(uart_dev, UART_CONFIG_SELECT, 0);

	// Exit PNP conf state
	outb(MEC_CFG_EXIT_KEY, EC_CFG_PORT);

	// Supply debug unlock key
	debug_write_key(DEBUG_RW_KEY_IDX, CONFIG_MEC1653_DEBUG_UNLOCK_KEY);

	// Use debug writes to set UART_TX and UART_RX GPIOs
	debug_write_dword(MEC1653_CFG_REG + MEC1653_CFG_TX_GPIO_OFFSET, 0x1000);
	debug_write_dword(MEC1653_CFG_REG + MEC1653_CFG_RX_GPIO_OFFSET, 0x1000);
}
