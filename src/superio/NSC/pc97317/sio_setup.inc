/*
    freebios/src/superio/NSC/pc97317/sio_setup.inc

    Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

    Set up a National Semiconductor PC97317 Super I/O
*/

#ifndef SIO_BASE
#define SIO_BASE 0x2e
#endif

sio_boot:
	lea	sio_boot_table, %esi
	movl	$sio_boot_size, %ecx
	movw	$SIO_BASE, %dx

sio_boot_loop:
	movb	(%esi), %al
	incl	%esi
	outb	%al, %dx
	xorb	$1, %dl
	loop	sio_boot_loop

sio_rtc_boot:
	movb	$0x0a, %al		/* RTC Control Register A */
	outb	%al, $0x70		/* RTC */
	inb	$0x71, %al
	andb	$0xf0, %al
	orb	$0x06, %al		/* Interrupt rate  */
	outb	%al, $0x71

sio_wakeup:
	movb	$0x03, %al		/* Use on chip clock multiplier */
	outb	%al, $0xe8
	inb	$0xe9, %al
	orb	$0x07, %al
	outb	%al, $0xe9

sio_wakeup_wait:
	inb	$0xe9, %al		/* Wait for the clock to stabilize */
	testb	$0x80, %al
	jz	sio_wakeup_wait

sio_setup:
	lea	sio_setup_table, %esi
	movl	$sio_setup_size, %ecx
	mov	$SIO_BASE, %dx

sio_setup_loop:
	movb	(%esi), %al
	incl	%esi
	outb	%al, %dx
	xorb	$1, %dl
	loop	sio_setup_loop

sio_rtc_setup:
	movb	$0x0a, %al		/* RTC Control Register A */
	outb	%al, $0x70
	movb	$0x26, %al		/* Normal operation */
	outb	%al, $0x71

	movw	$0x70, %dx
	movb	$0x0d, %al
	outb	%al, %dx
	movw	$0x71, %dx
	inb	%dx, %al
	inb	%dx, %al
	movw	$0x70, %dx
	movb	$0x0b, %al
	outb	%al, %dx
	movw	$0x71, %dx
	movb	$0x02, %al
	outb	%al, %dx

	jmp	sio_setup_end

sio_boot_table:
	.byte	0x07, 0x08		/* LDN 8 -- Power Management */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, 0x00		/* Base High */
	.byte	0x61, 0xe8		/* Base Low */
	.byte	0x30, 0x01		/* Enable */
sio_boot_size = .-sio_boot_table

sio_setup_table:
	.byte	0x07, 0x00		/* Device 0 -- Keyboard */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x70, 0x01		/* IRQ 1 */
	.byte	0x30, 0x01		/* Enable */

	.byte	0x07, 0x02		/* Device 2 -- RTC & APC */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x70, 0x08		/* IRQ 8 */
	.byte	0x30, 0x01		/* Enable */
	
#ifdef SIO_PP_BASE
	.byte	0x07, 0x04		/* Device 4 -- Parallel Port */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, SIO_PP_BASE>>8 /* I/O Base Address */
	.byte	0x61, SIO_PP_BASE&0xff
	.byte	0x70, SIO_PP_IRQ	/* IRQ */
	.byte	0xf0, 0xf2		/* PP Configuration, ECP, enabled */
	.byte	0x30, 0x01		/* Enable */
#endif

#ifdef SIO_SP2_BASE
	.byte	0x07, 0x05		/* Device 5 -- Serial port 2 */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, SIO_SP2_BASE>>8	/* I/O Base Address */
	.byte	0x61, SIO_SP2_BASE&0xff
	.byte	0x70, SIO_SP2_IRQ	/* IRQ */
	.byte	0xf0, 0x82		/* Serial Configuration Register */
	.byte	0x30, 0x01		/* Enable */
#endif

#ifdef SIO_SP1_BASE
	.byte	0x07, 0x06		/* Device 6 -- Serial port 1 */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, SIO_SP1_BASE>>8	/* I/O Base Address */
	.byte	0x61, SIO_SP1_BASE&0xff
	.byte	0x70, SIO_SP1_IRQ	/* IRQ */
	.byte	0xf0, 0x82		/* Serial Configuration Register */
	.byte	0x30, 0x01		/* Enable */
#endif

#ifdef SIO_GPIO_BASE
	.byte	0x07, 0x07		/* Device 7 -- GPIO Ports */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, SIO_GPIO_BASE>>8	/* I/O Base Address */
	.byte	0x61, SIO_GPIO_BASE&0xff
	.byte	0x30, 0x01		/* Enable */
#endif

sio_setup_size = .-sio_setup_table

sio_setup_end:
	nop

/*
    Local variables:
        compile-command: "make -C /export/bios/voyager2"
        c-basic-offset: 8
    End:
*/
