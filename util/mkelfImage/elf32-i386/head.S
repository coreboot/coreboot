/*
 * exec_kernel/user_space/head.S
 *
 * Copyright (C) 2000 Eric Biederman
 *
 * Parts of this code were take from the linux startup
 * code of linux-2.4.0-test9
 *
 * Other parts were taken from etherboot-5.0.5
 */
	
#define ASSEMBLY 1

#define RELOC 0x10000
#define KERN_CODE_SEG 0x10
#define KERN_DATA_SEG 0x18
#define REAL_CODE_SEG 0x08
#define REAL_DATA_SEG 0x20

	.equ	CR0_PE,1

#if 0
#define TTYS0_BASE	0x3f8
#define TTYS0_RBR (TTYS0_BASE+0x00)
#define TTYS0_TBR TTYS0_RBR
#define TTYS0_LSR (TTYS0_BASE+0x05)

	
	/* uses:	ax, dx */
#define TTYS0_TX_AL		\
	mov	%al, %ah	; \
9:	mov	$TTYS0_LSR, %dx	; \
	inb	%dx, %al	; \
	test	$0x20, %al	; \
	je	9b		; \
	mov	$TTYS0_TBR, %dx	; \
	mov	%ah, %al	; \
	outb	%al, %dx

	/* uses:	ax, dx */
#define TTYS0_TX_CHAR(byte)	\
	mov	byte, %al	; \
	TTYS0_TX_AL
#endif

.text
.code32

#define GDTLOC	0x91000

#define ET_EXEC 2
#define EM_386	3
#define ELF32_PHDR_SIZE (8*4)
#define PT_NULL 0
#define PT_LOAD 1
#define PT_NOTE 4
#define PF_X	(1<< 0)
#define PF_W	(1<< 1)
#define PF_R	(1<< 2)


	/* ELF Header */
		.globl elf_header
elf_header:	
e_ident:	.byte	0x7f, 'E', 'L', 'F', 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
e_type:		.short	ET_EXEC
e_machine:	.short	EM_386
e_version:	.long	1
e_entry:	.long	startup_32
e_phoff:	.long	elf_program_header - elf_header
e_shoff:	.long	0
e_flags:	.long	0
e_ehsize:	.short	elf_header_end - elf_header
e_phentsize:	.short	ELF32_PHDR_SIZE
e_phnum:	.short	(elf_program_header_end - elf_program_header)/ELF32_PHDR_SIZE
e_shentsize:	.short	0
e_shnum:	.short	0
e_shstrndx:	.short	0
elf_header_end:
	
elf_program_header:
phdr1_p_type:	.long	PT_NOTE
phdr1_p_offset:	.long	elf_note - elf_header
phdr1_p_vaddr:	.long	elf_note
phdr1_p_paddr:	.long	elf_note
phdr1_p_filesz:	.long	elf_note_end - elf_note
phdr1_p_memsz:	.long	elf_note_end - elf_note
phdr1_p_flags:	.long	PF_R | PF_W | PF_X
phdr1_p_align:	.long	0

/* Fixup code */
phdr2_p_type:	.long	PT_LOAD
phdr2_p_offset:	.long	elf_note - elf_header
phdr2_p_vaddr:	.long	elf_note
phdr2_p_paddr:	.long	elf_note
phdr2_p_filesz:	.long	_edata - elf_header
phdr2_p_memsz:	.long	_end - elf_header
phdr2_p_flags:	.long	PF_R | PF_W | PF_X
phdr2_p_align:	.long	0

/* Claim space for the gdt */
phdr3_p_type:	.long	PT_LOAD
phdr3_p_offset:	.long	_end - elf_header
phdr3_p_vaddr:	.long	GDTLOC
phdr3_p_paddr:	.long	GDTLOC
phdr3_p_filesz:	.long	0
phdr3_p_memsz:	.long	gdt_end - gdt
phdr3_p_flags:	.long	PF_R | PF_W | PF_X
phdr3_p_align:	.long	0

/* The kernel */
phdr4_p_type:	.long	PT_LOAD
phdr4_p_offset:	.long	kernel_data - elf_header
phdr4_p_vaddr:	.long	0x100000
phdr4_p_paddr:	.long	0x100000
phdr4_p_filesz:	.long	kernel_data_size
phdr4_p_memsz:	.long	0x700000
phdr4_p_flags:	.long	PF_R | PF_W | PF_X
phdr4_p_align:	.long	0

/* The ramdisk */
phdr5_p_type:	.long	PT_LOAD
phdr5_p_offset:	.long	ramdisk_data - elf_header
phdr5_p_vaddr:	.long	initrd_base
phdr5_p_paddr:	.long	initrd_base
phdr5_p_filesz:	.long	ramdisk_data_size
phdr5_p_memsz:	.long	ramdisk_data_size
phdr5_p_flags:	.long	PF_R | PF_W | PF_X
phdr5_p_align:	.long	0
	
elf_program_header_end:

#include "elf_boot.h"
	.globl elf_note
elf_note:
/* You can probably do this in C if everything was all in one structure,
 * but doing it pieces resulted in strange alignments of the data structures.
 * Since that breaks the definition of the .note section I do it here in
 * in assembly.  The control is better and for simplicity it is a toss up.
 */
	.balign 4
	.int 2f - 1f
	.int 4f - 3f
	.int EIN_PROGRAM_NAME
1:	.asciz "ELFBoot"
2:
	.balign 4
3:	
	.asciz "Linux"
4:


	.balign 4
	.int 2f - 1f
	.int 4f - 3f
	.int EIN_PROGRAM_VERSION
1:	.asciz "ELFBoot"
2:
	.balign 4
3:	
	.asciz DEFAULT_PROGRAM_VERSION
4:

	.balign 4
	.int 2f - 1f
	.int 4f - 3f
	.int EIN_PROGRAM_CHECKSUM
1:	.asciz "ELFBoot"
2:
	.balign 4
3:	
	.word 0
4:


	.balign 4
	.int 2f - 1f
	.int 4f - 3f
	.int LIN_COMMAND_LINE
1:	.asciz "Linux"
2:
	.balign 4
3:
	.globl note_command_line
note_command_line:
	.asciz DEFAULT_COMMAND_LINE
33:
	.fill 256 - (33b - 3b), 1, 0
4:	
	

	.balign 4
	.int 2f - 1f
	.int 4f - 3f
	.int LIN_ROOT_DEV
1:	.asciz "Linux"
2:
	.balign 4
3:
	.globl note_root_dev
note_root_dev:
	.word DEFAULT_ROOT_DEV
4:
	
	.balign 4
	.int 2f - 1f
	.int 4f - 3f
	.int LIN_RAMDISK_FLAGS
1:	.asciz "Linux"
2:
	.balign 4
3:
	.globl note_ramdisk_flags
note_ramdisk_flags:
	.word  0
4:

	.balign 4
	.int 2f - 1f
	.int 4f - 3f
	.int LIN_INITRD_START
1:	.asciz "Linux"
2:
	.balign 4
3:
	.globl note_initrd_start
note_initrd_start:
	.int initrd_base
4:

	.balign 4
	.int 2f - 1f
	.int 4f - 3f
	.int LIN_INITRD_SIZE
1:	.asciz "Linux"
2:
	.balign 4
3:
	.globl note_initrd_size
note_initrd_size:
	.int ramdisk_data_size
4:
	.balign 4
elf_note_end:
	
	.globl startup_32
startup_32:
	cld
	cli

	# Save the arguments safely out of the way
	movl %eax, %ebp
	movl %ebx, %esi

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
	movl	$ 0x18, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl    %eax, %ss
	
	pushl %esi	# boot data pointer as second arg
	pushl %ebp	# boot data type as first argument
	call convert_params
	
	movl %eax, %esi # put the real mode pointer in a safe place
	addl $8, %esp	# pop the arguments

	# Setup the registers before jumping to linux


	# clear eflags
	pushl	$0
	popfl	

	# Flag to indicate we are the bootstrap processor
	xorl	%ebx, %ebx
	
	# Clear the unspecified registers for good measure
	xorl	%eax, %eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	xorl	%edi, %edi
	xorl	%esp, %esp
	xorl	%ebp, %ebp


	# Jump to the linux kernel
	ljmp	$ 0x10 , $ 0x100000


	/* Routines to query the BIOS... */

#if 0
	.globl noop
noop:
	TTYS0_TX_CHAR($'a')
	TTYS0_TX_CHAR($'\r')
	TTYS0_TX_CHAR($'\n')
	call	_prot_to_real
	.code16
	TTYS0_TX_CHAR($'b')
	TTYS0_TX_CHAR($'\r')
	TTYS0_TX_CHAR($'\n')
	data32 call	_real_to_prot
	.code32
	TTYS0_TX_CHAR($'c')
	TTYS0_TX_CHAR($'\r')
	TTYS0_TX_CHAR($'\n')
	ret
#endif

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
	data32 ljmp	$KERN_CODE_SEG,$1f
1:
	.code32
	/* reload other segment registers */
	movl	$KERN_DATA_SEG,%eax
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

idt_real:
	.word	0x400				# idt limit = 256
	.word	0, 0
idt_48:
	.word	0				# idt limit = 0
	.word	0, 0				# idt base = 0L
gdt_48:
	.word	0x28				# gdt limit=40,
						# (5 GDT entries)
	.long   GDTLOC				# gdt base

# Descriptor tables
# These need to be in a seperate section so I can be
# certain later activities dont stomp them
gdt:
	.word	0, 0, 0, 0			# dummy

	/* 16 bit real mode code segment */
	.word	0xffff,(RELOC&0xffff)
	.byte	(RELOC>>16),0x9b,0x00,(RELOC>>24)

	.word	0xFFFF				# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0				# base address = 0
	.word	0x9A00				# code read/exec
	.word	0x00CF				# granularity = 4096, 386
						#  (+5th nibble of limit)

	.word	0xFFFF				# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0				# base address = 0
	.word	0x9200				# data read/write
	.word	0x00CF				# granularity = 4096, 386
						#  (+5th nibble of limit)

	/* 16 bit real mode data segment */
	.word	0xffff,(RELOC&0xffff)
	.byte	(RELOC>>16),0x93,0x00,(RELOC>>24)
gdt_end:
