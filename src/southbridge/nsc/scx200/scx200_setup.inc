/*
    freebios/src/northsouthbridge/NSC/scx200/chipset_setup.inc

    Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

    Do chipset setup for a National Semiconductor SCx200 CPU.
*/

/**********************************************************************/

scx200_setup_start:
/* Configure F0 */
	movl	$f0_setup_table, %esi
	movl	$f0_setup_len, %ecx
	movl	$0x80009000, %ebx	/* F0 Config Space */

f0_setup_loop:
	movb	(%esi), %bl
	CALLSP(scx200_pci_read_config_byte)
	incl	%esi
	andb	(%esi), %al
	incl	%esi
	orb	(%esi), %al
	CALLSP(scx200_pci_write_config_byte)
	incl	%esi
	loopl	f0_setup_loop

/* Configure F5 */
	movl	$f5_setup_table, %esi
	movl	$f5_setup_len, %ecx
	movl	$0x80009500, %ebx	/* F5 Config Space */

f5_setup_loop:
	movb	(%esi), %bl
	CALLSP(scx200_pci_read_config_byte)
	incl	%esi
	andb	(%esi), %al
	incl	%esi
	orb	(%esi), %al
	CALLSP(scx200_pci_write_config_byte)
	incl	%esi
	loopl	f5_setup_loop

/* Configure the SIO address */
	movw	$0x6200, %dx		/* I/O Control Register */
	inl	%dx, %eax
	movl	$0x0f040000, %eax	/* SIO at 0x15c, Enable USB */
	outl	%eax, %dx

	movw	$0x15c, %dx		/* Super I/O */
	movb	$0x20, %al		/* Index 0x20, SIO Config */
	outb	%al, %dx
	xorw	$1, %dx
	inb	%dx, %al
	cmpb	$0xf5, %al		/* SIO ID */
	jnz	scx200_setup_start

config_block_setup:
	movw	$0x2ea, %dx		/* initial register to set address */
	movw	$SCx200_CB_BASE, %ax	/* Config Block Base Address */
	outw	%ax, %dx

	movw	$SCx200_CB_BASE, %bx	/* Config Block Base Address */
	movw	%bx, %dx
	addw	$0x30, %dx		/* PMR */
	movl	$SCx200_PMR, %eax
	outl	%eax, %dx
	movw	%bx, %dx
	addw	$0x34, %dx		/* MCR */
	movl	$SCx200_MCR, %eax
	outl	%eax, %dx
	movw	%bx, %dx
	addw	$0x12, %dx		/* PPCR */
	inb	%dx, %al
	andb	$0xd1, %al		/* Enable PLLs */
	outb	%al, %dx

#if 0
	/* But this stops the serial port from working. */
	movw	%bx, %dx
	addw	$0x18, %dx		/* PLL3 */
	movl	$0x7601d004, %eax	/* Must be programmed with this */
	outl	%eax, %dx		/*     value according to databook */
#endif

#ifdef WATCHDOG_INIT
watchdog_setup:
	movw	$SCx200_CB_BASE, %bx	/* Config Block Base Address */
	movw	%bx, %dx
	addw	$0x2, %dx		/* WDCNFG */
	movw	$0xfa, %ax		/* enable watchdog */
	outw	%ax, %dx
	movw	%bx, %dx		/* WDTO */
	movw	$32*(WATCHDOG_INIT), %ax
	outw	%ax, %dx
#endif

/* RTC setup */
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

/* GPIO setup */
	/* FIXME ought to do GPIO setup here */

/* Configure the SIO itself */
	lea	sio_setup_table, %esi
	movl	$sio_setup_size, %ecx
	mov	$0x15c, %dx

sio_setup_loop:
	movb	(%esi), %al
	incl	%esi
	outb	%al, %dx
	xorb	$1, %dl
	loop	sio_setup_loop

	jmp	scx200_setup_end

scx200_pci_read_config_byte:
	movw	$0xcf8, %dx
	movl	%ebx, %eax
	outl	%eax, %dx
	movw	%ax, %dx
	andw	$3, %dx
	orw	$0xcfc, %dx
	inb	%dx, %al
	RETSP

scx200_pci_write_config_byte:
	movw	$0xcf8, %dx
	xchgl	%ebx, %eax
	outl	%eax, %dx
	movw	%ax, %dx
	andw	$3, %dx
	orw	$0xcfc, %dx
	xchgl	%ebx, %eax
	outb	%al, %dx
	RETSP

f0_setup_table:
	.byte	0x10, 0x00, 0x01	/* F0BAR0/GPIO = 0x00006401 */
	.byte	0x11, 0x00, 0x64
	.byte	0x12, 0x00, 0x00
	.byte	0x13, 0x00, 0x00

	.byte	0x47, 0xfb, 0x06	/* Enable F0BAR0/GPIO, F0BAR1/LPC */
	.byte	0x5a, 0x00, 0x01	/* Enable RTC Positive Decode */
	.byte	0x5b, 0x00, 0x01	/* Enable LPC Positive Decode */

#ifdef SCx200_DOCCS_BASE
	.byte	0x78, 0x00, (SCx200_DOCCS_BASE>>0)&0xff
	.byte	0x79, 0x00, (SCx200_DOCCS_BASE>>8)&0xff
	.byte	0x7a, 0x00, (SCx200_DOCCS_BASE>>16)&0xff
	.byte	0x7b, 0x00, (SCx200_DOCCS_BASE>>24)&0xff

	.byte	0x7c, 0x00, (SCx200_DOCCS_CTRL>>0)&0xff
	.byte	0x7d, 0x00, (SCx200_DOCCS_CTRL>>8)&0xff
	.byte	0x7e, 0x00, (SCx200_DOCCS_CTRL>>16)&0xff
	.byte	0x7f, 0x00, (SCx200_DOCCS_CTRL>>24)&0xff
#endif

	.byte	0x43, 0xff, 0x80	/* Must be set to 1 */
	.byte	0x43, 0xfd, 0x00	/* Speed up PIT accesses */

f0_setup_len = (.-f0_setup_table) / 3

f5_setup_table:
	.byte	0x40, 0x00, 0xc1	/* F5BAR0 Mask = 0xffffffc1 */
	.byte	0x41, 0x00, 0xff
	.byte	0x42, 0x00, 0xff
	.byte	0x43, 0x00, 0xff

	.byte	0x10, 0x00, 0x01	/* F5BAR0/X-Bus = 0x00006201 */
	.byte	0x11, 0x00, 0x62
	.byte	0x12, 0x00, 0x00
	.byte	0x13, 0x00, 0x00

	.byte	0x04, 0x00, 0x03	/* Enable F5 Memory and I/O Space */
f5_setup_len = (.-f5_setup_table) / 3

sio_setup_table:
	.byte	0x07, 0x00		/* Device 0 -- RTC */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x70, 0x08		/* Use IRQ 8 */
	.byte	0x30, 0x01		/* Enable */


	.byte	0x07, 0x01		/* Device 1 -- System Wake-Up Ctrl */
	.byte	0x60, 0x08		/* I/O Base Address 0x860 */
	.byte	0x61, 0x60
	.byte	0x30, 0x01		/* Enable */

#ifdef SCx200_SP3_BASE
	.byte	0x07, 0x02		/* Device 2 -- Serial port 3 */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, SCx200_SP3_BASE>>8 /* I/O Base Address */
	.byte	0x61, SCx200_SP3_BASE&0xff
	.byte	0x70, SCx200_SP3_IRQ	/* IRQ */
	.byte	0xf0, 0x82		/* Serial Configuration Register */
	.byte	0x30, 0x01		/* Enable */
#endif

#ifdef SCx200_SP1_BASE
	.byte	0x07, 0x03		/* Device 3 -- Serial port 1 */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, SCx200_SP1_BASE>>8 /* I/O Base Address */
	.byte	0x61, SCx200_SP1_BASE&0xff
	.byte	0x70, SCx200_SP1_IRQ	/* IRQ */
	.byte	0xf0, 0x82		/* Serial Configuration Register */
	.byte	0x30, 0x01		/* Enable */
#endif

#ifdef SCx200_ACB1_BASE
	.byte	0x07, 0x05		/* Device 5 -- ACCESS.bus 1 */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, SCx200_ACB1_BASE>>8 /* I/O Base Address */
	.byte	0x61, SCx200_ACB1_BASE&0xff
	.byte	0x70, SCx200_ACB1_IRQ	/* IRQ */
	.byte	0x30, 0x01		/* Enable */
#endif

#ifdef SCx200_ACB2_BASE
	.byte	0x07, 0x05		/* Device 6 -- ACCESS.bus 2 */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, SCx200_ACB2_BASE>>8 /* I/O Base Address */
	.byte	0x61, SCx200_ACB2_BASE&0xff
	.byte	0x70, SCx200_ACB2_IRQ	/* IRQ */
	.byte	0x30, 0x01		/* Enable */
#endif

#ifdef SCx200_PP_BASE
	.byte	0x07, 0x08		/* Device 7 -- Parallel Port */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, SCx200_PP_BASE>>8 /* I/O Base Address */
	.byte	0x61, SCx200_PP_BASE&0xff
	.byte	0x70, SCx200_PP_IRQ	/* IRQ */
	.byte	0xf0, 0x82		/* Serial Configuration Register */
	.byte	0x30, 0x01		/* Enable */
#endif

#ifdef SCx200_SP2_BASE
	.byte	0x07, 0x08		/* Device 8 -- Serial port 2 */
	.byte	0x30, 0x00		/* Disable */
	.byte	0x60, SCx200_SP2_BASE>>8 /* I/O Base Address */
	.byte	0x61, SCx200_SP2_BASE&0xff
	.byte	0x70, SCx200_SP2_IRQ	/* IRQ */
	.byte	0xf0, 0x82		/* Serial Configuration Register */
	.byte	0x30, 0x01		/* Enable */
#endif
sio_setup_size = .-sio_setup_table

scx200_setup_end:

/*
    Local variables:
        compile-command: "make -C /export/nano/bios/nano"
    End:
*/
