#define OUTIT(val, port) movb val, %al; \
	outb %al, port

#define OUTPNPADDR(val) OUTIT(val, $0x2e); OUTIT(val, $0xeb)
#define OUTPNPDATA(val) OUTIT(val, $0x2f); OUTIT(val, $0xeb)

/* to do: move this to a common include file! */
#define WRITESIOBYTE(register, value) movw register, %dx ;\
	movb	value, %al ;\
	outb	%al, %dx;\
	outb	%al, $0xeb;\
	outb	%al, $0xeb

#define WRITESIOWORD(register, value) movw register, %dx ;\
	movw	value, %ax ;\
	outw	%ax, %dx;\
	outb	%al, $0xeb;\
	outb	%al, $0xeb

enable_serial:
	/* turn on PnP */
	OUTPNPADDR($0x87)
	OUTPNPADDR($0x87)

	/* select com1 */
	OUTPNPADDR($7)
	OUTPNPDATA($2)
	/* set the enable in reg. 0x30 */
	OUTPNPADDR($0x30)
	OUTPNPDATA($0x1)

	/* Serial Port 1 Base Address (FEh) */
	OUTPNPADDR($0x60)
	OUTPNPDATA($0xfe)
	/* Serial Port 1 IRQ (04h) */
	OUTPNPADDR($0x70)
	OUTPNPDATA($0x4)
	/* Serial Port 1 Control */
	OUTPNPADDR($0xf0)
	OUTPNPDATA($0x2)

	/* turn off PnP */
	OUTPNPADDR($0xaa)

	/* all done that nonsense -- from here on it's standard pc80 */
	/* Enable DLAB to set baud rate. */
	WRITESIOBYTE($0x3fb, $0x80)
	/* Set Buad Rate Divisor = 1==> 115 kb */
	WRITESIOWORD($0x3f8, $0x1)
	/* now set no parity, one stop, 8 bits, disable DLAB */
	WRITESIOBYTE($0x3fb, $0x3)
	/* now turn on RTS, DTR */
	WRITESIOBYTE($0x3fc, $0x3)
	/* Enable interrupts */
	WRITESIOBYTE($0x3f9, $0xf)

	/* should be done. Dump a char for fun. */
	WRITESIOBYTE($0x3f8, $0x30)

