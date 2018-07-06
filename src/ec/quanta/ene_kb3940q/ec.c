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
 */

#ifndef __PRE_RAM__

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <delay.h>
#include <elog.h>
#include <stdlib.h>
#include <pc80/keyboard.h>

#include "ec.h"
#include "chip.h"

/* kbc helper functions from drivers/pc80/keyboard.c */
static int ec_input_buffer_empty(u8 status_port)
{
	u32 timeout;
	for (timeout = KBC_TIMEOUT_IN_MS; timeout && (inb(status_port) & KBD_IBF);
	    timeout--) {
		mdelay(1);
	}

	if (!timeout) {
		printk(BIOS_WARNING, "Unexpected EC/KBD input buffer full\n");
	}
	return !!timeout;
}


static int ec_output_buffer_full(u8 status_port)
{
	u32 timeout;
	for (timeout = KBC_TIMEOUT_IN_MS; timeout && ((inb(status_port)
	    & KBD_OBF) == 0); timeout--) {
		mdelay(1);
	}

	if (!timeout) {
		printk(BIOS_INFO, "EC/KBD output buffer result timeout\n");
	}
	return !!timeout;
}



/* The ENE 60/64 EC registers are the same command/status IB/OB KBC pair.
 * Check status from 64 port before each command.
 *
 *  Ex. Get panel ID command C43/D77
 *  Check IBF empty. Then Write 0x43(CMD) to 0x64 Port
 *  Check IBF empty. Then Write 0x77(DATA) to 0x60 Port
 *  Check OBF set. Then Get Data(0x03:panel ID) from 0x60
 * Different commands return may or may not respond and may have multiple
 * bytes. Keep it simple for now
 */

u8 ec_kbc_read_ob(void)
{
	if (!ec_output_buffer_full(KBD_STATUS)) return 0;
	return inb(KBD_DATA);
}

void ec_kbc_write_cmd(u8 cmd)
{
	if (!ec_input_buffer_empty(KBD_STATUS)) return;
	outb(cmd, KBD_COMMAND);
}

void ec_kbc_write_ib(u8 data)
{
	if (!ec_input_buffer_empty(KBD_STATUS)) return;
	outb(data, KBD_DATA);
}

/* EC Host Control Protocol routines */
u8 ec_read_ob(void)
{
	if (!ec_output_buffer_full(EC_SC)) return 0;
	return inb(EC_DATA);
}

void ec_write_cmd(u8 cmd)
{
	if (!ec_input_buffer_empty(EC_SC)) return;
	outb(cmd, EC_COMMAND);
}

void ec_write_ib(u8 data)
{
	if (!ec_input_buffer_empty(EC_SC)) return;
	outb(data, EC_DATA);
}

/*
 * These functions are for accessing the ENE932 device RAM space
 */
u8 ec_mem_read(u8 addr)
{
	ec_write_cmd(EC_CMD_READ_RAM);
	ec_write_ib(addr);
	return ec_read_ob();
}

void ec_mem_write(u8 addr, u8 data)
{
	ec_write_cmd(EC_CMD_WRITE_RAM);
	ec_write_ib(addr);
	ec_write_ib(data);
	return;
}

#ifndef __SMM__
static void ene_kb3940q_log_events(void)
{
#if IS_ENABLED(CONFIG_ELOG)
	u8 reason = ec_mem_read(EC_SHUTDOWN_REASON);
	if (reason)
		elog_add_event_byte(ELOG_TYPE_EC_SHUTDOWN, reason);
#endif
}

static void ene_kb3940q_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	printk(BIOS_DEBUG, "Quanta EnE KB3940Q: Initializing keyboard.\n");
	pc_keyboard_init(NO_AUX_DEVICE);

	ene_kb3940q_log_events();
}

static struct device_operations ops = {
	.init             = ene_kb3940q_init,
	.read_resources   = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, 0, 0, 0, }
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations ec_quanta_ene_kb3940q_ops = {
	CHIP_NAME("QUANTA EnE KB3940Q EC")
	.enable_dev = enable_dev
};
#endif /* ! __SMM__ */
#endif /* ! __PRE_RAM__ */
