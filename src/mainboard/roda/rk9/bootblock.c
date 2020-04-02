/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <arch/io.h>
#include <device/pnp_ops.h>
#include <superio/smsc/lpc47n227/lpc47n227.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47N227_SP1)

void bootblock_mainboard_early_init(void)
{
	/* Original settings:
	   idx 30 31 32 33 34 35 36 37  38 39
	   val 60 00 00 40 00 ff 00 e0  00 80
	   def 00 00 00 00 00 00 00 00  00 80

	   Values:
	   GP1 GP2 GP3 GP4
	    fd  17  88  14
	*/
	const pnp_devfn_t sio = PNP_DEV(0x2e, 0);

	/* Enter super-io's configuration state. */
	pnp_enter_conf_state(sio);

	/* Set lpc47n227's runtime register block's base address. */
	pnp_write_config(sio, 0x30, 0x600 >> 4);

	/* Set GP23 to alternate function. */
	pnp_write_config(sio, 0x33, 0x40);

	/* Set GP30 - GP37 to output mode: COM control */
	pnp_write_config(sio, 0x35, 0xff);

	/* Set GP45 - GP47 to output mode. */
	pnp_write_config(sio, 0x37, 0xe0);

	/* Set nIO_PME to open drain. */
	pnp_write_config(sio, 0x39, 0x80);

	/* Exit configuration state. */
	pnp_exit_conf_state(sio);

	/* Set GPIO output values: */
	outb(0x88, 0x600 + 0xb + 3); /* GP30 - GP37 */
	outb(0x10, 0x600 + 0xb + 4); /* GP40 - GP47 */

	lpc47n227_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
