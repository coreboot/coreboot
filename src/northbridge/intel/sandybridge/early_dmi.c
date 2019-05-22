/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko <phcoder@gmail.com>
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

#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>

void early_init_dmi(void)
{
	int i;

	DMIBAR32(0x0914) |= 0x80000000;
	DMIBAR32(0x0934) |= 0x80000000;

	for (i = 0; i < 4; i++) {
		DMIBAR32(0x0a00 + (i << 4)) &= 0xf3ffffff;
		DMIBAR32(0x0a04 + (i << 4)) |= 0x800;
	}
	DMIBAR32(0x0c30) = (DMIBAR32(0x0c30) & 0xfffffff) | 0x40000000;

	for (i = 0; i < 2; i++) {
		DMIBAR32(0x0904 + (i << 5)) &= 0xfe3fffff;
		DMIBAR32(0x090c + (i << 5)) &= 0xfff1ffff;
	}

	DMIBAR32(0x090c) &= 0xfe1fffff;
	DMIBAR32(0x092c) &= 0xfe1fffff;

	DMIBAR32(0x0904);	// !!! = 0x7a1842ec
	DMIBAR32(0x0904) = 0x7a1842ec;
	DMIBAR32(0x090c);	// !!! = 0x00000208
	DMIBAR32(0x090c) = 0x00000128;
	DMIBAR32(0x0924);	// !!! = 0x7a1842ec
	DMIBAR32(0x0924) = 0x7a1842ec;
	DMIBAR32(0x092c);	// !!! = 0x00000208
	DMIBAR32(0x092c) = 0x00000128;
	DMIBAR32(0x0700);	// !!! = 0x46139008
	DMIBAR32(0x0700) = 0x46139008;
	DMIBAR32(0x0720);	// !!! = 0x46139008
	DMIBAR32(0x0720) = 0x46139008;
	DMIBAR32(0x0c04);	// !!! = 0x2e680008
	DMIBAR32(0x0c04) = 0x2e680008;
	DMIBAR32(0x0904);	// !!! = 0x7a1842ec
	DMIBAR32(0x0904) = 0x3a1842ec;
	DMIBAR32(0x0924);	// !!! = 0x7a1842ec
	DMIBAR32(0x0924) = 0x3a1842ec;
	DMIBAR32(0x0910);	// !!! = 0x00006300
	DMIBAR32(0x0910) = 0x00004300;
	DMIBAR32(0x0930);	// !!! = 0x00006300
	DMIBAR32(0x0930) = 0x00004300;
	DMIBAR32(0x0a00);	// !!! = 0x03042010
	DMIBAR32(0x0a00) = 0x03042018;
	DMIBAR32(0x0a10);	// !!! = 0x03042010
	DMIBAR32(0x0a10) = 0x03042018;
	DMIBAR32(0x0a20);	// !!! = 0x03042010
	DMIBAR32(0x0a20) = 0x03042018;
	DMIBAR32(0x0a30);	// !!! = 0x03042010
	DMIBAR32(0x0a30) = 0x03042018;
	DMIBAR32(0x0c00);	// !!! = 0x29700c08
	DMIBAR32(0x0c00) = 0x29700c08;
	DMIBAR32(0x0a04);	// !!! = 0x0c0708f0
	DMIBAR32(0x0a04) = 0x0c0718f0;
	DMIBAR32(0x0a14);	// !!! = 0x0c0708f0
	DMIBAR32(0x0a14) = 0x0c0718f0;
	DMIBAR32(0x0a24);	// !!! = 0x0c0708f0
	DMIBAR32(0x0a24) = 0x0c0718f0;
	DMIBAR32(0x0a34);	// !!! = 0x0c0708f0
	DMIBAR32(0x0a34) = 0x0c0718f0;
	DMIBAR32(0x0900);	// !!! = 0x50000000
	DMIBAR32(0x0900) = 0x50000000;
	DMIBAR32(0x0920);	// !!! = 0x50000000
	DMIBAR32(0x0920) = 0x50000000;
	DMIBAR32(0x0908);	// !!! = 0x51ffffff
	DMIBAR32(0x0908) = 0x51ffffff;
	DMIBAR32(0x0928);	// !!! = 0x51ffffff
	DMIBAR32(0x0928) = 0x51ffffff;
	DMIBAR32(0x0a00);	// !!! = 0x03042018
	DMIBAR32(0x0a00) = 0x03042018;
	DMIBAR32(0x0a10);	// !!! = 0x03042018
	DMIBAR32(0x0a10) = 0x03042018;
	DMIBAR32(0x0a20);	// !!! = 0x03042018
	DMIBAR32(0x0a20) = 0x03042018;
	DMIBAR32(0x0a30);	// !!! = 0x03042018
	DMIBAR32(0x0a30) = 0x03042018;
	DMIBAR32(0x0700);	// !!! = 0x46139008
	DMIBAR32(0x0700) = 0x46139008;
	DMIBAR32(0x0720);	// !!! = 0x46139008
	DMIBAR32(0x0720) = 0x46139008;
	DMIBAR32(0x0904);	// !!! = 0x3a1842ec
	DMIBAR32(0x0904) = 0x3a1846ec;
	DMIBAR32(0x0924);	// !!! = 0x3a1842ec
	DMIBAR32(0x0924) = 0x3a1846ec;
	DMIBAR32(0x0a00);	// !!! = 0x03042018
	DMIBAR32(0x0a00) = 0x03042018;
	DMIBAR32(0x0a10);	// !!! = 0x03042018
	DMIBAR32(0x0a10) = 0x03042018;
	DMIBAR32(0x0a20);	// !!! = 0x03042018
	DMIBAR32(0x0a20) = 0x03042018;
	DMIBAR32(0x0a30);	// !!! = 0x03042018
	DMIBAR32(0x0a30) = 0x03042018;
	DMIBAR32(0x0908);	// !!! = 0x51ffffff
	DMIBAR32(0x0908) = 0x51ffffff;
	DMIBAR32(0x0928);	// !!! = 0x51ffffff
	DMIBAR32(0x0928) = 0x51ffffff;
	DMIBAR32(0x0c00);	// !!! = 0x29700c08
	DMIBAR32(0x0c00) = 0x29700c08;
	DMIBAR32(0x0c0c);	// !!! = 0x16063400
	DMIBAR32(0x0c0c) = 0x00063400;
	DMIBAR32(0x0700);	// !!! = 0x46139008
	DMIBAR32(0x0700) = 0x46339008;
	DMIBAR32(0x0720);	// !!! = 0x46139008
	DMIBAR32(0x0720) = 0x46339008;
	DMIBAR32(0x0700);	// !!! = 0x46339008
	DMIBAR32(0x0700) = 0x45339008;
	DMIBAR32(0x0720);	// !!! = 0x46339008
	DMIBAR32(0x0720) = 0x45339008;
	DMIBAR32(0x0700);	// !!! = 0x45339008
	DMIBAR32(0x0700) = 0x453b9008;
	DMIBAR32(0x0720);	// !!! = 0x45339008
	DMIBAR32(0x0720) = 0x453b9008;
	DMIBAR32(0x0700);	// !!! = 0x453b9008
	DMIBAR32(0x0700) = 0x45bb9008;
	DMIBAR32(0x0720);	// !!! = 0x453b9008
	DMIBAR32(0x0720) = 0x45bb9008;
	DMIBAR32(0x0700);	// !!! = 0x45bb9008
	DMIBAR32(0x0700) = 0x45fb9008;
	DMIBAR32(0x0720);	// !!! = 0x45bb9008
	DMIBAR32(0x0720) = 0x45fb9008;
	DMIBAR32(0x0914);	// !!! = 0x9021a080
	DMIBAR32(0x0914) = 0x9021a280;
	DMIBAR32(0x0934);	// !!! = 0x9021a080
	DMIBAR32(0x0934) = 0x9021a280;
	DMIBAR32(0x0914);	// !!! = 0x9021a280
	DMIBAR32(0x0914) = 0x9821a280;
	DMIBAR32(0x0934);	// !!! = 0x9021a280
	DMIBAR32(0x0934) = 0x9821a280;
	DMIBAR32(0x0a00);	// !!! = 0x03042018
	DMIBAR32(0x0a00) = 0x03242018;
	DMIBAR32(0x0a10);	// !!! = 0x03042018
	DMIBAR32(0x0a10) = 0x03242018;
	DMIBAR32(0x0a20);	// !!! = 0x03042018
	DMIBAR32(0x0a20) = 0x03242018;
	DMIBAR32(0x0a30);	// !!! = 0x03042018
	DMIBAR32(0x0a30) = 0x03242018;
	DMIBAR32(0x0258);	// !!! = 0x40000600
	DMIBAR32(0x0258) = 0x60000600;
	DMIBAR32(0x0904);	// !!! = 0x3a1846ec
	DMIBAR32(0x0904) = 0x2a1846ec;
	DMIBAR32(0x0914);	// !!! = 0x9821a280
	DMIBAR32(0x0914) = 0x98200280;
	DMIBAR32(0x0924);	// !!! = 0x3a1846ec
	DMIBAR32(0x0924) = 0x2a1846ec;
	DMIBAR32(0x0934);	// !!! = 0x9821a280
	DMIBAR32(0x0934) = 0x98200280;
	DMIBAR32(0x022c);	// !!! = 0x00c26460
	DMIBAR32(0x022c) = 0x00c2403c;

	early_pch_init_native_dmi_pre();

	/* Write once settings. */
	DMIBAR32(DMILCAP) = (DMIBAR32(DMILCAP) & ~0x3f00f) |
			    (2 << 0) |  // 5GT/s
			    (2 << 12) | // L0s 128 ns to less than 256 ns
			    (2 << 15);  // L1 2 us to less than 4 us

	DMIBAR8(DMILCTL) |= 0x20; // Retrain link
	while (DMIBAR16(DMILSTS) & TXTRN)
		;

	DMIBAR8(DMILCTL) |= 0x20; // Retrain link
	while (DMIBAR16(DMILSTS) & TXTRN)
		;

	const u8 w = (DMIBAR16(DMILSTS) >> 4) & 0x1f;
	const u16 t = (DMIBAR16(DMILSTS) & 0xf) * 2500;

	printk(BIOS_DEBUG, "DMI: Running at X%x @ %dMT/s\n", w, t);
	/*
	 * Virtual Channel resources must match settings in RCBA!
	 *
	 * Channel Vp and Vm are documented in
	 * "Desktop 4th Generation Intel Core Processor Family, Desktop Intel
	 * Pentium Processor Family, and Desktop Intel Celeron Processor Family
	 * Vol. 2"
	 */

	/* Channel 0: Enable, Set ID to 0, map TC0 and TC3 and TC4 to VC0. */
	DMIBAR32(DMIVC0RCTL) = (1 << 31) | (0 << 24) | (0x0c << 1) | 1;
	/* Channel 1: Enable, Set ID to 1, map TC1 and TC5 to VC1. */
	DMIBAR32(DMIVC1RCTL) = (1 << 31) | (1 << 24) | (0x11 << 1);
	/* Channel p: Enable, Set ID to 2, map TC2 and TC6 to VCp  */
	DMIBAR32(DMIVCPRCTL) = (1 << 31) | (2 << 24) | (0x22 << 1);
	/* Channel m: Enable, Set ID to 0, map TC7 to VCm */
	DMIBAR32(DMIVCMRCTL) = (1 << 31) | (7 << 24) | (0x40 << 1);

	/* Set Extended VC Count (EVCC) to 1 as Channel 1 is active. */
	DMIBAR8(DMIPVCCAP1) |= 1;

	early_pch_init_native_dmi_post();

	/*
	 * BIOS Requirement: Check if DMI VC Negotiation was successful.
	 * Wait for virtual channels negotiation pending.
	 */
	while (DMIBAR16(DMIVC0RSTS) & VC0NP)
		;
	while (DMIBAR16(DMIVC1RSTS) & VC1NP)
		;
	while (DMIBAR16(DMIVCPRSTS) & VCPNP)
		;
	while (DMIBAR16(DMIVCMRSTS) & VCMNP)
		;
}
