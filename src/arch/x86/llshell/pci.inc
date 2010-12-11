
	/*
	 * Macro:	PCI_WRITE_CONFIG_BYTE
	 * Arguments:	%eax address to write to (includes bus, device, function, &offset)
	 *              %dl byte to write
	 *
	 * Results:	none
	 *
	 * Trashed:	%eax, %edx
	 * Effects:	writes a single byte to pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *              And the tricks it does cannot scale beyond writing a single byte.
	 *
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the data byte
	 *              in the high half of edx.
	 *
	 *              In %edx[3] it stores the byte to write.
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_WRITE_CONFIG_BYTE \
	shll $8,   %edx		; \
	movb %al,  %dl		; \
	andb $0x3, %dl		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	movb %dh,  %al		; \
	movb $0,   %dh		; \
	addl $0xcfc, %edx	; \
	outb %al,  %dx


	/*
	 * Macro:	PCI_WRITE_CONFIG_WORD
	 * Arguments:	%eax address to write to (includes bus, device, function, &offset)
	 *              %ecx word to write
	 *
	 * Results:	none
	 *
	 * Trashed:	%eax, %edx
	 * Preserved:   %ecx
	 * Effects:	writes a single byte to pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the least significant
	 *              bits of the address in the high half of edx.
	 *
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_WRITE_CONFIG_WORD \
	movb %al,  %dl		; \
	andl $0x3, %edx		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	movl %ecx, %eax		; \
	addl $0xcfc, %edx	; \
	outw %ax,  %dx



	/*
	 * Macro:	PCI_WRITE_CONFIG_DWORD
	 * Arguments:	%eax address to write to (includes bus, device, function, &offset)
	 *              %ecx dword to write
	 *
	 * Results:	none
	 *
	 * Trashed:	%eax, %edx
	 * Preserved:   %ecx
	 * Effects:	writes a single byte to pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the least significant
	 *              bits of the address in the high half of edx.
	 *
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_WRITE_CONFIG_DWORD \
	movb %al,  %dl		; \
	andl $0x3, %edx		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	movl %ecx, %eax		; \
	addl $0xcfc, %edx	; \
	outl %eax,  %dx




	/*
	 * Macro:	PCI_READ_CONFIG_BYTE
	 * Arguments:	%eax address to read from (includes bus, device, function, &offset)
	 *
	 * Results:	%al Byte read
	 *
	 * Trashed:	%eax, %edx
	 * Effects:	reads a single byte from pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the least significant
	 *              bits of the address in the high half of edx.
	 *
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_READ_CONFIG_BYTE \
	movb %al,  %dl		; \
	andl $0x3, %edx		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	addl $0xcfc, %edx	; \
	inb  %dx,  %al



	/*
	 * Macro:	PCI_READ_CONFIG_WORD
	 * Arguments:	%eax address to read from (includes bus, device, function, &offset)
	 *
	 * Results:	%ax word read
	 *
	 * Trashed:	%eax, %edx
	 * Effects:	reads a 2 bytes from pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the least significant
	 *              bits of the address in the high half of edx.
	 *
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_READ_CONFIG_WORD \
	movb %al,  %dl		; \
	andl $0x3, %edx		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	addl $0xcfc, %edx	; \
	inw  %dx,  %ax



	/*
	 * Macro:	PCI_READ_CONFIG_DWORD
	 * Arguments:	%eax address to read from (includes bus, device, function, &offset)
	 *
	 * Results:	%eax
	 *
	 * Trashed:	%edx
	 * Effects:	reads 4 bytes from pci config space
	 *
	 * Notes:	This routine is optimized for minimal register usage.
	 *
	 *              What it does is almost simple.
	 *              It preserves %eax (baring special bits) until it is written
	 *              out to the appropriate port.  And hides the least significant
	 *              bits of the address in the high half of edx.
	 *
	 *              In %edx[2] it stores the lower three bits of the address.
	 */


#define PCI_READ_CONFIG_DWORD \
	movb %al,  %dl		; \
	andl $0x3, %edx		; \
	shll $16,  %edx		; \
	\
	orl  $0x80000000, %eax	; \
	andl $0xfffffffc, %eax	; \
	movw $0xcf8, %dx	; \
	outl %eax,  %dx		; \
	\
	shrl $16,  %edx		; \
	addl $0xcfc, %edx	; \
	inl  %dx,  %eax



