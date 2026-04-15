/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pnp.h>
#include <stdint.h>
#include <device/pnp_ops.h>

#include "mec152x.h"
#include "ldn.h"

#define MEC152x_NUM_UARTS 3

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

/*
 * Set IO base address and the valid bit. Caller must ensure to set LDN to ESPI I/O Component first.
 */
static void set_iobase(pnp_devfn_t dev, uint8_t bar, uint16_t iobase)
{
	// writing lsb first, then msb is crucial only for MEC152X_LDN_ESPI_IOC
	pnp_write_config(dev, bar + 2, iobase & 0xff); // address lsb
	pnp_write_config(dev, bar + 3, (iobase >> 8) & 0xff); // address msb
	pnp_write_config(dev, bar + 0, 1); // valid bit
}

void mec152x_enable_early_uart(uint16_t port, uint8_t uart_no, uint16_t uart_iobase)
{
	if (uart_no >= MEC152x_NUM_UARTS) {
		printk(BIOS_ERR, "%s: uart_no %u is invalid\n", __func__, uart_no);
		return;
	}

	static const uint8_t uart_ldn[] = {
		MEC152X_LDN_UART0,
		MEC152X_LDN_UART1,
		MEC152X_LDN_UART2
	};
	static const uint8_t uart_bar[] = {0x60, 0x64, 0x88};

	const pnp_devfn_t espiioc_dev = PNP_DEV(port, 0x0d);
	pnp_enter_conf_state(espiioc_dev);
	pnp_set_logical_device(espiioc_dev);
	set_iobase(espiioc_dev, uart_bar[uart_no], uart_iobase);

	const pnp_devfn_t uart_dev = PNP_DEV(port, uart_ldn[uart_no]);
	pnp_set_logical_device(uart_dev);
	pnp_set_enable(uart_dev, 1);

	pnp_exit_conf_state(espiioc_dev);
}
