#include <console/console.h>
/*
 * Enable the serial evices on the VIA
 */


/* The base address is 0x15c, 0x2e, depending on config bytes */

#define SIO_BASE 0x3f0
#define SIO_DATA  SIO_BASE+1

static void vt8231_writesuper(uint8_t reg, uint8_t val)
{
	outb(reg, SIO_BASE);
	outb(val, SIO_DATA);
}

static void vt8231_writesiobyte(uint16_t reg, uint8_t val)
{
	outb(val, reg);
}

static void vt8231_writesioword(uint16_t reg, uint16_t val)
{
	outw(val, reg);
}


/* regs we use: 85, and the southbridge devfn is defined by the
   mainboard
 */

static void enable_vt8231_serial(void)
{
	uint8_t c;
	device_t dev;
	post_code(0x06);
	dev = pci_locate_device(PCI_ID(0x1106,0x8231), 0);

	if (dev == PCI_DEV_INVALID) {
		post_code(0x07);
		die("Serial controller not found\n");
	}

	/* first, you have to enable the superio and superio config.
	   put a 6 reg 80
	*/
	c = pci_read_config8(dev, 0x50);
	c |= 6;
	pci_write_config8(dev, 0x50, c);
	post_code(0x02);
	// now go ahead and set up com1.
	// set address
	vt8231_writesuper(0xf4, 0xfe);
	// enable serial out
	vt8231_writesuper(0xf2, 7);
	// That's it for the sio stuff.
	//	movl	$SUPERIOCONFIG, %eax
	//	movb	$9, %dl
	//	PCI_WRITE_CONFIG_BYTE
	// set up reg to set baud rate.
	vt8231_writesiobyte(0x3fb, 0x80);
	// Set 115 kb
	vt8231_writesioword(0x3f8, 1);
	// Set 9.6 kb
	//	WRITESIOWORD(0x3f8, 12)
	// now set no parity, one stop, 8 bits
	vt8231_writesiobyte(0x3fb, 3);
	// now turn on RTS, DRT
	vt8231_writesiobyte(0x3fc, 3);
	// Enable interrupts
	vt8231_writesiobyte(0x3f9, 0xf);
	// should be done. Dump a char for fun.
	vt8231_writesiobyte(0x3f8, 48);
}
