#include <arch/asm.h>
#include <arch/intel.h>
#ifdef SMP
#include <cpu/p6/apic.h>
#endif
	.section ".text"
	.code32
	.globl _start
_start:
	cli
	lgdt	%cs:gdtaddr
	ljmp	$0x10, $1f
1:	movl	$0x18, %ax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %ss
	movl	%eax, %fs
	movl	%eax, %gs

	intel_chip_post_macro(0x13)		/* post 12 */

	/** clear stack */
	leal	EXT(_stack), %edi
	movl	$EXT(_estack), %ecx
	subl	%edi, %ecx
	xorl	%eax, %eax
	rep
	stosb

	/** clear bss */
	leal	EXT(_bss), %edi
	movl	$EXT(_ebss), %ecx
	subl	%edi, %ecx
	jz	.Lnobss
	xorl	%eax, %eax
	rep
	stosb
.Lnobss:

	/* set new stack */
	movl	$_estack, %esp
#ifdef SMP
	/* Get the cpu id */
	movl	$APIC_DEFAULT_BASE, %edi
	movl	APIC_ID(%edi), %eax
	shrl	$24, %eax

	/* Get the cpu index (MAX_CPUS on error) */
	movl	$-4, %ebx
1:	addl	$4, %ebx
	cmpl	$(MAX_CPUS << 2), %ebx
	je	2
	cmpl	%eax, EXT(initial_apicid)(%ebx)
	jne	1b
2:	shrl	$2, %ebx

	/* Now compute the appropriate stack */
	movl	%ebx, %eax
	movl	$STACK_SIZE, %ebx
	mull	%ebx
	subl	%eax, %esp
#endif

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

	/* Resort the stack location */
	movl	%ebp, %esp
	
	/* The boot_complete flag has already been pushed */
	call	EXT(hardwaremain)
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

#if (CONFIG_PLAN9GDT==1)
//selgdt 0x8
/* flat data segment */
	.word	0xffff, 0x0000		
	.byte	0x00, 0x93, 0xcf, 0x00	
#else
// selgdt 8
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00
#endif

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

#if defined(CONFIG_VGABIOS) && (CONFIG_VGABIOS == 1)
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
