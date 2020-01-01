/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/smbus_def.h>
#include <spd_bin.h>
#include <string.h>
#include <timer.h>
#include "smbuslib.h"

static int smbus_wait_till_ready(u16 smbus_base)
{
	struct stopwatch sw;
	unsigned char byte;

	stopwatch_init_msecs_expire(&sw, SMBUS_TIMEOUT);
	do {
		byte = inb(smbus_base + SMBHSTSTAT);
		if (!(byte & 1))
			return 0;
	} while (!stopwatch_expired(&sw));
	return -1;
}

static int smbus_wait_till_done(u16 smbus_base)
{
	struct stopwatch sw;
	unsigned char byte;

	stopwatch_init_msecs_expire(&sw, SMBUS_TIMEOUT);
	do {
		byte = inb(smbus_base + SMBHSTSTAT);
		if (!((byte & 1) || (byte & ~((1 << 6) | (1 << 0))) == 0))
			return 0;
	} while (!stopwatch_expired(&sw));
	return -1;
}

int do_smbus_read_byte(unsigned int smbus_base, u8 device, unsigned int address)
{
	unsigned char global_status_register;
	unsigned char byte;

	if (smbus_wait_till_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Setup transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & (~1), smbus_base + SMBHSTCTL);
	/* Set the device I'm talking to */
	outb(((device & 0x7f) << 1) | 1, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(address & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a byte data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xe3) | (0x2 << 2),
		(smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* Clear the data byte... */
	outb(0, smbus_base + SMBHSTDAT0);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | 0x40),
	     smbus_base + SMBHSTCTL);

	/* Poll for transaction completion */
	if (smbus_wait_till_done(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;

	global_status_register = inb(smbus_base + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	global_status_register &= ~(3 << 5);

	/* Read results of transaction */
	byte = inb(smbus_base + SMBHSTDAT0);
	if (global_status_register != (1 << 1))
		return SMBUS_ERROR;

	return byte;
}

int do_smbus_write_byte(unsigned int smbus_base, u8 device, unsigned int address,
			unsigned int data)
{
	unsigned char global_status_register;

	if (smbus_wait_till_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Setup transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & (~1), smbus_base + SMBHSTCTL);
	/* Set the device I'm talking to */
	outb(((device & 0x7f) << 1) & ~0x01, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(address & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a byte data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xe3) | (0x2 << 2),
	     (smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* Clear the data byte... */
	outb(data, smbus_base + SMBHSTDAT0);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | 0x40),
	     smbus_base + SMBHSTCTL);

	/* Poll for transaction completion */
	if (smbus_wait_till_done(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;

	global_status_register = inb(smbus_base + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	global_status_register &= ~(3 << 5);

	/* Read results of transaction */
	if (global_status_register != (1 << 1))
		return SMBUS_ERROR;

	return 0;
}

int do_smbus_read_word(unsigned int smbus_base, u8 device, unsigned int address)
{
	unsigned char global_status_register;
	unsigned short data;

	if (smbus_wait_till_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Set up transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & ~1, smbus_base + SMBHSTCTL);
	/* Set the device I'm talking to */
	outb(((device & 0x7f) << 1) | 1, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(address & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a word data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xe3) | (0x3 << 2),
		(smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | 0x40),
	     smbus_base + SMBHSTCTL);

	/* Poll for transaction completion */
	if (smbus_wait_till_done(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;

	global_status_register = inb(smbus_base + SMBHSTSTAT);
	/* Ignore the "In Use" status... */
	if ((global_status_register & ~(3 << 5)) != (1 << 1))
		return SMBUS_ERROR;

	/* Read results of transaction */
	data = inw(smbus_base + SMBHSTDAT0);

	return data;
}

static void update_spd_len(struct spd_block *blk)
{
	u8 i, j = 0;
	for (i = 0 ; i < CONFIG_DIMM_MAX; i++)
		if (blk->spd_array[i] != NULL)
			j |= blk->spd_array[i][SPD_DRAM_TYPE];

	/* If spd used is DDR4, then its length is 512 byte. */
	if (j == SPD_DRAM_DDR4)
		blk->len = SPD_PAGE_LEN_DDR4;
	else
		blk->len = SPD_PAGE_LEN;
}

static void smbus_read_spd(u8 *spd, u8 addr)
{
	u16 i;
	u8 step = 1;

	if (CONFIG(SPD_READ_BY_WORD))
		step = sizeof(uint16_t);

	for (i = 0; i < SPD_PAGE_LEN; i += step) {
		if (CONFIG(SPD_READ_BY_WORD))
			((u16*)spd)[i / sizeof(uint16_t)] =
				 do_smbus_read_word(SMBUS_IO_BASE, addr, i);
		else
			spd[i] = do_smbus_read_byte(SMBUS_IO_BASE, addr, i);
	}
}

static void get_spd(u8 *spd, u8 addr)
{
	if (do_smbus_read_byte(SMBUS_IO_BASE, addr, 0) == 0xff) {
		printk(BIOS_INFO, "No memory dimm at address %02X\n",
			addr << 1);
		/* Make sure spd is zeroed if dimm doesn't exist. */
		memset(spd, 0, CONFIG_DIMM_SPD_SIZE);
		return;
	}
	smbus_read_spd(spd, addr);

	/* Check if module is DDR4, DDR4 spd is 512 byte. */
	if (spd[SPD_DRAM_TYPE] == SPD_DRAM_DDR4 &&
		CONFIG_DIMM_SPD_SIZE > SPD_PAGE_LEN) {
		/* Switch to page 1 */
		do_smbus_write_byte(SMBUS_IO_BASE, SPD_PAGE_1, 0, 0);
		smbus_read_spd(spd + SPD_PAGE_LEN, addr);
		/* Restore to page 0 */
		do_smbus_write_byte(SMBUS_IO_BASE, SPD_PAGE_0, 0, 0);
	}
}

static u8 spd_data[CONFIG_DIMM_MAX * CONFIG_DIMM_SPD_SIZE];

void get_spd_smbus(struct spd_block *blk)
{
	u8 i;
	for (i = 0 ; i < CONFIG_DIMM_MAX; i++) {
		get_spd(&spd_data[i * CONFIG_DIMM_SPD_SIZE],
			blk->addr_map[i]);
		blk->spd_array[i] = &spd_data[i * CONFIG_DIMM_SPD_SIZE];
	}

	update_spd_len(blk);
}
