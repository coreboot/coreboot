/*
 * Enable the peripheral devices on the IT8671F Super IO chip
 */

/* The base address is 0x3F0, 0x3bd or 0x370, depending on config bytes */

#define SIO_BASE $0x3f0
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


	/* Perform MB PnP setup to put the SIO chip at 0x3f0 */

	/* Base address 0x3f0 :	 0x86 0x80 0x55 0x55 */
	/* Base address 0x3bd :	 0x86 0x80 0x55 0xaa */
	/* Base address 0x370 :	 0x86 0x80 0xaa 0x55 */
		
	mov $0x279, %dx
	mov $0x86, %al
	outb %al, %dx
	mov $0x80, %al
	outb %al, %dx
	mov $0x55, %al
	outb %al, %dx
	mov $0x55, %al
	outb %al, %dx

	/* Enter the configuration state */

	mov SIO_BASE, %dx

	/* 6a, b5, da, ed : f6, fb, 7d, be */

	mov $0x6a, %al
	outb %al, %dx
	mov $0xb5, %al
	outb %al, %dx
	mov $0xda, %al
	outb %al, %dx
	mov $0xed, %al
	outb %al, %dx

	mov $0xf6, %al
	outb %al, %dx
	mov $0xfb, %al
	outb %al, %dx
	mov $0x7d, %al
	outb %al, %dx
	mov $0xbe, %al
	outb %al, %dx
			
	/* df, 6f, 37, 1b : 0d, 86, c3, 61 */

	mov $0xdf, %al
	outb %al, %dx
	mov $0x6f, %al
	outb %al, %dx
	mov $0x37, %al
	outb %al, %dx
	mov $0x1b, %al
	outb %al, %dx

	mov $0x0d, %al
	outb %al, %dx
	mov $0x86, %al
	outb %al, %dx
	mov $0xc3, %al
	outb %al, %dx
	mov $0x61, %al
	outb %al, %dx
			
	/* b0, 58, 2c, 16 : 8b, 45, a2, d1 */

	mov $0xb0, %al
	outb %al, %dx
	mov $0x58, %al
	outb %al, %dx
	mov $0x2c, %al
	outb %al, %dx
	mov $0x16, %al
	outb %al, %dx

	mov $0x8b, %al
	outb %al, %dx
	mov $0x45, %al
	outb %al, %dx
	mov $0xa2, %al
	outb %al, %dx
	mov $0xd1, %al
	outb %al, %dx

	/* e8, 74, 3a, 9d : ce, e7, 73, 39 */

	mov $0xe8, %al
	outb %al, %dx
	mov $0x74, %al
	outb %al, %dx
	mov $0x3a, %al
	outb %al, %dx
	mov $0x9d, %al
	outb %al, %dx

	mov $0xce, %al
	outb %al, %dx
	mov $0xe7, %al
	outb %al, %dx
	mov $0x73, %al
	outb %al, %dx
	mov $0x39, %al
	outb %al, %dx

	/* Enable parallel port, serial 1, serial port 2, Floppy */
	
	SIO_WRITE($0x00, $0x23, $0x0f)

	/* Activate serial port 1 and 2 */

	SIO_WRITE($0x01, $0x30, $0x1)
	SIO_WRITE($0x02, $0x30, $0x1)

	/* Select 24MHz CLKIN and clear software suspend mode */
	
	SIO_WRITE($0x00, $0x24, $0x00)

	/* Exit the configuration state */

	SIO_WRITE($0x00, $0x02, $0x02); 
