// mainboardinit cpu/x86/16bit/entry16.inc

/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */

/* Start code to put an i386 or later processor into 32-bit 
 * protected mode. 
 */

/* .section ".rom.text" */
#include <arch/rom_segs.h>
.code16
.globl _start
.type _start, @function

_start: 
	cli
	/* Save the BIST result */
	movl	%eax, %ebp

	/* thanks to kmliu@sis.tw.com for this TBL fix ... */

	/* IMMEDIATELY invalidate the translation lookaside buffer before executing*/
	/* any further code.  Even though paging is disabled we could still get*/
	/*false address translations due to the TLB if we didn't invalidate it.*/

	xorl	%eax, %eax
	movl	%eax, %cr3    /* Invalidate TLB*/

	/* Invalidating the cache here seems to be a bad idea on
	 * modern processors.  Don't.
	 * If we are hyperthreaded or we have multiple cores it is bad,
	 * for SMP startup.  On Opterons it causes a 5 second delay.
	 * Invalidating the cache was pure paranoia in any event.
	 * If you cpu needs it you can write a cpu dependent version of
	 * entry16.inc.
	 */

	/* Note: gas handles memory addresses in 16 bit code very poorly.
	 * In particular it doesn't appear to have a directive allowing you
	 * associate a section or even an absolute offset with a segment register.
	 *
	 * This means that anything except cs:ip relative offsets are
	 * a real pain in 16 bit mode.  And explains why it is almost
	 * imposible to get gas to do lgdt correctly.
	 *
	 * One way to work around this is to have the linker do the
	 * math instead of the assembler.  This solves the very
	 * pratical problem of being able to write code that can
	 * be relocated.
	 *
	 * An lgdt call before we have memory enabled cannot be 
	 * position independent, as we cannot execute a call
	 * instruction to get our current instruction pointer.
	 * So while this code is relocateable it isn't arbitrarily
	 * relocatable.
	 *
	 * The criteria for relocation have been relaxed to their 
	 * utmost, so that we can use the same code for both
	 * our initial entry point and startup of the second cpu.
	 * The code assumes when executing at _start that:
	 * (((cs & 0xfff) == 0) and (ip == _start & 0xffff))
	 * or
	 * ((cs == anything) and (ip == 0)).
	 *
	 * The restrictions in reset16.inc mean that _start initially
	 * must be loaded at or above 0xffff0000 or below 0x100000.
	 *
	 * The linker scripts computs gdtptr16_offset by simply returning
	 * the low 16 bits.  This means that the intial segment used
	 * when start is called must be 64K aligned.  This should not
	 * restrict the address as the ip address can be anything.
	 */

	movw	%cs, %ax
	shlw	$4, %ax
	movw	$gdtptr16_offset, %bx
	subw	%ax, %bx
	data32  lgdt %cs:(%bx)

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	/* Restore BIST to %eax */
	movl	%ebp, %eax

	/* Now that we are in protected mode jump to a 32 bit code segment. */
	data32	ljmp	$ROM_CODE_SEG, $__protected_start

	/**
	 * The gdt is defined in entry32.inc, it has a 4 Gb code segment
	 * at 0x08, and a 4 GB data segment at 0x10;
	 */
.align	4
.globl gdtptr16
gdtptr16:
	.word	gdt_end - gdt -1 /* compute the table limit */
	.long	gdt		 /* we know the offset */

.globl _estart
_estart:
	.code32


/* For starting linuxBIOS in protected mode */

#include <arch/rom_segs.h>

/* 	.section ".rom.text" */
	.code32

	.align	4
.globl gdtptr

	/* This is the gdt for ROMCC/ASM part of LinuxBIOS.
	 * It is different from the gdt in GCC part of LinuxBIOS
	 * which is defined in c_start.S */
gdt:
gdtptr:
	.word	gdt_end - gdt -1 /* compute the table limit */
	.long	gdt		 /* we know the offset */
	.word	0

	/* selgdt 0x08, flat code segment */
	.word	0xffff, 0x0000		
	.byte	0x00, 0x9b, 0xcf, 0x00	

	/* selgdt 0x10,flat data segment */
	.word	0xffff, 0x0000		
	.byte	0x00, 0x93, 0xcf, 0x00	

gdt_end:
	


// mainboardinit cpu/x86/32bit/entry32.inc



/*
 *	When we come here we are in protected mode. We expand 
 *	the stack and copies the data segment from ROM to the
 *	memory.
 *
 *	After that, we call the chipset bootstrap routine that
 *	does what is left of the chipset initialization. 
 *
 *	NOTE aligned to 4 so that we are sure that the prefetch
 *	cache will be reloaded.
 */
	.align	4
.globl protected_start
protected_start:

	lgdt	%cs:gdtptr
	ljmp	$ROM_CODE_SEG, $__protected_start
	
__protected_start:
	/* Save the BIST value */
	movl	%eax, %ebp

	intel_chip_post_macro(0x10)	/* post 10 */

	movw	$ROM_DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* Restore the BIST value to %eax */
	movl	%ebp, %eax




// mainboardinit cpu/x86/16bit/reset16.inc
	

	.section ".reset"
	.code16
.globl	reset_vector
reset_vector:
	.byte  0xe9
	.int   _start - ( . + 2 )
	/* Note: The above jump is hand coded to work around bugs in binutils.
	 * 5 byte are used for a 3 byte instruction.  This works because x86
	 * is little endian and allows us to use supported 32bit relocations
	 * instead of the weird 16 bit relocations that binutils does not
	 * handle consistenly between versions because they are used so rarely.
	 */
	. = 0x8;
	.code32
	jmp	protected_start
	.previous






// mainboardinit arch/i386/lib/cpu_reset.inc

jmp cpu_reset_out

__cpu_reset:
	/* set the boot_complete flag */
	movl	$0xffffffff, %ebp
	jmp	__main

cpu_reset_out:


// mainboardinit arch/i386/lib/id.inc

	.section ".id", "a", @progbits

	.globl __id_start
__id_start:
vendor:	
	.asciz MAINBOARD_VENDOR
part:		
	.asciz MAINBOARD_PART_NUMBER
.long __id_end + 0x10 - vendor  /* Reverse offset to the vendor id */
.long __id_end + 0x10 - part    /* Reverse offset to the part number */
.long PAYLOAD_SIZE + ROM_IMAGE_SIZE  /* Size of this romimage */
	.globl __id_end

__id_end:
.previous


// mainboardinit ./failover.inc
// mainboardinit ./auto.inc
// 

/* by yhlu 6.2005 */
/* yhlu 2005.12 make it support HDT Memory Debuggers with Disassmbly, please select the PCI Bus mem for Phys Type*/
/* yhlu 2006.3 copy data from cache to ram and reserve 0x1000 for global variables */
#define CacheSize DCACHE_RAM_SIZE
#define CacheBase (0xd0000 - CacheSize)
/* leave some space for global variable to pass to RAM stage */
#define GlobalVarSize DCACHE_RAM_GLOBAL_VAR_SIZE

#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>

	/* Save the BIST result */
	movl    %eax, %ebp
	
	/*for normal part %ebx already contain cpu_init_detected from fallback call */

cache_as_ram_setup:

	/* hope we can skip the double set for normal part */
#if ((HAVE_FAILOVER_BOOT==1) && (USE_FAILOVER_IMAGE==1)) || ((HAVE_FAILOVER_BOOT==0) && (USE_FALLBACK_IMAGE==1))
	/* check if cpu_init_detected */
	movl	$MTRRdefType_MSR, %ecx
	rdmsr
	andl $0x00000800, %eax
	movl %eax, %ebx  /* We store the status */
	
	/* Set MtrrFixDramModEn for clear fixed mtrr */
enable_fixed_mtrr_dram_modify:
	movl	$SYSCFG_MSR, %ecx
	rdmsr
	andl	$(~(SYSCFG_MSR_MtrrFixDramEn|SYSCFG_MSR_MtrrVarDramEn)), %eax
	orl	$SYSCFG_MSR_MtrrFixDramModEn, %eax
	wrmsr

	/*Clear all MTRRs */

	xorl    %edx, %edx
	movl    $fixed_mtrr_msr, %esi
clear_fixed_var_mtrr:
        lodsl   (%esi), %eax
        testl   %eax, %eax
        jz      clear_fixed_var_mtrr_out

        movl    %eax, %ecx
        xorl    %eax, %eax
        wrmsr   

        jmp     clear_fixed_var_mtrr
clear_fixed_var_mtrr_out:

#if CacheSize == 0x10000 
        /* enable caching for 64K using fixed mtrr */
        movl    $0x268, %ecx  /* fix4k_c0000*/
        movl    $0x06060606, %eax /* WB IO type */
	movl    %eax, %edx
        wrmsr
	movl	$0x269, %ecx
	wrmsr
#endif

#if CacheSize == 0xc000
        /* enable caching for 16K using fixed mtrr */
        movl    $0x268, %ecx  /* fix4k_c4000*/
        movl    $0x06060606, %edx /* WB IO type */
        xorl    %eax, %eax
        wrmsr
        /* enable caching for 32K using fixed mtrr */
        movl    $0x269, %ecx  /* fix4k_c8000*/
        movl    $0x06060606, %eax /* WB IO type */
        movl    %eax, %edx
        wrmsr

#endif


#if CacheSize == 0x8000
        /* enable caching for 32K using fixed mtrr */
        movl    $0x269, %ecx  /* fix4k_c8000*/
        movl    $0x06060606, %eax /* WB IO type */
	movl    %eax, %edx
	wrmsr
#endif

#if CacheSize < 0x8000
        /* enable caching for 16K/8K/4K using fixed mtrr */
        movl    $0x269, %ecx  /* fix4k_cc000*/
    #if CacheSize == 0x4000
        movl    $0x06060606, %edx /* WB IO type */
    #endif
    #if CacheSize == 0x2000
        movl    $0x06060000, %edx /* WB IO type */
    #endif
    #if CacheSize == 0x1000
        movl    $0x06000000, %edx /* WB IO type */
    #endif
	xorl    %eax, %eax
	wrmsr
#endif

        /* enable memory access for first MBs using top_mem */
        movl    $TOP_MEM, %ecx
        xorl    %edx, %edx
        movl    $(((CONFIG_LB_MEM_TOPK << 10) + TOP_MEM_MASK) & ~TOP_MEM_MASK) , %eax
        wrmsr
#endif /*  USE_FAILOVER_IMAGE == 1*/


#if ((HAVE_FAILOVER_BOOT==1) && (USE_FAILOVER_IMAGE == 0)) || ((HAVE_FAILOVER_BOOT==0) && (USE_FALLBACK_IMAGE==0))
        /* disable cache */
        movl    %cr0, %eax
        orl    $(0x1<<30),%eax
        movl    %eax, %cr0

#endif 

#if defined(XIP_ROM_SIZE) && defined(XIP_ROM_BASE)
        /* enable write base caching so we can do execute in place
         * on the flash rom.
         */
        movl    $0x202, %ecx
        xorl    %edx, %edx
        movl    $(XIP_ROM_BASE | MTRR_TYPE_WRBACK), %eax
        wrmsr

        movl    $0x203, %ecx
        movl    $((1<<(CPU_ADDR_BITS-32))-1), %edx  /* AMD 40 bit */
        movl    $(~(XIP_ROM_SIZE - 1) | 0x800), %eax
        wrmsr
#endif /* XIP_ROM_SIZE && XIP_ROM_BASE */

#if ((HAVE_FAILOVER_BOOT==1) && (USE_FAILOVER_IMAGE==1)) || ((HAVE_FAILOVER_BOOT==0) && (USE_FALLBACK_IMAGE==1))
        /* Set the default memory type and enable fixed and variable MTRRs */
        movl    $MTRRdefType_MSR, %ecx
        xorl    %edx, %edx
        /* Enable Variable and Fixed MTRRs */
        movl    $0x00000c00, %eax
        wrmsr

        /* Enable the MTRRs and IORRs in SYSCFG */
        movl    $SYSCFG_MSR, %ecx
        rdmsr
        orl     $(SYSCFG_MSR_MtrrVarDramEn | SYSCFG_MSR_MtrrFixDramEn), %eax
        wrmsr
#endif

        /* enable cache */
        movl    %cr0, %eax
        andl    $0x9fffffff,%eax
        movl    %eax, %cr0

#if ((HAVE_FAILOVER_BOOT==1) && (USE_FAILOVER_IMAGE==1)) || ((HAVE_FAILOVER_BOOT==0) && (USE_FALLBACK_IMAGE==1))

        /* Read the range with lodsl*/
	cld
        movl    $CacheBase, %esi
        movl    $(CacheSize>>2), %ecx
        rep     
		lodsl
        /* Clear the range */
        movl    $CacheBase, %edi
        movl    $(CacheSize>>2), %ecx
        xorl    %eax, %eax
        rep     
		stosl

#endif /*USE_FAILOVER_IMAGE == 1*/

	/* set up the stack pointer */
	movl	$(CacheBase+CacheSize - GlobalVarSize), %eax 
	movl    %eax, %esp

	/* Restore the BIST result */
	movl    %ebp, %eax
	/* We need to set ebp ? No need */
	movl	%esp, %ebp
	pushl %ebx  /* init detected */
	pushl %eax  /* bist */
	call    cache_as_ram_main
	/* We will not go back */

fixed_mtrr_msr: 
        .long   0x250, 0x258, 0x259
        .long   0x268, 0x269, 0x26A
        .long   0x26B, 0x26C, 0x26D
        .long   0x26E, 0x26F
var_mtrr_msr:   
        .long   0x200, 0x201, 0x202, 0x203
        .long   0x204, 0x205, 0x206, 0x207
        .long   0x208, 0x209, 0x20A, 0x20B
        .long   0x20C, 0x20D, 0x20E, 0x20F
var_iorr_msr:   
        .long   0xC0010016, 0xC0010017, 0xC0010018, 0xC0010019
mem_top:
        .long   0xC001001A, 0xC001001D
        .long   0x000 /* NULL, end of table */
cache_as_ram_setup_out:

