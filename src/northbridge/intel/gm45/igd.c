/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <option.h>

#include "gm45.h"

/* TODO: Chipset supports Protected Audio Video Path (PAVP) */

/* TODO: We could pass DVMT structure in GetBIOSData() SCI interface */

/* The PEG settings have to be set before ASPM is setup on DMI. */
static void enable_igd(const sysinfo_t *const sysinfo, const int no_peg)
{
	const pci_devfn_t mch_dev = PCI_DEV(0, 0, 0);
	const pci_devfn_t peg_dev = PCI_DEV(0, 1, 0);
	const pci_devfn_t igd_dev = PCI_DEV(0, 2, 0);

	printk(BIOS_DEBUG, "Enabling IGD.\n");

	/* HSync/VSync */
	mchbar_write8(0xbd0 + 3, 0x5a);
	mchbar_write8(0xbd0 + 4, 0x5a);

	static const u16 display_clock_from_f0_and_vco[][4] = {
		  /* VCO 2666    VCO 3200    VCO 4000    VCO 5333 */
		{         222,        228,        222,        222,  },
		{         333,        320,        333,        333,  },
	};
	const int f0_12 = (pci_read_config16(igd_dev, 0xf0) >> 12) & 1;
	const int vco = raminit_read_vco_index();

	pci_update_config16(igd_dev, 0xcc, 0xfc00, display_clock_from_f0_and_vco[f0_12][vco]);

	pci_update_config16(mch_dev, D0F0_GGC, 0xf00f, sysinfo->ggc);

	if ((sysinfo->gfx_type != GMCH_GL40) &&
			(sysinfo->gfx_type != GMCH_GS40) &&
			(sysinfo->gfx_type != GMCH_GL43)) {
		const u32 deven = pci_read_config32(mch_dev, D0F0_DEVEN);
		if (!(deven & 2))
			/* Enable PEG temporarily to access D1:F0. */
			pci_write_config32(mch_dev, D0F0_DEVEN, deven | 2);

		/* Some IGD related settings on D1:F0. */
		pci_and_config16(peg_dev, 0xa08, (u16)~(1 << 15));

		pci_or_config16(peg_dev, 0xa84, 1 << 8);

		pci_or_config16(peg_dev, 0xb00, (3 << 8) | (7 << 3));

		pci_and_config32(peg_dev, 0xb14, ~(1 << 17));

		if (!(deven & 2) || no_peg) {
			/* Disable PEG finally. */
			printk(BIOS_DEBUG, "Finally disabling "
					   "PEG in favor of IGD.\n");
			mchbar_setbits8(0xc14, 1 << 5 | 1 << 0);

			pci_or_config32(peg_dev, 0x200, 1 << 18);

			pci_or_config16(peg_dev, 0x224, 1 << 8);

			pci_and_config32(peg_dev, 0x200, ~(1 << 18));

			while (pci_read_config32(peg_dev, 0x214) & (0xf << 16))
				;

			pci_write_config32(mch_dev, D0F0_DEVEN, deven & ~2);
			mchbar_clrbits8(0xc14, 1 << 5 | 1 << 0);
		}
	}
}

static void disable_igd(const sysinfo_t *const sysinfo)
{
	const pci_devfn_t mch_dev = PCI_DEV(0, 0, 0);

	printk(BIOS_DEBUG, "Disabling IGD.\n");

	/* Disable Graphics Stolen Memory. */
	pci_update_config16(mch_dev, D0F0_GGC, 0xff0f, 0x0002);

	mchbar_setbits8(0xf10, 1 << 0);

	if (!(pci_read_config8(mch_dev, D0F0_CAPID0 + 4) & (1 << (33 - 32)))) {
		mchbar_setbits16(0x1190, 1 << 14);
		mchbar_clrsetbits16(0x119e, 7 << 13, 4 << 13);
		mchbar_setbits16(0x119e, 1 << 12);
	}

	/* Hide IGD. */
	pci_and_config32(mch_dev, D0F0_DEVEN, ~(3 << 3));
}

void init_igd(const sysinfo_t *const sysinfo)
{
	const pci_devfn_t mch_dev = PCI_DEV(0, 0, 0);

	const u8 capid = pci_read_config8(mch_dev, D0F0_CAPID0 + 4);
	if (!sysinfo->enable_igd || (capid & (1 << (33 - 32))))
		disable_igd(sysinfo);
	else
		enable_igd(sysinfo, !sysinfo->enable_peg);
}

void igd_compute_ggc(sysinfo_t *const sysinfo)
{
	const pci_devfn_t mch_dev = PCI_DEV(0, 0, 0);

	const u32 capid = pci_read_config32(mch_dev, D0F0_CAPID0 + 4);
	if (!sysinfo->enable_igd || (capid & (1 << (33 - 32))))
		sysinfo->ggc = 0x0002;
	else {
		/* 4 for 32MB, default if not set in CMOS */
		u8 gfxsize = 4;

		/* Graphics Stolen Memory: 2MB GTT (0x0300) when VT-d disabled,
		   2MB GTT + 2MB shadow GTT (0x0b00) else. */
		get_option(&gfxsize, "gfx_uma_size");
		/* Handle invalid CMOS settings */
		/* Only allow settings between 32MB and 352MB */
		gfxsize = MIN(MAX(gfxsize, 4), 12);

		sysinfo->ggc = 0x0300 | ((gfxsize + 1) << 4);
		if (!(capid & (1 << (48 - 32))))
			sysinfo->ggc |= 0x0800;
	}
}
