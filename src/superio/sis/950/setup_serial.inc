#define OUTIT(val, port) movb val, %al; \
	outb %al, port

#define OUT2E(val) OUTIT(val, $0x2e); OUTIT(val, $0xeb)
#define OUT2F(val) OUTIT(val, $0x2f); OUTIT(val, $0xeb)

/* to do: move this to a common include file! */
#define WRITESIOBYTE(register, value) movw register, %dx ;\
	movb	value, %al ;\
	outb	%al, %dx;\
	OUTIT(%al, $0x80)

#define WRITESIOWORD(register, value) movw register, %dx ;\
	movw	value, %ax ;\
	outw	%ax, %dx;\
	OUTIT(%al, $0x80)

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

	/* set the proper control bits for clock etc. in register 0x24 */
	OUT2E($0x24)
	/* NOTE: this also turns on all the proper bits for FLASH. 
	 * they're combined in here. 
	 */
	OUT2F($0xfd)

	/* turn off PnP */
	OUT2E($2)
	OUT2F($2)

	/* all done that nonsense -- from here on it's standard pc80 */

	/* Enable DLAB to set baud rate. */
	WRITESIOBYTE($0x3fb, $0x80)

	/* Set Buad Rate Divisor = 1==> 115 kb */
	WRITESIOWORD($0x3f8, $0x01)

	/* now set no parity, one stop, 8 bits, disable DLAB */
	WRITESIOBYTE($0x3fb, $0x03)
	/* now turn on RTS, DTR */
	WRITESIOBYTE($0x3fc, $0x03)
	/* Enable interrupts */
	WRITESIOBYTE($0x3f9, $0x0f)

	/* should be done. Dump a char for fun. */
	WRITESIOBYTE($0x3f8, $0x30)
