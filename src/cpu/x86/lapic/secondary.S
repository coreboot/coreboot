#include <cpu/x86/mtrr.h>
#include <cpu/x86/lapic_def.h>

	.text
	.globl _secondary_start, _secondary_start_end
	.balign 4096
_secondary_start:
	.code16
	cli
	xorl	%eax, %eax
	movl	%eax, %cr3    /* Invalidate TLB*/

	/* On hyper threaded cpus, invalidating the cache here is
	 * very very bad.  Don't.
	 */

	/* setup the data segment */
	movw	%cs, %ax
	movw	%ax, %ds

	data32	lgdt	gdtaddr  - _secondary_start

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	ljmpl	$0x10, $1f
1:	
	.code32
	movw	$0x18, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* Load the Interrupt descriptor table */
	lidt	idtarg

	/* Set the stack pointer, and flag that we are done */
	xorl	%eax, %eax
	movl	secondary_stack, %esp
	movl	%eax, secondary_stack

	call	secondary_cpu_init
1:	hlt
	jmp	1b

	gdtaddr:
	.word   gdt_limit	/* the table limit */
	.long   gdt             /* we know the offset */

_secondary_start_end:
.code32
