/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/pci_def.h>
#include <spd.h>

#include "delay.h"
#include "gm45.h"

void raminit_thermal(const sysinfo_t *sysinfo)
{
	const mem_clock_t freq = sysinfo->selected_timings.mem_clock;
	int x;
	FOR_EACH_POPULATED_CHANNEL(sysinfo->dimms, x) {
		const chip_width_t width = sysinfo->dimms[x].chip_width;
		const chip_capacity_t size = sysinfo->dimms[x].chip_capacity;
		if ((freq == MEM_CLOCK_1067MT) && (width == CHIP_WIDTH_x16)) {
			mchbar_write32(CxDTPEW(x), 0x0d0b0403);
			mchbar_write32(CxDTPEW(x) + 4, 0x060d);
			mchbar_write32(CxDTAEW(x), 0x2d0b221a);
			mchbar_write32(CxDTAEW(x) + 4, 0xc779956e);
		} else
		if ((freq == MEM_CLOCK_1067MT) && (width == CHIP_WIDTH_x8)) {
			mchbar_write32(CxDTPEW(x), 0x06040101);
			mchbar_write32(CxDTPEW(x) + 4, 0x0506);
			if (size == CHIP_CAP_2G)
				mchbar_write32(CxDTAEW(x), 0xa1071416);
			else
				mchbar_write32(CxDTAEW(x), 0x1a071416);
			mchbar_write32(CxDTAEW(x) + 4, 0x7246643f);
		} else
		if ((freq == MEM_CLOCK_800MT) && (width == CHIP_WIDTH_x16)) {
			mchbar_write32(CxDTPEW(x), 0x06030100);
			mchbar_write32(CxDTPEW(x) + 4, 0x0506);
			mchbar_write32(CxDTAEW(x), 0x3e081714);
			mchbar_write32(CxDTAEW(x) + 4, 0xbb79a171);
		} else
		if ((freq == MEM_CLOCK_800MT) && (width == CHIP_WIDTH_x8)) {
			if (size <= CHIP_CAP_512M)
				mchbar_write32(CxDTPEW(x), 0x05050101);
			else
				mchbar_write32(CxDTPEW(x), 0x05060101);
			mchbar_write32(CxDTPEW(x) + 4, 0x0503);
			if (size == CHIP_CAP_2G) {
				mchbar_write32(CxDTAEW(x), 0x57051010);
				mchbar_write32(CxDTAEW(x) + 4, 0x5fd15dde);
			} else
			if (size == CHIP_CAP_1G) {
				mchbar_write32(CxDTAEW(x), 0x3306130e);
				mchbar_write32(CxDTAEW(x) + 4, 0x5763485d);
			} else
			if (size <= CHIP_CAP_512M) {
				mchbar_write32(CxDTAEW(x), 0x1e08170d);
				mchbar_write32(CxDTAEW(x) + 4, 0x502f3827);
			}
		} else
		if ((freq == MEM_CLOCK_667MT) && (width == CHIP_WIDTH_x16)) {
			mchbar_write32(CxDTPEW(x), 0x02000000);
			mchbar_write32(CxDTPEW(x) + 4, 0x0402);
			mchbar_write32(CxDTAEW(x), 0x46061111);
			mchbar_write32(CxDTAEW(x) + 4, 0xb579a772);
		} else
		if ((freq == MEM_CLOCK_667MT) && (width == CHIP_WIDTH_x8)) {
			mchbar_write32(CxDTPEW(x), 0x04070101);
			mchbar_write32(CxDTPEW(x) + 4, 0x0501);
			if (size == CHIP_CAP_2G) {
				mchbar_write32(CxDTAEW(x), 0x32040e0d);
				mchbar_write32(CxDTAEW(x) + 4, 0x55ff59ff);
			} else
			if (size == CHIP_CAP_1G) {
				mchbar_write32(CxDTAEW(x), 0x3f05120a);
				mchbar_write32(CxDTAEW(x) + 4, 0x49713a6c);
			} else
			if (size <= CHIP_CAP_512M) {
				mchbar_write32(CxDTAEW(x), 0x20081808);
				mchbar_write32(CxDTAEW(x) + 4, 0x3f23221b);
			}
		}

		/* also L-Shaped */
		if (sysinfo->selected_timings.channel_mode ==
				CHANNEL_MODE_DUAL_INTERLEAVED) {
			if (freq == MEM_CLOCK_1067MT) {
				mchbar_write32(CxGTEW(x), 0xc8f81717);
			} else
			if (freq == MEM_CLOCK_800MT) {
				mchbar_write32(CxGTEW(x), 0x96ba1717);
			} else
			if (freq == MEM_CLOCK_667MT) {
				mchbar_write32(CxGTEW(x), 0x7d9b1717);
			}
		} else {
			if (freq == MEM_CLOCK_1067MT) {
				mchbar_write32(CxGTEW(x), 0x53661717);
			} else
			if (freq == MEM_CLOCK_800MT) {
				mchbar_write32(CxGTEW(x), 0x886e1717);
			} else
			if (freq == MEM_CLOCK_667MT) {
				mchbar_write32(CxGTEW(x), 0x38621717);
			}
		}
	}

	// always?
	mchbar_write32(CxDTC(0), 0x00004020);
	mchbar_write32(CxDTC(1), 0x00004020);
	mchbar_write32(CxGTC(0), 0x00304848);
	mchbar_write32(CxGTC(1), 0x00304848);

	/* enable thermal sensors */
	u32 tmp;
	tmp = mchbar_read32(0x1290) & 0xfff8;
	mchbar_write32(0x1290, tmp | 0xa4810007);
	tmp = mchbar_read32(0x1390) & 0xfff8;
	mchbar_write32(0x1390, tmp | 0xa4810007);
	tmp = mchbar_read32(0x12b4) & 0xfff8;
	mchbar_write32(0x12b4, tmp | 0xa2810007);
	tmp = mchbar_read32(0x13b4) & 0xfff8;
	mchbar_write32(0x13b4, tmp | 0xa2810007);
	mchbar_write8(0x1070, 1);
	mchbar_write8(0x1080, 6);
	if (sysinfo->gfx_type == GMCH_PM45) {
		mchbar_write16(0x1001, 0);
		mchbar_write8(0x1007, 0);
		mchbar_write32(0x1010, 0);
		mchbar_write32(0x1014, 0);
		mchbar_write8(0x101c, 0x98);
		mchbar_write16(0x1041, 0x9200);
		mchbar_write8(0x1047, 0);
		mchbar_write32(0x1050, 0x2309);
		mchbar_write32(0x1054, 0);
		mchbar_write8(0x105c, 0x98);
	} else {
		mchbar_write16(0x1001, 0x9200);
		mchbar_write8(0x1007, 0);
		mchbar_write32(0x1010, 0x2309);
		mchbar_write32(0x1014, 0);
		mchbar_write8(0x101c, 0x98);
		mchbar_write16(0x1041, 0);
		mchbar_write8(0x1047, 0);
		mchbar_write32(0x1050, 0);
		mchbar_write32(0x1054, 0);
		mchbar_write8(0x105c, 0x98);
	}

	mchbar_setbits32(0x1010, 1 << 31);
	mchbar_setbits32(0x1050, 1 << 31);
	mchbar_setbits32(CxGTC(0), 1 << 31);
	mchbar_setbits32(CxGTC(1), 1 << 31);

	if (sysinfo->gs45_low_power_mode) {
		mchbar_write32(0x11b0, 0xa000083a);
	} else if (sysinfo->gfx_type == GMCH_GM49) {
		mchbar_write32(0x11b0, 0x2000383a);
		mchbar_clrbits16(0x1190, 1 << 15);
	} else if ((sysinfo->gfx_type != GMCH_PM45) &&
			(sysinfo->gfx_type != GMCH_UNKNOWN)) {
		mchbar_write32(0x11b0, 0xa000383a);
	}

	switch (sysinfo->selected_timings.fsb_clock) {
		case FSB_CLOCK_667MHz:
			mchbar_write32(0x11d0, 0x0fd88000);
			break;
		case FSB_CLOCK_800MHz:
			mchbar_write32(0x11d0, 0x1303c000);
			break;
		case FSB_CLOCK_1067MHz:
			mchbar_write32(0x11d0, 0x194a0000);
			break;
	}
	tmp = mchbar_read32(0x11d4) & ~0x1f;
	mchbar_write32(0x11d4, tmp | 4);
}
