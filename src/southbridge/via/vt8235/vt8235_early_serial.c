/*
 * Enable the serial evices on the VIA
 */


/* The base address is 0x15c, 0x2e, depending on config bytes */

#define SIO_BASE 0x3f0
#define SIO_DATA  SIO_BASE+1

static void vt8235_writepnpaddr(uint8_t val)
{
	outb(val, 0x2e);
	outb(val, 0xeb);
}

static void vt8235_writepnpdata(uint8_t val)
{
	outb(val, 0x2f);
	outb(val, 0xeb);
}


static void vt8235_writesiobyte(uint16_t reg, uint8_t val)
{
	outb(val, reg);
}

static void vt8235_writesioword(uint16_t reg, uint16_t val)
{
	outw(val, reg);
}


/* regs we use: 85, and the southbridge devfn is defined by the
   mainboard
 */

static void enable_vt8235_serial(void)
{
	// turn on pnp
	vt8235_writepnpaddr(0x87);
	vt8235_writepnpaddr(0x87);
	// now go ahead and set up com1.
	// set address
	vt8235_writepnpaddr(0x7);
	vt8235_writepnpdata(0x2);
	// enable serial out
	vt8235_writepnpaddr(0x30);
	vt8235_writepnpdata(0x1);
	// serial port 1 base address (FEh)
	vt8235_writepnpaddr(0x60);
	vt8235_writepnpdata(0xfe);
	// serial port 1 IRQ (04h)
	vt8235_writepnpaddr(0x70);
	vt8235_writepnpdata(0x4);
	// serial port 1 control
	vt8235_writepnpaddr(0xf0);
	vt8235_writepnpdata(0x2);
	// turn of pnp
	vt8235_writepnpaddr(0xaa);

	// set up reg to set baud rate.
	vt8235_writesiobyte(0x3fb, 0x80);
	// Set 115 kb
	vt8235_writesioword(0x3f8, 1);
	// Set 9.6 kb
	//	WRITESIOWORD(0x3f8, 12)
	// now set no parity, one stop, 8 bits
	vt8235_writesiobyte(0x3fb, 3);
	// now turn on RTS, DRT
	vt8235_writesiobyte(0x3fc, 3);
	// Enable interrupts
	vt8235_writesiobyte(0x3f9, 0xf);
	// should be done. Dump a char for fun.
	vt8235_writesiobyte(0x3f8, 48);
}
