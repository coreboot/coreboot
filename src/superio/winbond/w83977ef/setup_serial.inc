#define OUTIT(val, port) movb val, %al; \
	movw port, %dx; \
	outb %al, %dx

#ifndef SERIAL_SUPERIO_BASEADDRESS
#define SERIAL_SUPERIO_BASEADDRES $0x3f0
#endif

/* the second outit is a cheap delay */
#define OUTPNPADDR(val) OUTIT(val, SERIAL_SUPERIO_BASEADDRESS); OUTIT(val, $0xeb)
#define OUTPNPDATA(val) OUTIT(val, SERIAL_SUPERIO_BASEADDRESS+1); OUTIT(val, $0xeb)

/* to do: move this to a common include file! */
#define WRITESIOBYTE(register, value) movw register, %dx ;\
				movb value, %al ;\
				outb %al, %dx; 	OUTIT(%al, $0x80)

#define WRITESIOWORD(register, value) movw register, %dx ;\
				movw value, %ax ;\
				outw %ax, %dx; 	OUTIT(%al, $0x80)

/* turn on PnP */
	OUTPNPADDR($0x87)
	OUTPNPADDR($0x87)

/* select com1 */
	OUTPNPADDR($7)
	OUTPNPDATA($2)
/* set the enable in reg. 0x30 */
	OUTPNPADDR($0x30)
	OUTPNPDATA($1)
/* set the proper control bits for clock etc. in register 0x24 */
	OUTPNPADDR($0x24)
	OUTPNPDATA($0xc4)
/* who knows if we need this, but the bios does it. Set PIN58S to GP13 */
	OUTPNPADDR($0x2b)
	OUTPNPDATA($0x1)

/* turn off PnP */
	OUTPNPADDR($0xaa)
/* all done that nonsense -- from here on it's standard pc80 */
// set up register to set baud rate.
	WRITESIOBYTE($0x3fb, $0x80)
// Set 115 kb
// I don't think this thing can run at that rate. 
//	WRITESIOWORD($0x3f8, $1)
// Set 38.4 kb
	WRITESIOWORD($0x3f8, $3)
// now set no parity, one stop, 8 bits
	WRITESIOBYTE($0x3fb, $3)
// now turn on RTS, DRT
	WRITESIOBYTE($0x3fc, $3)
// Enable interrupts
	WRITESIOBYTE($0x3f9, $0xf)
// should be done. Dump a char for fun.
	WRITESIOBYTE($0x3f8, $48)
