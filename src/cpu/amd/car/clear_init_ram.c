/* by yhlu 6.2005 */
/* be warned, this file will be used core 0/node 0 only */

static void __attribute__((noinline)) clear_init_ram(void)
{
	// gcc 3.4.5 will inline the copy_and_run and clear_init_ram in post_cache_as_ram
	// will reuse %edi as 0 from clear_memory for copy_and_run part, actually it is increased already
	// so noline clear_init_ram
        clear_memory(0,  ((CONFIG_LB_MEM_TOPK<<10) - DCACHE_RAM_SIZE));

}

/* be warned, this file will be used by core other than core 0/node 0 or core0/node0 when cpu_reset*/
static inline __attribute__((always_inline)) void set_init_ram_access(void)
{
        __asm__ volatile (

	"pushl %%ecx\n\t"
	"pushl %%edx\n\t"
	"pushl %%eax\n\t"

        /* enable caching for first 1M using variable mtrr */
        "movl    $0x200, %%ecx\n\t"
        "xorl    %%edx, %%edx\n\t"
        "movl     $(0 | 6), %%eax\n\t"
//      "movl     $(0 | MTRR_TYPE_WRBACK), %%eax\n\t"
        "wrmsr\n\t"

        "movl    $0x201, %%ecx\n\t"
        "movl    $0x0000000f, %%edx\n\t"
#if CONFIG_USE_INIT
        "movl    %%esi, %%eax\n\t"
#else
        "movl    $((~(( 0 + (CONFIG_LB_MEM_TOPK<<10) ) -1)) | 0x800), %%eax\n\t"
#endif
        "wrmsr\n\t"

#if 0
        /* enable caching for 64K using fixed mtrr */
        "movl    $0x26e, %%ecx\n\t"  /* fix4k_f0000*/
        "movl    $0x1e1e1e1e, %%eax\n\t" /* WB MEM type */
        "movl    %%eax, %%edx\n\t"
        "wrmsr\n\t"
        "movl    $0x26f, %%ecx\n\t" /* fix4k_f8000*/
        "wrmsr\n\t"
#endif

	"popl	%%eax\n\t"
	"popl   %%edx\n\t"
	"popl	%%ecx\n\t"

        :
        :
#if CONFIG_USE_INIT
        "S"((~(( 0 + (CONFIG_LB_MEM_TOPK<<10) ) -1)) | 0x800)
#endif
        );
}

