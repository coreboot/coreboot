/* SPDX-License-Identifier: GPL-2.0-only */

/* Pre-RAM driver for the SMSC KBC1100 Super I/O chip */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <stdint.h>

#include "sio1036.h"

static inline void sio1036_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(0x55, port);
}

static inline void sio1036_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(0xaa, port);
}

/* Detect SMSC SIO1036 LPC Debug Card status */
static u8 detect_sio1036_chip(unsigned int port)
{
	pnp_devfn_t dev = PNP_DEV(port, SIO1036_SP1);
	u8 data;

	sio1036_enter_conf_state(dev);
	data = pnp_read_config(dev, 0x0D);
	sio1036_exit_conf_state(dev);

	/* Detect SMSC SIO1036 chip */
	if (data == 0x82) {
		/* Found SMSC SIO1036 chip */
		return 0;
	} else {
		return 1;
	};
}

void sio1036_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	unsigned int port = dev >> 8;

	if (detect_sio1036_chip(port) != 0)
		return;

	sio1036_enter_conf_state(dev);

	/* Enable SMSC UART 0 */
	/* Valid configuration cycle */
	pnp_write_config(dev, 0x00, 0x28);

	/* PP power/mode/cr lock */
	pnp_write_config(dev, 0x01, 0x98 | LPT_POWER_DOWN);
	pnp_write_config(dev, 0x02, 0x08 | UART_POWER_DOWN);

	/*Auto power management*/
	pnp_write_config(dev, 0x07, 0x00);

	/*ECP FIFO threhod */
	pnp_write_config(dev, 0x0A, 0x00 | IR_OUTPUT_MUX);

	/*GPIO direction register 2 */
	pnp_write_config(dev, 0x033, 0x00);

	/*UART Mode */
	pnp_write_config(dev, 0x0C, 0x02);

	/* GPIO polarity regisgter 2 */
	pnp_write_config(dev, 0x034, 0x00);

	/* Enable SMSC UART 0 */
	/*Set base io address */
	pnp_write_config(dev, 0x25, (u8)(iobase >> 2));

	/* Set UART IRQ onto 0x04 */
	pnp_write_config(dev, 0x28, 0x04);

	sio1036_exit_conf_state(dev);
}
