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
#include <arch/io.h>
#include <device/pci_def.h>
#include <console/console.h>

#include "gm45.h"

/* TODO: Chipset supports Protected Audio Video Path (PAVP) */

/* TODO: We could pass DVMT structure in GetBIOSData() SCI interface */

/* The PEG settings have to be set before ASPM is setup on DMI. */
static void enable_igd(const sysinfo_t *const sysinfo, const int no_peg)
{
	const device_t mch_dev	= PCI_DEV(0, 0, 0);
	const device_t peg_dev	= PCI_DEV(0, 1, 0);
	const device_t igd_dev	= PCI_DEV(0, 2, 0);

	u16 reg16;
	u32 reg32;

	printk(BIOS_DEBUG, "Enabling IGD.\n");

	/* HSync/VSync */
	MCHBAR8(0xbd0 + 3) = 0x5a;
	MCHBAR8(0xbd0 + 4) = 0x5a;

	static const u16 display_clock_from_f0_and_vco[][4] = {
		  /* VCO 2666    VCO 3200    VCO 4000    VCO 5333 */
		{         222,        228,        222,        222,  },
		{         333,        320,        333,        333,  },
	};
	const int f0_12 = (pci_read_config16(igd_dev, 0xf0) >> 12) & 1;
	const int vco = raminit_read_vco_index();
	reg16 = pci_read_config16(igd_dev, 0xcc);
	reg16 &= 0xfc00;
	reg16 |= display_clock_from_f0_and_vco[f0_12][vco];
	pci_write_config16(igd_dev, 0xcc, reg16);

	/* Graphics Stolen Memory: 2MB GTT (0x0300) when VT-d disabled,
	                           2MB GTT + 2MB shadow GTT (0x0b00) else. */
	/* Graphics Mode Select: 32MB framebuffer (0x0050) */
	/* TODO: We could switch to 64MB (0x0070), config flag? */
	const u32 capid = pci_read_config32(mch_dev, D0F0_CAPID0 + 4);
	reg16 = pci_read_config16(mch_dev, D0F0_GGC);
	reg16 &= 0xf00f;
	reg16 |= 0x0350;
	if (!(capid & (1 << (48 - 32))))
		reg16 |= 0x0800;
	pci_write_config16(mch_dev, D0F0_GGC, reg16);

	if ((sysinfo->gfx_type != GMCH_GL40) &&
			(sysinfo->gfx_type != GMCH_GS40) &&
			(sysinfo->gfx_type != GMCH_GL43)) {
		const u32 deven = pci_read_config32(mch_dev, D0F0_DEVEN);
		if (!(deven & 2))
			/* Enable PEG temporarily to access D1:F0. */
			pci_write_config32(mch_dev, D0F0_DEVEN, deven | 2);

		/* Some IGD related settings on D1:F0. */
		reg16 = pci_read_config16(peg_dev, 0xa08);
		reg16 &= ~(1 << 15);
		pci_write_config16(peg_dev, 0xa08, reg16);

		reg16 = pci_read_config16(peg_dev, 0xa84);
		reg16 |= (1 << 8);
		pci_write_config16(peg_dev, 0xa84, reg16);

		reg16 = pci_read_config16(peg_dev, 0xb00);
		reg16 |= (3 << 8) | (7 << 3);
		pci_write_config16(peg_dev, 0xb00, reg16);

		reg32 = pci_read_config32(peg_dev, 0xb14);
		reg32 &= ~(1 << 17);
		pci_write_config32(peg_dev, 0xb14, reg32);

		if (!(deven & 2) || no_peg) {
			/* Disable PEG finally. */
			printk(BIOS_DEBUG, "Finally disabling "
					   "PEG in favor of IGD.\n");
			MCHBAR8(0xc14) |= (1 << 5) | (1 << 0);

			reg32 = pci_read_config32(peg_dev, 0x200);
			reg32 |= (1 << 18);
			pci_write_config32(peg_dev, 0x200, reg32);
			reg16 = pci_read_config16(peg_dev, 0x224);
			reg16 |= (1 << 8);
			pci_write_config16(peg_dev, 0x224, reg16);
			reg32 = pci_read_config32(peg_dev, 0x200);
			reg32 &= ~(1 << 18);
			pci_write_config32(peg_dev, 0x200, reg32);
			while (pci_read_config32(peg_dev, 0x214) & 0x000f0000);

			pci_write_config32(mch_dev, D0F0_DEVEN, deven & ~2);
			MCHBAR8(0xc14) &= ~((1 << 5) | (1 << 0));
		}
	}
}

static void disable_igd(const sysinfo_t *const sysinfo)
{
	const device_t mch_dev	= PCI_DEV(0, 0, 0);

	printk(BIOS_DEBUG, "Disabling IGD.\n");

	u16 reg16;

	reg16 = pci_read_config16(mch_dev, D0F0_GGC);
	reg16 &= 0xff0f; /* Disable Graphics Stolen Memory. */
	reg16 |= 0x0002; /* Disable IGD. */
	pci_write_config16(mch_dev, D0F0_GGC, reg16);
	MCHBAR8(0xf10) |= (1 << 0);

	if (!(pci_read_config8(mch_dev, D0F0_CAPID0 + 4) & (1 << (33 - 32)))) {
		MCHBAR16(0x1190) |= (1 << 14);
		MCHBAR16(0x119e) = (MCHBAR16(0x119e) & ~(7 << 13)) | (4 << 13);
		MCHBAR16(0x119e) |= (1 << 12);
	}
}

void init_igd(const sysinfo_t *const sysinfo,
	      const int no_igd, const int no_peg)
{
	const device_t mch_dev	= PCI_DEV(0, 0, 0);

	const u8 capid = pci_read_config8(mch_dev, D0F0_CAPID0 + 4);
	if (no_igd || (capid & (1 << (33 - 32))))
		disable_igd(sysinfo);
	else
		enable_igd(sysinfo, no_peg);
}
