/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include "sch555x.h"

static void pnp_enter_conf_state(pnp_devfn_t dev)
{
	unsigned int port = dev >> 8;
	outb(0x55, port);
}

static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	unsigned int port = dev >> 8;
	outb(0xaa, port);
}

static void pnp_write_config32(pnp_devfn_t dev, uint8_t offset, uint32_t value)
{
	pnp_write_config(dev, offset, value & 0xff);
	pnp_write_config(dev, offset + 1, (value >> 8) & 0xff);
	pnp_write_config(dev, offset + 2, (value >> 16) & 0xff);
	pnp_write_config(dev, offset + 3, (value >> 24) & 0xff);
}

/*
 * Do just enough init so that the motherboard specific magic EMI
 * sequences can be sent before sch555x_enable_serial is called
 */
void sch555x_early_init(pnp_devfn_t global_dev)
{
	pnp_enter_conf_state(global_dev);

	// Enable IRQs
	pnp_set_logical_device(global_dev);
	pnp_write_config(global_dev, SCH555x_DEVICE_MODE, 0x04);

	// Map EMI and runtime registers
	pnp_devfn_t lpci_dev = PNP_DEV(global_dev >> 8, SCH555x_LDN_LPCI);

	pnp_set_logical_device(lpci_dev);
	pnp_write_config32(lpci_dev, SCH555x_LPCI_EMI_BAR,
		(SCH555x_EMI_IOBASE << 16) | 0x800f);
	pnp_write_config32(lpci_dev, SCH555x_LPCI_RUNTIME_BAR,
		(SCH555x_RUNTIME_IOBASE << 16) | 0x8a3f);

	pnp_exit_conf_state(global_dev);
}

void sch555x_enable_serial(pnp_devfn_t uart_dev, uint16_t serial_iobase)
{
	pnp_enter_conf_state(uart_dev);

	// Set LPCI BAR register to map UART into I/O space
	pnp_devfn_t lpci_dev = PNP_DEV(uart_dev >> 8, SCH555x_LDN_LPCI);

	pnp_set_logical_device(lpci_dev);
	u8 uart_bar = (uart_dev & 0xff) == SCH555x_LDN_UART1
					? SCH555x_LPCI_UART1_BAR
					: SCH555x_LPCI_UART2_BAR;
	pnp_write_config32(lpci_dev, uart_bar, serial_iobase << 16 | 0x8707);

	// Set up the UART's configuration registers
	pnp_set_logical_device(uart_dev);
	pnp_set_enable(uart_dev, 1);			// Activate
	pnp_write_config(uart_dev, 0x0f, 0x02);		// Config select

	pnp_exit_conf_state(uart_dev);
}
