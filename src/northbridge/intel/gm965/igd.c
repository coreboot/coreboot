/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - IGD Init
 *
 * Reverse-engineered from ThinkPad X61 Phoenix BIOS bioscode_7.rom,
 * function at file offsets 0x36C0-0x39D1 (PEG negotiation + IGD setup).
 *
 * The vendor code accumulates flags in a byte (cs:0x3608) during PEG
 * link training.  Bits [5:4] both set means PEG is absent or failed
 * link training -- i.e., IGD-only mode.  The subsequent IGD init at
 * offset 0x3850 uses those flags to select one of two paths:
 *
 *   - PEG+IGD path (flags[5:4] != 0x30): PEG present and trained.
 *     Sets PM_F10 bit 0, programs IGD scheduling (0x1190/0x119E).
 *
 *   - IGD-only path (flags[5:4] == 0x30): No PEG, IGD only.
 *     Writes HSync/VSync (BD0=0xFD), computes display clock from a
 *     VCO/FSB table and writes D2:F0 config 0xCC, sets PM_CTRL0 bit 31.
 *
 * The X61 has no discrete GPU, so it always takes the IGD-only path.
 * The vendor dump confirms: BD0=0xFD, F10 bit 0 absent, no 0x1190.
 */

#include <stdint.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <option.h>

#include "gm965.h"

/*
 * Display clock table extracted from bioscode_7.rom at file offset 0x3609.
 * Indexed by [vco_index][fsb_code], where:
 *   vco_index = ((D2:F0 config 0x1F0 >> 8) & 0x1F) - 1
 *   fsb_code  = HPLLVCO bits [2:0] (0=unused, 1=800MHz, 2=667MHz)
 *
 * Values are display clock dividers written to D2:F0 config 0xCC bits [9:0].
 */
static const uint16_t display_clock_table[3][4] = {
	/* VCO 1 */  { 200, 200, 222,   0 },
	/* VCO 2 */  { 320, 333, 333,   0 },
	/* VCO 3 */  { 400, 400, 381,   0 },
};

/*
 * igd_compute_ggc() - Compute GGC register value from user CMOS option
 *
 * Sets sysinfo->ggc based on the gfx_uma_size NVRAM option.  The value
 * is later written to D0:F0 offset 0x52 (GGC) by program_memory_map()
 * in raminit.c so that the hardware places BSM (Base of Stolen Memory)
 * at the right address below TOLUD.
 *
 * GM965 uses the same GMS encoding as i945.  gfx_uma_size maps to the
 * GMS field as (gfxsize + 1), so the default of 4 gives GMS=5=32 MB,
 * matching the GM45 convention:
 *
 *   gfx_uma_size ->  GMS -> stolen size
 *          0     ->   1  ->   1 MB
 *          1     ->   2  ->   4 MB
 *          2     ->   3  ->   8 MB
 *          3     ->   4  ->  16 MB
 *          4     ->   5  ->  32 MB  (default, same as GM45)
 *          5     ->   6  ->  48 MB
 *          6     ->   7  ->  64 MB
 *
 * Unlike GM45, GM965 has no separate GTT stolen region (GGMS), so
 * only the GMS field (bits [6:4]) is set in GGC; bits [7] and [11:8] stay 0.
 *
 * coreboot equivalent: igd_compute_ggc() in gm45/igd.c
 */
u16 igd_compute_ggc(void)
{
	const uint32_t deven = pci_read_config32(D0F0, D0F0_DEVEN);

	if (!(deven & DEVEN_D2F0)) {
		/* IGD is not present/enabled: set IVD (bit 1), GMS = 0 */
		printk(BIOS_DEBUG, "IGD: disabled, GGC=0x0002 (no stolen memory)\n");
		return 0x0002;
	}

	/*
	 * Read user option.  Default 4 -> GMS=5 -> 32 MB, matching the
	 * GM45 convention.  Clamp to 0-6 so GMS stays within 1-7.
	 */
	u8 gfxsize = get_uint_option("gfx_uma_size", 4);
	if (gfxsize > 6) {
		printk(BIOS_WARNING, "IGD: gfx_uma_size %u out of range, "
		       "using default (32 MB)\n", gfxsize);
		gfxsize = 4;
	}

	const u8 gms = gfxsize + 1;
	const u16 ggc = (u16)(gms << 4);

	printk(BIOS_DEBUG, "IGD: gfx_uma_size=%u GMS=%u GGC=0x%04x (%u MB stolen)\n",
	       gfxsize, gms, ggc,
	       decode_igd_memory_size(gms) >> 10);

	return ggc;
}

/*
 * gm965_igd_init() - Configure Integrated Graphics Device
 *
 * Must be called after gm965_pm_init() and PEG/PCIe init.
 * Vendor BIOS: bioscode_7.rom PEG negotiation (0x36C0) flows into
 * IGD init (0x3850).
 */
void gm965_igd_init(const sysinfo_t *si)
{
	const uint32_t deven = pci_read_config32(D0F0, D0F0_DEVEN);
	const int peg_enabled = (deven & DEVEN_D1F0) &&
				(pci_read_config16(D1F0, 0x00) != 0xffff);

	printk(BIOS_DEBUG, "GM965 IGD init: DEVEN=0x%08x PEG %s\n",
	       deven, peg_enabled ? "present" : "absent");

	if (peg_enabled) {
		/*
		 * PEG+IGD path: PEG device present and trained.
		 * Vendor: offset 0x3872-0x38D4 (flags[5:4] != 0x30)
		 *
		 * 1. Set PM_F10 bit 0 (IGD enable in PM controller)
		 * 2. Program IGD scheduling registers
		 */

		/* PM_F10 |= 0x01.  Vendor: offset 0x3872-0x3878 */
		mchbar_setbits8(PM_F10, 1 << 0);

		/*
		 * IGD scheduling registers.
		 * Vendor: offset 0x38A5-0x38D2
		 * Conditioned on HPLLVCO bit 7 clear (always true on X61).
		 */
		{
			const uint8_t hpllvco = mchbar_read8(HPLLVCO_MCHBAR);
			if (!(hpllvco & 0x80)) {
				mchbar_setbits32(IOSCHED_190, 1 << 14);
				mchbar_setbits16(IOSCHED_19E, (1 << 15) | (1 << 12));
			}
		}

		printk(BIOS_DEBUG, "GM965 IGD init: PEG+IGD path done "
		       "(F10=0x%02x)\n", mchbar_read8(PM_F10));
	} else {
		/*
		 * IGD-only path: No PEG device (X61 case).
		 * Vendor: offset 0x38D7-0x39CF (flags[5:4] == 0x30)
		 *
		 * 1. Write HSync/VSync = 0xFD to MCHBAR+0xBD0/BD4
		 * 2. Compute display clock from VCO/FSB table
		 * 3. Set PM_CTRL0 bit 31
		 */

		/*
		 * HSync/VSync: write 0xFD to BD0 byte 3 and BD4 byte 0.
		 * Vendor: offset 0x38EC-0x38FB
		 */
		mchbar_write32(IGD_HSYNC_VSYNC, 0xfd000000);
		mchbar_write8(IGD_HSYNC_VSYNC + 4, 0xfd);

		/*
		 * Display clock computation.
		 * Vendor: offset 0x3901-0x3944
		 *
		 * The Phoenix helper encodes access width in DX upper bits:
		 *   dx=0x01F0 = WORD read from offset 0xF0 (bit 8 = word)
		 * So the vendor reads VCO from D2:F0 config 0xF0 (GCFGC)
		 * bits[12:8], NOT 0x1F0.  GCFGC is in standard PCI config
		 * space and is accessible at romstage (D2F0 enabled in DEVEN).
		 * It's the same register raminit's program_gcfgc() writes.
		 *
		 * fsb_bits comes from HPLLVCO (MCHBAR+0x0C0F) bits[2:0].
		 *
		 * index = (vco_field - 1) * 4 + fsb_bits
		 * clock_val = display_clock_table[vco_field - 1][fsb_bits]
		 * D2:F0 config 0xCC = (old & 0xFC00) | clock_val
		 */
		{
			const uint16_t gcfgc = pci_read_config16(D2F0, GCFGC_OFFSET);
			const uint8_t hpllvco = mchbar_read8(HPLLVCO_MCHBAR);
			const unsigned int vco_field = (gcfgc >> 8) & 0x1f;
			const unsigned int fsb_bits = hpllvco & 0x07;

			printk(BIOS_DEBUG, "GM965 IGD: GCFGC=0x%04x "
			       "HPLLVCO=0x%02x VCO=%d FSB=%d\n",
			       gcfgc, hpllvco, vco_field, fsb_bits);

			if (vco_field >= 1 && vco_field <= 3 && fsb_bits <= 3) {
				const uint16_t clock_val =
					display_clock_table[vco_field - 1][fsb_bits];

				if (clock_val) {
					uint16_t cc = pci_read_config16(D2F0, 0xcc);
					cc = (cc & 0xfc00) | clock_val;
					pci_write_config16(D2F0, 0xcc, cc);

					printk(BIOS_DEBUG, "GM965 IGD: display clock "
					       "= %d (VCO=%d FSB=%d)\n",
					       clock_val, vco_field, fsb_bits);
				}
			} else if (vco_field == 7) {
				/*
				 * VCO field 7: special case handled by raminit's
				 * program_gcfgc() which programs MCHBAR 0x1190/119E
				 * instead.  No display clock write needed here.
				 */
				printk(BIOS_DEBUG, "GM965 IGD: VCO=7, display "
				       "clock handled by raminit\n");
			} else {
				printk(BIOS_WARNING, "GM965 IGD: unexpected "
				       "VCO=%d FSB=%d, skipping display clock\n",
				       vco_field, fsb_bits);
			}
		}

		/*
		 * PM_CTRL0 bit 31.
		 * Vendor: offset 0x3A46-0x3A5C
		 * Set only on IGD-only path (not when PEG is present).
		 */
		mchbar_setbits32(PM_CTRL0, 1 << 31);

		printk(BIOS_DEBUG, "GM965 IGD init: IGD-only path done "
		       "(BD0=0x%08x CTRL0=0x%08x)\n",
		       mchbar_read32(IGD_HSYNC_VSYNC),
		       mchbar_read32(PM_CTRL0));
	}
}
