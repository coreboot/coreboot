	jmp preserial


#define S0_WAIT() \
	movw $0x3fd, %dx	;\
9:	inb %dx, %al	;\
	test $0x40, %al	;\
	je 9b

#define S0_EMIT(char) \
9:	movw $0x3fd, %dx	;\
	inb %dx, %al	;\
	test $0x20, %al	;\
	je 9b			;\
	movb $char,%al	;\
	movw $0x3f8, %dx	;\
	outb %al, %dx

#define __CONSOLE_INLINE_TX_HEX8(byte)	\
	movb	byte, %al	; \
	shr	$4, %al		; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
    movw $0x3f8, %dx	;\
    outb %al, %dx	;\
10: movw $0x3fd, %dx	;\
    inb %dx, %al	;\
    test $0x20, %al	;\
    je 10b	;\
    movw $0x3f8, %dx	;\
	mov	byte, %al	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
    movw $0x3f8, %dx	;\
    outb %al, %dx

southbridge_table_start:

	.byte 0x01, 0x04, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00  	# 40
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x04, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00	# 50
	.byte 0x0a, 0x0b, 0x80, 0x09, 0xd1, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00	# 60
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	# 70
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	# 80
	.byte 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	# 90
	.byte 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	# a0
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	# b0
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	# c0
	.byte 0x86, 0x39, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	# d0
	.byte 0x10, 0x00, 0x00, 0xc0, 0x00, 0xa0, 0x0f, 0x1c, 0x33, 0x22, 0x11, 0x00, 0x00, 0x00, 0x67, 0x45	# e0
	.byte 0x0f, 0x00, 0x60, 0x84, 0x00, 0x00, 0x00, 0x00, 0x47, 0x0f, 0x0e, 0x00, 0x00, 0x00, 0x81, 0x00	# f0

southbridge_end:

.globl	 preserial  
 preserial:

	.code32

	cli

#call off the dog
	movw $0x461, %dx
	movb $0x20, %al
	outb %al, %dx
	dec %dx
	movb $0x01, %al
	outb %al, %dx
	
	movl $0x8000f8d4, %eax
	movw $0xcf8, %dx
	outl %eax, %dx
	movw $0xcfc, %dx
	movb $0x2, %al
#	movb $0x0, %al	# 0x02 here to disable watchdog function
	outb %al, %dx

#	movw $0x468, %dx
#	movw $0x800, %ax
#	outw %ax, %dx
#

#turn on LPC I/o access to superio
	movl $0x8000f8e6, %eax
	mov $0xcf8, %dx
	outl %eax, %dx

	mov $0xcfc, %dx
	movw $0x1c0f, %ax
	outw %ax, %dx

#assign i/o ranges for serial I/O 
	movl $0x8000f8e0, %eax
	mov $0xcf8, %dx
	outl %eax, %dx

	mov $0xcfc, %dx
	mov $0x10, %al
	outb %al, %dx

#attempt to get Natsemi superio to ID itself
#movw $0x2e, %dx
#movb $0x20, %al
#outb %al, %dx

#inc %dx
#inb %dx, %al

#cmp $0x0ee, %al
#je corrupt_cmos

	#OK, now, lets turn serial on!

	jmp noreset
	#first, reset the serial chip to defaults
	movw $0x2e, %dx
	movb $0x21, %al
	outb %al, %dx

	inc %dx
	movb $0x13, %al
	outb %al, %dx

noreset:
	movw $0x2e, %dx
	movb $0x21, %al
	outb %al, %dx

	inc %dx
	movb $0x19, %al
	outb %al, %dx

	dec %dx
	movb $0x22, %al
	outb %al, %dx
	inc %dx
	outb %al, %dx	# YES! 22->22

	dec %dx
	movb $0x23, %al
	outb %al, %dx
	inc %dx
	movb $0x72, %al
	outb %al, %dx

	dec %dx
	movb $0x24, %al
	outb %al, %dx
	inc %dx
	movb $0x26, %al
	outb %al, %dx

	dec %dx
	movb $0x2a, %al
	outb %al, %dx
	inc %dx
	movb $0x0b, %al
	outb %al, %dx

#enable serial
	dec %dx
	movb $0x07, %al
	outb %al, %dx
	inc %dx
	movb $0x03, %al
	outb %al, %dx

	dec %dx
	movb $0x30, %al
	outb %al, %dx
	inc %dx
	movb $0x01, %al
	outb %al, %dx

#O.K. now lets try to set up serial part way, then confirm that its active

	#turn off serial interrupts, we have no vectors
	movw $0x3f9, %dx
	movb $0x0, %al
	outb %al, %dx

	#set divisor to 115200
	movw $0x3fb, %dx
	movb $0x83, %al
	outb %al, %dx

	movw $0x3f8, %dx
	movb $0x01, %al
	outb %al, %dx
	inc %dx

	movb $0x00, %al
	outb %al, %dx

	#set n81
	movw $0x3fb, %dx
	movb $0x03, %al
	outb %al, %dx

	#FIFO setup $0xc7 -> 0x3fa
	movw $0x3fa, %dx
	movb $0xc7, %al
	out %al, %dx



	#set DTR, RTS, OUT2 (whatever that is)
	movw $0x3fc, %dx
	movb $0xb, %al
	outb %al, %dx

	jmp preserial_out

#set null descriptor to all but CS
#movw $0x0, %ax
#movw %ax, %ds
#movw %ax, %es
#movw %ax, %fs
#movw %ax, %gs

	S0_EMIT('C')
	S0_EMIT('S')
	S0_EMIT(':')
	movw %cs, %ax
	movw %ax, %bx
	__CONSOLE_INLINE_TX_HEX8(%bh)
	__CONSOLE_INLINE_TX_HEX8(%bl)
	S0_EMIT('\r')
	S0_EMIT('\n')

	S0_EMIT('D')
	S0_EMIT('S')
	S0_EMIT(':')
	S0_WAIT()
	movw %ds, %ax
	movw %ax, %bx
	__CONSOLE_INLINE_TX_HEX8(%bh)
	__CONSOLE_INLINE_TX_HEX8(%bl)
	S0_EMIT('\r')
	S0_EMIT('\n')

	S0_EMIT('C')
	S0_EMIT('R')
	S0_EMIT('0')
	S0_EMIT(':')

	S0_WAIT()

	movl %cr0, %eax
	movl %eax, %ebx
	movl %eax, %ecx
	shr $0x16, %ebx

	__CONSOLE_INLINE_TX_HEX8(%bh)
	__CONSOLE_INLINE_TX_HEX8(%bl)
	movw %cx,%bx

	__CONSOLE_INLINE_TX_HEX8(%bh)
	__CONSOLE_INLINE_TX_HEX8(%bl)
	
	S0_EMIT('\r')
	S0_EMIT('\n')

	movl $0x10002, %esi
#sgdt %cs:(%esi)
	sgdt (%esi)

	S0_EMIT('G')
	S0_EMIT('D')
	S0_EMIT('T')
	S0_EMIT(':')

	movw (0x10002), %cx
	movl (0x10004), %ebx

out1:
	__CONSOLE_INLINE_TX_HEX8( %cs:(%ebx) )
	inc %bx
	dec %cx
	jnz out1

#movw $0x4, %cx
#loop3:
#__CONSOLE_INLINE_TX_HEX8(%bl)
#shr $0x08, %ebx
#dec %cx
#jnz loop3


	#just a test, emit a PYRO
	S0_EMIT('P')

	jmp dolgdt

	.align 4
dolgdt:
	lgdt %cs:gdtptr
	S0_EMIT('Y')
	ljmp $0x10,$csload
csload:
	S0_EMIT('R')

	movw $0x18, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

	S0_EMIT('O')
	S0_EMIT('\r')
	S0_EMIT('\n')

#movb $0x50, %al
#movw $0x3f8, %dx
#outb %al, %dx
#movb $0x59, %al
#outb %al, %dx
#movb $0x52, %al
#outb %al, %dx
#movb $0x4f, %al
#outb %al, %dx

#movb $0x0d, %al
#outb %al, %dx

#movb $0x0a, %al
#outb %al, %dx


	#indicate we did it by clearing DTR
	movw $0x3fc, %dx
	movb $0x0a, %al
	outb %al, %dx

	
	
	jmp preserial_out

	movl $southbridge_table_start, %esi
	movl    $(southbridge_end - southbridge_table_start), %ecx
	shrl $0x02, %ecx
	movl $0x8000f840, %eax
	movl %eax, %ebx

next_dword:
	movw $0xcf8, %dx
	movl %ebx, %eax
	outl %eax, %dx
	inc %eax
	inc %eax
	inc %eax
	inc %eax
	movl %eax, %ebx
	lodsl (%esi), %eax
	movw $0xcfc, %dx
	outl %eax, %dx
	loopnz next_dword


	jmp preserial_out

 
	movl $0x8000f8e0, %eax
	mov $0xcf8, %dx
	outl %eax, %dx

	mov $0xcfc, %dx
	mov $0x10, %al
	outb %al, %dx



	jmp preserial_out
 
corrupt_cmos:
	mov $0x72, %dx
	mov $0x50, %al
	outb %al, %dx
	inc %dx
	mov $0x42, %al
	outb %al, %dx

	dec %dx
	mov $0x51, %al
	outb %al, %dx
	inc %dx
	mov $0xbe, %al
	outb %al, %dx


#now, strobe HD light a few times to prove we made it here

	mov $30, %bx
	mov $0x480, %dx
again:
	mov $0xa7, %al
	outb %al, %dx
	movl $0xfffffff,%ecx
loop1:
	dec %ecx
	jnz loop1

	mov $0xa6, %al
	outb %al, %dx
	movl $0xfffffff,%ecx
loop2:
	dec %ecx
	jnz loop2


	dec %bx
	jnz again
	

preserial_out:

	S0_EMIT('A')
	S0_EMIT('P')
	S0_EMIT('I')
	S0_EMIT('C')
	S0_EMIT('I')
	S0_EMIT('D')
	S0_EMIT(':')

	movl	$APIC_DEFAULT_BASE, %edi
	movl	APIC_ID(%edi), %eax
	shrl	$24, %eax

	__CONSOLE_INLINE_TX_HEX8(%al)
	S0_EMIT('\r')
	S0_EMIT('\n')
	

#very temporary, set TOLM to 1 Gig. This should be enumerated or otherwise tested!
	movl $0x800000c4, %eax
	movw $0xcf8, %dx
	outl %eax, %dx
	movw $0x4000, %ax
	movw $0xcfc, %dx
	outw %ax, %dx

#set PCI dev 0:0.0.e0 < 0x1e
	movl $0x800000e0, %eax
	movw $0xcf8, %dx
	outl %eax, %dx
	movb $0x1e, %al
	movw $0xcfc, %dx
	outb %al, %dx

jmp console0

