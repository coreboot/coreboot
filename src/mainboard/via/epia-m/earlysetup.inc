// enable VGA device
	movl	$CONFIG_ADDR(0, 0x00, 0xe1), %eax
	movb	$0xdd, %dl // enable vga with 32 megs
	PCI_WRITE_CONFIG_BYTE


// The stupid firewire device (undocumented) steps on IO 0+ on reset
	movl	$CONFIG_ADDR(0, 0x0d*8, 0x15), %eax
	movb	$0x1c, %dl
	PCI_WRITE_CONFIG_BYTE

// use CMOS byte 0x0e as marker so we only reboot once
	movb	$0x0e, %al
	mov	$0x70, %dx
	outb	%al, %dx
	inc	%dx
	inb	%dx, %al
	cmpb	$0xda, %al
	je	1f

	movl	$0x100000, %ecx
0:
	movw	$0x70,%dx
	mov	$0x0e,%al
	outb	%al, %dx
	inc	%dx
	movb	$0xda, %al
	outb	%al, %dx
	inb	%dx, %al
	cmpb	$0xda, %al
	je	4f
	dec	%ecx
	jne	0b

4:

	CONSOLE_INFO_TX_CHAR($'R')
	CONSOLE_INFO_TX_CHAR($'B')
	CONSOLE_INFO_TX_CHAR($'!')
	movl	$CONFIG_ADDR(0, 0x88, 0x4f), %eax
	movb	$0x09, %dl
	PCI_WRITE_CONFIG_BYTE

2:
	jmp	2b

1:
	sub	%al,%al
	outb	%al, %dx
3:


	jmp	enable_smbus
enable_smbus_done:
	jmp	setup_smbus
setup_smbus_done:

	movl	$0x50,%ecx
spddump:
	movl	%ecx,%ebx
	CALLSP(smbus_read_byte)
	CONSOLE_INFO_TX_HEX8(%al)
	addl	$0x100,%ecx
	movl	%ecx,%eax
	and	$0x0f00,%eax
	jz	1f // need crlf
	CONSOLE_INFO_TX_CHAR($' ')
	jmp	2f
1:
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
2:
	movl	%ecx,%eax
	and	$0xff00,%eax
	cmpl	$0x8000,%eax
	jnz	spddump

spd_set_drb_done:
spd_set_dramc_done:
spd_enable_refresh_done:
spd_set_rps_done:
spd_set_pgpol_done:
spd_set_nbxcfg_done:
no_memory:
