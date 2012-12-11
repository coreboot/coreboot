/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef __PRE_RAM__

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <stdlib.h>
#include <arch/io.h>
#include <delay.h>
#include "ec.h"
#include "chip.h"

/* kbc helper functions from drivers/pc80/keyboard.c. TODO: share functions. */
static int kbc_input_buffer_empty(void)
{
	u32 timeout;
	for(timeout = KBC_TIMEOUT_IN_MS; timeout && (inb(KBD_STATUS) & KBD_IBF);
	    timeout--) {
		mdelay(1);
	}

	if (!timeout) {
		printk(BIOS_WARNING,
		       "Unexpected Keyboard controller input buffer full\n");
	}
	return !!timeout;
}


static int kbc_output_buffer_full(void)
{
	u32 timeout;
	for(timeout = KBC_TIMEOUT_IN_MS; timeout && ((inb(KBD_STATUS)
	    & KBD_OBF) == 0); timeout--) {
		mdelay(1);
	}

	if (!timeout) {
		printk(BIOS_INFO, "Keyboard controller output buffer result timeout\n");
	}
	return !!timeout;
}

int kbc_cleanup_buffers(void)
{
	u32 timeout;
	for(timeout = KBC_TIMEOUT_IN_MS; timeout && (inb(KBD_STATUS)
	    & (KBD_OBF | KBD_IBF)); timeout--) {
		mdelay(1);
		inb(KBD_DATA);
	}

	if (!timeout) {
		printk(BIOS_ERR, "Couldn't cleanup the keyboard controller buffers\n");
		printk(BIOS_ERR, "Status (0x%x): 0x%x, Buffer (0x%x): 0x%x\n",
				KBD_STATUS, inb(KBD_STATUS), KBD_DATA, inb(KBD_DATA));
	}

	return !!timeout;
}


/* The ENE 60/64 EC registers are the same comand/status IB/OB KBC pair.
 * Check status from 64 port before each command.
 *
 *  Ex. Get panel ID command C43/D77
 *  Check IBF empty. Then Write 0x43(CMD) to 0x64 Port
 *  Check IBF empty. Then Write 0x77(DATA) to 0x60 Port
 *  Check OBF set. Then Get Data(0x03:panel ID) from 0x60
 * Different commands return may or maynot respond and may have multiple
 * bytes. Keep it simple for nor
 */

u8 ec_kbc_read_ob(void)
{
	if (!kbc_output_buffer_full()) return 0;
	return inb(KBD_DATA);
}

void ec_kbc_write_cmd(u8 cmd)
{
	if (!kbc_input_buffer_empty()) return;
	outb(cmd, KBD_COMMAND);
}

void ec_kbc_write_ib(u8 data)
{
	if (!kbc_input_buffer_empty()) return;
	outb(data, KBD_DATA);
}


/*
 * These functions are for accessing the ENE932 device space, but are not
 * currently used.
 */
/*
static u8 ec_io_read(u16 addr)
{
	outb(addr >> 8, EC_IO_HIGH);
	outb(addr & 0xff, EC_IO_LOW);
	return inb(EC_IO_DATA);
}
*/
/*static void ec_write(u16 addr, u8 data)
{
	outb(addr >> 8, EC_IO_HIGH);
	outb(addr & 0xff, EC_IO_LOW;
	outb(data, EC_IO_DATA);
}
*/

#ifndef __SMM__
static void ene932_init(device_t dev)
{
	struct ec_compal_ene932_config *conf = dev->chip_info;


	if (!dev->enabled)
		return;

	printk(BIOS_DEBUG, "Compal ENE932: Initializing keyboard.\n");
	pc_keyboard_init(&conf->keyboard);

}


static void ene932_read_resources(device_t dev)
{
	/* This function avoids an error on serial console. */
}


static void ene932_enable_resources(device_t dev)
{
	/* This function avoids an error on serial console. */
}

static struct device_operations ops = {
	.init             = ene932_init,
	.read_resources   = ene932_read_resources,
	.enable_resources = ene932_enable_resources
};

static struct pnp_info pnp_dev_info[] = {
        { &ops, 0, 0, { 0, 0 }, }
};

static void enable_dev(device_t dev)
{
	pnp_enable_devices(dev, &pnp_ops, ARRAY_SIZE(pnp_dev_info),
			   pnp_dev_info);
}

struct chip_operations ec_compal_ene932_ops = {
	CHIP_NAME("COMPAL ENE932 EC")
	.enable_dev = enable_dev
};
#endif /* ! __SMM__ */
#endif /* ! __PRE_RAM__ */
