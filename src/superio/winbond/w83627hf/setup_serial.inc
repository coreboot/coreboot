/*
 * Enable the peripheral devices on the windbond w83627hf
 */


/* The base address is 0x2e,0x4e depending on config bytes */
#include <superio/w83627hf.h>

#define SIO_INDEX SIO_BASE
#define SIO_DATA  SIO_BASE+1

#define SIO_ENTER_PNP_MODE() \
	movb	$0x87, %al		; \
	outb	%al, $(SIO_BASE)	; \
	outb	%al, $(SIO_BASE)

#define SIO_EXIT_PNP_MODE() \
	movb	$0xaa, %al		; \
	outb	%al, $(SIO_BASE)

#define SIO_WRITE_CONFIG(value, reg) \
	movb	$reg, %al		; \
	outb	%al, $(SIO_INDEX)	; \
	movb	$value, %al		; \
	outb	%al, $(SIO_DATA)

#define SIO_READ_CONFIG(value, reg) \
	movb	$reg, %al		; \
	outb	%al, $(SIO_INDEX)	; \
	inb	%al, $(SIO_DATA)

#define SIO_SET_LOGICAL_DEVICE(device) \
	SIO_WRITE_CONFIG(device, 0x07)

	/* Enable pnp */
	SIO_ENTER_PNP_MODE()

#if defined(SIO_SYSTEM_CLK_INPUT)
	/* Setup up the clock input */
	SIO_WRITE_CONFIG((0x84 | SIO_SYSTEM_CLK_INPUT), 0x24)
#endif
	/* enable serial 1 */
	SIO_SET_LOGICAL_DEVICE(COM1_DEVICE)
	SIO_WRITE_CONFIG(1, 0x30)
	SIO_WRITE_CONFIG(0x3, 0x60)
	SIO_WRITE_CONFIG(0xf8, 0x61)
	SIO_EXIT_PNP_MODE()

