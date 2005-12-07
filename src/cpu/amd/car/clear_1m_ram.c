/* by yhlu 6.2005 */
/* be warned, this file will be used core 0/node 0 only */
static inline __attribute__((always_inline)) void clear_1m_ram(void)
{
        __asm__ volatile (
        	/* disable cache */
	        "movl    %cr0, %eax\n\t"
        	"orl    $(0x1<<30),%eax\n\t"
	        "movl    %eax, %cr0\n\t"
	);

	/* enable caching for first 1M using variable mtrr */
        __asm__ volatile (
                "wrmsr"
                : /* No outputs */
                : "c" (0x200), "a" (0 | MTRR_TYPE_WRCOMB), "d" (0)
        );

        __asm__ volatile (
                "wrmsr"
                : /* No outputs */
                : "c" (0x201), "a" ((~(( 0 + (CONFIG_LB_MEM_TOPK<<10) ) -1)) | 0x800), "d" (0x0000000f)
        );

	__asm__ volatile(
	        /* clear the first 1M */
	        "cld\n\t"
	        "rep     stosl\n\t"
		:
		: "a"(0), "D"(0) ,"c" ((CONFIG_LB_MEM_TOPK<<10)>>2)
	);

	__asm__ volatile (
        	/* disable cache */
	        "movl    %cr0, %eax\n\t"
        	"orl    $(0x1<<30),%eax\n\t"
	        "movl    %eax, %cr0\n\t"
	);

        /* enable caching for first 1M using variable mtrr */
        __asm__ volatile (
                "wrmsr"
                : /* No outputs */
                : "c" (0x200), "a" (0 | MTRR_TYPE_WRBACK), "d" (0)
        );

        __asm__ volatile (
                "wrmsr"
                : /* No outputs */
                : "c" (0x201), "a" ((~(( 0 + (CONFIG_LB_MEM_TOPK<<10) ) -1)) | 0x800), "d" (0x0000000f)
        );

	__asm__ volatile (
	        /* enable cache */
	        "movl    %cr0, %eax\n\t"
        	"andl    $0x9fffffff,%eax\n\t"
	        "movl    %eax, %cr0\n\t"
		"invd\n\t"
        );
}
