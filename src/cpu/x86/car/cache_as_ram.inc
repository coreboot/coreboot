/* We will use 4K bytes only */
/* disable HyperThreading is done by eswar*/
/* other's is the same as AMD except remove amd specific msr */

#define CacheSize DCACHE_RAM_SIZE
#define CacheBase (0xd0000 - CacheSize) 

#include <cpu/x86/mtrr.h>

	/* Save the BIST result */
	movl    %eax, %ebp

CacheAsRam:
	/* hope we can skip the double set for normal part */
#if USE_FALLBACK_IMAGE == 1

        // Check whether the processor has HT capability
        movl    $01, %eax
        cpuid
        btl     $28, %edx
        jnc     NotHtProcessor
        bswapl  %ebx
        cmpb    $01, %bh
        jbe     NotHtProcessor

        // It is a HT processor; Send SIPI to the other logical processor
        // within this processor so that the CAR related common system registers
        // are programmed accordingly

        // Use some register that is common to both logical processors
        // as semaphore. Refer Appendix B, Vol.3
        xorl    %eax, %eax
        xorl    %edx, %edx
        movl    $0x250, %ecx
        wrmsr

        // Figure out the logical AP's APIC ID; the following logic will work
        // only for processors with 2 threads
        // Refer to Vol 3. Table 7-1 for details about this logic
        movl    $0xFEE00020, %esi
        movl    (%esi), %ebx
        andl    $0xFF000000, %ebx
        bswapl  %ebx
        btl     $0, %ebx
        jnc     LogicalAP0
        andb    $0xFE, %bl
        jmp     Send_SIPI
LogicalAP0:
        orb     $0x01, %bl
Send_SIPI:
        bswapl  %ebx  // ebx - logical AP's APIC ID

        // Fill up the IPI command registers in the Local APIC mapped to default address
        // and issue SIPI to the other logical processor within this processor die.
Retry_SIPI:
        movl    %ebx, %eax
        movl    $0xFEE00310, %esi
        movl    %eax, (%esi)

        // SIPI vector - F900:0000
        movl    $0x000006F9, %eax
        movl    $0xFEE00300, %esi
        movl    %eax, (%esi)

        movl    $0x30, %ecx
SIPI_Delay:
        pause
        decl    %ecx
        jnz     SIPI_Delay

        movl    (%esi), %eax
        andl    $0x00001000, %eax
        jnz     Retry_SIPI

        // Wait for the Logical AP to complete initialization
LogicalAP_SIPINotdone:
        movl    $0x250, %ecx
        rdmsr
        orl     %eax, %eax
        jz      LogicalAP_SIPINotdone

NotHtProcessor:

#if 1
        /* Set the default memory type and enable fixed and variable MTRRs */
        movl    $MTRRdefType_MSR, %ecx
        xorl    %edx, %edx
        /* Enable Variable and Fixed MTRRs */
        movl    $0x00000c00, %eax
        wrmsr
#endif

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

#if CacheSize == 0x8000
        /* enable caching for 32K using fixed mtrr */
        movl    $0x269, %ecx  /* fix4k_c8000*/
        movl    $0x06060606, %eax /* WB IO type */
	movl    %eax, %edx
	wrmsr
#endif

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


#else
        /* disable cache */
        movl    %cr0, %eax
        orl    $(0x1<<30),%eax
        movl    %eax, %cr0

#endif /*  USE_FALLBACK_IMAGE == 1*/

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

        /* enable cache */
        movl    %cr0, %eax
        andl    $0x9fffffff,%eax
        movl    %eax, %cr0

#if USE_FALLBACK_IMAGE == 1

	/* Read the range with lodsl*/
        movl    $CacheBase, %esi
	cld
        movl    $(CacheSize>>2), %ecx
        rep     lodsl

	/* Clear the range */
        movl    $CacheBase, %edi
        movl    $(CacheSize>>2), %ecx
        xorl    %eax, %eax
        rep     stosl


#if 0
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

	/* Restore the BIST result */
	movl	%ebp, %eax
	/* We need to set ebp ? No need */
	movl	%esp, %ebp
	pushl 	%eax  /* bist */
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

#if USE_FALLBACK_IMAGE == 1
        .align 0x1000
        .code16
.global LogicalAP_SIPI
LogicalAP_SIPI:
        // cr0 register is shared among the logical processors; 
        // so clear CD & NW bits so that the BSP's cr0 register 
        // controls the cache behavior
        // Note: The cache behavior is determined by "OR" result 
        // of the cr0 registers of the logical processors

        movl    %cr0, %eax
        andl    $0x9FFFFFFF, %eax
        movl    %eax, %cr0

        finit

        // Set the semaphore to indicate the Logical AP is done
        // with CAR specific initialization
        movl    $0x250, %ecx
        movl    $0x06, %eax
        xorl    %edx, %edx
        wrmsr

        // Halt this AP
        cli
Halt_LogicalAP:
        hlt
        jmp     Halt_LogicalAP
        .code32
#endif /*USE_FALLBACK_IMAGE == 1*/
.CacheAsRam_out:
