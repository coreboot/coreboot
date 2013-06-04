my_outb(0x03c2, 0x23);		// Device I/O <--
my_outb(0x03da, 0x02);		// Device I/O <--
my_inb(0x03c2);			// Device I/O --> 10
my_outb(0x03da, 0x01);		// Device I/O <--
my_inb(0x03c2);			// Device I/O --> 10
my_outl(0x1040, 0x00070080);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x00070180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x00071180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x00041000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00002900
my_outl(0x1044, 0x8000298e);	// Device I/O 
my_outl(0x1040, 0x0007019c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0007119c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x00000000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> ffffffff
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x00000000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> ffffffff
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x00000000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> ffffffff
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x00000000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> ffffffff
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x00000000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> ffffffff
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x000fc008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 2c010757
my_outl(0x1044, 0x2c010000);	// Device I/O 
my_outl(0x1040, 0x000fc020);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 2c010757
my_outl(0x1044, 0x2c010000);	// Device I/O 
my_outl(0x1040, 0x000fc038);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 2c010757
my_outl(0x1044, 0x2c010000);	// Device I/O 
my_outl(0x1040, 0x000fc050);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 2c010757
my_outl(0x1044, 0x2c010000);	// Device I/O 
my_outl(0x1040, 0x000fc408);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 2c010757
my_outl(0x1044, 0x2c010000);	// Device I/O 
my_outl(0x1040, 0x000fc420);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 2c010757
my_outl(0x1044, 0x2c010000);	// Device I/O 
my_outl(0x1040, 0x000fc438);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 2c010757
my_outl(0x1044, 0x2c010000);	// Device I/O 
my_outl(0x1040, 0x000fc450);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 2c010757
my_outl(0x1044, 0x2c010000);	// Device I/O 
my_outw(0x03ce, 0x0018);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x01000001);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f048);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x03030000);	// Device I/O 
my_outl(0x1040, 0x0004f050);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f054);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000001);	// Device I/O 
my_outl(0x1040, 0x0004f058);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1044, 0x03030000);	// Device I/O 
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1044, 0x03030000);	// Device I/O 
my_outl(0x1040, 0x00042004);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x02000000);	// Device I/O 
my_outl(0x1040, 0x000fd034);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 39cfffe0
my_outl(0x1044, 0x8421ffe0);	// Device I/O 
int i;
for (i = 0; i < 0x1fff; i++) {
	my_outl(0x1040, 0x00000001 | (i << 2));	// Device I/O 
	my_outl(0x1044, 0xc2000001 | (i << 12));	// Device I/O 
}

my_outw(0x03c4, 0x0302);	// Device I/O 
my_outw(0x03c4, 0x0003);	// Device I/O 
my_outw(0x03c4, 0x0204);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outw(0x03c4, 0x0300);	// Device I/O 
my_outb(0x03c2, 0x67);		// Device I/O <--
my_outb(0x03d4, 0x11);		// Device I/O <--
my_inw(0x03d4);			// Device I/O --> 0x0011
my_outw(0x03d4, 0x0011);	// Device I/O 
my_outw(0x03d4, 0x5f00);	// Device I/O 
my_outw(0x03d4, 0x4f01);	// Device I/O 
my_outw(0x03d4, 0x5002);	// Device I/O 
my_outw(0x03d4, 0x8203);	// Device I/O 
my_outw(0x03d4, 0x5504);	// Device I/O 
my_outw(0x03d4, 0x8105);	// Device I/O 
my_outw(0x03d4, 0xbf06);	// Device I/O 
my_outw(0x03d4, 0x1f07);	// Device I/O 
my_outw(0x03d4, 0x0008);	// Device I/O 
my_outw(0x03d4, 0x4f09);	// Device I/O 
my_outw(0x03d4, 0x0d0a);	// Device I/O 
my_outw(0x03d4, 0x0e0b);	// Device I/O 
my_outw(0x03d4, 0x000c);	// Device I/O 
my_outw(0x03d4, 0x000d);	// Device I/O 
my_outw(0x03d4, 0x000e);	// Device I/O 
my_outw(0x03d4, 0x000f);	// Device I/O 
my_outw(0x03d4, 0x9c10);	// Device I/O 
my_outw(0x03d4, 0x8e11);	// Device I/O 
my_outw(0x03d4, 0x8f12);	// Device I/O 
my_outw(0x03d4, 0x2813);	// Device I/O 
my_outw(0x03d4, 0x1f14);	// Device I/O 
my_outw(0x03d4, 0x9615);	// Device I/O 
my_outw(0x03d4, 0xb916);	// Device I/O 
my_outw(0x03d4, 0xa317);	// Device I/O 
my_outw(0x03d4, 0xff18);	// Device I/O 
my_inb(0x03da);			// Device I/O --> 31
my_inb(0x03ba);			// Device I/O --> ff
my_inb(0x03da);			// Device I/O --> 21
my_inb(0x03ba);			// Device I/O --> ff
my_inb(0x03da);			// Device I/O --> 01
my_inb(0x03ba);			// Device I/O --> ff
my_outw(0x03ce, 0x0000);	// Device I/O 
my_outw(0x03ce, 0x0001);	// Device I/O 
my_outw(0x03ce, 0x0002);	// Device I/O 
my_outw(0x03ce, 0x0003);	// Device I/O 
my_outw(0x03ce, 0x0004);	// Device I/O 
my_outw(0x03ce, 0x1005);	// Device I/O 
my_outw(0x03ce, 0x0e06);	// Device I/O 
my_outw(0x03ce, 0x0007);	// Device I/O 
my_outw(0x03ce, 0xff08);	// Device I/O 
my_outl(0x1040, 0x000e1100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000e1100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x000e1100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00010000);	// Device I/O 
my_outl(0x1040, 0x000e1100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00010000
my_outl(0x1040, 0x000e1100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00010000
my_outl(0x1040, 0x000e1100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000e1100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x000e1100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f054);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000001
my_outl(0x1040, 0x0004f054);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000001
my_outl(0x1044, 0x00000001);	// Device I/O 
my_outl(0x1040, 0x000e4200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0000001c
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00050000
my_outl(0x1044, 0x8004003e);	// Device I/O 
my_outl(0x1040, 0x000e4214);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x80060002);	// Device I/O 
my_outl(0x1040, 0x000e4218);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x01000000);	// Device I/O 
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1044, 0x5344003e);	// Device I/O 
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0144003e
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0144003e
my_outl(0x1044, 0x8074003e);	// Device I/O 
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1044, 0x5344003e);	// Device I/O 
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0144003e
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0144003e
my_outl(0x1044, 0x8074003e);	// Device I/O 
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1044, 0x5344003e);	// Device I/O 
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0144003e
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0144003e
my_outl(0x1044, 0x8074003e);	// Device I/O 
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1040, 0x000e4210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 5144003e
my_outl(0x1044, 0x5344003e);	// Device I/O 
my_outl(0x1040, 0x000e4f00);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0100038e
my_outl(0x1044, 0x0100030c);	// Device I/O 
my_outl(0x1040, 0x000e4f04);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00b8338e
my_outl(0x1044, 0x00b8230c);	// Device I/O 
my_outl(0x1040, 0x000e4f08);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0178838e
my_outl(0x1044, 0x06f8930c);	// Device I/O 
my_outl(0x1040, 0x000e4f0c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 09f8e38e
my_outl(0x1044, 0x09f8e38e);	// Device I/O 
my_outl(0x1040, 0x000e4f10);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00b8038e
my_outl(0x1044, 0x00b8030c);	// Device I/O 
my_outl(0x1040, 0x000e4f14);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0978838e
my_outl(0x1044, 0x0b78830c);	// Device I/O 
my_outl(0x1040, 0x000e4f18);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 09f8b38e
my_outl(0x1044, 0x0ff8d3cf);	// Device I/O 
my_outl(0x1040, 0x000e4f1c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0178038e
my_outl(0x1044, 0x01e8030c);	// Device I/O 
my_outl(0x1040, 0x000e4f20);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 09f8638e
my_outl(0x1044, 0x0ff863cf);	// Device I/O 
my_outl(0x1040, 0x000e4f24);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 09f8038e
my_outl(0x1044, 0x0ff803cf);	// Device I/O 
my_outl(0x1040, 0x000c4030);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00001000);	// Device I/O 
my_outl(0x1040, 0x000c4000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000c4030);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00001000
my_outl(0x1044, 0x00001000);	// Device I/O 
my_outl(0x1040, 0x000e1150);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0000001c
my_outl(0x1040, 0x000e1150);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0000001c
my_outl(0x1044, 0x0000089c);	// Device I/O 
my_outl(0x1040, 0x000fcc00);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01773f30
my_outl(0x1044, 0x01986f00);	// Device I/O 
my_outl(0x1040, 0x000fcc0c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01773f30
my_outl(0x1044, 0x01986f00);	// Device I/O 
my_outl(0x1040, 0x000fcc18);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01773f30
my_outl(0x1044, 0x01986f00);	// Device I/O 
my_outl(0x1040, 0x000fcc24);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01773f30
my_outl(0x1044, 0x01986f00);	// Device I/O 
my_outl(0x1040, 0x000c4000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000e1180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 40000002
my_inb(0x03d4);			// Device I/O --> 18
my_inb(0x03d6);			// Device I/O --> ff
my_inb(0x03d0);			// Device I/O --> ff
my_inb(0x03ce);			// Device I/O --> 08
my_inb(0x03d2);			// Device I/O --> ff
my_inb(0x03c4);			// Device I/O --> 00
my_inb(0x03c7);			// Device I/O --> 00
my_inb(0x03c8);			// Device I/O --> 00
my_outb(0x03c4, 0x01);		// Device I/O <--
my_inw(0x03c4);			// Device I/O --> 0x2001
my_outw(0x03c4, 0x2001);	// Device I/O 
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000800
my_outl(0x1040, 0x000c5100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000003);	// Device I/O 
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008800
my_outl(0x1040, 0x000c5120);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x000c5104);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x460000a0);	// Device I/O 
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0000ca00
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0000ca00
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0000ca00
my_outl(0x1040, 0x000c5120);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x000c5104);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 060000a0
my_outl(0x1044, 0x4a8000a1);	// Device I/O 
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a08
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a08
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> ffffff00
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a0c
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a0c
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00ffffff
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a10
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a10
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 4011ae30
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a14
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a14
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a18
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a18
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03011300
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a1c
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a1c
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 78101a80
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a20
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a20
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 9795baea
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a24
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a24
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 278c5559
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a28
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a28
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00545021
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a2c
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a2c
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01010000
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a30
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a30
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01010101
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a34
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a34
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01010101
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a38
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a38
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01010101
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a3c
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a3c
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 1b120101
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a40
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a40
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 20508000
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a44
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a44
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 20183014
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a48
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a48
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> a3050044
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a4c
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a4c
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 1f000010
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a50
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a50
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 80001693
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a54
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a54
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 30142050
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a58
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a58
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00442018
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a5c
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a5c
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0010a305
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a60
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a60
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00001f00
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a64
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a64
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 81000f00
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a68
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a68
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0a813c0a
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a6c
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a6c
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00091632
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a70
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a70
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01f0e430
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a74
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a74
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> fe000000
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a78
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a78
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 31504c00
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a7c
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008a7c
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 58573132
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008800
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008800
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 4c542d33
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008800
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008800
my_outl(0x1040, 0x000c510c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> ac003143
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008000
my_outl(0x1040, 0x000c5104);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 028000a1
my_outl(0x1044, 0x480000a0);	// Device I/O 
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008000
my_outl(0x1040, 0x000c5100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000003
my_outl(0x1044, 0x48000000);	// Device I/O 
my_outl(0x1040, 0x000c5108);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008000
my_outl(0x1044, 0x00008000);	// Device I/O 
my_outb(0x03c4, 0x01);		// Device I/O <--
my_inw(0x03c4);			// Device I/O --> 0x2001
my_outw(0x03c4, 0x0001);	// Device I/O 
my_outb(0x03d4, 0x18);		// Device I/O <--
my_outb(0x03d6, 0xff);		// Device I/O <--
my_outb(0x03d0, 0xff);		// Device I/O <--
my_outb(0x03ce, 0x08);		// Device I/O <--
my_outb(0x03d2, 0xff);		// Device I/O <--
my_outb(0x03c4, 0x00);		// Device I/O <--
my_outb(0x03c8, 0x00);		// Device I/O <--
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000e1180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 40000002
my_outl(0x1044, 0x00000300);	// Device I/O 
my_outl(0x1040, 0x000c7208);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00fa09c4);	// Device I/O 
my_outl(0x1040, 0x000c720c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00fa09c4);	// Device I/O 
my_outl(0x1040, 0x000c7210);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00186904
my_outl(0x1044, 0x00186903);	// Device I/O 
my_outl(0x1040, 0x00048250);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x80000000);	// Device I/O 
my_outl(0x1040, 0x00048254);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x061a061a);	// Device I/O 
my_outl(0x1040, 0x000c8254);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x061a061a);	// Device I/O 
my_outl(0x1040, 0x000c8250);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x000c8250);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x80000000);	// Device I/O 
my_outl(0x1040, 0x000c7204);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000c4000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f054);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000001
my_outl(0x1044, 0x0000020d);	// Device I/O 
my_outl(0x1040, 0x0004f054);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0000020d
my_outl(0x1040, 0x0004f050);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f050);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f054);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0000020d
my_outl(0x1044, 0x0000020d);	// Device I/O 
my_outl(0x1040, 0x0004f050);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0xc0000000);	// Device I/O 
my_outl(0x1040, 0x0004f054);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0000020d
my_outl(0x1040, 0x0004f054);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 0000020d
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000400);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000400
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000400
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outb(0x03c6, 0xff);		// Device I/O <--
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1044, 0x03300000);	// Device I/O 
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03300000
my_outl(0x1044, 0x30300000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 30300000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 30300000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 30300000
my_outl(0x1040, 0x0004f048);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 30300000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 30300000
my_outl(0x1044, 0x30030000);	// Device I/O 
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 30030000
my_outl(0x1044, 0x03030000);	// Device I/O 

vga_textmode_init();

my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000001
my_outl(0x1044, 0x01000008);	// Device I/O 
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1044, 0x03030000);	// Device I/O 
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1044, 0x03030000);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00070080);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000700c0);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outb(0x03c4, 0x01);		// Device I/O <--
my_inw(0x03c4);			// Device I/O --> 0x0001
my_outw(0x03c4, 0x2001);	// Device I/O 
my_outl(0x1040, 0x00041000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 8000298e
my_outl(0x1040, 0x00041000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 8000298e
my_outl(0x1044, 0x8000298e);	// Device I/O 
my_outl(0x1040, 0x00070180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00071180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00068070);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00068080);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00068074);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000400);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000400
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000400
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00041000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 8000298e
my_outl(0x1044, 0x8020298e);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outw(0x03ce, 0x0010);	// Device I/O 
my_outw(0x03ce, 0x0011);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outw(0x03c4, 0x0100);	// Device I/O 
my_outw(0x03c4, 0x2001);	// Device I/O 
my_outw(0x03c4, 0x0302);	// Device I/O 
my_outw(0x03c4, 0x0003);	// Device I/O 
my_outw(0x03c4, 0x0204);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outw(0x03c4, 0x0300);	// Device I/O 
my_outb(0x03c2, 0x67);		// Device I/O <--
my_outb(0x03d4, 0x11);		// Device I/O <--
my_inw(0x03d4);			// Device I/O --> 0x8e11
my_outw(0x03d4, 0x0e11);	// Device I/O 
my_outw(0x03d4, 0x5f00);	// Device I/O 
my_outw(0x03d4, 0x4f01);	// Device I/O 
my_outw(0x03d4, 0x5002);	// Device I/O 
my_outw(0x03d4, 0x8203);	// Device I/O 
my_outw(0x03d4, 0x5504);	// Device I/O 
my_outw(0x03d4, 0x8105);	// Device I/O 
my_outw(0x03d4, 0xbf06);	// Device I/O 
my_outw(0x03d4, 0x1f07);	// Device I/O 
my_outw(0x03d4, 0x0008);	// Device I/O 
my_outw(0x03d4, 0x4f09);	// Device I/O 
my_outw(0x03d4, 0x0d0a);	// Device I/O 
my_outw(0x03d4, 0x0e0b);	// Device I/O 
my_outw(0x03d4, 0x000c);	// Device I/O 
my_outw(0x03d4, 0x000d);	// Device I/O 
my_outw(0x03d4, 0x000e);	// Device I/O 
my_outw(0x03d4, 0x000f);	// Device I/O 
my_outw(0x03d4, 0x9c10);	// Device I/O 
my_outw(0x03d4, 0x8e11);	// Device I/O 
my_outw(0x03d4, 0x8f12);	// Device I/O 
my_outw(0x03d4, 0x2813);	// Device I/O 
my_outw(0x03d4, 0x1f14);	// Device I/O 
my_outw(0x03d4, 0x9615);	// Device I/O 
my_outw(0x03d4, 0xb916);	// Device I/O 
my_outw(0x03d4, 0xa317);	// Device I/O 
my_outw(0x03d4, 0xff18);	// Device I/O 
my_inb(0x03da);			// Device I/O --> 01
my_inb(0x03ba);			// Device I/O --> ff
my_inb(0x03da);			// Device I/O --> 21
my_inb(0x03ba);			// Device I/O --> ff
my_inb(0x03da);			// Device I/O --> 01
my_inb(0x03ba);			// Device I/O --> ff
my_outw(0x03ce, 0x0000);	// Device I/O 
my_outw(0x03ce, 0x0001);	// Device I/O 
my_outw(0x03ce, 0x0002);	// Device I/O 
my_outw(0x03ce, 0x0003);	// Device I/O 
my_outw(0x03ce, 0x0004);	// Device I/O 
my_outw(0x03ce, 0x1005);	// Device I/O 
my_outw(0x03ce, 0x0e06);	// Device I/O 
my_outw(0x03ce, 0x0007);	// Device I/O 
my_outw(0x03ce, 0xff08);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outb(0x03c6, 0xff);		// Device I/O <--
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
vga_textmode_init();
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outb(0x03c4, 0x01);		// Device I/O <--
my_inw(0x03c4);			// Device I/O --> 0x2001
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f050);	// Device I/O 
my_inl(0x1044);			// Device I/O --> c0000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outb(0x03c4, 0x01);		// Device I/O <--
my_inw(0x03c4);			// Device I/O --> 0x2001
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outb(0x03ce, 0x06);		// Device I/O <--
my_inw(0x03ce);			// Device I/O --> 0x0e06
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outb(0x03c4, 0x01);		// Device I/O <--
my_inw(0x03c4);			// Device I/O --> 0x2001
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00041000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 8020298e
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000e1180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000302
my_outl(0x1044, 0x00008302);	// Device I/O 
my_outl(0x1040, 0x00048250);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 80000000
my_outl(0x1044, 0x80000000);	// Device I/O 
my_outl(0x1040, 0x000e1180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008302
my_outl(0x1040, 0x000e1180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008302
my_outl(0x1040, 0x000e1180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008302
my_outl(0x1040, 0x000c6200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00001000);	// Device I/O 
my_outl(0x1040, 0x000c6200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00001000
my_outl(0x1044, 0x00001002);	// Device I/O 
my_outl(0x1040, 0x000c7204);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000c7204);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0xabcd0000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f00c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000c6040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00030d07
my_outl(0x1044, 0x00021005);	// Device I/O 
my_outl(0x1040, 0x000c6014);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 04800080
my_outl(0x1044, 0x88046004);	// Device I/O 
my_outl(0x1040, 0x000c6014);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 88046004
my_outl(0x1044, 0x88046004);	// Device I/O 
my_outl(0x1040, 0x000c7204);	// Device I/O 
my_inl(0x1044);			// Device I/O --> abcd0000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000e1180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008302
my_outl(0x1044, 0x00008302);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00060000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x057f04ff);	// Device I/O 
my_outl(0x1040, 0x00060004);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x057f04ff);	// Device I/O 
my_outl(0x1040, 0x00060008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x05370517);	// Device I/O 
my_outl(0x1040, 0x0006000c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x0333031f);	// Device I/O 
my_outl(0x1040, 0x00060010);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x0333031f);	// Device I/O 
my_outl(0x1040, 0x00060014);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x03270323);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outb(0x03c4, 0x01);		// Device I/O <--
my_inw(0x03c4);			// Device I/O --> 0x2001
my_outl(0x1040, 0x0006001c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x02cf018f);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00070008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f050);	// Device I/O 
my_inl(0x1044);			// Device I/O --> c0000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outb(0x03c4, 0x01);		// Device I/O <--
my_inw(0x03c4);			// Device I/O --> 0x2001
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outb(0x03ce, 0x06);		// Device I/O <--
my_inw(0x03ce);			// Device I/O --> 0x0e06
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outb(0x03c4, 0x01);		// Device I/O <--
my_inw(0x03c4);			// Device I/O --> 0x2001
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0006001c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 02cf018f
my_outl(0x1044, 0x027f018f);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00068080);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x80800000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00068070);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00068074);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x05000320);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00070008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00070008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00060030);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x7e127ae1);	// Device I/O 
my_outl(0x1040, 0x00060034);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00800000);	// Device I/O 
my_outl(0x1040, 0x00060040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00020da7);	// Device I/O 
my_outl(0x1040, 0x00060044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00080000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f000c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000040
my_outl(0x1044, 0x00002040);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f000c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00002040
my_outl(0x1044, 0x00002050);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00060100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00044000
my_outl(0x1044, 0x00044000);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00070008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000040);	// Device I/O 
my_outl(0x1040, 0x000f0008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000040);	// Device I/O 
my_outl(0x1040, 0x000f000c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00002050
my_outl(0x1044, 0x00022050);	// Device I/O 
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00070008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000040
my_outl(0x1044, 0x00000050);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00070008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000050
my_outl(0x1044, 0x80000050);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x00041000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 8020298e
my_outl(0x1044, 0x0020298e);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000080);	// Device I/O 
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f0018);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 000007ff
my_outl(0x1044, 0x000000ff);	// Device I/O 
my_outl(0x1040, 0x000f1018);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 000007ff
my_outl(0x1044, 0x000000ff);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f000c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00022050
my_outl(0x1044, 0x001a2050);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00060100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00044000
my_outl(0x1044, 0x001c4000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00060100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 001c4000
my_outl(0x1044, 0x801c4000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f000c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 001a2050
my_outl(0x1044, 0x801a2050);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00060100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 801c4000
my_outl(0x1044, 0x801c4000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f000c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 801a2050
my_outl(0x1044, 0x801a2050);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f0014);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000100
my_outl(0x1040, 0x000f0014);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000100
my_outl(0x1044, 0x00000100);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00060100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 801c4000
my_outl(0x1044, 0x901c4000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f000c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 801a2050
my_outl(0x1044, 0x901a2050);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f0014);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000600
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000e0000);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x057f04ff);	// Device I/O 
my_outl(0x1040, 0x000e0004);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x057f04ff);	// Device I/O 
my_outl(0x1040, 0x000e0008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x05370517);	// Device I/O 
my_outl(0x1040, 0x000e000c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x0333031f);	// Device I/O 
my_outl(0x1040, 0x000e0010);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x0333031f);	// Device I/O 
my_outl(0x1040, 0x000e0014);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x03270323);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00060100);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 901c4000
my_outl(0x1044, 0xb01c4000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f000c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 901a2050
my_outl(0x1044, 0xb01a2050);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000f0008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000040
my_outl(0x1044, 0x80000040);	// Device I/O 
my_outl(0x1040, 0x000e1180);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00008302
my_outl(0x1044, 0x80008302);	// Device I/O 
my_outl(0x1040, 0x000c7204);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0xabcd0000);	// Device I/O 
my_outl(0x1040, 0x000c7204);	// Device I/O 
my_inl(0x1044);			// Device I/O --> abcd0000
my_outl(0x1044, 0xabcd0002);	// Device I/O 
my_outl(0x1040, 0x000c7204);	// Device I/O 
my_inl(0x1044);			// Device I/O --> abcd0002
my_outl(0x1044, 0xabcd0003);	// Device I/O 
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d000000a
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> d0000009
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> c0000008
my_outl(0x1040, 0x000c7200);	// Device I/O 
my_inl(0x1044);			// Device I/O --> c0000008
my_outl(0x1040, 0x000c7204);	// Device I/O 
my_inl(0x1044);			// Device I/O --> abcd0003
my_outl(0x1044, 0x00000003);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f040);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 01000008
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000400);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000400
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000400
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x0004f044);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x0004f04c);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 03030000
my_outl(0x1040, 0x000c4030);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00001000
my_outl(0x1040, 0x000c4030);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00001000
my_outl(0x1044, 0x00001000);	// Device I/O 
my_outl(0x1040, 0x000c4008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x000c4008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x000c4008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1040, 0x00044008);	// Device I/O 
my_inl(0x1044);			// Device I/O --> 00000000
my_outl(0x1044, 0x00000000);	// Device I/O 
