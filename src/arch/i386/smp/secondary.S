#include <arch/asm.h>
#include <arch/intel.h>
#include <cpu/p6/mtrr.h>
#include <cpu/p6/apic.h>
	.text
	.globl _secondary_start
	.balign 4096
_secondary_start:
	.code16
	cli
	xorl	%eax, %eax
	movl	%eax, %cr3    /* Invalidate TLB*/

	/* On hyper threaded cpus invalidating the cache here is
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

	ljmpl $0x10, $1f
1:	
	.code32
	movw	$0x18, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* Enable the local apic, and map it where we expext it */
	movl	$APIC_BASE_MSR, %ecx
	rdmsr
	orl	$APIC_BASE_MSR_ENABLE, %eax
	andl	$(~APIC_BASE_MSR_ADDR_MASK), %eax
	orl	$APIC_DEFAULT_BASE, %eax
	wrmsr

	/* Get the apic_id */
	movl	(APIC_ID + APIC_DEFAULT_BASE), %edi
	shrl	$24, %edi

	/* Get the cpu index (MAX_CPUS on error) */
	movl	$-4, %ebx
1:	addl	$4, %ebx
	cmpl	$(MAX_CPUS << 2), %ebx
	je	2
	cmpl	%edi, initial_apicid(%ebx)
	jne	1b
2:	shrl	$2, %ebx
	
	/* set the stack pointer */
	movl	$_estack, %esp
	movl	%ebx, %eax
	movl	$STACK_SIZE, %ebx
	mull	%ebx
	subl	%eax, %esp

	call	secondary_cpu_init
1:	hlt
	jmp	1b

gdtaddr:
	.word	gdt_limit	/* the table limit */
	.long	gdt		/* we know the offset */


.code32
