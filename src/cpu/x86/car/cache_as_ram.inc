/* We will use 4K bytes only */
#define CacheSize DCACHE_RAM_SIZE
#define CacheBase DCACHE_RAM_BASE

#include <cpu/x86/mtrr.h>

	/* Save the BIST result */
	movl    %eax, %ebp

CacheAsRam:
	/* hope we can skip the double set for normal part */
#if USE_FALLBACK_IMAGE == 1

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

        /* enable caching for 64K using variable mtrr */
        movl    $0x200, %ecx
        xorl    %edx, %edx
        movl     $(CacheBase | MTRR_TYPE_WRBACK), %eax
        wrmsr

        movl    $0x201, %ecx
        movl    $0x0000000f, %edx 
        movl    $((~((CacheBase + CacheSize) - 1)) | 0x800), %eax
        wrmsr  
	
        /* Set the default memory type and enable variable MTRRs */
        movl    $MTRRdefType_MSR, %ecx
        xorl    %edx, %edx
        /* Enable Variable MTRRs */
        movl    $0x00000800, %eax
        wrmsr

        /* Disable fast string operation   */
	movl    $0x1a0, %ecx        
	rdmsr        
	andl    $(~0x1), %eax        
	wrmsr
#else
        /* disable cache */
        movl    %cr0, %eax
        orl    $(0x1<<30),%eax
        movl    %eax, %cr0

#endif /*  USE_FALLBACK_IMAGE == 1*/

#if 0
#if defined(XIP_ROM_SIZE) && defined(XIP_ROM_BASE)
        /* enable write base caching so we can do execute in place
         * on the flash rom.
         */
        movl    $0x202, %ecx
        xorl    %edx, %edx
        movl    $(XIP_ROM_BASE | MTRR_TYPE_WRBACK), %eax
        wrmsr

        movl    $0x203, %ecx
        movl    $0x0000000f, %edx
        movl    $(~(XIP_ROM_SIZE - 1) | 0x800), %eax
        wrmsr
#endif /* XIP_ROM_SIZE && XIP_ROM_BASE */
#endif

        /* enable cache */
        movl    %cr0, %eax
        andl    $0x9fffffff,%eax
        movl    %eax, %cr0

#if USE_FALLBACK_IMAGE == 1

//	intel_chip_post_macro(0x11)     /* post 11 */

	/* Read the range with lodsl*/
        movl    $CacheBase, %esi
	cld
        movl    $(CacheSize>>2), %ecx
        rep     lodsl

        // Disable the cache. This is the trick. Processors
        // Pentium Pro and above are designed to respond to cache
        // hits with CD=1 and NW=1. That is read hits access the
        // cache; write hits update the cache. With the tags
        // established above and no snoop hit, the cache will
        // behave as RAM.        
	movl    %cr0, %eax        
	orl     $0x60000000, %eax        
	movl    %eax, %cr0

	/* Clear the range */
        movl    $CacheBase, %edi
	cld
        movl    $(CacheSize>>2), %ecx
        xorl    %eax, %eax
        rep     stosl


#if 1
        /* check the cache as ram */
        movl  $CacheBase, %esi
        movl    $(CacheSize>>2), %ecx
.xin1:  
        movl  %esi, %eax
        movl  %eax, (%esi)
        decl  %ecx
        je      .xout1
        add     $4, %esi
        jmp     .xin1
.xout1: 

        movl  $CacheBase, %esi
//        movl    $(CacheSize>>2), %ecx
        movl $4, %ecx
.xin1x: 
        movl  %esi, %eax
        
        movl    $0x4000, %edx
        movb    %ah, %al 
.testx1:  
        outb %al, $0x80
        decl    %edx
        jnz .testx1
        
        movl  (%esi), %eax
        cmpb 0xff, %al
        je .xin2  /* dont show */
        
        movl    $0x4000, %edx
.testx2:
        outb %al, $0x80
        decl    %edx
        jnz .testx2

.xin2:  decl     %ecx
        je      .xout1x
        add     $4, %esi
        jmp     .xin1x
.xout1x:

#endif
#endif /*USE_FALLBACK_IMAGE == 1*/

//	intel_chip_post_macro(0x12)     /* post 12 */

	movl	$(CacheBase+CacheSize-4), %eax
	movl    %eax, %esp

	/* Load a different set of data segments */
#if CONFIG_USE_INIT
	movw    $CACHE_RAM_DATA_SEG, %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %ss
#endif

lout:
//	intel_chip_post_macro(0x13)     /* post 13 */

	/* Restore the BIST result */
	movl    %ebp, %eax
	/* We need to set ebp ? No need */
	movl	%esp, %ebp
	pushl %eax  /* bist */
	call    amd64_main
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
        .long   0x000 /* NULL, end of table */
.CacheAsRam_out:
