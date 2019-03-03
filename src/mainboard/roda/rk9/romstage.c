/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* __PRE_RAM__ means: use "unsigned" for device, not a struct. */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <device/pci_ops.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <northbridge/intel/gm45/gm45.h>
#include <superio/smsc/lpc47n227/lpc47n227.h>

#define LPC_DEV PCI_DEV(0, 0x1f, 0)
#define SERIAL_DEV PNP_DEV(0x2e, LPC47N227_SP1)

void mb_setup_lpc(void)
{
	/* Set up SuperIO LPC forwards */

	/* Configure serial IRQs.*/
	pci_write_config8(LPC_DEV, D31F0_SERIRQ_CNTL, 0xd0);
	/* Map COMa on 0x3f8, COMb on 0x2f8. */
	pci_write_config16(LPC_DEV, D31F0_LPC_IODEC, 0x0010);
	/* Enable COMa, COMb, Kbd, SuperIO at 0x2e, MCs at 0x4e and 0x62/66. */
	pci_write_config16(LPC_DEV, D31F0_LPC_EN, 0x3c03);
}

void mb_setup_superio(void)
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

	/* Enable decoding of 0x600-0x60f through lpc. */
	pci_write_config32(LPC_DEV, D31F0_GEN1_DEC, 0x000c0601);

	/* Set GPIO output values: */
	outb(0x88, 0x600 + 0xb + 3); /* GP30 - GP37 */
	outb(0x10, 0x600 + 0xb + 4); /* GP40 - GP47 */

	lpc47n227_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void get_mb_spd_addrmap(u8 *spd_addrmap)
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[2] = 0x52;
}
