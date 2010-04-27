/*
 * exec_kernel/user_space/head.S
 *
 * Copyright (C) 2000, 2002, 2003 Eric Biederman
 *
 * Parts of this code were take from the linux startup
 * code of linux-2.4.0-test9
 *
 * Other parts were taken from etherboot-5.0.5
 */

#define ASSEMBLY 1

#define RELOC 0x10000
#define PROT_CODE_SEG 0x10
#define PROT_DATA_SEG 0x18
#define REAL_CODE_SEG 0x08
#define REAL_DATA_SEG 0x20

	.equ	CR0_PE,1

.text
.code32


#include "convert.h"

	.globl startup_32
startup_32:
	cld
	cli

	# Save the arguments safely out of the way
	movl	%eax, boot_type
	movl	%ebx, boot_data
	cmp	$0,%esp
	jz	1f
	movl	4(%esp), %eax
	movl	%eax, boot_param
1:

	movl stack_start, %esp

	# Clear eflags
	pushl $0
	popfl

	# Clear BSS
	xorl %eax,%eax
	movl $ _edata,%edi
	movl $ _end,%ecx
	subl %edi,%ecx
	cld
	rep
	stosb

	# Move the gdt where Linux will not smash it during decompression
	movl	$gdt, %esi
	movl	$GDTLOC, %edi
	movl	$(gdt_end - gdt), %ecx
	rep	movsb

	# Linux makes stupid assumptions about the segments
	# that are already setup, so setup a new gdt & ldt
	# and then reload the segment registers.

	lgdt	gdt_48
	lidt	idt_48

	# Load the data segment registers
	movl	$ PROT_DATA_SEG, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl    %eax, %ss

	pushl	$image_params	# image build time parameters as forth arg
	pushl	boot_param	# boot_param pointer as third arg
	pushl	boot_data	# boot data pointer as second arg
	pushl	boot_type	# boot data type as first argument
	call	convert_params

	movl	%eax, %esi	# put the real mode pointer in a safe place
	addl	$16, %esp	# pop the arguments


	# Setup the registers before jumping to linux


	# clear eflags
	pushl	$0
	popfl

	# Flag to indicate we are the bootstrap processor
	xorl	%ebx, %ebx

	movl    switch_64, %eax
	cmp	$1, %eax
	jz	switch_to_64

	# Clear the unspecified registers for good measure
	xorl	%eax, %eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	xorl	%edi, %edi
	xorl	%ebp, %ebp

	# do not clear esp, we still need to use lret later

        pushl $PROT_CODE_SEG
        movl entry, %eax
        pushl %eax

	lret

switch_to_64:

	/* We need to switch to 64bit before use startup_64 entry go to kernel */
 /*
  * Prepare for entering 64 bit mode
  */
        # Move the gdt64 where Linux will not smash it during decompression
	movl	%esi, %eax # save the real mode pointer
	movl    $gdt64, %esi
	movl    $GDT64LOC, %edi
	movl    $(gdt64_end - gdt64), %ecx
	rep     movsb
	movl	%eax, %esi

	/* Load new GDT with the 64bit segments using 32bit descriptor */
	lgdt	gdt64

	/* Enable PAE mode */
	xorl    %eax, %eax
	btsl    $5, %eax
	movl    %eax, %cr4

 /*
  * Build early 4G boot pagetable
  */
       /* Initialize Page tables to 0*/
       movl    $PGTLOC, %edi
       xorl    %eax, %eax
       movl    $((4096*6)/4), %ecx
       rep     stosl

       /* Build Level 4 */
       movl    $(PGTLOC + 0), %edi
       leal    0x1007 (%edi), %eax
       movl    %eax, 0(%edi)

       /* Build Level 3 */
       movl    $(PGTLOC + 0x1000), %edi
       leal    0x1007(%edi), %eax
       movl    $4, %ecx
1:     movl    %eax, 0x00(%edi)
       addl    $0x00001000, %eax
       addl    $8, %edi
       decl    %ecx
       jnz     1b

       /* Build Level 2 */
       movl    $(PGTLOC + 0x2000), %edi
       movl    $0x00000183, %eax
       movl    $2048, %ecx
1:     movl    %eax, 0(%edi)
       addl    $0x00200000, %eax
       addl    $8, %edi
       decl    %ecx
       jnz     1b

       /* Enable the boot page tables */
       movl    $PGTLOC, %eax
       movl    %eax, %cr3

       /* Enable Long mode in EFER (Extended Feature Enable Register) */
       movl    $0xc0000080, %ecx
       rdmsr
       btsl    $8, %eax
       wrmsr

	/* Preparing for 64bit jmp */
        pushl $PROT_CODE_SEG
        movl entry, %eax
        pushl %eax

       /* Enter paged protected Mode, activating Long Mode */
        xorl    %eax, %eax
        btsl    $31, %eax
        btsl    $0, %eax
        movl    %eax, %cr0

        /*
         * At this point we're in long mode but in 32bit compatibility mode
         * with EFER.LME = 1, CS.L = 0, CS.D = 1 (and in turn
         * EFER.LMA = 1). Now we want to jump in 64bit mode, to do that we use
         * the new gdt/idt that has __KERNEL_CS with CS.L = 1.
         */

	lret


	/* Routines to query the BIOS... */
/**************************************************************************
E820_MEMSIZE - Get a listing of memory regions
**************************************************************************/
#define SMAP	0x534d4150
	.globl	meme820
meme820:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	movl	8(%ebp), %edi	/* Address to return e820 structures at */
	subl	$RELOC, %edi
	movl	12(%ebp), %esi	/* Maximum number of e820 structurs to return */
	pushl	%esi
	call	_prot_to_real
	.code16
	xorl	%ebx, %ebx
jmpe820:
	movl	$0xe820, %eax
	movl	$SMAP, %edx
	movl	$20, %ecx
	/* %di was setup earlier */
	int	$0x15
	jc	bail820

	cmpl	$SMAP, %eax
	jne	bail820

good820:
	/* If this is useable memory, we save it by simply advancing %di by
	 * sizeof(e820rec)
	 */
	decl	%esi
	testl	%esi,%esi
	jz	bail820

	addw	$20, %di
again820:
	cmpl	$0, %ebx	/* check to see if %ebx is set to EOF */
	jne	jmpe820

bail820:
	data32 call	_real_to_prot
	.code32
	popl	%eax
	subl	%esi, %eax	/* Compute how many structure we read */

	/* Restore everything else */
	popl	%edi
	popl	%esi
	popl	%ebx
	movl	%ebp, %esp
	popl	%ebp
	ret


/**************************************************************************
MEME801 - Determine size of extended memory
**************************************************************************/
	.globl meme801
meme801:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	call	_prot_to_real
	.code16

	stc					# fix to work around buggy
	xorw	%cx,%cx				# BIOSes which dont clear/set
	xorw	%dx,%dx				# carry on pass/error of
						# e801h memory size call
						# or merely pass cx,dx though
						# without changing them.
	movw	$0xe801,%ax
	int	$0x15
	jc	e801absent

	cmpw	$0x0, %cx			# Kludge to handle BIOSes
	jne	e801usecxdx			# which report their extended
	cmpw	$0x0, %dx			# memory in AX/BX rather than
	jne	e801usecxdx			# CX/DX.  The spec I have read
	movw	%ax, %cx			# seems to indicate AX/BX
	movw	%bx, %dx			# are more reasonable anyway...

e801usecxdx:
	andl	$0xffff, %edx			# clear sign extend
	shll	$6, %edx			# and go from 64k to 1k chunks
	movl	%edx, %eax			# store extended memory size
	andl	$0xffff, %ecx			# clear sign extend
 	addl	%ecx, %eax			# and add lower memory into

	jmp	e801out
e801absent:
	xorl	%eax,%eax

e801out:
	data32 call	_real_to_prot
	.code32
	/* Restore Everything */
	popl	%edi
	popl	%esi
	popl	%ebx
	ret

/**************************************************************************
MEM88 - Determine size of extended memory
**************************************************************************/
	.globl mem88
mem88:
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	call	_prot_to_real
	.code16

	movb	$0x88, %ah
	int	$0x15
	andl	$0xffff, %eax

	data32 call	_real_to_prot
	.code32

	/* Restore Everything */
	popl	%edi
	popl	%esi
	popl	%ebx
	ret


/**************************************************************************
BASEMEMSIZE - Get size of the conventional (base) memory
**************************************************************************/
	.globl	basememsize
basememsize:
	call	_prot_to_real
	.code16
	int	$0x12
	movw	%ax,%cx
	DATA32 call	_real_to_prot
	.code32
	movw	%cx,%ax
	ret

/**************************************************************************
_REAL_TO_PROT - Go from REAL mode to Protected Mode
**************************************************************************/
	.globl	_real_to_prot
_real_to_prot:
	.code16
	cli
	cs
	addr32 lgdt	gdt_48 - RELOC
	movl	%cr0,%eax
	orl	$CR0_PE,%eax
	movl	%eax,%cr0		/* turn on protected mode */

	/* flush prefetch queue, and reload %cs:%eip */
	data32 ljmp	$PROT_CODE_SEG,$1f
1:
	.code32
	/* reload other segment registers */
	movl	$PROT_DATA_SEG,%eax
	movl	%eax,%ds
	movl	%eax,%es
	movl	%eax,%ss
	addl	$RELOC,%esp		/* Fix up stack pointer */
	xorl	%eax,%eax
	movl	%eax,%fs
	movl	%eax,%gs
	popl	%eax			/* Fix up return address */
	addl	$RELOC,%eax
	pushl	%eax

	/* switch to protected mode idt */
	cs
	lidt	idt_48
	ret

/**************************************************************************
_PROT_TO_REAL - Go from Protected Mode to REAL Mode
**************************************************************************/
	.globl	_prot_to_real
_prot_to_real:
	.code32
	popl	%eax
	subl	$RELOC,%eax		/* Adjust return address */
	pushl	%eax
	subl	$RELOC,%esp		/* Adjust stack pointer */
	ljmp	$REAL_CODE_SEG,$1f- RELOC	/* jump to a 16 bit segment */
1:
	.code16
	/* clear the PE bit of CR0 */
	movl	%cr0,%eax
	andl	$0!CR0_PE,%eax
	movl	%eax,%cr0

	/* make intersegment jmp to flush the processor pipeline
	 * and reload %cs:%eip (to clear upper 16 bits of %eip).
	 */
	data32 ljmp	$(RELOC)>>4,$2f- RELOC
2:
	/* we are in real mode now
	 * set up the real mode segment registers : %ds, $ss, %es
	 */
	movw	%cs,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%ss
	movw	%ax,%fs
	movw	%ax,%gs

	/* Switch to the real mode idt */
	cs
	addr32	lidt	idt_real - RELOC

	sti
	data32 ret	/* There is a 32 bit return address on the stack */
	.code32

boot_type:	.long 0
boot_data:	.long 0
boot_param:	.long 0

idt_real:
	.word	0x400				# idt limit = 256
	.word	0, 0
idt_48:
	.word	0				# idt limit = 0
	.word	0, 0				# idt base = 0L
gdt_48:
	.word	gdt_end - gdt - 1		# gdt limit=40,
						# (5 GDT entries)
	.long   GDTLOC				# gdt base

# Descriptor tables
# These need to be in a seperate section so I can be
# certain later activities dont stomp them
gdt:
	/* 0x00 */
	.word	0, 0, 0, 0			# dummy

	/* 0x08 */
	/* 16 bit real mode code segment */
	.word	0xffff,(RELOC&0xffff)
	.byte	(RELOC>>16),0x9b,0x00,(RELOC>>24)

	/* 0x10 */
	.word	0xFFFF				# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0				# base address = 0
	.word	0x9A00				# code read/exec
	.word	0x00CF				# granularity = 4096, 386
						#  (+5th nibble of limit)

	/* 0x18 */
	.word	0xFFFF				# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0				# base address = 0
	.word	0x9200				# data read/write
	.word	0x00CF				# granularity = 4096, 386
						#  (+5th nibble of limit)

	/* 0x20 */
	/* 16 bit real mode data segment */
	.word	0xffff,(RELOC&0xffff)
	.byte	(RELOC>>16),0x93,0x00,(RELOC>>24)

	/* For 2.5.x the kernel segments have moved */

	/* 0x28 dummy */
	.quad	0

	/* 0x30 dummy */
	.quad	0
	/* 0x38 dummy */
	.quad	0
	/* 0x40 dummy */
	.quad	0
	/* 0x48 dummy */
	.quad	0
	/* 0x50 dummy */
	.quad	0
	/* 0x58 dummy */
	.quad	0


	/* 0x60 */
	.word	0xFFFF				# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0				# base address = 0
	.word	0x9A00				# code read/exec
	.word	0x00CF				# granularity = 4096, 386
						#  (+5th nibble of limit)

	/* 0x68 */
	.word	0xFFFF				# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0				# base address = 0
	.word	0x9200				# data read/write
	.word	0x00CF				# granularity = 4096, 386
						#  (+5th nibble of limit)

/*
 * The layout of the per-CPU GDT under Linux:
 *
 *   0 - null
 *   1 - reserved
 *   2 - reserved
 *   3 - reserved
 *
 *   4 - default user CS                <==== new cacheline
 *   5 - default user DS
 *
 *  ------- start of TLS (Thread-Local Storage) segments:
 *
 *   6 - TLS segment #1                 [ glibc's TLS segment ]
 *   7 - TLS segment #2                 [ Wine's %fs Win32 segment ]
 *   8 - TLS segment #3
 *   9 - reserved
 *  10 - reserved
 *  11 - reserved
 *
 *  ------- start of kernel segments:
 *
 *  12 - kernel code segment            <==== new cacheline
 *  13 - kernel data segment
 *  14 - TSS
 *  15 - LDT
 *  16 - PNPBIOS support (16->32 gate)
 *  17 - PNPBIOS support
 *  18 - PNPBIOS support
 *  19 - PNPBIOS support
 *  20 - PNPBIOS support
 *  21 - APM BIOS support
 *  22 - APM BIOS support
 *  23 - APM BIOS support
 */
gdt_end:

gdt64:
	.word	gdt64_end - gdt64
	.long	GDT64LOC
	.word	0
	.quad	0x0000000000000000	/* NULL descriptor */
	.quad	0x00af9a000000ffff	/* __KERNEL_CS */
	.quad	0x00cf92000000ffff	/* __KERNEL_DS */
gdt64_end:

	.section ".trailer", "a"
	/* Constants set at build time, these are at the very end of my image */
	.balign 16
	.global image_params
image_params:

convert_magic:
	.long	CONVERT_MAGIC
gdt_size:
	.long	gdt_end - gdt
gdt64_size:
	.long	gdt64_end - gdt64
pgt_size:
	.long   4096*6
bss_size:
	.long	bss_sizex
ramdisk_flags:
	.word	0
root_dev:
	.word	DEFAULT_ROOT_DEV
entry:
	.long	0
switch_64:
	.long	0
initrd_start:
	.long	0
initrd_size:
	.long	0
cmdline:
	.asciz "BOOT_IMAGE=head.S console=ttyS0 ip=dhcp root=/dev/nfs"
	.org cmdline + CMDLINE_MAX, 0
cmdline_end:
