#include <asm.h>
#include <cpu/p5/macros.h>
#define DOC_WIN_SEG		0xfe00
#define DOC_STACK_SEG		0x0400
#define SPL_RAM_SEG		0x8000

#define DOC_SPL_START_BLK	2	/* 0,1 for IPL */
#define DOC_SPL_SIZE_IN_PAGE	126     /* 1 page = 512 bytes, total 63kB */

/* wow, the usual way to to this hurts. So we do it our way:
 * 32-bit test not needed. 
 */
/* the MCR is 32-bits. You set it, it programs SDRAM. 
 * first check: get Column address size (CAS)
 * Start out assuming that it is 8 bits, then grow. 
 * nicely, if we put the 32-bit MCR value in %ecs, we can 
 * ror it 16 bits, and as we grow the CAS, we just inc cs, and that will
 * set the right value. 
 */
#define INIT_MCR $0xf6641000
/*#include <cpu/p5/start32.inc>*/
.org 0xfe000
.code16
_start:	jmp _realstart
gdt:	 //GDT entry 0 (null segment)
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	// GDT 1
	.word	0xffff, 0x0000		/* flat code segment */
	.byte	0x0, 0x9a, 0xcf, 0x0	
	
	.word	0xffff, 0x0000		/* flat data segment */
	.byte	0x0, 0x92, 0xcf, 0x0	

gdtptr:
	.word	3*8-1
	.long	gdt		/* we know the offset */
	.long 0

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
	data32	ljmp	$0x8, $.Lprotected

.Lprotected:
	.code32
	movw	$0x10, %bx
	movw	%bx, %es
	movw	%bx, %ds
#ifdef USE_BIG_REAL
	andb	$0xfe, %al
	movl	%eax, %cr0
	ljmp $0xfe00, $.Lunprotected
Lunprotected:
code16
#endif

	/* WARNING:	 The new CALLSP code trashes the stack! */
	/* I will leave this here for now, but you should also set it later. */

	xorl	%edi, %edi
	movb	$0x6c, %al
	CALLSP(pci_read_dword)
	andl    $0xfffc, %ecx
	orl	INIT_MCR,  %ecx
	movb	$0x6c, %al
	CALLSP(pci_write_dword)
	rorl	$16, %ecx
	/* test 8 bit CAS */
	movb	$0, 0
	movb	$1, 0x800
	cmpb	$0,0
	jnz	sizeram
	inc	%cl
	/* change the value we store each time. It makes debugging easier */
	movb	$2, 0x1000
	cmpb	$0, 0
	jnz	sizeram
	inc	%cl
	movb	$3, 0x2000
	cmpb	$0, 0
	jnz	sizeram
	inc	%cl
	
sizeram:		
	rorl	$16, %ecx
	/* clear the 'no multi page' bit. */
	andw	$0xefff, %cx
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
/* size is now in cx[19:16] */
/* now size the dram */
/* you had best have at least 32M; that's as small as we go */
/* rorr 20 the ecx value, to get row size into lsb */
	movb	$0, 0
	ror	$20, %ecx
	movl	$0x400000, %esi
1:
#ifdef USE_BIG_REAL
	addr32 	movb	$4, %es:(%esi)
#else
 	movb	$4, %es:(%esi)
#endif
	cmpb	$4, %es:(%esi)
	jne	1f
	inc	%cl
	rol	$1, %esi
	jmp	1b
/* size is now in esi */
/* %ecx has setting for register */
1:
	ror	$1, %esi
	rol	$20, %ecx
/* set 4 banks. */
	orb	$1, %cl
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
/* bank detect */
/* you don't need to even care how large CAS is. 
 * Just set 4-bank mode, and set non-zero numbers into a few places. 
 * if the byte at 0 changes, you have two banks. Trivial.
 */
	movb	$0, 0
	movb	$5, 0x1000
	movb	$6, 0x2000
	movb	$7, 0x4000
	cmpb	$0, 0
	jz	1f /* only one bank */
	orl	$80000, %ecx
1: 
	/* clear 4 banks */
	andb	$0xfe, %cl
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	/* at this point, dram slot 0 is up.  we hope. */

	/* now for flash ... */
	/* NOTE: MUST NOT DESTROY %ecx! */
	/* INPUT: %al, the register. %ecx, the write data */

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

	.org 0xfe1f0
reset_vector:
	.byte	0xea				# jmp to fe00:0000, where IPL
	.word	0x0000, DOC_WIN_SEG		# starts in DoC

spl_vector:
	.byte	0xea				# jmp to 8000:0000, where SPL
	.word	0x0000, SPL_RAM_SEG		# (LinuxBIOS) starts in RAM
pad: 	.byte 0, 0, 0, 0, 0, 0

