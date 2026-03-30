/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - Power Management Init
 *
 * Reverse-engineered from ThinkPad X61 Phoenix BIOS bioscode_5.rom,
 * function at file offset 0x0050-0x025C (16-bit real-mode, 32-bit
 * MCHBAR accesses via edi=0xFED14000 with es: segment override).
 *
 * This is the GM965 equivalent of GM45's init_pm() in pm.c.
 * The register sequence and FSB-indexed lookup tables are
 * GM965-specific and differ from GM45.
 *
 * Without this function, the memory controller operates without
 * proper power state management, causing intermittent data
 * corruption under sustained load (e.g., memtest86+ failures).
 *
 * Vendor BIOS calls this from bioscode_5.rom after raminit and
 * DMI/PCIe init have completed, but before the payload.
 */

#include <stdint.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include <console/console.h>

#include "gm965.h"

#define MSR_EXTENDED_CONFIG 0xee

/*
 * FSB-indexed lookup tables extracted from bioscode_5.rom header
 * at file offsets 0x04-0x4B.
 *
 */

/* HGIPMC2 upper 16 bits (written to MCHBAR+0x0C3A) */
static const uint16_t hgipmc2_hi_table[] = {
	[FSB_CLOCK_533MHz] = 0x0c3d,
	[FSB_CLOCK_667MHz] = 0x0f4c,
	[FSB_CLOCK_800MHz] = 0x125c,
};

/* HGIPMC2 lower 16 bits (written to MCHBAR+0x0C38) */
static const uint16_t hgipmc2_lo_table[] = {
	[FSB_CLOCK_533MHz] = 0x0bb8,
	[FSB_CLOCK_667MHz] = 0x0ea6,
	[FSB_CLOCK_800MHz] = 0x1194,
};

/* CLKCFG_C16 byte value (OR'd into MCHBAR+0x0C16 low 7 bits) */
static const uint8_t clkcfg_c16_table[] = {
	[FSB_CLOCK_533MHz] = 0x0b,
	[FSB_CLOCK_667MHz] = 0x0d,
	[FSB_CLOCK_800MHz] = 0x10,
};

/* C2/C3 idle timer 0 (written to MCHBAR+0x0F00) */
static const uint32_t pm_f00_table[] = {
	[FSB_CLOCK_533MHz] = 0x00000480,
	[FSB_CLOCK_667MHz] = 0x00000600,
	[FSB_CLOCK_800MHz] = 0x00000700,
};

/* C2/C3 idle timer 1 (written to MCHBAR+0x0F04) */
static const uint32_t pm_f04_table[] = {
	[FSB_CLOCK_533MHz] = 0x00001780,
	[FSB_CLOCK_667MHz] = 0x00001d80,
	[FSB_CLOCK_800MHz] = 0x00002380,
};

/*
 * gm965_pm_init() - Configure MCH power management
 *
 * Must be called after raminit completes but before the payload.
 * Vendor BIOS: bioscode_5.rom offset 0x0050, called post-raminit.
 *
 * Reads MCH stepping from D0:F0+0x08 and FSB clock from CLKCFG
 * bits [2:0] to select stepping-dependent and clock-dependent
 * register values.
 */
void gm965_pm_init(const sysinfo_t *si)
{
	const uint8_t stepping = northbridge_stepping();
	const unsigned int fsb = si->timings.fsb_clock;

	printk(BIOS_DEBUG, "GM965 PM init: stepping=0x%02x fsb=%d\n",
	       stepping, fsb);

	/*
	 * Step 1: CLKCFG_C14 = 0x0010 (16-bit write).
	 * If PEG device (D1:F0) is absent (reads 0xFF), OR in 0x21.
	 *
	 * Vendor: offset 0x0089-0x00A9
	 *   [edi+0xc14] = 0x10
	 *   if (D1:F0 config[0] == 0xff): [edi+0xc14] |= 0x21
	 *
	 * (0xC14 is excluded from diff analysis per user, but we
	 * program it here for completeness as the vendor does.)
	 */
	mchbar_write16(CLKCFG_C14, 0x0010);
	if (pci_read_config8(D1F0, 0x00) == 0xff)
		mchbar_setbits16(CLKCFG_C14, 0x21);

	/* Step 2: CLKCFG_C20 = 0x0001.  Vendor: offset 0x00AB */
	mchbar_write16(CLKCFG_C20, 0x0001);

	/*
	 * Step 3: UPMC3 - stepping-dependent PM control word.
	 *
	 * Vendor: offset 0x00B5-0x00F8
	 *   stepping 0 (A0) + NOT (FSB667 && memclk==0x30): 0x041f06fd
	 *   stepping 1 (A1):                                 0x041f0efd
	 *   stepping >= 2 (B0+):                             0x061f0efd
	 *
	 * The X61 uses C0 stepping (0x03), so the B0+ path applies.
	 */
	if (stepping == 0x00) {
		/* A0 has a special case for FSB667+memclk combo */
		mchbar_write32(UPMC3, 0x041f06fd);
	} else if (stepping == 0x01) {
		mchbar_write32(UPMC3, 0x041f0efd);
	} else {
		/* B0 and later (including C0 on X61) */
		mchbar_write32(UPMC3, 0x061f0efd);
	}

	/* Step 4: GIPMC1 = 0x03 (byte).  Vendor: offset 0x00FA */
	mchbar_write8(GIPMC1, 0x03);

	/* Step 5: PM_F10 |= 0x02 (set bit 1).  Vendor: offset 0x0103 */
	mchbar_setbits8(PM_F10, 1 << 1);

	/*
	 * Step 6: HGIPMC2 upper (0xC3A) from FSB table.
	 * Vendor: offset 0x010C-0x0121
	 */
	mchbar_write16(HGIPMC2_HI, hgipmc2_hi_table[fsb]);

	/*
	 * Step 7: CLKCFG_C16 - keep bit 7, set FSB-dependent value.
	 * Vendor: offset 0x0122-0x013B
	 */
	mchbar_clrsetbits8(CLKCFG_C16, 0x7f, clkcfg_c16_table[fsb]);

	/* Step 8: FSBPMC1 = 0x03 (byte).  Vendor: offset 0x013C */
	mchbar_write8(FSBPMC1, 0x03);

	/*
	 * Step 9: HGIPMC2 lower (0xC38) from FSB table.
	 * Vendor: offset 0x0145-0x0155
	 *
	 * CRITICAL: Without this, the host-graphics PM handshake
	 * timing is wrong (stale reset default 0x1f40 instead of
	 * the correct FSB-dependent value).
	 */
	mchbar_write16(HGIPMC2_LO, hgipmc2_lo_table[fsb]);

	/*
	 * Step 10: PM_F10 |= 0x20 (set bit 5).  Vendor: offset 0x0156
	 *
	 * CRITICAL: Bit 5 coordinates PM transitions with the memory
	 * interface.  Must be set AFTER HGIPMC2 is programmed.
	 */
	mchbar_setbits8(PM_F10, 1 << 5);

	/*
	 * Step 11: CLKCFG_C16 bits [13:12] = 0x3 (set both).
	 * Vendor: offset 0x015F-0x0174
	 *   (read 16-bit, AND 0xC3FF, OR 0x3400, write 16-bit)
	 */
	mchbar_clrsetbits16(CLKCFG_C16, ~0xc3ffU & 0xffff, 0x3400);

	/* Step 12: PM_F60 = 0x01030419 (fixed).  Vendor: offset 0x0175 */
	mchbar_write32(PM_F60, 0x01030419);

	/*
	 * Steps 13-14: C2/C3 idle timers from FSB table.
	 * Vendor: offset 0x0182-0x01A6
	 */
	mchbar_write32(C2C3TT, pm_f00_table[fsb]);
	mchbar_write32(C3C4TT, pm_f04_table[fsb]);

	/*
	 * Step 15: PM_F08 = 0x730F (16-bit, fixed).
	 * Vendor: offset 0x01A8
	 *
	 * CRITICAL: This is the C-state transition timer.  The reset
	 * default (0x0300) causes the MC to transition power states
	 * ~6x too fast, corrupting in-flight memory transactions
	 * under sustained load.
	 */
	mchbar_write16(PM_F08, 0x730f);

	/* Step 16: PM_F80 |= bit 31.  Vendor: offset 0x01B2 */
	mchbar_setbits32(PM_F80, 1 << 31);

	/*
	 * Step 17: PM_CTRL0 (0x0040) - clear bits 19,13; set bits 21,9,2.
	 * Vendor: offset 0x01BF-0x01D2
	 *
	 * Bit 21 is set by the vendor early-init before bioscode_5 runs
	 * and preserved by its RMW.  GM45 pm.c also sets it explicitly
	 * (line 178).  We set it here since coreboot's early_init doesn't.
	 */
	mchbar_clrsetbits32(PM_CTRL0, (1 << 19) | (1 << 13),
			    (1 << 21) | (1 << 9) | (1 << 2));

	/*
	 * Step 18: PM_CTRL1 (0x0044) - RMW.
	 * Vendor: offset 0x01D3-0x01F7
	 *   AND 0xFEFFFFFF (clear bit 24)
	 *   OR  0x42200020 (set bits 30,25,21,5)
	 *   if stepping != A0: OR 0x10 (set bit 4)
	 */
	{
		uint32_t val = mchbar_read32(PM_CTRL1);
		val &= 0xfeffffff;
		val |= 0x42200020;
		if (stepping != 0x00)
			val |= 0x10;
		mchbar_write32(PM_CTRL1, val);
	}

	/*
	 * Step 19: PM_NOCARB (0x0090) bits [2:0] = 4.
	 * Vendor: offset 0x01F8-0x020D
	 */
	mchbar_clrsetbits16(PM_NOCARB, 0x07, 0x04);

	/*
	 * Step 20: PM_NOCARB_HI (0x0094) - clear bit 18, set bits 29,13,11,8.
	 * If stepping > 1 (B0+): also set bits 5,4.
	 * Vendor: offset 0x020E-0x0237
	 */
	mchbar_clrbits32(PM_NOCARB_HI, 1 << 18);
	mchbar_setbits32(PM_NOCARB_HI, (1 << 29) | (1 << 13) | (1 << 11) | (1 << 8));
	if (stepping > 0x01)
		mchbar_setbits32(PM_NOCARB_HI, (1 << 5) | (1 << 4));

	/*
	 * Step 21: PM_SCHED (0x0B00) |= 0x01 (16-bit OR, set bit 0).
	 * Vendor: offset 0x0238
	 */
	mchbar_setbits16(PM_SCHED, 1);

	/*
	 * Step 22: PM_SCHED_B90 (0x0B90) AND 0xFF7FFF7F (clear bits 23,7).
	 * Vendor: offset 0x0241
	 */
	mchbar_clrbits32(PM_SCHED_B90, (1 << 23) | (1 << 7));

	/*
	 * Step 23: PM_BD8 (0x0BD8) |= 0x0C (set bits 3,2).
	 * Vendor: offset 0x024E
	 */
	mchbar_setbits32(PM_BD8, 0x0c);

	/*
	 * Step 24: Super Low Frequency Mode (SLFM) enable.
	 *
	 * Not in bioscode_5.rom -- set by the vendor's early-init before
	 * bioscode_5 runs.  We add it here following the GM45 pm.c pattern
	 * (lines 250-259).
	 *
	 * When the CPU supports SLFM (MSR 0xEE bit 27), coordinate with
	 * the memory controller by:
	 *   - Setting CLKCFG bit 14 (SLFM MC coordination)
	 *   - Clearing CLKCFG bit 7
	 *   - Setting PM_CTRL1 bit 31 (SLFM enable in PM controller)
	 *
	 * Without this, the CPU may autonomously enter SLFM but the MC
	 * doesn't compensate for the clock ratio change, causing timing
	 * violations on DRAM accesses.
	 *
	 * Vendor dump confirms: CLKCFG=0x4342 (bit 14 set, bit 7 clear),
	 * PM_CTRL1=0xC2200032 (bit 31 set).
	 */
	{
		const int cpu_supports_slfm =
			rdmsr(MSR_EXTENDED_CONFIG).lo & (1 << 27);

		if (cpu_supports_slfm) {
			mchbar_clrbits16(CLKCFG_MCHBAR, 1 << 7);
			mchbar_setbits16(CLKCFG_MCHBAR, 1 << 14);
			mchbar_setbits32(PM_CTRL1, 1 << 31);
			printk(BIOS_DEBUG, "GM965 PM: SLFM enabled "
			       "(CLKCFG=0x%08x, CTRL1=0x%08x)\n",
			       mchbar_read32(CLKCFG_MCHBAR),
			       mchbar_read32(PM_CTRL1));
		} else {
			mchbar_clrbits16(CLKCFG_MCHBAR, 1 << 14);
			mchbar_setbits16(CLKCFG_MCHBAR, 1 << 7);
			mchbar_clrbits32(PM_CTRL1, 1 << 31);
			printk(BIOS_DEBUG, "GM965 PM: SLFM not supported\n");
		}
	}

	printk(BIOS_DEBUG, "GM965 PM init: done (UPMC3=0x%08x F08=0x%04x "
	       "F10=0x%02x C38=0x%04x C3A=0x%04x)\n",
	       mchbar_read32(UPMC3),
	       mchbar_read16(PM_F08),
	       mchbar_read8(PM_F10),
	       mchbar_read16(HGIPMC2_LO),
	       mchbar_read16(HGIPMC2_HI));
}
