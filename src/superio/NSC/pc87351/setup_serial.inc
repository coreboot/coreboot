/*
 * Enable the serial peripheral devices on the NSC Super IO chip PC87351
 */


/* The base address is 0x15c, 0x2e, depending on config bytes */

#define SIO_BASE $0x2e
#define SIO_INDEX SIO_BASE
#define SIO_DATA  SIO_BASE+1

#define SIO_READ(ldn, index) \
	mov SIO_BASE, %dx	; \
	mov $0x07, %al		; \
	outb %al, %dx		; \
	inc %dx			; \
	mov ldn, %al		; \
	outb %al, %dx		; \
	dec %dx			; \
	mov index, %al		; \
	outb %al, %dx		; \
	inc %dx			; \
	inb %dx, %al		;

#define SIO_WRITE(ldn, index, data) \
	mov SIO_BASE, %dx	; \
	mov $0x07, %al		; \
	outb %al, %dx		; \
	inc %dx			; \
	mov ldn, %al		; \
	outb %al, %dx		; \
	dec %dx			; \
	mov index, %al		; \
	outb %al, %dx		; \
	inc %dx			; \
	mov data, %al		; \
	outb %al, %dx		;


	/* At boot up the chip is in configure mode so don't worry
	 * about getting it there just configure some peripherals.
	 */

	/* enable serial 1 Logical device 3 */
	// the default address is 0x3f8
	SIO_WRITE($0x03, $0x30, $0x01)
	
	// enable serial 2 Logical device 2 
	// the default address is 0x2f8 
	SIO_WRITE($0x02, $0x30, $0x01)
