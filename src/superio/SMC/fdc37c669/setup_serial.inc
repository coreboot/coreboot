/*
 * Enable the peripheral devices on the FDC37N769 Super IO chip
 */

/*
 * This code is untested but should at least get a serial port working.
 *   -Tyson
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

#define SMC_READ(index) \
	mov index, %al		; \
	mov $SMC_BASE, %dx	; \
	outb %al, %dx		; \
	inc %dx			; \
	inb %dx, %al		;

#define SMC_WRITE(data, index) \
	mov data, %ah		; \
	mov index, %al		; \
	mov $SMC_BASE, %dx	; \
	outb %al, %dx		; \
	inc %dx			; \
	mov %ah, %al		; \
	outb %al, %dx		;

	
	/* Enter the configuration state */

	mov $0x55, %al
	mov $SMC_BASE, %dx
	outb %al, %dx

	/* Check for Device ID */

	SMC_READ($0x0d)
	cmp %al, 0x04
	je 2f
	mov $0xfe, %al
	outb %al, $0x80
#if 0
1:	hlt
	jmp	1b
#endif
2:

	/* Pin configuration - need to enable IRQ 4 output */
	
	SMC_WRITE($0xf4, $0x03)	;

	SMC_WRITE($0x88, $0x02)	;

	/* Set address of parallel port */
#if 0
	SMC_WRITE($0xde, $0x23)	;
#endif
	/* Set address of serial port 0 */

	SMC_WRITE($0xfe, $0x24)	;

	/* Set address of serial port 1 */

	SMC_WRITE($0xbe, $0x25)	;

	/* Set IRQs of serial ports */

	SMC_WRITE($0x21, $0x28)	;

	/* Set valid bit */

	SMC_READ($0x00)
	or $0x80, %al
	SMC_WRITE(%al, $0x00)
	
	/* Exit the configuration state */

	mov $0xAA, %al
	mov $SMC_BASE, %dx
	outb %al, %dx

