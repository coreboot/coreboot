/*
 * Enable the serial evices on the VIA
 */


/* The base address is 0x15c, 0x2e, depending on config bytes */

#define SIO_BASE $0x3f0
#define SIO_DATA  SIO_BASE+1

#define WRITESUPER(register, value) movb register, %al ;\
				movw SIO_BASE, %dx ;\
				outb %al, %dx ;\
				movb value, %al ;\
				movw SIO_DATA, %dx ;\
				outb %al, %dx
#define WRITESIOBYTE(register, value) movw register, %dx ;\
				movb value, %al ;\
				outb %al, %dx
#define WRITESIOWORD(register, value) movw register, %dx ;\
				movw value, %ax ;\
				outw %ax, %dx
/* first, you have to enable the superio and superio config. 
   put a 3 in devfn 38 register 85
 */
	movl	$0x3885, %eax
	movb	$3, %dl
	PCI_WRITE_CONFIG_BYTE
// now go ahead and set up com1. 
// set address
	WRITESUPER($0xe7, $0xfe)
// enable serial out
	WRITESUPER($0xe2, $7)
// That's it for the sio stuff.
	movl	$0x3885, %eax
	movb	$9, %dl
	PCI_WRITE_CONFIG_BYTE
// set up register to set baud rate.
	WRITESIOBYTE($0x3fb, $0x80)
// Set 115 kb
	WRITESIOWORD($0x3f8, $1)
// Set 9.6 kb
//	WRITESIOWORD($0x3f8, $12)
// now set no parity, one stop, 8 bits
	WRITESIOBYTE($0x3fb, $3)
// now turn on RTS, DRT
	WRITESIOBYTE($0x3fc, $3)
// Enable interrupts
	WRITESIOBYTE($0x3f9, $0xf)
// should be done. Dump a char for fun.
	WRITESIOBYTE($0x3f8, $48)
	
