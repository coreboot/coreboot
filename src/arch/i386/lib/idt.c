// we had hoped to avoid this. 
#if 0
/*
 *  setup_idt
 *
 *  sets up a idt with 256 entries pointing to
 *  ignore_int, interrupt gates. It doesn't actually load
 *  idt - that can be done only after paging has been enabled
 *  and the kernel moved to PAGE_OFFSET. Interrupts
 *  are enabled elsewhere, when we can be relatively
 *  sure everything is ok.
 */
setup_idt:
	lea ignore_int,%edx
	movl $(__KERNEL_CS << 16),%eax
	movw %dx,%ax		/* selector = 0x0010 = cs */
	movw $0x8E00,%dx	/* interrupt gate - dpl=0, present */

	lea SYMBOL_NAME(idt_table),%edi
	mov $256,%ecx
rp_sidt:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	dec %ecx
	jne rp_sidt
	ret

ENTRY(stack_start)
	.long SYMBOL_NAME(init_task_union)+8192
	.long __KERNEL_DS

/* This is the default interrupt "handler" :-) */
int_msg:
	.asciz "Unknown interrupt\n"
	ALIGN
ignore_int:
	cld
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %es
	pushl %ds
	movl $(__KERNEL_DS),%eax
	movl %eax,%ds
	movl %eax,%es
	pushl $int_msg
	call SYMBOL_NAME(printk)
	popl %eax
	popl %ds
	popl %es
	popl %edx
	popl %ecx
	popl %eax
	iret

/*
 * The interrupt descriptor table has room for 256 idt's,
 * the global descriptor table is dependent on the number
 * of tasks we can have..
 */
#define IDT_ENTRIES	256
#define GDT_ENTRIES	(__TSS(NR_CPUS))


.globl SYMBOL_NAME(idt)
.globl SYMBOL_NAME(gdt)

	ALIGN
	.word 0
idt_descr:
	.word IDT_ENTRIES*8-1		# idt contains 256 entries
SYMBOL_NAME(idt):
	.long SYMBOL_NAME(idt_table)

	.word 0
#endif

/*
 * This starts the data section. Note that the above is all
 * in the text section because it has alignment requirements
 * that we cannot fulfill any other way.
 */
.data

ALIGN
/*
 * This contains typically 140 quadwords, depending on NR_CPUS.
 *
 * NOTE! Make sure the gdt descriptor in head.S matches this if you
 * change anything.
 */
ENTRY(gdt_table)
	.quad 0x0000000000000000	/* NULL descriptor */
	.quad 0x0000000000000000	/* not used */
	.quad 0x00cf9a000000ffff	/* 0x10 kernel 4GB code at 0x00000000 */
	.quad 0x00cf92000000ffff	/* 0x18 kernel 4GB data at 0x00000000 */
	.quad 0x00cffa000000ffff	/* 0x23 user   4GB code at 0x00000000 */
	.quad 0x00cff2000000ffff	/* 0x2b user   4GB data at 0x00000000 */
	.quad 0x0000000000000000	/* not used */
	.quad 0x0000000000000000	/* not used */
	/*
	 * The APM segments have byte granularity and their bases
	 * and limits are set at run time.
	 */
	.quad 0x0040920000000000	/* 0x40 APM set up for bad BIOS's */
	.quad 0x00409a0000000000	/* 0x48 APM CS    code */
	.quad 0x00009a0000000000	/* 0x50 APM CS 16 code (16 bit) */
	.quad 0x0040920000000000	/* 0x58 APM DS    data */
	.fill NR_CPUS*4,8,0		/* space for TSS's and LDT's */
		
/*
 * This is to aid debugging, the various locking macros will be putting
 * code fragments here.  When an oops occurs we'd rather know that it's
 * inside the .text.lock section rather than as some offset from whatever
 * function happens to be last in the .text segment.
 */
.section .text.lock
ENTRY(stext_lock)
#endif

void
lidt(void *base, unsigned int limit)
{

  unsigned int i[2];

  i[0] = limit << 16;
  i[1] = // later
}
// this is too awful. 
}
