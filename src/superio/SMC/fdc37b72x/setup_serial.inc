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

	SMC_READ($0x20)
	cmp %al, $0x4c
	je 2f
	mov $0xfe, %al
	outb %al, $0x80
#if 0
1:	hlt
	jmp	1b
#endif
2:
	// GLOBAL SETUP
	/* Pin configuration - need to enable IRQ 4 output */
	
	SMC_WRITE($0x39, $0x22);
	SMC_WRITE($0x84, $0x24);

	// COM1 SETUO
	/* set the LDN to 4 (serial port 1) */
	SMC_WRITE($4, $7) ;

	/* Set address of serial port 0 */

	SMC_WRITE($0x3, $0x60)	;
	SMC_WRITE($0xf8, $0x61)	;

	/* Set IRQs of serial ports */

	SMC_WRITE($0x4, $0x70)	;

	/* Set valid bit */

	SMC_WRITE($0x1, $0x30)	;
	/* Exit the configuration state */

	mov $0xAA, %al
	mov $SMC_BASE, %dx
	outb %al, %dx

