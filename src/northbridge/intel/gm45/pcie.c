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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <console/console.h>

#include "gm45.h"

static void init_egress(void)
{
	/* VC0: TC0 only */
	EPBAR8(0x14) &= 1;
	EPBAR8(0x4) = (EPBAR8(0x4) & ~7) | 1;

	/* VC1: isoch */
	EPBAR32(0x28) = 0x0a0a0a0a;
	EPBAR32(0x1c) = (EPBAR32(0x1c) & ~(127 << 16)) | (0x0a << 16);

	/* VC1: ID1, TC7 */
	EPBAR32(0x20) = (EPBAR32(0x20) & ~(7 << 24)) | (1 << 24);
	EPBAR8(0x20) = (EPBAR8(0x20) & 1) | (1 << 7);

	/* VC1 ARB table: setup and enable */
	EPBAR32(0x100) = 0x55555555;
	EPBAR32(0x104) = 0x55555555;
	EPBAR32(0x108) = 0x55555555;
	EPBAR32(0x10c) = 0x55555555;
	EPBAR32(0x110) = 0x55555555;
	EPBAR32(0x114) = 0x55555555;
	EPBAR32(0x118) = 0x55555555;
	EPBAR32(0x11c) = 0x00005555;
	EPBAR32(0x20) |= 1 << 16;

	while ((EPBAR8(0x26) & 1) != 0) ;

	/* VC1: enable */
	EPBAR32(0x20) |= 1 << 31;

	while ((EPBAR8(0x26) & 2) != 0) ;
}

/* MCH side */
/* b2step: b2 stepping or higher */
static void init_dmi(int b2step)
{
	/* VC0: TC0 only */
	DMIBAR8(DMIVC0RCTL) &= 1;
	DMIBAR8(0x4) = (DMIBAR8(0x4) & ~7) | 1;

	/* VC1: ID1, TC7 */
	DMIBAR32(0x20) = (DMIBAR32(0x20) & ~(7 << 24)) | (1 << 24);
	DMIBAR8(0x20) = (DMIBAR8(0x20) & 1) | (1 << 7);

	/* VC1: enable */
	DMIBAR32(0x20) |= 1 << 31;

	while ((DMIBAR8(0x26) & 2) != 0) ;

	/* additional configuration. */
	DMIBAR32(0x200) |= 3 << 13;
	DMIBAR32(0x200) &= ~(1 << 21);
	DMIBAR32(0x200) = (DMIBAR32(0x200) & ~(3 << 26)) | (2 << 26);
	DMIBAR32(0x2c) = 0x86000040;
	DMIBAR32(0xfc) |= 1 << 0;
	DMIBAR32(0xfc) |= 1 << 1;
	DMIBAR32(0xfc) |= 1 << 4;
	if (!b2step) {
		DMIBAR32(0xfc) |= 1 << 11;
	} else {
		DMIBAR32(0xfc) &= ~(1 << 11);
	}
	DMIBAR32(0x204) &= ~(3 << 10);
	DMIBAR32(0xf4) &= ~(1 << 4);
	DMIBAR32(0xf0) |= 3 << 24;
	DMIBAR32(0xf04) = 0x07050880;
	DMIBAR32(0xf44) = 0x07050880;
	DMIBAR32(0xf84) = 0x07050880;
	DMIBAR32(0xfc4) = 0x07050880;

	/* lock down write-once registers
	   DMIBAR32(0x84) will be set in setup_aspm(). */
	DMIBAR32(0x308) = DMIBAR32(0x308);
	DMIBAR32(0x314) = DMIBAR32(0x314);
	DMIBAR32(0x324) = DMIBAR32(0x324);
	DMIBAR32(0x328) = DMIBAR32(0x328);
	DMIBAR32(0x334) = DMIBAR32(0x334);
	DMIBAR32(0x338) = DMIBAR32(0x338);
}

static void init_pcie(const int peg_enabled,
		      const int sdvo_enabled,
		      const int peg_x16)
{
	u8 tmp8;
	u16 tmp16;
	u32 tmp;
	const device_t mch = PCI_DEV(0, 0, 0);
	const device_t pciex = PCI_DEV(0, 1, 0);

	printk(BIOS_DEBUG, "PEG x%d %s, SDVO %s\n", peg_x16?16:1,
		peg_enabled?"enabled":"disabled",
		sdvo_enabled?"enabled":"disabled");

	if (peg_enabled) {
		tmp8 = pci_read_config8(mch, D0F0_DEVEN) | (1 << 1);
		pci_write_config8(mch, D0F0_DEVEN, tmp8);

		tmp8 = pci_read_config8(pciex, 0x224) & ~31;
		pci_write_config8(pciex, 0x224, tmp8 | (peg_x16?16:0) | 1);

		tmp16 = pci_read_config16(pciex, 0x224) & ~(1 << 8);
		pci_write_config16(pciex, 0x224, tmp16);

		/* FIXME: fill in: slot or fixed? -> devicetree */
		int peg_is_slot = 0;
		if (peg_is_slot) {
			tmp16 = pci_read_config16(pciex, PEG_CAP) | (1 << 8);
			pci_write_config16(pciex, PEG_CAP, tmp16);
		}

		/* FIXME: fill in: slot number, slot power -> devicetree */
		/* Use slot number 0 by now, slots on sb count from 1. */
		int peg_slot = 0; /* unique within chassis */
		/* peg_power := val * 10^-exp */
		int peg_power_val = 75;
		int peg_power_exp = 0; /* 0..3 */
		tmp = (peg_slot << 17) | (peg_power_exp << 15) |
			(peg_power_val << 7);
		pci_write_config32(pciex, SLOTCAP, tmp);

		/* GPEs */
		tmp8 = pci_read_config8(pciex, PEGLC) | 7;
		pci_write_config8(pciex, PEGLC, tmp8);

		/* VC0: TC0 only, VC0 only */
		tmp8 = pci_read_config8(pciex, D1F0_VC0RCTL);
		pci_write_config8(pciex, D1F0_VC0RCTL, tmp8 & 1);

		tmp8 = pci_read_config8(pciex, D1F0_VCCAP);
		pci_write_config8(pciex, D1F0_VCCAP, tmp8 & ~7);
	}
}

static void setup_aspm(const stepping_t stepping, const int peg_enabled)
{
	u32 tmp32;
	const device_t pciex = PCI_DEV(0, 1, 0);

	/* Prerequisites for ASPM: */
	if (peg_enabled) {
		tmp32 = pci_read_config32(pciex, 0x200) | (3 << 13);
		pci_write_config32(pciex, 0x200, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0f0);
		tmp32 &= ~((1 << 27) | (1 << 26));
		pci_write_config32(pciex, 0x0f0, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0f0) | (3 << 24);
		pci_write_config32(pciex, 0x0f0, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0f4) & ~(1 << 4);
		pci_write_config32(pciex, 0x0f4, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0fc) | (1 << 0);
		pci_write_config32(pciex, 0x0fc, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0fc) | (1 << 1);
		pci_write_config32(pciex, 0x0fc, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0fc) | (1 << 4);
		pci_write_config32(pciex, 0x0fc, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0fc) & ~(7 << 5);
		pci_write_config32(pciex, 0x0fc, tmp32);

		/* Set L0s, L1 supported in LCTL? */
		tmp32 = pci_read_config32(pciex, 0x0b0) | (3 << 0);
		pci_write_config32(pciex, 0x0b0, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0f0) | (3 << 24);
		pci_write_config32(pciex, 0x0f0, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0f0);
		if ((stepping >= STEPPING_B0) && (stepping <= STEPPING_B1))
			tmp32 |= (1 << 31);
		else if (stepping >= STEPPING_B2)
			tmp32 &= ~(1 << 31);
		pci_write_config32(pciex, 0x0f0, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0fc);
		if ((stepping >= STEPPING_B0) && (stepping <= STEPPING_B1))
			tmp32 |= (1 << 10);
		else if (stepping >= STEPPING_B2)
			tmp32 &= ~(1 << 10);
		pci_write_config32(pciex, 0x0fc, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0fc);
		if (stepping >= STEPPING_B2)
			tmp32 |= (1 << 14);
		pci_write_config32(pciex, 0x0fc, tmp32);

		tmp32 = pci_read_config32(pciex, 0x0fc);
		if (stepping >= STEPPING_B1)
			tmp32 &= ~(1 << 13);
		pci_write_config32(pciex, 0x0fc, tmp32);
	}
	DMIBAR8 (0x0e1c) |= (1 <<  0);
	DMIBAR16(0x0f00) |= (3 <<  8);
	DMIBAR16(0x0f00) |= (7 <<  3);
	DMIBAR32(0x0f14) &= ~(1 << 17);
	DMIBAR16(0x0e1c) &= ~(1 <<  8);
	if (stepping >= STEPPING_B0) {
		DMIBAR32(0x0e28 + 4) = (DMIBAR32(0x0e28 + 4) &
						~(0xf << (52 - 32))) |
					(0xd << (52 - 32));
		DMIBAR32(0x0e2c) = 0x88d07333;
	}
	if (peg_enabled) {
		tmp32 = pci_read_config32(pciex, 0xa08) & ~(1 << 15);
		pci_write_config32(pciex, 0xa08, tmp32);

		tmp32 = pci_read_config32(pciex, 0xa84) | (1 << 8);
		pci_write_config32(pciex, 0xa84, tmp32);

		tmp32 = pci_read_config32(pciex, 0xb14) & ~(1 << 17);
		pci_write_config32(pciex, 0xb14, tmp32);

		tmp32 = pci_read_config32(pciex, 0xb00) | (3 << 8);
		pci_write_config32(pciex, 0xb00, tmp32);

		tmp32 = pci_read_config32(pciex, 0xb00) | (7 << 3);
		pci_write_config32(pciex, 0xb00, tmp32);

		tmp32 = pci_read_config32(pciex, 0xa84) & ~(1 << 8);
		pci_write_config32(pciex, 0xa84, tmp32);

		tmp32 = pci_read_config32(pciex, 0xa84) | (1 << 8);
		pci_write_config32(pciex, 0xa84, tmp32);

		tmp32 = pci_read_config32(pciex, 0xb04);
		tmp32 = (tmp32 & ~(0x1f << 23)) | (0xe << 23);
		pci_write_config32(pciex, 0xb04, tmp32);

		tmp32 = pci_read_config32(pciex, 0xb04);
		tmp32 |= (1 << 31);
		pci_write_config32(pciex, 0xb04, tmp32);

		tmp32 = pci_read_config32(pciex, 0xb04);
		tmp32 = (tmp32 & ~(0x03 << 29)) | (0x1 << 29);
		pci_write_config32(pciex, 0xb04, tmp32);
	}


	/*\ Setup ASPM on DMI \*/

	/* Exit latencies should be checked to be supported by
	   the endpoint (ICH), but ICH doesn't give any limits. */

	if (LPC_IS_MOBILE(PCI_DEV(0, 0x1f, 0)))
		DMIBAR8(0x88) |= (3 << 0); // enable ASPM L0s, L1 (write-once)
	else
		DMIBAR8(0x88) |= (1 << 0); // enable ASPM L0s (write-once)
	/* timing */
	DMIBAR32(0x84) = (DMIBAR32(0x84) & ~(63 << 12)) | (2 << 12) | (2 << 15);
	DMIBAR8(0x208 + 3) = 0;
	DMIBAR32(0x208) &= ~(3 << 20);


	/*\ Setup ASPM on PEG \*/
	/*
	 * Maybe we just have to advertise ASPM through LCAP[11:10]
	 * (LCAP[17:15] == 010b is the default, will be locked, as it's R/WO),
	 * set 0x208[31:24,23:22] to zero, 0x224[24:21] = 1 and let the
	 * generic ASPM code do the rest? – Nico
	 */
	/* TODO: Prepare PEG for ASPM. */
}

static void setup_rcrb(const int peg_enabled)
{
	/*\ RCRB setup: Egress Port \*/

	/* Set component ID of MCH (1). */
	EPBAR8(EPESD + 2) = 1;

	/* Link1: component ID 1, link valid. */
	EPBAR32(EPLE1D) = (EPBAR32(EPLE1D) & 0xff000000) | (1 << 16) | (1 << 0);
	EPBAR32(EPLE1A) = DEFAULT_DMIBAR;

	if (peg_enabled)
		/* Link2: link_valid. */
		EPBAR8(EPLE2D) |= (1 << 0); /* link valid */


	/*\ RCRB setup: DMI Port \*/

	/* Set component ID of MCH (1). */
	DMIBAR8(DMIESD + 2) = 1;

	/* Link1: target port 0, component id 2 (ICH), link valid. */
	DMIBAR32(DMILE1D) = (0 << 24) | (2 << 16) | (1 << 0);
	DMIBAR32(DMILE1A) = DEFAULT_RCBA;

	/* Link2: component ID 1 (MCH), link valid */
	DMIBAR32(DMILE2D) =
		(DMIBAR32(DMILE2D) & 0xff000000) | (1 << 16) | (1 << 0);
	DMIBAR32(DMILE2A) = DEFAULT_MCHBAR;
}

void gm45_late_init(const stepping_t stepping)
{
	const device_t mch = PCI_DEV(0, 0, 0);
	const int peg_enabled = (pci_read_config8(mch, D0F0_DEVEN) >> 1) & 1;
	const int sdvo_enabled = (MCHBAR16(0x40) >> 8) & 1;
	const int peg_x16 = (peg_enabled && !sdvo_enabled);

	init_egress();
	init_dmi(stepping >= STEPPING_B2);
	init_pcie(peg_enabled, sdvo_enabled, peg_x16);

	setup_aspm(stepping, peg_enabled);
	setup_rcrb(peg_enabled);
}
