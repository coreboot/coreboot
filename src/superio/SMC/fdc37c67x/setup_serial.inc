/*
 * Enable the peripheral devices on the FDC37C67x Super IO chip
 *
 * I had to modify Tyson's code a little and add some Super IO stuff
 * from the Winbond setup_serial.inc to set the baudrate correctly
 * - Bharath
 */

/* The base address is either 0x3F0 or 0x370, depending on config pin */

#ifndef SMC_BASE
#error SMC_BASE must be defined as 0x3F0 or 0x370
#endif

#if (SMC_BASE == 0x370)
/* #warning SMC_BASE set to 0x370 */
#elif (SMC_BASE == 0x3f0)
/* #warning SMC_BASE set to 0x3f0 */
#else
#error SMC_BASE defined as other than 0x3F0 or 0x370
#endif

#define SMC_INDEX SMC_BASE
#define SMC_DATA  SMC_BASE+1

#define OUTIT(val, port) movb val, %al; \
	movw port, %dx; \
	outb %al, %dx

#define WRITESIOBYTE(register, value) movw register, %dx ;\
				movb value, %al ;\
				outb %al, %dx; OUTIT(%al, $0x80)

#define WRITESIOWORD(register, value) movw register, %dx ;\
				movw value, %ax ;\
				outw %ax, %dx; OUTIT(%al, $0x80)

#define SMC_READ(index) \
	mov index, %al		; \
	mov $SMC_INDEX, %dx	; \
	outb %al, %dx		; \
	inc %dx			; \
	inb %dx, %al		;

#define SMC_WRITE(device, data, index) \
	mov $0x07, %al		; \
	mov $SMC_INDEX, %dx  	; \
	outb %al, %dx		; \
	mov device, %al		; \
	mov $SMC_DATA, %dx	; \
	outb %al, %dx		; \
	mov data, %ah		; \
	mov index, %al		; \
	mov $SMC_INDEX, %dx	; \
	outb %al, %dx		; \
	inc %dx			; \
	mov %ah, %al		; \
	outb %al, %dx		;

	
	/* Enter the configuration state */

	mov $0x55, %al
	mov $SMC_BASE, %dx
	outb %al, %dx

	/* Check for Device ID */

	SMC_READ($0x20)
	cmp $0x40, %al 
	je 2f
	mov $0xfe, %al
	outb %al, $0x80
2:

	/* Set address of serial port 0 (Device 4)*/

	SMC_WRITE($0x04, $0x3, $0x60)	;
	SMC_WRITE($0x04, $0xf8, $0x61)	;

	/* Set address of serial port 1 (Device 5)*/

	SMC_WRITE($0x05, $0x2, $0x60)	;
	SMC_WRITE($0x05, $0xf8, $0x61)	;

	/* Set IRQs of serial ports */

	SMC_WRITE($0x04, $0x04, $0x70)	;
	SMC_WRITE($0x05, $0x03, $0x70)  ;

	/* Set mode registers of serial ports */

	SMC_WRITE($0x04, $0x00, $0xf0)  ;
	SMC_WRITE($0x05, $0x00, $0xf0)  ;

	/* Set DMA for serial port 2 */

	SMC_WRITE($0x05, $0x4, $0x74)  ;

	/* Set IR registers for serial port 2 */

	SMC_WRITE($0x05, $0x2, $0xf1)  ;
	SMC_WRITE($0x05, $0x3, $0xf2)  ;

	/* Set valid bit for serial ports */

	SMC_WRITE($0x04, $0x01, $0x30)  ;
	SMC_WRITE($0x05, $0x01, $0x30)  ;

        /* Set baud rate */
 
	WRITESIOBYTE($0x3fb, $0x80)
	// Set 115 kb
	WRITESIOWORD($0x3f8, $1)
	// Set no parity, one stop, 8 data bits
	WRITESIOBYTE($0x3fb, $3)
	// Turn on RTS, DRT
	WRITESIOBYTE($0x3fc, $3)
	// Enable interrupts
	WRITESIOBYTE($0x3f9, $0xf)

	/* Exit the configuration state */

	mov $0xAA, %al
	mov $SMC_BASE, %dx
	outb %al, %dx



