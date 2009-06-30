
        __asm__ volatile (
	/* 
	FIXME : backup stack in CACHE_AS_RAM into mmx and sse and after we get STACK up, we restore that.
		It is only needed if we want to go back
	*/
	
        /* We don't need cache as ram for now on */
        /* disable cache */
        "movl    %cr0, %eax\n\t"
        "orl    $(0x1<<30),%eax\n\t"
        "movl    %eax, %cr0\n\t"

        /* clear sth */
        "movl    $0x269, %ecx\n\t"  /* fix4k_c8000*/
        "xorl    %edx, %edx\n\t"
        "xorl    %eax, %eax\n\t"
	"wrmsr\n\t"
#if CONFIG_DCACHE_RAM_SIZE > 0x8000
	"movl    $0x268, %ecx\n\t"  /* fix4k_c0000*/
        "wrmsr\n\t"
#endif

        /* Set the default memory type and disable fixed and enable variable MTRRs */
        "movl    $0x2ff, %ecx\n\t"
//        "movl    $MTRRdefType_MSR, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        /* Enable Variable and Disable Fixed MTRRs */
        "movl    $0x00000800, %eax\n\t"
        "wrmsr\n\t"

#if defined(CLEAR_FIRST_1M_RAM)
        /* enable caching for first 1M using variable mtrr */
        "movl    $0x200, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        "movl     $(0 | 1), %eax\n\t"
//	"movl     $(0 | MTRR_TYPE_WRCOMB), %eax\n\t"
        "wrmsr\n\t"

        "movl    $0x201, %ecx\n\t"
        "movl    $0x0000000f, %edx\n\t" /* AMD 40 bit 0xff*/
        "movl    $((~(( 0 + 0x100000) - 1)) | 0x800), %eax\n\t"
        "wrmsr\n\t"
#endif

        /* enable cache */
        "movl    %cr0, %eax\n\t"
        "andl    $0x9fffffff,%eax\n\t"
        "movl    %eax, %cr0\n\t"
#if defined(CLEAR_FIRST_1M_RAM)
        /* clear the first 1M */
        "movl    $0x0, %edi\n\t"
        "cld\n\t"
        "movl    $(0x100000>>2), %ecx\n\t"
        "xorl    %eax, %eax\n\t"
        "rep     stosl\n\t"

        /* disable cache */
        "movl    %cr0, %eax\n\t"
        "orl    $(0x1<<30),%eax\n\t"
        "movl    %eax, %cr0\n\t"

        /* enable caching for first 1M using variable mtrr */
        "movl    $0x200, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        "movl     $(0 | 6), %eax\n\t"
//	"movl     $(0 | MTRR_TYPE_WRBACK), %eax\n\t"
        "wrmsr\n\t"

        "movl    $0x201, %ecx\n\t"
        "movl    $0x0000000f, %edx\n\t" /* AMD 40 bit 0xff*/
        "movl    $((~(( 0 + 0x100000) - 1)) | 0x800), %eax\n\t"
        "wrmsr\n\t"

        /* enable cache */
        "movl    %cr0, %eax\n\t"
        "andl    $0x9fffffff,%eax\n\t"
        "movl    %eax, %cr0\n\t"
	"invd\n\t"

	/* 
	FIXME: I hope we don't need to change esp and ebp value here, so we can restore value from mmx sse back
		But the problem is the range is some io related, So don't go back
	*/
#endif
        );
