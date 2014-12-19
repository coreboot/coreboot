/*
 * Enable the serial devices on the VIA
 */


/* The base address is 0x15c, 0x2e, depending on config bytes */

#define SIO_BASE 0x3f0
#define SIO_DATA  SIO_BASE+1

static void vt8237r_writepnpaddr(uint8_t val)
{
	outb(val, 0x2e);
	outb(val, 0xeb);
}

static void vt8237r_writepnpdata(uint8_t val)
{
	outb(val, 0x2f);
	outb(val, 0xeb);
}


static void vt8237r_writesiobyte(uint16_t reg, uint8_t val)
{
	outb(val, reg);
}

static void vt8237r_writesioword(uint16_t reg, uint16_t val)
{
	outw(val, reg);
}


/* regs we use: 85, and the southbridge devfn is defined by the
   mainboard
 */

static void enable_vt8237r_serial(void)
{
	// turn on pnp
	vt8237r_writepnpaddr(0x87);
	vt8237r_writepnpaddr(0x87);
	// now go ahead and set up com1.
	// set address
	vt8237r_writepnpaddr(0x7);
	vt8237r_writepnpdata(0x2);
	// enable serial out
	vt8237r_writepnpaddr(0x30);
	vt8237r_writepnpdata(0x1);
	// serial port 1 base address (FEh)
	vt8237r_writepnpaddr(0x60);
	vt8237r_writepnpdata(0xfe);
	// serial port 1 IRQ (04h)
	vt8237r_writepnpaddr(0x70);
	vt8237r_writepnpdata(0x4);
	// serial port 1 control
	vt8237r_writepnpaddr(0xf0);
	vt8237r_writepnpdata(0x2);
	// turn of pnp
	vt8237r_writepnpaddr(0xaa);

	// set up reg to set baud rate.
	vt8237r_writesiobyte(0x3fb, 0x80);
	// Set 115 kb
	vt8237r_writesioword(0x3f8, 1);
	// Set 9.6 kb
	//	WRITESIOWORD(0x3f8, 12)
	// now set no parity, one stop, 8 bits
	vt8237r_writesiobyte(0x3fb, 3);
	// now turn on RTS, DRT
	vt8237r_writesiobyte(0x3fc, 3);
	// Enable interrupts
	vt8237r_writesiobyte(0x3f9, 0xf);
	// should be done. Dump a char for fun.
	vt8237r_writesiobyte(0x3f8, 48);
}
