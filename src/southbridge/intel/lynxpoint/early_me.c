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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/hlt.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci_ids.h>
#include <string.h>
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

static inline void pci_write_dword_ptr(void *ptr, int offset)
{
	u32 dword = 0;
	memcpy(&dword, ptr, sizeof(dword));
	pci_write_config32(PCH_ME_DEV, offset, dword);
}

void intel_early_me_status(void)
{
	struct me_hfs hfs;
	struct me_hfs2 hfs2;

	pci_read_dword_ptr(&hfs, PCI_ME_HFS);
	pci_read_dword_ptr(&hfs2, PCI_ME_HFS2);

	intel_me_status(&hfs, &hfs2);
}

int intel_early_me_init(void)
{
	int count;
	struct me_uma uma;
	struct me_hfs hfs;

	printk(BIOS_INFO, "Intel ME early init\n");

	/* Wait for ME UMA SIZE VALID bit to be set */
	/* FIXME: ME9 BGW indicates a 5 sec poll timeout. */
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
	u32 pmir = pci_read_config32(PCH_LPC_DEV, PMIR);

	/* CF9GR indicates a Global Reset */
	if (enable)
		pmir |= PMIR_CF9GR;
	else
		pmir &= ~PMIR_CF9GR;

	pci_write_config32(PCH_LPC_DEV, PMIR, pmir);
}

int intel_early_me_init_done(u8 status)
{
	u8 reset;
	int count;
	u32 mebase_l, mebase_h;
	struct me_hfs hfs;
	struct me_did did = {
		.init_done = ME_INIT_DONE,
		.status = status
	};

	/* MEBASE from MESEG_BASE[35:20] */
	mebase_l = pci_read_config32(PCI_CPU_DEVICE, PCI_CPU_MEBASE_L);
	mebase_h = pci_read_config32(PCI_CPU_DEVICE, PCI_CPU_MEBASE_H) & 0xf;
	did.uma_base = (mebase_l >> 20) | (mebase_h << 12);

	/* Send message to ME */
	printk(BIOS_DEBUG, "ME: Sending Init Done with status: %d, "
	       "UMA base: 0x%04x\n", status, did.uma_base);

	pci_write_dword_ptr(&did, PCI_ME_H_GS);

	/*
	 * The ME firmware does not respond with an ACK when NOMEM or ERROR
	 * are sent.
	 */
	if (status == ME_INIT_STATUS_NOMEM || status == ME_INIT_STATUS_ERROR)
		return 0;

	/* Must wait for ME acknowledgement */
	for (count = ME_RETRY; count > 0; --count) {
		pci_read_dword_ptr(&hfs, PCI_ME_HFS);
		if (hfs.bios_msg_ack)
			break;
		udelay(ME_DELAY);
	}
	if (!count) {
		printk(BIOS_ERR, "ERROR: ME failed to respond\n");
		return -1;
	}

	/* Return the requested BIOS action */
	printk(BIOS_NOTICE, "ME: Requested BIOS Action: %s\n",
	       me_ack_values[hfs.ack_data]);

	/* Check status after acknowledgement */
	intel_early_me_status();

	reset = 0;
	switch (hfs.ack_data) {
	case ME_HFS_ACK_CONTINUE:
		/* Continue to boot */
		return 0;
	case ME_HFS_ACK_RESET:
		/* Non-power cycle reset */
		set_global_reset(0);
		reset = 0x06;
		break;
	case ME_HFS_ACK_PWR_CYCLE:
		/* Power cycle reset */
		set_global_reset(0);
		reset = 0x0e;
		break;
	case ME_HFS_ACK_GBL_RESET:
		/* Global reset */
		set_global_reset(1);
		reset = 0x0e;
		break;
	case ME_HFS_ACK_S3:
	case ME_HFS_ACK_S4:
	case ME_HFS_ACK_S5:
		break;
	}

	/* Perform the requested reset */
	if (reset) {
		outb(reset, 0xcf9);
		while (1) {
			hlt();
		}
	}
	return -1;
}
