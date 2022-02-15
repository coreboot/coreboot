/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pci_ops.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci_def.h>
#include <halt.h>
#include <southbridge/intel/common/me.h>
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

void intel_early_me_status(void)
{
	union me_hfs hfs;
	union me_gmes gmes;
	u32 id = pci_read_config32(PCH_ME_DEV, PCI_VENDOR_ID);

	if ((id == 0xffffffff) || (id == 0x00000000) ||
	    (id == 0x0000ffff) || (id == 0xffff0000)) {
		printk(BIOS_DEBUG, "Missing Intel ME PCI device.\n");
	} else {
		hfs.raw = pci_read_config32(PCH_ME_DEV, PCI_ME_HFS);
		gmes.raw = pci_read_config32(PCH_ME_DEV, PCI_ME_GMES);

		intel_me_status(&hfs, &gmes);
	}
}

int intel_early_me_init(void)
{
	int count;
	union me_uma uma;
	union me_hfs hfs;

	printk(BIOS_INFO, "Intel ME early init\n");

	/* Wait for ME UMA SIZE VALID bit to be set */
	for (count = ME_RETRY; count > 0; --count) {
		uma.raw = pci_read_config32(PCH_ME_DEV, PCI_ME_UMA);
		if (uma.valid)
			break;
		udelay(ME_DELAY);
	}
	if (!count) {
		printk(BIOS_ERR, "ME is not ready!\n");
		return -1;
	}

	/* Check for valid firmware */
	hfs.raw = pci_read_config32(PCH_ME_DEV, PCI_ME_HFS);
	if (hfs.fpt_bad) {
		printk(BIOS_WARNING, "ME has bad firmware\n");
		return -1;
	}

	printk(BIOS_INFO, "Intel ME firmware is ready\n");
	return 0;
}

int intel_early_me_uma_size(void)
{
	union me_uma uma;

	uma.raw = pci_read_config32(PCH_ME_DEV, PCI_ME_UMA);
	if (uma.valid) {
		printk(BIOS_DEBUG, "ME: Requested %uMB UMA\n", uma.size);
		return uma.size;
	}

	printk(BIOS_DEBUG, "ME: Invalid UMA size\n");
	return 0;
}

int intel_early_me_init_done(u8 status)
{
	u8 reset, errorcode, opmode;
	u32 mebase_l, mebase_h;
	u32 millisec;
	u32 hfs, me_fws2;
	union me_did did = {
		.init_done = ME_INIT_DONE,
		.status = status
	};
	u32 meDID;

	hfs = (pci_read_config32(PCH_ME_DEV, PCI_ME_HFS) & 0xff000) >> 12;

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

	me_fws2 = pci_read_config32(PCH_ME_DEV, PCI_ME_GMES);
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

	/* Poll CPU replaced for 50ms */
	millisec = 0;
	while ((((me_fws2 & 0x100) >> 8) == 0) && millisec < 50) {
		udelay(1000);
		me_fws2 = pci_read_config32(PCH_ME_DEV, PCI_ME_GMES);
		millisec++;
	}
	if (millisec >= 50 || ((me_fws2 & 0x100) >> 8) == 0x0) {
		printk(BIOS_NOTICE, "Waited long enough, or CPU was not replaced, continue...\n");
	} else if ((me_fws2 & 0x100) == 0x100) {
		if ((me_fws2 & 0x80) == 0x80) {
			printk(BIOS_NOTICE, "CPU was replaced & warm reset required...\n");
			pci_and_config16(PCH_LPC_DEV, GEN_PMCON_2, ~0x80);
			set_global_reset(0);
			system_reset();
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
	pci_write_config32(PCH_ME_DEV, PCI_ME_H_GS, meDID);

	/* Must wait for ME acknowledgement */
	if (opmode == ME_HFS_MODE_DEBUG) {
		printk(BIOS_NOTICE,
			"ME: ME is reporting as disabled, "
			"so not waiting for a response.\n");
	} else {
		timestamp_add_now(TS_ME_INFORM_DRAM_START);
		udelay(100);
		millisec = 0;
		do {
			udelay(1000);
			hfs = (pci_read_config32(
				PCH_ME_DEV, PCI_ME_HFS) & 0xfe000000)
				>> 24;
			millisec++;
		} while ((((hfs & 0xf0) >> 4) != ME_HFS_BIOS_DRAM_ACK)
			&& (millisec <= 5000));
		timestamp_add_now(TS_ME_INFORM_DRAM_END);
	}

	me_fws2 = pci_read_config32(PCH_ME_DEV, PCI_ME_GMES);
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
