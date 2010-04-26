#include <cpu/x86/post_code.h>

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

	post_code(0x13)		/* post 13 */

	/** clear stack */
	cld
	leal	_stack, %edi
	movl	$_estack, %ecx
	subl	%edi, %ecx
	shrl	$2, %ecx   /* it is 32 bit aligned, right? */
	xorl	%eax, %eax
	rep
	stosl

	/** clear bss */
	leal	_bss, %edi
	movl	$_ebss, %ecx
	subl	%edi, %ecx
	jz	.Lnobss
	shrl	$2, %ecx  /* it is 32 bit aligned, right? */
	xorl	%eax, %eax
	rep
	stosl
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

	/* Initialize the Interrupt Descriptor table */
	leal	_idt, %edi
	leal	vec0, %ebx
	movl	$(0x10 << 16), %eax	/* cs selector */

1:	movw	%bx, %ax
	movl	%ebx, %edx
	movw	$0x8E00, %dx		/* Interrupt gate - dpl=0, present */
	movl	%eax, 0(%edi)
	movl	%edx, 4(%edi)
	addl	$6, %ebx
	addl	$8, %edi
	cmpl	$_idt_end, %edi
	jne	1b

	/* Load the Interrupt descriptor table */
	lidt	idtarg

	/*
	 *	Now we are finished. Memory is up, data is copied and
	 *	bss is cleared.   Now we call the main routine and
	 *	let it do the rest.
	 */
	post_code(0xfe)	/* post fe */

	/* Restore the stack location */
	movl	%ebp, %esp

	/* The boot_complete flag has already been pushed */
	call	hardwaremain
	/* NOTREACHED */
.Lhlt:
	post_code(0xee)	/* post ee */
	hlt
	jmp	.Lhlt

vec0:
	pushl	$0 /* error code */
	pushl	$0 /* vector */
	jmp int_hand
vec1:
	pushl	$0 /* error code */
	pushl	$1 /* vector */
	jmp int_hand

vec2:
	pushl	$0 /* error code */
	pushl	$2 /* vector */
	jmp int_hand

vec3:
	pushl	$0 /* error code */
	pushl	$3 /* vector */
	jmp	int_hand

vec4:
	pushl	$0 /* error code */
	pushl	$4 /* vector */
	jmp	int_hand

vec5:
	pushl	$0 /* error code */
	pushl	$5 /* vector */
	jmp	int_hand

vec6:
	pushl	$0 /* error code */
	pushl	$6 /* vector */
	jmp	int_hand

vec7:
	pushl	$0 /* error code */
	pushl	$7 /* vector */
	jmp	int_hand

vec8:
	/* error code */
	pushl	$8 /* vector */
	jmp	int_hand
	.word	0x9090

vec9:
	pushl	$0 /* error code */
	pushl	$9 /* vector */
	jmp int_hand

vec10:
	/* error code */
	pushl	$10 /* vector */
	jmp	int_hand
	.word	0x9090

vec11:
	/* error code */
	pushl	$11 /* vector */
	jmp	int_hand
	.word	0x9090

vec12:
	/* error code */
	pushl	$12 /* vector */
	jmp	int_hand
	.word	0x9090

vec13:
	/* error code */
	pushl	$13 /* vector */
	jmp	int_hand
	.word	0x9090

vec14:
	/* error code */
	pushl	$14 /* vector */
	jmp	int_hand
	.word	0x9090

vec15:
	pushl	$0 /* error code */
	pushl	$15 /* vector */
	jmp	int_hand

vec16:
	pushl	$0 /* error code */
	pushl	$16 /* vector */
	jmp	int_hand

vec17:
	/* error code */
	pushl	$17 /* vector */
	jmp	int_hand
	.word	0x9090

vec18:
	pushl	$0 /* error code */
	pushl	$18 /* vector */
	jmp	int_hand

vec19:
	pushl	$0 /* error code */
	pushl	$19 /* vector */
	jmp	int_hand

int_hand:
	/* At this point on the stack there is:
	 *  0(%esp) vector
	 *  4(%esp) error code
	 *  8(%esp) eip
	 * 12(%esp) cs
	 * 16(%esp) eflags
	 */
	pushl	%edi
	pushl	%esi
	pushl	%ebp
	/* Original stack pointer */
	leal	32(%esp), %ebp
	pushl	%ebp
	pushl	%ebx
	pushl	%edx
	pushl	%ecx
	pushl	%eax

	pushl	%esp	/* Pointer to structure on the stack */
	call	x86_exception
	pop	%eax	/* Drop the pointer */

	popl	%eax
	popl	%ecx
	popl	%edx
	popl	%ebx
	popl	%ebp	/* Ignore saved %esp value */
	popl	%ebp
	popl	%esi
	popl	%edi

	addl	$8, %esp /* pop of the vector and error code */

	iret

#if CONFIG_GDB_STUB == 1

	.globl gdb_stub_breakpoint
gdb_stub_breakpoint:
	popl	%eax	/* Return address */
	pushfl
	pushl	%cs
	pushl	%eax	/* Return address */
	pushl	$0	/* No error code */
	pushl	$32	/* vector 32 is user defined */
	jmp	int_hand

#endif

	.globl gdt, gdt_end, gdt_limit, idtarg

gdt_limit = gdt_end - gdt - 1	/* compute the table limit */
gdtaddr:
	.word	gdt_limit
	.long	gdt		/* we know the offset */

	 .data

	/* This is the gdt for GCC part of coreboot.
	 * It is different from the gdt in ROMCC/ASM part of coreboot
	 * which is defined in entry32.inc
	 *
	 * When the machine is initially started, we use a very simple
	 * gdt from rom (that in entry32.inc) which only contains those
	 * entries we need for protected mode.
	 *
	 * When we're executing code from RAM, we want to do more complex
	 * stuff, like initializing PCI option roms in real mode, or doing
	 * a resume from a suspend to ram.
	 */
gdt:
	/* selgdt 0, unused */
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

	/* selgdt 8, unused */
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

	/* selgdt 0x10, flat code segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00 /* G=1 and 0x0f, So we get 4Gbytes for limit */

	/* selgdt 0x18, flat data segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00

	/* selgdt 0x20, unused */
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

	/* The next two entries are used for executing VGA option ROMs */

	/* selgdt 0x28 16 bit 64k code at 0x00000000 */
	.word   0xffff, 0x0000
	.byte   0, 0x9a, 0, 0

	/* selgdt 0x30 16 bit 64k data at 0x00000000 */
	.word   0xffff, 0x0000
	.byte   0, 0x92, 0, 0

	/* The next two entries are used for ACPI S3 RESUME */

	/* selgdt 0x38, flat data segment 16 bit */
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x93, 0x8f, 0x00 /* G=1 and 0x0f, So we get 4Gbytes for limit */

	/* selgdt 0x40, flat code segment 16 bit */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0x8f, 0x00 /* G=1 and 0x0f, So we get 4Gbytes for limit */
gdt_end:

idtarg:
	.word	_idt_end - _idt - 1	/* limit */
	.long	_idt
	.word	0
_idt:
	.fill	20, 8, 0	# idt is uninitialized
_idt_end:

	.previous
.code32
