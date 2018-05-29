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
#include <string.h>
#include <arch/io.h>
#include <cbmem.h>
#include <arch/cbfs.h>
#include <cbfs.h>
#include <ip_checksum.h>
#include <pc80/mc146818rtc.h>
#include <device/pci_def.h>
#include <delay.h>

#include <southbridge/intel/common/rcba.h>
#include "pch.h"
/* For DMI bar.  */
#include "northbridge/intel/sandybridge/sandybridge.h"

#define SOUTHBRIDGE PCI_DEV(0, 0x1f, 0)

static void
wait_iobp(void)
{
	while (read8(DEFAULT_RCBA + IOBPS) & 1)
		; // implement timeout?
}

static u32
read_iobp(u32 address)
{
	u32 ret;

	write32(DEFAULT_RCBA + IOBPIRI, address);
	write16(DEFAULT_RCBA + IOBPS, (read16(DEFAULT_RCBA + IOBPS)
					 & 0x1ff) | 0x600);
	wait_iobp();
	ret = read32(DEFAULT_RCBA + IOBPD);
	wait_iobp();
	read8(DEFAULT_RCBA + IOBPS); // call wait_iobp() instead here?
	return ret;
}

static void
write_iobp(u32 address, u32 val)
{
	/* this function was probably pch_iobp_update with the andvalue
	 * being 0. So either the IOBP read can be removed or this function
	 * and the pch_iobp_update function in ramstage could be merged */
	read_iobp(address);
	write16(DEFAULT_RCBA + IOBPS, (read16(DEFAULT_RCBA + IOBPS)
					 & 0x1ff) | 0x600);
	wait_iobp();

	write32(DEFAULT_RCBA + IOBPD, val);
	wait_iobp();
	write16(DEFAULT_RCBA + IOBPS,
		 (read16(DEFAULT_RCBA + IOBPS) & 0x1ff) | 0x600);
	read8(DEFAULT_RCBA + IOBPS); // call wait_iobp() instead here?
}


static void
init_dmi (void)
{
	int i;

	write32 (DEFAULT_DMIBAR + 0x0914,
		 read32 (DEFAULT_DMIBAR + 0x0914) | 0x80000000);
	write32 (DEFAULT_DMIBAR + 0x0934,
		 read32 (DEFAULT_DMIBAR + 0x0934) | 0x80000000);
	for (i = 0; i < 4; i++)
	{
		write32 (DEFAULT_DMIBAR + 0x0a00 + (i << 4),
			 read32 (DEFAULT_DMIBAR + 0x0a00 + (i << 4)) & 0xf3ffffff);
		write32 (DEFAULT_DMIBAR + 0x0a04 + (i << 4),
			 read32 (DEFAULT_DMIBAR + 0x0a04 + (i << 4)) | 0x800);
	}
	write32 (DEFAULT_DMIBAR + 0x0c30, (read32 (DEFAULT_DMIBAR + 0x0c30)
					   & 0xfffffff) | 0x40000000);
	for (i = 0; i < 2; i++)
	{
		write32 (DEFAULT_DMIBAR + 0x0904 + (i << 5),
			 read32 (DEFAULT_DMIBAR + 0x0904 + (i << 5)) & 0xfe3fffff);
		write32 (DEFAULT_DMIBAR + 0x090c + (i << 5),
			 read32 (DEFAULT_DMIBAR + 0x090c + (i << 5)) & 0xfff1ffff);
	}
	write32 (DEFAULT_DMIBAR + 0x090c,
		 read32 (DEFAULT_DMIBAR + 0x090c) & 0xfe1fffff);
	write32 (DEFAULT_DMIBAR + 0x092c,
		 read32 (DEFAULT_DMIBAR + 0x092c) & 0xfe1fffff);
	read32 (DEFAULT_DMIBAR + 0x0904);	// !!! = 0x7a1842ec
	write32 (DEFAULT_DMIBAR + 0x0904, 0x7a1842ec);
	read32 (DEFAULT_DMIBAR + 0x090c);	// !!! = 0x00000208
	write32 (DEFAULT_DMIBAR + 0x090c, 0x00000128);
	read32 (DEFAULT_DMIBAR + 0x0924);	// !!! = 0x7a1842ec
	write32 (DEFAULT_DMIBAR + 0x0924, 0x7a1842ec);
	read32 (DEFAULT_DMIBAR + 0x092c);	// !!! = 0x00000208
	write32 (DEFAULT_DMIBAR + 0x092c, 0x00000128);
	read32 (DEFAULT_DMIBAR + 0x0700);	// !!! = 0x46139008
	write32 (DEFAULT_DMIBAR + 0x0700, 0x46139008);
	read32 (DEFAULT_DMIBAR + 0x0720);	// !!! = 0x46139008
	write32 (DEFAULT_DMIBAR + 0x0720, 0x46139008);
	read32 (DEFAULT_DMIBAR + 0x0c04);	// !!! = 0x2e680008
	write32 (DEFAULT_DMIBAR + 0x0c04, 0x2e680008);
	read32 (DEFAULT_DMIBAR + 0x0904);	// !!! = 0x7a1842ec
	write32 (DEFAULT_DMIBAR + 0x0904, 0x3a1842ec);
	read32 (DEFAULT_DMIBAR + 0x0924);	// !!! = 0x7a1842ec
	write32 (DEFAULT_DMIBAR + 0x0924, 0x3a1842ec);
	read32 (DEFAULT_DMIBAR + 0x0910);	// !!! = 0x00006300
	write32 (DEFAULT_DMIBAR + 0x0910, 0x00004300);
	read32 (DEFAULT_DMIBAR + 0x0930);	// !!! = 0x00006300
	write32 (DEFAULT_DMIBAR + 0x0930, 0x00004300);
	read32 (DEFAULT_DMIBAR + 0x0a00);	// !!! = 0x03042010
	write32 (DEFAULT_DMIBAR + 0x0a00, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0a10);	// !!! = 0x03042010
	write32 (DEFAULT_DMIBAR + 0x0a10, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0a20);	// !!! = 0x03042010
	write32 (DEFAULT_DMIBAR + 0x0a20, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0a30);	// !!! = 0x03042010
	write32 (DEFAULT_DMIBAR + 0x0a30, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0c00);	// !!! = 0x29700c08
	write32 (DEFAULT_DMIBAR + 0x0c00, 0x29700c08);
	read32 (DEFAULT_DMIBAR + 0x0a04);	// !!! = 0x0c0708f0
	write32 (DEFAULT_DMIBAR + 0x0a04, 0x0c0718f0);
	read32 (DEFAULT_DMIBAR + 0x0a14);	// !!! = 0x0c0708f0
	write32 (DEFAULT_DMIBAR + 0x0a14, 0x0c0718f0);
	read32 (DEFAULT_DMIBAR + 0x0a24);	// !!! = 0x0c0708f0
	write32 (DEFAULT_DMIBAR + 0x0a24, 0x0c0718f0);
	read32 (DEFAULT_DMIBAR + 0x0a34);	// !!! = 0x0c0708f0
	write32 (DEFAULT_DMIBAR + 0x0a34, 0x0c0718f0);
	read32 (DEFAULT_DMIBAR + 0x0900);	// !!! = 0x50000000
	write32 (DEFAULT_DMIBAR + 0x0900, 0x50000000);
	read32 (DEFAULT_DMIBAR + 0x0920);	// !!! = 0x50000000
	write32 (DEFAULT_DMIBAR + 0x0920, 0x50000000);
	read32 (DEFAULT_DMIBAR + 0x0908);	// !!! = 0x51ffffff
	write32 (DEFAULT_DMIBAR + 0x0908, 0x51ffffff);
	read32 (DEFAULT_DMIBAR + 0x0928);	// !!! = 0x51ffffff
	write32 (DEFAULT_DMIBAR + 0x0928, 0x51ffffff);
	read32 (DEFAULT_DMIBAR + 0x0a00);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a00, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0a10);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a10, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0a20);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a20, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0a30);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a30, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0700);	// !!! = 0x46139008
	write32 (DEFAULT_DMIBAR + 0x0700, 0x46139008);
	read32 (DEFAULT_DMIBAR + 0x0720);	// !!! = 0x46139008
	write32 (DEFAULT_DMIBAR + 0x0720, 0x46139008);
	read32 (DEFAULT_DMIBAR + 0x0904);	// !!! = 0x3a1842ec
	write32 (DEFAULT_DMIBAR + 0x0904, 0x3a1846ec);
	read32 (DEFAULT_DMIBAR + 0x0924);	// !!! = 0x3a1842ec
	write32 (DEFAULT_DMIBAR + 0x0924, 0x3a1846ec);
	read32 (DEFAULT_DMIBAR + 0x0a00);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a00, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0a10);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a10, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0a20);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a20, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0a30);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a30, 0x03042018);
	read32 (DEFAULT_DMIBAR + 0x0908);	// !!! = 0x51ffffff
	write32 (DEFAULT_DMIBAR + 0x0908, 0x51ffffff);
	read32 (DEFAULT_DMIBAR + 0x0928);	// !!! = 0x51ffffff
	write32 (DEFAULT_DMIBAR + 0x0928, 0x51ffffff);
	read32 (DEFAULT_DMIBAR + 0x0c00);	// !!! = 0x29700c08
	write32 (DEFAULT_DMIBAR + 0x0c00, 0x29700c08);
	read32 (DEFAULT_DMIBAR + 0x0c0c);	// !!! = 0x16063400
	write32 (DEFAULT_DMIBAR + 0x0c0c, 0x00063400);
	read32 (DEFAULT_DMIBAR + 0x0700);	// !!! = 0x46139008
	write32 (DEFAULT_DMIBAR + 0x0700, 0x46339008);
	read32 (DEFAULT_DMIBAR + 0x0720);	// !!! = 0x46139008
	write32 (DEFAULT_DMIBAR + 0x0720, 0x46339008);
	read32 (DEFAULT_DMIBAR + 0x0700);	// !!! = 0x46339008
	write32 (DEFAULT_DMIBAR + 0x0700, 0x45339008);
	read32 (DEFAULT_DMIBAR + 0x0720);	// !!! = 0x46339008
	write32 (DEFAULT_DMIBAR + 0x0720, 0x45339008);
	read32 (DEFAULT_DMIBAR + 0x0700);	// !!! = 0x45339008
	write32 (DEFAULT_DMIBAR + 0x0700, 0x453b9008);
	read32 (DEFAULT_DMIBAR + 0x0720);	// !!! = 0x45339008
	write32 (DEFAULT_DMIBAR + 0x0720, 0x453b9008);
	read32 (DEFAULT_DMIBAR + 0x0700);	// !!! = 0x453b9008
	write32 (DEFAULT_DMIBAR + 0x0700, 0x45bb9008);
	read32 (DEFAULT_DMIBAR + 0x0720);	// !!! = 0x453b9008
	write32 (DEFAULT_DMIBAR + 0x0720, 0x45bb9008);
	read32 (DEFAULT_DMIBAR + 0x0700);	// !!! = 0x45bb9008
	write32 (DEFAULT_DMIBAR + 0x0700, 0x45fb9008);
	read32 (DEFAULT_DMIBAR + 0x0720);	// !!! = 0x45bb9008
	write32 (DEFAULT_DMIBAR + 0x0720, 0x45fb9008);
	read32 (DEFAULT_DMIBAR + 0x0914);	// !!! = 0x9021a080
	write32 (DEFAULT_DMIBAR + 0x0914, 0x9021a280);
	read32 (DEFAULT_DMIBAR + 0x0934);	// !!! = 0x9021a080
	write32 (DEFAULT_DMIBAR + 0x0934, 0x9021a280);
	read32 (DEFAULT_DMIBAR + 0x0914);	// !!! = 0x9021a280
	write32 (DEFAULT_DMIBAR + 0x0914, 0x9821a280);
	read32 (DEFAULT_DMIBAR + 0x0934);	// !!! = 0x9021a280
	write32 (DEFAULT_DMIBAR + 0x0934, 0x9821a280);
	read32 (DEFAULT_DMIBAR + 0x0a00);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a00, 0x03242018);
	read32 (DEFAULT_DMIBAR + 0x0a10);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a10, 0x03242018);
	read32 (DEFAULT_DMIBAR + 0x0a20);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a20, 0x03242018);
	read32 (DEFAULT_DMIBAR + 0x0a30);	// !!! = 0x03042018
	write32 (DEFAULT_DMIBAR + 0x0a30, 0x03242018);
	read32 (DEFAULT_DMIBAR + 0x0258);	// !!! = 0x40000600
	write32 (DEFAULT_DMIBAR + 0x0258, 0x60000600);
	read32 (DEFAULT_DMIBAR + 0x0904);	// !!! = 0x3a1846ec
	write32 (DEFAULT_DMIBAR + 0x0904, 0x2a1846ec);
	read32 (DEFAULT_DMIBAR + 0x0914);	// !!! = 0x9821a280
	write32 (DEFAULT_DMIBAR + 0x0914, 0x98200280);
	read32 (DEFAULT_DMIBAR + 0x0924);	// !!! = 0x3a1846ec
	write32 (DEFAULT_DMIBAR + 0x0924, 0x2a1846ec);
	read32 (DEFAULT_DMIBAR + 0x0934);	// !!! = 0x9821a280
	write32 (DEFAULT_DMIBAR + 0x0934, 0x98200280);
	read32 (DEFAULT_DMIBAR + 0x022c);	// !!! = 0x00c26460
	write32 (DEFAULT_DMIBAR + 0x022c, 0x00c2403c);
	read8 (DEFAULT_RCBA + 0x21a4);	// !!! = 0x42

	read32 (DEFAULT_RCBA + 0x21a4);	// !!! = 0x00012c42
	read32 (DEFAULT_RCBA + 0x2340);	// !!! = 0x0013001b
	write32 (DEFAULT_RCBA + 0x2340, 0x003a001b);
	read8 (DEFAULT_RCBA + 0x21b0);	// !!! = 0x01
	write8 (DEFAULT_RCBA + 0x21b0, 0x02);
	read32 (DEFAULT_DMIBAR + 0x0084);	// !!! = 0x0041ac41
	write32 (DEFAULT_DMIBAR + 0x0084, 0x0041ac42);
	read8 (DEFAULT_DMIBAR + 0x0088);	// !!! = 0x00
	write8 (DEFAULT_DMIBAR + 0x0088, 0x20);
	read16 (DEFAULT_DMIBAR + 0x008a);	// !!! = 0x0041
	read8 (DEFAULT_DMIBAR + 0x0088);	// !!! = 0x00
	write8 (DEFAULT_DMIBAR + 0x0088, 0x20);
	read16 (DEFAULT_DMIBAR + 0x008a);	// !!! = 0x0042
	read16 (DEFAULT_DMIBAR + 0x008a);	// !!! = 0x0042

	read32 (DEFAULT_DMIBAR + 0x0014);	// !!! = 0x8000007f
	write32 (DEFAULT_DMIBAR + 0x0014, 0x80000019);
	read32 (DEFAULT_DMIBAR + 0x0020);	// !!! = 0x01000000
	write32 (DEFAULT_DMIBAR + 0x0020, 0x81000022);
	read32 (DEFAULT_DMIBAR + 0x002c);	// !!! = 0x02000000
	write32 (DEFAULT_DMIBAR + 0x002c, 0x82000044);
	read32 (DEFAULT_DMIBAR + 0x0038);	// !!! = 0x07000080
	write32 (DEFAULT_DMIBAR + 0x0038, 0x87000080);
	read8 (DEFAULT_DMIBAR + 0x0004);	// !!! = 0x00
	write8 (DEFAULT_DMIBAR + 0x0004, 0x01);

	read32 (DEFAULT_RCBA + 0x0050);	// !!! = 0x01200654
	write32 (DEFAULT_RCBA + 0x0050, 0x01200654);
	read32 (DEFAULT_RCBA + 0x0050);	// !!! = 0x01200654
	write32 (DEFAULT_RCBA + 0x0050, 0x012a0654);
	read32 (DEFAULT_RCBA + 0x0050);	// !!! = 0x012a0654
	read8 (DEFAULT_RCBA + 0x1114);	// !!! = 0x00
	write8 (DEFAULT_RCBA + 0x1114, 0x05);
	read32 (DEFAULT_RCBA + 0x2014);	// !!! = 0x80000011
	write32 (DEFAULT_RCBA + 0x2014, 0x80000019);
	read32 (DEFAULT_RCBA + 0x2020);	// !!! = 0x00000000
	write32 (DEFAULT_RCBA + 0x2020, 0x81000022);
	read32 (DEFAULT_RCBA + 0x2020);	// !!! = 0x81000022
	read32 (DEFAULT_RCBA + 0x2030);	// !!! = 0x00000000
	write32 (DEFAULT_RCBA + 0x2030, 0x82000044);
	read32 (DEFAULT_RCBA + 0x2030);	// !!! = 0x82000044
	read32 (DEFAULT_RCBA + 0x2040);	// !!! = 0x00000000
	write32 (DEFAULT_RCBA + 0x2040, 0x87000080);
	read32 (DEFAULT_RCBA + 0x0050);	// !!! = 0x012a0654
	write32 (DEFAULT_RCBA + 0x0050, 0x812a0654);
	read32 (DEFAULT_RCBA + 0x0050);	// !!! = 0x812a0654
	read16 (DEFAULT_RCBA + 0x201a);	// !!! = 0x0000
	read16 (DEFAULT_RCBA + 0x2026);	// !!! = 0x0000
	read16 (DEFAULT_RCBA + 0x2036);	// !!! = 0x0000
	read16 (DEFAULT_RCBA + 0x2046);	// !!! = 0x0000
	read16 (DEFAULT_DMIBAR + 0x001a);	// !!! = 0x0000
	read16 (DEFAULT_DMIBAR + 0x0026);	// !!! = 0x0000
	read16 (DEFAULT_DMIBAR + 0x0032);	// !!! = 0x0000
	read16 (DEFAULT_DMIBAR + 0x003e);	// !!! = 0x0000
}

void
early_pch_init_native (void)
{
	pci_write_config8 (SOUTHBRIDGE, 0xa6,
			    pci_read_config8 (SOUTHBRIDGE, 0xa6) | 2);

	write32 (DEFAULT_RCBA + 0x2088, 0x00109000);
	read32 (DEFAULT_RCBA + 0x20ac);	// !!! = 0x00000000
	write32 (DEFAULT_RCBA + 0x20ac, 0x40000000);
	write32 (DEFAULT_RCBA + 0x100c, 0x01110000);
	write8 (DEFAULT_RCBA + 0x2340, 0x1b);
	read32 (DEFAULT_RCBA + 0x2314);	// !!! = 0x0a080000
	write32 (DEFAULT_RCBA + 0x2314, 0x0a280000);
	read32 (DEFAULT_RCBA + 0x2310);	// !!! = 0xc809605b
	write32 (DEFAULT_RCBA + 0x2310, 0xa809605b);
	write32 (DEFAULT_RCBA + 0x2324, 0x00854c74);
	read8 (DEFAULT_RCBA + 0x0400);	// !!! = 0x00
	read32 (DEFAULT_RCBA + 0x2310);	// !!! = 0xa809605b
	write32 (DEFAULT_RCBA + 0x2310, 0xa809605b);
	read32 (DEFAULT_RCBA + 0x2310);	// !!! = 0xa809605b
	write32 (DEFAULT_RCBA + 0x2310, 0xa809605b);

	write_iobp(0xea007f62, 0x00590133);
	write_iobp(0xec007f62, 0x00590133);
	write_iobp(0xec007f64, 0x59555588);
	write_iobp(0xea0040b9, 0x0001051c);
	write_iobp(0xeb0040a1, 0x800084ff);
	write_iobp(0xec0040a1, 0x800084ff);
	write_iobp(0xea004001, 0x00008400);
	write_iobp(0xeb004002, 0x40201758);
	write_iobp(0xec004002, 0x40201758);
	write_iobp(0xea004002, 0x00601758);
	write_iobp(0xea0040a1, 0x810084ff);
	write_iobp(0xeb0040b1, 0x0001c598);
	write_iobp(0xec0040b1, 0x0001c598);
	write_iobp(0xeb0040b6, 0x0001c598);
	write_iobp(0xea0000a9, 0x80ff969f);
	write_iobp(0xea0001a9, 0x80ff969f);
	write_iobp(0xeb0040b2, 0x0001c396);
	write_iobp(0xeb0040b3, 0x0001c396);
	write_iobp(0xec0040b2, 0x0001c396);
	write_iobp(0xea0001a9, 0x80ff94ff);
	write_iobp(SATA_IOBP_SP0G3IR, 0x0088037f);
	write_iobp(0xea0000a9, 0x80ff94ff);
	write_iobp(SATA_IOBP_SP1G3IR, 0x0088037f);

	write_iobp(0xea007f05, 0x00010642);
	write_iobp(0xea0040b7, 0x0001c91c);
	write_iobp(0xea0040b8, 0x0001c91c);
	write_iobp(0xeb0040a1, 0x820084ff);
	write_iobp(0xec0040a1, 0x820084ff);
	write_iobp(0xea007f0a, 0xc2480000);

	write_iobp(0xec00404d, 0x1ff177f);
	write_iobp(0xec000084, 0x5a600000);
	write_iobp(0xec000184, 0x5a600000);
	write_iobp(0xec000284, 0x5a600000);
	write_iobp(0xec000384, 0x5a600000);
	write_iobp(0xec000094, 0x000f0501);
	write_iobp(0xec000194, 0x000f0501);
	write_iobp(0xec000294, 0x000f0501);
	write_iobp(0xec000394, 0x000f0501);
	write_iobp(0xec000096, 0x00000001);
	write_iobp(0xec000196, 0x00000001);
	write_iobp(0xec000296, 0x00000001);
	write_iobp(0xec000396, 0x00000001);
	write_iobp(0xec000001, 0x00008c08);
	write_iobp(0xec000101, 0x00008c08);
	write_iobp(0xec000201, 0x00008c08);
	write_iobp(0xec000301, 0x00008c08);
	write_iobp(0xec0040b5, 0x0001c518);
	write_iobp(0xec000087, 0x06077597);
	write_iobp(0xec000187, 0x06077597);
	write_iobp(0xec000287, 0x06077597);
	write_iobp(0xec000387, 0x06077597);
	write_iobp(0xea000050, 0x00bb0157);
	write_iobp(0xea000150, 0x00bb0157);
	write_iobp(0xec007f60, 0x77777d77);
	write_iobp(0xea00008d, 0x01320000);
	write_iobp(0xea00018d, 0x01320000);
	write_iobp(0xec0007b2, 0x04514b5e);
	write_iobp(0xec00078c, 0x40000200);
	write_iobp(0xec000780, 0x02000020);

	init_dmi();
}
