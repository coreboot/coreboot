#include <arch/asm.h>
#include <arch/intel.h>
	.section ".text"
	.code32
	.globl _start
_start:
	cli
	lgdt	%cs:gdtaddr
	ljmp	$0x10, $1f
1:	movl	$0x18, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %ss
	movl	%eax, %fs
	movl	%eax, %gs

	intel_chip_post_macro(0x13)		/* post 12 */

	/** clear stack */
	leal	_stack, %edi
	movl	$_estack, %ecx
	subl	%edi, %ecx
	xorl	%eax, %eax
	rep
	stosb

	/** clear bss */
	leal	_bss, %edi
	movl	$_ebss, %ecx
	subl	%edi, %ecx
	jz	.Lnobss
	xorl	%eax, %eax
	rep
	stosb
.Lnobss:

	/* set new stack */
	movl	$_estack, %esp

	/* Push the cpu index and struct cpu */
	pushl	$0
	pushl	$0

	/* push the boot_complete flag */
	pushl	%ebp

	/* Save the stack location */
	movl	%esp, %ebp

	/*
	 *	Now we are finished. Memory is up, data is copied and
	 *	bss is cleared.   Now we call the main routine and
	 *	let it do the rest.
	 */ 
	intel_chip_post_macro(0xfe)	/* post fe */

	/* Restore the stack location */
	movl	%ebp, %esp
	
	/* The boot_complete flag has already been pushed */
	call	hardwaremain
	/*NOTREACHED*/
.Lhlt:
	intel_chip_post_macro(0xee)	/* post fe */
	hlt
	jmp	.Lhlt


	.globl gdt, gdt_end, gdt_limit

gdt_limit = gdt_end - gdt - 1 /* compute the table limit */
gdtaddr:
	.word	gdt_limit
	.long	gdt                /* we know the offset */

gdt:
// selgdt 0
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

// selgdt 8
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

// selgdt 0x10 
/* flat code segment */
	.word	0xffff, 0x0000		
	.byte	0x00, 0x9b, 0xcf, 0x00	
	
//selgdt 0x18
/* flat data segment */
	.word	0xffff, 0x0000		
	.byte	0x00, 0x93, 0xcf, 0x00	

//selgdt 0x20
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

#if defined(CONFIG_LEGACY_VGABIOS) && (CONFIG_LEGACY_VGABIOS == 1)
	// from monty:
	/* 0x00009a00,0000ffffULL,   20h: 16-bit 64k code at 0x00000000 */
        /* 0x00009200,0000ffffULL    28h: 16-bit 64k data at 0x00000000 */
// selgdt 0x28
/*16-bit 64k code at 0x00000000 */
	.word 0xffff, 0x0000
	.byte 0, 0x9a, 0, 0

// selgdt 0x30
/*16-bit 64k data at 0x00000000 */
	.word 0xffff, 0x0000
	.byte 0, 0x92, 0, 0
#endif // defined(CONFIG_VGABIOS) && (CONFIG_VGABIOS == 1)
gdt_end:

.code32
