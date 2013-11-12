/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Vladimir Serbinenko.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* This is a replay-based init for nehalem video.  */

outb(0x23, 0x03c2);		// Device I/O <--
outb(0x02, 0x03da);		// Device I/O <--
inb(0x03c2);			// Device I/O --> 10
outb(0x01, 0x03da);		// Device I/O <--
inb(0x03c2);			// Device I/O --> 10
outl(0x00070080, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x00070180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x00071180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x00041000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00002900
outl(0x8000298e, 0x1044);	// Device I/O
outl(0x0007019c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0007119c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x00000000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> ffffffff
outl(0x00000000, 0x1044);	// Device I/O
outl(0x00000000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> ffffffff
outl(0x00000000, 0x1044);	// Device I/O
outl(0x00000000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> ffffffff
outl(0x00000000, 0x1044);	// Device I/O
outl(0x00000000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> ffffffff
outl(0x00000000, 0x1044);	// Device I/O
outl(0x00000000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> ffffffff
outl(0x00000000, 0x1044);	// Device I/O
outl(0x000fc008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 2c010757
outl(0x2c010000, 0x1044);	// Device I/O
outl(0x000fc020, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 2c010757
outl(0x2c010000, 0x1044);	// Device I/O
outl(0x000fc038, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 2c010757
outl(0x2c010000, 0x1044);	// Device I/O
outl(0x000fc050, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 2c010757
outl(0x2c010000, 0x1044);	// Device I/O
outl(0x000fc408, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 2c010757
outl(0x2c010000, 0x1044);	// Device I/O
outl(0x000fc420, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 2c010757
outl(0x2c010000, 0x1044);	// Device I/O
outl(0x000fc438, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 2c010757
outl(0x2c010000, 0x1044);	// Device I/O
outl(0x000fc450, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 2c010757
outl(0x2c010000, 0x1044);	// Device I/O
outw(0x0018, 0x03ce);		// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x01000001, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f048, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x03030000, 0x1044);	// Device I/O
outl(0x0004f050, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f054, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000001, 0x1044);	// Device I/O
outl(0x0004f058, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x03030000, 0x1044);	// Device I/O
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x03030000, 0x1044);	// Device I/O
outl(0x00042004, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x02000000, 0x1044);	// Device I/O
outl(0x000fd034, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 39cfffe0
outl(0x8421ffe0, 0x1044);	// Device I/O
int i;
for (i = 0; i < 0x1fff; i++) {
	outl(0x00000001 | (i << 2), 0x1040);	// Device I/O
	outl(0xc2000001 | (i << 12), 0x1044);	// Device I/O
}

outw(0x0302, 0x03c4);		// Device I/O
outw(0x0003, 0x03c4);		// Device I/O
outw(0x0204, 0x03c4);		// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outw(0x0300, 0x03c4);		// Device I/O
outb(0x67, 0x03c2);		// Device I/O <--
outb(0x11, 0x03d4);		// Device I/O <--
inw(0x03d4);			// Device I/O --> 0x0011
outw(0x0011, 0x03d4);		// Device I/O
outw(0x5f00, 0x03d4);		// Device I/O
outw(0x4f01, 0x03d4);		// Device I/O
outw(0x5002, 0x03d4);		// Device I/O
outw(0x8203, 0x03d4);		// Device I/O
outw(0x5504, 0x03d4);		// Device I/O
outw(0x8105, 0x03d4);		// Device I/O
outw(0xbf06, 0x03d4);		// Device I/O
outw(0x1f07, 0x03d4);		// Device I/O
outw(0x0008, 0x03d4);		// Device I/O
outw(0x4f09, 0x03d4);		// Device I/O
outw(0x0d0a, 0x03d4);		// Device I/O
outw(0x0e0b, 0x03d4);		// Device I/O
outw(0x000c, 0x03d4);		// Device I/O
outw(0x000d, 0x03d4);		// Device I/O
outw(0x000e, 0x03d4);		// Device I/O
outw(0x000f, 0x03d4);		// Device I/O
outw(0x9c10, 0x03d4);		// Device I/O
outw(0x8e11, 0x03d4);		// Device I/O
outw(0x8f12, 0x03d4);		// Device I/O
outw(0x2813, 0x03d4);		// Device I/O
outw(0x1f14, 0x03d4);		// Device I/O
outw(0x9615, 0x03d4);		// Device I/O
outw(0xb916, 0x03d4);		// Device I/O
outw(0xa317, 0x03d4);		// Device I/O
outw(0xff18, 0x03d4);		// Device I/O
inb(0x03da);			// Device I/O --> 31
inb(0x03ba);			// Device I/O --> ff
inb(0x03da);			// Device I/O --> 21
inb(0x03ba);			// Device I/O --> ff
inb(0x03da);			// Device I/O --> 01
inb(0x03ba);			// Device I/O --> ff
outw(0x0000, 0x03ce);		// Device I/O
outw(0x0001, 0x03ce);		// Device I/O
outw(0x0002, 0x03ce);		// Device I/O
outw(0x0003, 0x03ce);		// Device I/O
outw(0x0004, 0x03ce);		// Device I/O
outw(0x1005, 0x03ce);		// Device I/O
outw(0x0e06, 0x03ce);		// Device I/O
outw(0x0007, 0x03ce);		// Device I/O
outw(0xff08, 0x03ce);		// Device I/O
outl(0x000e1100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000e1100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x000e1100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00010000, 0x1044);	// Device I/O
outl(0x000e1100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00010000
outl(0x000e1100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00010000
outl(0x000e1100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000e1100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x000e1100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f054, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000001
outl(0x0004f054, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000001
outl(0x00000001, 0x1044);	// Device I/O
outl(0x000e4200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0000001c
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00050000
outl(0x8004003e, 0x1044);	// Device I/O
outl(0x000e4214, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x80060002, 0x1044);	// Device I/O
outl(0x000e4218, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x01000000, 0x1044);	// Device I/O
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x5344003e, 0x1044);	// Device I/O
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0144003e
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0144003e
outl(0x8074003e, 0x1044);	// Device I/O
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x5344003e, 0x1044);	// Device I/O
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0144003e
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0144003e
outl(0x8074003e, 0x1044);	// Device I/O
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x5344003e, 0x1044);	// Device I/O
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0144003e
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0144003e
outl(0x8074003e, 0x1044);	// Device I/O
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x000e4210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 5144003e
outl(0x5344003e, 0x1044);	// Device I/O
outl(0x000e4f00, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0100038e
outl(0x0100030c, 0x1044);	// Device I/O
outl(0x000e4f04, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00b8338e
outl(0x00b8230c, 0x1044);	// Device I/O
outl(0x000e4f08, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0178838e
outl(0x06f8930c, 0x1044);	// Device I/O
outl(0x000e4f0c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 09f8e38e
outl(0x09f8e38e, 0x1044);	// Device I/O
outl(0x000e4f10, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00b8038e
outl(0x00b8030c, 0x1044);	// Device I/O
outl(0x000e4f14, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0978838e
outl(0x0b78830c, 0x1044);	// Device I/O
outl(0x000e4f18, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 09f8b38e
outl(0x0ff8d3cf, 0x1044);	// Device I/O
outl(0x000e4f1c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0178038e
outl(0x01e8030c, 0x1044);	// Device I/O
outl(0x000e4f20, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 09f8638e
outl(0x0ff863cf, 0x1044);	// Device I/O
outl(0x000e4f24, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 09f8038e
outl(0x0ff803cf, 0x1044);	// Device I/O
outl(0x000c4030, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00001000, 0x1044);	// Device I/O
outl(0x000c4000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000c4030, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00001000
outl(0x00001000, 0x1044);	// Device I/O
outl(0x000e1150, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0000001c
outl(0x000e1150, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0000001c
outl(0x0000089c, 0x1044);	// Device I/O
outl(0x000fcc00, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01773f30
outl(0x01986f00, 0x1044);	// Device I/O
outl(0x000fcc0c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01773f30
outl(0x01986f00, 0x1044);	// Device I/O
outl(0x000fcc18, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01773f30
outl(0x01986f00, 0x1044);	// Device I/O
outl(0x000fcc24, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01773f30
outl(0x01986f00, 0x1044);	// Device I/O
outl(0x000c4000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000e1180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 40000002
inb(0x03d4);			// Device I/O --> 18
inb(0x03d6);			// Device I/O --> ff
inb(0x03d0);			// Device I/O --> ff
inb(0x03ce);			// Device I/O --> 08
inb(0x03d2);			// Device I/O --> ff
inb(0x03c4);			// Device I/O --> 00
inb(0x03c7);			// Device I/O --> 00
inb(0x03c8);			// Device I/O --> 00
outb(0x01, 0x03c4);		// Device I/O <--
inw(0x03c4);			// Device I/O --> 0x2001
outw(0x2001, 0x03c4);		// Device I/O
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000800
outl(0x000c5100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000003, 0x1044);	// Device I/O
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008800
outl(0x000c5120, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x000c5104, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x460000a0, 0x1044);	// Device I/O
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0000ca00
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0000ca00
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0000ca00
outl(0x000c5120, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x000c5104, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 060000a0
outl(0x4a8000a1, 0x1044);	// Device I/O
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a08
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a08
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> ffffff00
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a0c
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a0c
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00ffffff
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a10
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a10
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 4011ae30
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a14
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a14
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a18
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a18
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03011300
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a1c
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a1c
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 78101a80
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a20
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a20
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 9795baea
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a24
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a24
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 278c5559
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a28
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a28
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00545021
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a2c
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a2c
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01010000
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a30
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a30
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01010101
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a34
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a34
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01010101
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a38
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a38
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01010101
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a3c
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a3c
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 1b120101
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a40
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a40
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 20508000
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a44
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a44
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 20183014
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a48
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a48
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> a3050044
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a4c
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a4c
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 1f000010
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a50
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a50
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 80001693
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a54
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a54
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 30142050
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a58
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a58
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00442018
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a5c
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a5c
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0010a305
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a60
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a60
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00001f00
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a64
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a64
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 81000f00
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a68
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a68
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0a813c0a
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a6c
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a6c
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00091632
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a70
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a70
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01f0e430
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a74
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a74
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> fe000000
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a78
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a78
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 31504c00
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a7c
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008a7c
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 58573132
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008800
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008800
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 4c542d33
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008800
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008800
outl(0x000c510c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> ac003143
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008000
outl(0x000c5104, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 028000a1
outl(0x480000a0, 0x1044);	// Device I/O
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008000
outl(0x000c5100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000003
outl(0x48000000, 0x1044);	// Device I/O
outl(0x000c5108, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008000
outl(0x00008000, 0x1044);	// Device I/O
outb(0x01, 0x03c4);		// Device I/O <--
inw(0x03c4);			// Device I/O --> 0x2001
outw(0x0001, 0x03c4);		// Device I/O
outb(0x18, 0x03d4);		// Device I/O <--
outb(0xff, 0x03d6);		// Device I/O <--
outb(0xff, 0x03d0);		// Device I/O <--
outb(0x08, 0x03ce);		// Device I/O <--
outb(0xff, 0x03d2);		// Device I/O <--
outb(0x00, 0x03c4);		// Device I/O <--
outb(0x00, 0x03c8);		// Device I/O <--
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000e1180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 40000002
outl(0x00000300, 0x1044);	// Device I/O
outl(0x000c7208, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00fa09c4, 0x1044);	// Device I/O
outl(0x000c720c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00fa09c4, 0x1044);	// Device I/O
outl(0x000c7210, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00186904
outl(0x00186903, 0x1044);	// Device I/O
outl(0x00048250, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x80000000, 0x1044);	// Device I/O
outl(0x00048254, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x061a061a, 0x1044);	// Device I/O
outl(0x000c8254, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x061a061a, 0x1044);	// Device I/O
outl(0x000c8250, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x000c8250, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x80000000, 0x1044);	// Device I/O
outl(0x000c7204, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000c4000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f054, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000001
outl(0x0000020d, 0x1044);	// Device I/O
outl(0x0004f054, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0000020d
outl(0x0004f050, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f050, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f054, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0000020d
outl(0x0000020d, 0x1044);	// Device I/O
outl(0x0004f050, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0xc0000000, 0x1044);	// Device I/O
outl(0x0004f054, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0000020d
outl(0x0004f054, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 0000020d
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000400, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000400
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000400
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outb(0xff, 0x03c6);		// Device I/O <--
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x03300000, 0x1044);	// Device I/O
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03300000
outl(0x30300000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 30300000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 30300000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 30300000
outl(0x0004f048, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 30300000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 30300000
outl(0x30030000, 0x1044);	// Device I/O
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 30030000
outl(0x03030000, 0x1044);	// Device I/O

vga_textmode_init();

outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000001
outl(0x01000008, 0x1044);	// Device I/O
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x03030000, 0x1044);	// Device I/O
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x03030000, 0x1044);	// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00070080, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000700c0, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outb(0x01, 0x03c4);		// Device I/O <--
inw(0x03c4);			// Device I/O --> 0x0001
outw(0x2001, 0x03c4);		// Device I/O
outl(0x00041000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 8000298e
outl(0x00041000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 8000298e
outl(0x8000298e, 0x1044);	// Device I/O
outl(0x00070180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00071180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00068070, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00068080, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00068074, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000400, 0x1044);	// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000400
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000400
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00041000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 8000298e
outl(0x8020298e, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outw(0x0010, 0x03ce);		// Device I/O
outw(0x0011, 0x03ce);		// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outw(0x0100, 0x03c4);		// Device I/O
outw(0x2001, 0x03c4);		// Device I/O
outw(0x0302, 0x03c4);		// Device I/O
outw(0x0003, 0x03c4);		// Device I/O
outw(0x0204, 0x03c4);		// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outw(0x0300, 0x03c4);		// Device I/O
outb(0x67, 0x03c2);		// Device I/O <--
outb(0x11, 0x03d4);		// Device I/O <--
inw(0x03d4);			// Device I/O --> 0x8e11
outw(0x0e11, 0x03d4);		// Device I/O
outw(0x5f00, 0x03d4);		// Device I/O
outw(0x4f01, 0x03d4);		// Device I/O
outw(0x5002, 0x03d4);		// Device I/O
outw(0x8203, 0x03d4);		// Device I/O
outw(0x5504, 0x03d4);		// Device I/O
outw(0x8105, 0x03d4);		// Device I/O
outw(0xbf06, 0x03d4);		// Device I/O
outw(0x1f07, 0x03d4);		// Device I/O
outw(0x0008, 0x03d4);		// Device I/O
outw(0x4f09, 0x03d4);		// Device I/O
outw(0x0d0a, 0x03d4);		// Device I/O
outw(0x0e0b, 0x03d4);		// Device I/O
outw(0x000c, 0x03d4);		// Device I/O
outw(0x000d, 0x03d4);		// Device I/O
outw(0x000e, 0x03d4);		// Device I/O
outw(0x000f, 0x03d4);		// Device I/O
outw(0x9c10, 0x03d4);		// Device I/O
outw(0x8e11, 0x03d4);		// Device I/O
outw(0x8f12, 0x03d4);		// Device I/O
outw(0x2813, 0x03d4);		// Device I/O
outw(0x1f14, 0x03d4);		// Device I/O
outw(0x9615, 0x03d4);		// Device I/O
outw(0xb916, 0x03d4);		// Device I/O
outw(0xa317, 0x03d4);		// Device I/O
outw(0xff18, 0x03d4);		// Device I/O
inb(0x03da);			// Device I/O --> 01
inb(0x03ba);			// Device I/O --> ff
inb(0x03da);			// Device I/O --> 21
inb(0x03ba);			// Device I/O --> ff
inb(0x03da);			// Device I/O --> 01
inb(0x03ba);			// Device I/O --> ff
outw(0x0000, 0x03ce);		// Device I/O
outw(0x0001, 0x03ce);		// Device I/O
outw(0x0002, 0x03ce);		// Device I/O
outw(0x0003, 0x03ce);		// Device I/O
outw(0x0004, 0x03ce);		// Device I/O
outw(0x1005, 0x03ce);		// Device I/O
outw(0x0e06, 0x03ce);		// Device I/O
outw(0x0007, 0x03ce);		// Device I/O
outw(0xff08, 0x03ce);		// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outb(0xff, 0x03c6);		// Device I/O <--
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
vga_textmode_init();
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outb(0x01, 0x03c4);		// Device I/O <--
inw(0x03c4);			// Device I/O --> 0x2001
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f050, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> c0000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outb(0x01, 0x03c4);		// Device I/O <--
inw(0x03c4);			// Device I/O --> 0x2001
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outb(0x06, 0x03ce);		// Device I/O <--
inw(0x03ce);			// Device I/O --> 0x0e06
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outb(0x01, 0x03c4);		// Device I/O <--
inw(0x03c4);			// Device I/O --> 0x2001
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00041000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 8020298e
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000e1180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000302
outl(0x00008302, 0x1044);	// Device I/O
outl(0x00048250, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 80000000
outl(0x80000000, 0x1044);	// Device I/O
outl(0x000e1180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008302
outl(0x000e1180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008302
outl(0x000e1180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008302
outl(0x000c6200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00001000, 0x1044);	// Device I/O
outl(0x000c6200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00001000
outl(0x00001002, 0x1044);	// Device I/O
outl(0x000c7204, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000c7204, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0xabcd0000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f00c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000c6040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00030d07
outl(0x00021005, 0x1044);	// Device I/O
outl(0x000c6014, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 04800080
outl(0x88046004, 0x1044);	// Device I/O
outl(0x000c6014, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 88046004
outl(0x88046004, 0x1044);	// Device I/O
outl(0x000c7204, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> abcd0000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000e1180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008302
outl(0x00008302, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00060000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x057f04ff, 0x1044);	// Device I/O
outl(0x00060004, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x057f04ff, 0x1044);	// Device I/O
outl(0x00060008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x05370517, 0x1044);	// Device I/O
outl(0x0006000c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0333031f, 0x1044);	// Device I/O
outl(0x00060010, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0333031f, 0x1044);	// Device I/O
outl(0x00060014, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x03270323, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outb(0x01, 0x03c4);		// Device I/O <--
inw(0x03c4);			// Device I/O --> 0x2001
outl(0x0006001c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x02cf018f, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00070008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f050, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> c0000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outb(0x01, 0x03c4);		// Device I/O <--
inw(0x03c4);			// Device I/O --> 0x2001
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outb(0x06, 0x03ce);		// Device I/O <--
inw(0x03ce);			// Device I/O --> 0x0e06
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outb(0x01, 0x03c4);		// Device I/O <--
inw(0x03c4);			// Device I/O --> 0x2001
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0006001c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 02cf018f
outl(0x027f018f, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00068080, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x80800000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00068070, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00068074, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x05000320, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00070008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00070008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00060030, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x7e127ae1, 0x1044);	// Device I/O
outl(0x00060034, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00800000, 0x1044);	// Device I/O
outl(0x00060040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00020da7, 0x1044);	// Device I/O
outl(0x00060044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00080000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f000c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000040
outl(0x00002040, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f000c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00002040
outl(0x00002050, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00060100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00044000
outl(0x00044000, 0x1044);	// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00070008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000040, 0x1044);	// Device I/O
outl(0x000f0008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000040, 0x1044);	// Device I/O
outl(0x000f000c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00002050
outl(0x00022050, 0x1044);	// Device I/O
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00070008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000040
outl(0x00000050, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00070008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000050
outl(0x80000050, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x00041000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 8020298e
outl(0x0020298e, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000080, 0x1044);	// Device I/O
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f0018, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 000007ff
outl(0x000000ff, 0x1044);	// Device I/O
outl(0x000f1018, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 000007ff
outl(0x000000ff, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f000c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00022050
outl(0x001a2050, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00060100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00044000
outl(0x001c4000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00060100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 001c4000
outl(0x801c4000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f000c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 001a2050
outl(0x801a2050, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00060100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 801c4000
outl(0x801c4000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f000c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 801a2050
outl(0x801a2050, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f0014, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000100
outl(0x000f0014, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000100
outl(0x00000100, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00060100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 801c4000
outl(0x901c4000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f000c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 801a2050
outl(0x901a2050, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f0014, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000600
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000e0000, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x057f04ff, 0x1044);	// Device I/O
outl(0x000e0004, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x057f04ff, 0x1044);	// Device I/O
outl(0x000e0008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x05370517, 0x1044);	// Device I/O
outl(0x000e000c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0333031f, 0x1044);	// Device I/O
outl(0x000e0010, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0333031f, 0x1044);	// Device I/O
outl(0x000e0014, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x03270323, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00060100, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 901c4000
outl(0xb01c4000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f000c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 901a2050
outl(0xb01a2050, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000f0008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000040
outl(0x80000040, 0x1044);	// Device I/O
outl(0x000e1180, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00008302
outl(0x80008302, 0x1044);	// Device I/O
outl(0x000c7204, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0xabcd0000, 0x1044);	// Device I/O
outl(0x000c7204, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> abcd0000
outl(0xabcd0002, 0x1044);	// Device I/O
outl(0x000c7204, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> abcd0002
outl(0xabcd0003, 0x1044);	// Device I/O
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d000000a
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> d0000009
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> c0000008
outl(0x000c7200, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> c0000008
outl(0x000c7204, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> abcd0003
outl(0x00000003, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f040, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 01000008
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000400, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000400
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000400
outl(0x00000000, 0x1044);	// Device I/O
outl(0x0004f044, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x0004f04c, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 03030000
outl(0x000c4030, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00001000
outl(0x000c4030, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00001000
outl(0x00001000, 0x1044);	// Device I/O
outl(0x000c4008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x000c4008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x000c4008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00044008, 0x1040);	// Device I/O
inl(0x1044);			// Device I/O --> 00000000
outl(0x00000000, 0x1044);	// Device I/O
