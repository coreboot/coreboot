/*
 * Enable the peripheral devices on the windbond w83627hf
 */


/* The base address is 0x2e,0x4e depending on config bytes */

#define SIO_INDEX SIO_BASE
#define SIO_DATA  SIO_BASE+1

#define SIO_COM1_DEVICE 2

#define SIO_ENTER_PNP_MODE() \
	movw	$SIO_BASE, %dx	; \
	movb	$0x87, %al	; \
	outb	%al, %dx	; \
	outb	%al, %dx


#define SIO_EXIT_PNP_MODE() \
	movw	$SIO_BASE, %dx	; \
	movb	$0xaa, %al	; \
	outb	%al, %dx

#define SIO_WRITE_CONFIG(value, reg) \
	movw	$SIO_BASE, %dx	; \
	movb	$reg, %al	; \
	outb	%al, %dx	; \
	incw	%dx		; \
	movb	$value, %al	; \
	outb	%al, %dx

#define SIO_READ_CONFIG(value, reg) \
	movw	$SIO_BASE, %dx	; \
	movb	$reg, %al	; \
	outb	%al, %dx	; \
	incw	%dx		; \
	inb	%al, %dx

#define SIO_SET_LOGICAL_DEVICE(device) \
	SIO_WRITE_CONFIG(device, 0x07)


	/* enable serial 1 */
	SIO_ENTER_PNP_MODE()
	SIO_SET_LOGICAL_DEVICE(SIO_COM1_DEVICE)
	SIO_WRITE_CONFIG(1, 0x30)
	SIO_WRITE_CONFIG(0x3, 0x60)
	SIO_WRITE_CONFIG(0xf8, 0x61)
	SIO_EXIT_PNP_MODE()

