#include <asm.h>
#include <cpu/p5/macros.h>
#include <intel.h>
/*
 */

#define DOC_WIN_SEG		0xfe00
#define DOC_STACK_SEG		0x0400
#define SPL_RAM_SEG		0x8000

#define DOC_SPL_START_BLK	2	/* 0,1 for IPL */
#define DOC_SPL_SIZE_IN_PAGE	126     /* 1 page = 512 bytes, total 63kB */

/* wow, the usual way to to this hurts. So we do it our way:
 * 32-bit test not needed. 
 */
/*#include <cpu/p5/start32.inc>*/
.org 0xfe000
.code16
_start:	jmp _realstart
gdt:	 //GDT entry 0 (null segment)
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	// GDT 1
	.word	0xffff, 0x0000		/* flat code segment */
	.byte	0x0, 0x9a, 0xcf, 0x0	
	
	.word	0xffff, 0x0000		/* flat data segment */
	.byte	0x0, 0x92, 0xcf, 0x0	

gdtptr:
	.word	4*8-1
	.long	gdt		/* we know the offset */
	.long 0

/*  initialize registers */
#define REG(a) a&0xfc, (a&0x3)|4
register_table:
	.byte REG(0x78), 0x01
	.byte REG(0x68), 0x00
	.byte REG(0x6b), 0x00
	.byte REG(0x58), 0x88
	.byte REG(0x59), 0x88
	.byte REG(0x5a), 0x08
	.byte REG(0x5b), 0x10
	.byte REG(0x5c), 0x10
	.byte REG(0x5d), 0x10
	.byte REG(0x5e), 0x10
	.byte REG(0x5f), 0x10
	.byte REG(0x56), 0x10
	.byte REG(0x57), 0x10
	.byte REG(0x60), 0xff
	.byte REG(0x64), 0xee
	.byte REG(0x65), 0xee
	.byte REG(0x66), 0xee
	.byte REG(0x69), 0x04
	.byte REG(0x6a), 0x00
	.byte REG(0x6c), 0x00
	.byte REG(0x6d), 0x37
	// This extra set is redundant, but needed below.
	// LEAVE IT HERE. 
	.byte REG(0x6c), 0x0
	.byte 0x0
_realstart:	

	cli
	movw	%cs, %ax
	movw	%ax, %es

		.byte 0x66 /* prefix */
	.byte 0x26
	.byte 0x0f, 0x01, 0x16 /* lgdt [es:offset]*/
	.word  gdtptr -  EXT(_start) /* offset in segment */

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	/* invalidate the cache */
	invd
	data32	ljmp	$0x10, $.Lprotected

.Lprotected:
	.code32
	movw	$0x18, %bx
	movw	%bx, %es
	movw	%bx, %ds
/*
	movw	%bx, %es
	movw	%bx, %fs
	movw	%bx, %gs
	movw	%bx, %ss
 */

	// Registers
	// %esp holds the 0x80000000 pattern
	// %edx always hold $0xcf8
	// %esi holds 0
	// %edi holds 0x40000000
	// %bl is reused as needed. 
	// store this constant in %esp
	movl	$0x80000000, %esp
/* make the flash writeable */
/* select the south bridge, register 40 (base) */
/* south bridge is device 7. 7 << 3 is 0x38 */
	movl	%esp, %eax
#define FLASHREG ((SUPERIO_DEVFN<<8) | 0x40)
	orw	$FLASHREG, %ax
	mov	$0xcf8,%dx
	outl	%eax,%dx
#if (SUPERIO_DEVFN == 0x88)
	mov	$0xfc,%dl
	inl     %dx,%eax 
/*
        For 8231:
                Please see databook revision 1.2, November 13, 2000. Page68.
                PCI to ISA bridge: Regx40[4]=1, Regx41[0-6]=1.
*/
	orw	$0x7f10, %ax
	outl	%eax,%dx 
#endif
#if (SUPERIO_DEVFN == 0x38)
                Please see databook revision 1.33, January 7, 2000. Page55
and 56. 
                PCI to ISA bridge: Regx40[0]=1 (write enable)
Regx43[0-7]=1(all bits set to 1)

/* but 0x01 makes flash writeable. You need this for DoC */
	mov	$0x0cfc,%dx
	inl     %dx,%eax 
	orl	$0xc0000001, %eax
	outl	%eax,%dx 
#endif

#define loop200 $0x5000
#define loop100 $0x2500

ram_set_registers:
	movl	$register_table, %esi
1:
	movl	%esp, %eax
	orb	(%esi), %al
	testb	%al, %al
	jz	done_ram_set_registers
	andb	$0xf8, %dl
	outl	%eax, %dx
	inc	%esi
	orb	(%esi), %dl
	inc	%esi
	movb	(%esi), %al
	outb	%al, %dx
//	PCI_WRITE_CONFIG_BYTE
	inc	%esi
	jmp	1b

done_ram_set_registers:
	xorl	%esi, %esi
	movl	$0x4000000, %edi
/* begin to initialize*/
	// I forget why we need this, but we do
	mov $0xa55a5aa5, %ecx
	mov %ecx, (%esi)
	mov %ecx, (%edi)
// REMEMBER:	 0xcf8 was set to 0x8000006c above. No need to set 
// again!
// Also, remember:	%edx has 0xcfc from the above loop!
//                      that is the correct value!
/* set NOP*/
#define WRITE_6c(unused,val) movb	val, %al ; outb 	%al, %dx
	WRITE_6c($0x6C, $0x01)

/* wait 200us*/
	// You need to do the memory reference. That causes the nop cycle. 
	mov	(%esi), %ecx
	mov	(%edi), %ecx
	DELAY(loop200)

/* set precharge */
	WRITE_6c($0x6C, $0x02)

/* dummy reads*/
	mov	(%esi), %ecx
	mov	(%edi), %ecx

/* set CBR*/
	WRITE_6c($0x6C, $0x04)
	
/* do 8 reads and wait 100us between each - from via*/

	movb	$8, %bl
eightreads:	
	mov	(%esi), %ecx
	mov	(%edi), %ecx
	DELAY(loop100)
	dec	%bl
	cmpb	$0, %bl
	ja	eightreads

/* set MRS*/
	// 0x150 is cas2. We are now using 0x1d0, which is cas3
	WRITE_6c($0x6c, $0x03)

	movl $0x1d0, %ecx
	movl (%ecx), %ecx
	movl $0x40001d0, %ecx
	movl (%ecx), %ecx

/* set to normal mode */
	WRITE_6c($0x6C, $0x00)

	movl $0x55aa55aa, %ecx
	mov %ecx, (%esi)
	mov (%esi), %ecx

	// Set the refresh rate. 
	movb	$0x68, %al
	movb	$0xf8, %dl
	outl	%eax, %dx
	addw	$7, %dx
	movb	$0x65, %al
	outb	%al, %dx
	//CS_WRITE($0x6A, $0x65)
	// enable multi-page open
	inc	%dl
	movb	$1, %al
	outb	%al, %dx
	//CS_WRITE($0x6B, $0x01)
	
	/* now for flash ... */
	/* NOTE: MUST NOT DESTROY %ecx! */
	/* INPUT: %al, the register. %ecx, the write data */
	/* Following code courtesy Ollie Lho:	*/
/*
 * ipl.S:	 Initial Program Loader (IPL) for SiS 630 and M-System DoC Millennium
 *
 *
 * Copyright 2000 Silicon Integrated Systems Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *	1. SiS 630 Specification
 *	2. System Management Bus Specification Rev 1.1
 *	3. PC SDRAM Serial Presence Detect (SPD) Specification Rev 1.2B
 *	4. Booting From the DiskOnChip Millennium, M-Systems Application Note Ap-DOC-044
 *	   APR-2000, 93-SR-001-44-7L REV. 1.0
 *
 * $Id$
 */
#define DOC_WIN_SEG		0xfe00
#define DOC_STACK		0x04000
#define SPL_RAM		0x80000

#define DOC_SPL_START_BLK	2	/* 0,1 for IPL */
#define DOC_SPL_SIZE_IN_PAGE	126     /* 1 page = 512 bytes, total 63kB */

ipl_start:
#if 0
	/* stack and es already set. */
	/* O.K. we have DRAM now, so set up STACK for CALL/RET */
	movw	$DOC_STACK_SEG, %ax
	movw	%ax, %ss
	movw	$SPL_RAM_SEG, %ax
	movw	%ax, %es
	movl	$DOC_STACK_SEG, %esp			# set %sp
#endif
	/* NOTE: in ollie's original code, %ds was at 0xfe00, i.e. 
	 * 0xfe000, i.e. base of DoC. We have to adjust for that when 
	 * we set %edi and %esi
	 */
	movl	$DOC_STACK, %esp		# set %sp
	movl	$SPL_RAM, %edx			# start of RAM
	xorl	%ecx, %ecx			# clear %ecx
	movl	$0xfe800, %esi			# point %si to CDSN Data area
	movl	$0xff000, %edi			# point %di to CDSN Control area
	movl	$DOC_SPL_START_BLK, %ebp	# start page of LinuxBIOS

	movb	$0x84, %al			# Reset DOC Millennium
	call	doc_reset

	movb	$0x85, %al			# Turn into Normal Mode
	call	doc_reset
	
read_next_page:
	movl	$0xff000, %edi			# point %di to CDSN Control area

flash_command:
	movb	$0x03, 0x04(%edi)		# start command cycle
	movb	$0x00, (%esi)			# issue flash command Read00
	call	doc_cycle_end
	
	movw	%bp, %bx			# %bp is current page number

flash_address:
	shll	$0x08, %ebx
        movb	$0x03, %cl			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00
	
	movb	$0x05, 0x04(%edi)		# start address cycle
0:
	movb	%bl, (%esi)			# write address to CDSNIO
	shrw	$0x08, %bx			# shift next address byte
	loop	0b

	call	doc_cycle_end

wait_for_flash_ready:
	/* delay by reding NOP register before polling the FLASH READY bit,
	   this is inlined to save a call/ret pair */
doc_delay:
	movb	$0x04, %cl			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00
0:
	movb	0x20(%edi), %al			# read DOC NOP retisger
	loop	0b				# four times
	
	testb	$0x80, 0x04(%edi)		# is flash ready ?
	jz	wait_for_flash_ready

	movb	0x1d(%edi), %al			# init read pipeline
	movw	$0x100, %cx			# 1 page = 512 bytes
	movl	$0xfe800, %esi			# point %si to CDSN Data area
	movl	%edx, %edi			# restore saved current destination
	rep
	movsw

	movl	%edi, %edx			# save current destination
	incw	%bp				# increse current page number
	cmpw	$128, %bp			# moved 63 KB ??
	jl	read_next_page			# no, read next page

sis630ipl_end:
	# jmp to 8000:0000, where SPL
	# (LinuxBIOS) starts in RAM
#	ljmp $0x10,$0x8000
	jmp	spl_vector

doc_reset:
        /* Input: AL = value write to DOC_CONTROL register
           Clobberd: CX */
        movb    %al, 0x02(%edi)                  # write DoC Control retister
        movb    %al, 0x02(%edi)                  # twice
        ret                                     # End of doc_reset

doc_cycle_end:
        movb    $0x00, 0x1e(%edi)                # flush write pepeline
        movb    $0x01, 0x04(%edi)                # end command cycle
        ret


/* we don't need these. We only do I/O to MCR 0 */
#if 0
pci_write_dword:
	mov	$0,%ah
	mov	$0x80000000,%edx
	or	%ax,%dx
	mov	%edx,%eax
	mov	$0x0cf8,%dx
	outl	%eax,%dx
	mov	$0x0fc,%dl
	mov	%ecx, %eax
	outl	%eax,%dx
	RETSP
pci_read_dword:
	mov	$0,%ah
	mov	$0x80000000,%edx
	or	%ax,%dx
	mov	%edx,%eax
	mov	$0x0cf8,%dx
	outl	%eax,%dx
	mov	$0x0fc,%dl
	inl	%dx, %eax
	mov	%eax, %ecx
	RETSP
#endif

	.org 0xfe1f0
reset_vector:
	.byte	0xea				#jump to fe00:0000, where IPL
	.word	0x0000, DOC_WIN_SEG		#starts in DoC
.code32
spl_vector:
	invd
	jmpl	$0x10, $0x80000
# jumpl	$0x80000
# put this here to buy some room
pad:	.byte 0,0
