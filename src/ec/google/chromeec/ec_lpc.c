/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <stdlib.h>

#include <console/console.h>
#include <arch/io.h>
#include <delay.h>
#include <device/pnp.h>
#include "chip.h"
#include "ec.h"
#include "ec_commands.h"

static int google_chromeec_wait_ready(u16 port)
{
	u8 ec_status = inb(port);
	u32 time_count = 0;

	/*
	 * One second is more than plenty for any EC operation to complete
	 * (and the bus accessing/code execution) overhead will make the
	 * timeout even longer.
	 */
#define MAX_EC_TIMEOUT_US 1000000

	while (ec_status &
	       (EC_LPC_CMDR_PENDING | EC_LPC_CMDR_BUSY)) {
		udelay(1);
		if (time_count++ == MAX_EC_TIMEOUT_US)
			return -1;
		ec_status = inb(port);
	}
	return 0;
}

int google_chromeec_command(struct chromeec_command *cec_command)
{
	struct ec_lpc_host_args args;
	const u8 *d;
	u8 *dout;
	u8 cmd_code = cec_command->cmd_code;
	int csum;
	int i;

	/* Fill in args */
	args.flags = EC_HOST_ARGS_FLAG_FROM_HOST;
	args.command_version = cec_command->cmd_version;
	args.data_size = cec_command->cmd_size_in;

	/* Initialize checksum */
	csum = cmd_code + args.flags + args.command_version + args.data_size;

	/* Write data and update checksum */
	for (i = 0, d = (const u8 *)cec_command->cmd_data_in;
	     i < cec_command->cmd_size_in; i++, d++) {
		outb(*d, EC_LPC_ADDR_HOST_PARAM + i);
		csum += *d;
	}

	/* Finalize checksum and write args */
	args.checksum = (u8)csum;
	for (i = 0, d = (const u8 *)&args; i < sizeof(args); i++, d++)
		outb(*d, EC_LPC_ADDR_HOST_ARGS + i);


	/* Issue the command */
	outb(cmd_code, EC_LPC_ADDR_HOST_CMD);

	if (google_chromeec_wait_ready(EC_LPC_ADDR_HOST_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC process command %d!\n",
		       cec_command->cmd_code);
		return 1;
	}

	/* Check result */
	cec_command->cmd_code = inb(EC_LPC_ADDR_HOST_DATA);
	if (cec_command->cmd_code)
		return 1;

	/* Read back args */
	for (i = 0, dout = (u8 *)&args; i < sizeof(args); i++, dout++)
		*dout = inb(EC_LPC_ADDR_HOST_ARGS + i);

	/*
	 * If EC didn't modify args flags, then somehow we sent a new-style
	 * command to an old EC, which means it would have read its params
	 * from the wrong place.
	 */
	if (!(args.flags & EC_HOST_ARGS_FLAG_TO_HOST)) {
		printk(BIOS_ERR, "EC protocol mismatch\n");
		return 1;
	}

	if (args.data_size > cec_command->cmd_size_out) {
		printk(BIOS_ERR, "EC returned too much data\n");
		return 1;
	}
	cec_command->cmd_size_out = args.data_size;

	/* Start calculating response checksum */
	csum = cmd_code + args.flags + args.command_version + args.data_size;

	/* Read data, if any */
	for (i = 0, dout = (u8 *)cec_command->cmd_data_out;
	     i < args.data_size; i++, dout++) {
		*dout = inb(EC_LPC_ADDR_HOST_PARAM + i);
		csum += *dout;
	}

	/* Verify checksum */
	if (args.checksum != (u8)csum) {
		printk(BIOS_ERR, "EC response has invalid checksum\n");
		return 1;
	}

	return 0;
}

#ifndef __PRE_RAM__

#ifndef __SMM__
static void lpc_ec_init(device_t dev)
{
	struct ec_google_chromeec_config *conf = dev->chip_info;

	if (!dev->enabled)
		return;
	pc_keyboard_init(&conf->keyboard);
	google_chromeec_init();
}

static void lpc_ec_read_resources(device_t dev)
{
	/* Nothing, but this function avoids an error on serial console. */
}

static void lpc_ec_enable_resources(device_t dev)
{
	/* Nothing, but this function avoids an error on serial console. */
}

static struct device_operations ops = {
	.init             = lpc_ec_init,
	.read_resources   = lpc_ec_read_resources,
	.enable_resources = lpc_ec_enable_resources
};

static struct pnp_info pnp_dev_info[] = {
	{ &ops, 0, 0, { 0, 0 }, }
};

static void enable_dev(device_t dev)
{
	pnp_enable_devices(dev, &pnp_ops, ARRAY_SIZE(pnp_dev_info),
			   pnp_dev_info);
}

struct chip_operations ec_google_chromeec_ops = {
	CHIP_NAME("Google Chrome EC")
	.enable_dev = enable_dev,
};

#endif /* __SMM__ */

u8 google_chromeec_get_event(void)
{
	if (google_chromeec_wait_ready(EC_LPC_ADDR_ACPI_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC ready!\n");
		return 1;
	}

	/* Issue the ACPI query-event command */
	outb(EC_CMD_ACPI_QUERY_EVENT, EC_LPC_ADDR_ACPI_CMD);

	if (google_chromeec_wait_ready(EC_LPC_ADDR_ACPI_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC QUERY_EVENT!\n");
		return 0;
	}

	/* Event (or 0 if none) is returned directly in the data byte */
	return inb(EC_LPC_ADDR_ACPI_DATA);
}
#endif
