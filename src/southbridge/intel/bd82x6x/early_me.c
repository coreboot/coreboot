/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <halt.h>
#include <string.h>
#include <timestamp.h>
#include "me.h"
#include "pch.h"

static const char *me_ack_values[] = {
	[ME_HFS_ACK_NO_DID]	= "No DID Ack received",
	[ME_HFS_ACK_RESET]	= "Non-power cycle reset",
	[ME_HFS_ACK_PWR_CYCLE]	= "Power cycle reset",
	[ME_HFS_ACK_S3]		= "Go to S3",
	[ME_HFS_ACK_S4]		= "Go to S4",
	[ME_HFS_ACK_S5]		= "Go to S5",
	[ME_HFS_ACK_GBL_RESET]	= "Global Reset",
	[ME_HFS_ACK_CONTINUE]	= "Continue to boot"
};

static inline void pci_read_dword_ptr(void *ptr, int offset)
{
	u32 dword = pci_read_config32(PCH_ME_DEV, offset);
	memcpy(ptr, &dword, sizeof(dword));
}

void intel_early_me_status(void)
{
	struct me_hfs hfs;
	struct me_gmes gmes;
	u32 id = pci_read_config32(PCH_ME_DEV, PCI_VENDOR_ID);

	if ((id == 0xffffffff) || (id == 0x00000000) ||
	    (id == 0x0000ffff) || (id == 0xffff0000)) {
		printk(BIOS_DEBUG, "Missing Intel ME PCI device.\n");
	} else {
		pci_read_dword_ptr(&hfs, PCI_ME_HFS);
		pci_read_dword_ptr(&gmes, PCI_ME_GMES);

		intel_me_status(&hfs, &gmes);
	}
}

int intel_early_me_init(void)
{
	int count;
	struct me_uma uma;
	struct me_hfs hfs;

	printk(BIOS_INFO, "Intel ME early init\n");

	/* Wait for ME UMA SIZE VALID bit to be set */
	for (count = ME_RETRY; count > 0; --count) {
		pci_read_dword_ptr(&uma, PCI_ME_UMA);
		if (uma.valid)
			break;
		udelay(ME_DELAY);
	}
	if (!count) {
		printk(BIOS_ERR, "ERROR: ME is not ready!\n");
		return -1;
	}

	/* Check for valid firmware */
	pci_read_dword_ptr(&hfs, PCI_ME_HFS);
	if (hfs.fpt_bad) {
		printk(BIOS_WARNING, "WARNING: ME has bad firmware\n");
		return -1;
	}

	printk(BIOS_INFO, "Intel ME firmware is ready\n");
	return 0;
}

int intel_early_me_uma_size(void)
{
	struct me_uma uma;

	pci_read_dword_ptr(&uma, PCI_ME_UMA);
	if (uma.valid) {
		printk(BIOS_DEBUG, "ME: Requested %uMB UMA\n", uma.size);
		return uma.size;
	}

	printk(BIOS_DEBUG, "ME: Invalid UMA size\n");
	return 0;
}

static inline void set_global_reset(int enable)
{
	u32 etr3 = pci_read_config32(PCH_LPC_DEV, ETR3);

	/* Clear CF9 Without Resume Well Reset Enable */
	etr3 &= ~ETR3_CWORWRE;

	/* CF9GR indicates a Global Reset */
	if (enable)
		etr3 |= ETR3_CF9GR;
	else
		etr3 &= ~ETR3_CF9GR;

	pci_write_config32(PCH_LPC_DEV, ETR3, etr3);
}

int intel_early_me_init_done(u8 status)
{
	u8 reset, errorcode, opmode;
	u16 reg16;
	u32 mebase_l, mebase_h;
	u32 millisec;
	u32 hfs, me_fws2;
	struct me_did did = {
		.init_done = ME_INIT_DONE,
		.status = status
	};
	u32 meDID;

	hfs = (pci_read_config32(PCI_DEV(0, 0x16, 0), PCI_ME_HFS) & 0xff000) >> 12;

	opmode = (hfs & 0xf0) >> 4;
	errorcode = hfs & 0xf;

	if (opmode != ME_HFS_MODE_NORMAL) {
		printk(BIOS_NOTICE, "ME: Wrong mode : %d\n", opmode);
		//return 0;
	}
	if (errorcode) {
		printk(BIOS_NOTICE, "ME: HFS error : %d\n", errorcode);
		//return 0;
	}

	me_fws2 = pci_read_config32(PCI_DEV(0, 0x16, 0), 0x48);
	printk(BIOS_NOTICE, "ME: FWS2: 0x%x\n", me_fws2);
	printk(BIOS_NOTICE, "ME:  Bist in progress: 0x%x\n", me_fws2 & 0x1);
	printk(BIOS_NOTICE, "ME:  ICC Status      : 0x%x\n", (me_fws2 & 0x6) >> 1);
	printk(BIOS_NOTICE, "ME:  Invoke MEBx     : 0x%x\n", (me_fws2 & 0x8) >> 3);
	printk(BIOS_NOTICE, "ME:  CPU replaced    : 0x%x\n", (me_fws2 & 0x10) >> 4);
	printk(BIOS_NOTICE, "ME:  MBP ready       : 0x%x\n", (me_fws2 & 0x20) >> 5);
	printk(BIOS_NOTICE, "ME:  MFS failure     : 0x%x\n", (me_fws2 & 0x40) >> 6);
	printk(BIOS_NOTICE, "ME:  Warm reset req  : 0x%x\n", (me_fws2 & 0x80) >> 7);
	printk(BIOS_NOTICE, "ME:  CPU repl valid  : 0x%x\n", (me_fws2 & 0x100) >> 8);
	printk(BIOS_NOTICE, "ME:  (Reserved)      : 0x%x\n", (me_fws2 & 0x600) >> 9);
	printk(BIOS_NOTICE, "ME:  FW update req   : 0x%x\n", (me_fws2 & 0x800) >> 11);
	printk(BIOS_NOTICE, "ME:  (Reserved)      : 0x%x\n", (me_fws2 & 0xf000) >> 12);
	printk(BIOS_NOTICE, "ME:  Current state   : 0x%x\n", (me_fws2 & 0xff0000) >> 16);
	printk(BIOS_NOTICE, "ME:  Current PM event: 0x%x\n", (me_fws2 & 0xf000000) >> 24);
	printk(BIOS_NOTICE, "ME:  Progress code   : 0x%x\n", (me_fws2 & 0xf0000000) >> 28);

	// Poll CPU replaced for 50ms
	millisec = 0;
	while ((((me_fws2 & 0x100) >> 8) == 0) && millisec < 50) {
		udelay(1000);
		me_fws2 = pci_read_config32(PCI_DEV(0, 0x16, 0), 0x48);
		millisec++;
	}
	if (millisec >= 50 || ((me_fws2 & 0x100) >> 8) == 0x0) {
		printk(BIOS_NOTICE, "Waited long enough, or CPU was not replaced, continue...\n");
	} else if ((me_fws2 & 0x100) == 0x100) {
		if ((me_fws2 & 0x80) == 0x80) {
			printk(BIOS_NOTICE, "CPU was replaced & warm reset required...\n");
			reg16 = pci_read_config16(PCI_DEV(0, 31, 0), 0xa2) & ~0x80;
			pci_write_config16(PCI_DEV(0, 31, 0), 0xa2, reg16);
			set_global_reset(0);
			outb(0x6, 0xcf9);
			halt();
		}

		if (((me_fws2 & 0x10) == 0x10) && (me_fws2 & 0x80) == 0x00) {
			printk(BIOS_NOTICE, "Full training required\n");
		}
	}

	printk(BIOS_NOTICE, "PASSED! Tell ME that DRAM is ready\n");

	/* MEBASE from MESEG_BASE[35:20] */
	mebase_l = pci_read_config32(PCI_CPU_DEVICE, PCI_CPU_MEBASE_L);
	mebase_h = pci_read_config32(PCI_CPU_DEVICE, PCI_CPU_MEBASE_H) & 0xf;
	did.uma_base = (mebase_l >> 20) | (mebase_h << 12);

	meDID = did.uma_base | (1 << 28);// | (1 << 23);
	pci_write_config32(PCI_DEV(0, 0x16, 0), PCI_ME_H_GS, meDID);

	/* Must wait for ME acknowledgement */
	if (opmode == ME_HFS_MODE_DEBUG) {
		printk(BIOS_NOTICE,
			"ME: ME is reporting as disabled, "
			"so not waiting for a response.\n");
	} else {
		timestamp_add_now(TS_ME_INFORM_DRAM_WAIT);
		udelay(100);
		millisec = 0;
		do {
			udelay(1000);
			hfs = (pci_read_config32(
				PCI_DEV(0, 0x16, 0), PCI_ME_HFS) & 0xfe000000)
				>> 24;
			millisec++;
		} while ((((hfs & 0xf0) >> 4) != ME_HFS_BIOS_DRAM_ACK)
			&& (millisec <= 5000));
		timestamp_add_now(TS_ME_INFORM_DRAM_DONE);
	}


	me_fws2 = pci_read_config32(PCI_DEV(0, 0x16, 0), 0x48);
	printk(BIOS_NOTICE, "ME: FWS2: 0x%x\n", me_fws2);
	printk(BIOS_NOTICE, "ME:  Bist in progress: 0x%x\n", me_fws2 & 0x1);
	printk(BIOS_NOTICE, "ME:  ICC Status      : 0x%x\n", (me_fws2 & 0x6) >> 1);
	printk(BIOS_NOTICE, "ME:  Invoke MEBx     : 0x%x\n", (me_fws2 & 0x8) >> 3);
	printk(BIOS_NOTICE, "ME:  CPU replaced    : 0x%x\n", (me_fws2 & 0x10) >> 4);
	printk(BIOS_NOTICE, "ME:  MBP ready       : 0x%x\n", (me_fws2 & 0x20) >> 5);
	printk(BIOS_NOTICE, "ME:  MFS failure     : 0x%x\n", (me_fws2 & 0x40) >> 6);
	printk(BIOS_NOTICE, "ME:  Warm reset req  : 0x%x\n", (me_fws2 & 0x80) >> 7);
	printk(BIOS_NOTICE, "ME:  CPU repl valid  : 0x%x\n", (me_fws2 & 0x100) >> 8);
	printk(BIOS_NOTICE, "ME:  (Reserved)      : 0x%x\n", (me_fws2 & 0x600) >> 9);
	printk(BIOS_NOTICE, "ME:  FW update req   : 0x%x\n", (me_fws2 & 0x800) >> 11);
	printk(BIOS_NOTICE, "ME:  (Reserved)      : 0x%x\n", (me_fws2 & 0xf000) >> 12);
	printk(BIOS_NOTICE, "ME:  Current state   : 0x%x\n", (me_fws2 & 0xff0000) >> 16);
	printk(BIOS_NOTICE, "ME:  Current PM event: 0x%x\n", (me_fws2 & 0xf000000) >> 24);
	printk(BIOS_NOTICE, "ME:  Progress code   : 0x%x\n", (me_fws2 & 0xf0000000) >> 28);


	/* Return the requested BIOS action */
	printk(BIOS_NOTICE, "ME: Requested BIOS Action: %s\n",
		me_ack_values[(hfs & 0xe) >> 1]);

	reset = inb(0xcf9);
	reset &= 0xf1;
	switch ((hfs & 0xe) >> 1) {
	case ME_HFS_ACK_NO_DID:
	case ME_HFS_ACK_CONTINUE:
		/* Continue to boot */
		return 0;
	case ME_HFS_ACK_RESET:
		/* Non-power cycle reset */
		set_global_reset(0);
		reset |= 0x06;
		break;
	case ME_HFS_ACK_PWR_CYCLE:
		/* Power cycle reset */
		set_global_reset(0);
		reset |= 0x0e;
		break;
	case ME_HFS_ACK_GBL_RESET:
		/* Global reset */
		set_global_reset(1);
		reset |= 0x0e;
		break;
	case ME_HFS_ACK_S3:
	case ME_HFS_ACK_S4:
	case ME_HFS_ACK_S5:
		break;
	}

	/* Perform the requested reset */
	if (reset) {
		outb(reset, 0xcf9);
		halt();
	}
	return -1;
}
