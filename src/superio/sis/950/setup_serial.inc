#define OUTIT(val, port) movb val, %al; \
	outb %al, port

#define OUT2E(val) OUTIT(val, $0x2e)
#define OUT2F(val) OUTIT(val, $0x2f)
/* to do: move this to a common include file! */
#define WRITESIOBYTE(register, value) movw register, %dx ;\
				movb value, %al ;\
				outb %al, %dx
#define WRITESIOWORD(register, value) movw register, %dx ;\
				movw value, %ax ;\
				outw %ax, %dx

/* turn on PnP */
	OUT2E($0x87)
	OUT2E($1)
	OUT2E($0x55)
	OUT2E($0x55)

/* select com1 */
	OUT2E($7)
	OUT2F($1)
/* set the enable in reg. 0x30 */
	OUT2E($0x30)
	OUT2F($1)
/* turn off PnP */
	OUT2E($2)
	OUT2F($2)
/* all done that nonsense -- from here on it's standard pc80 */
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
