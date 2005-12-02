/* by yhlu 6.2005 */
/* be warned, this file will be used core 0/node 0 only */
static inline __attribute__((always_inline)) void clear_1m_ram(void)
{
        __asm__ volatile (

        /* disable cache */
        "movl    %cr0, %eax\n\t"
        "orl    $(0x1<<30),%eax\n\t"
        "movl    %eax, %cr0\n\t"

        /* enable caching for first 1M using variable mtrr */
        "movl    $0x200, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        "movl     $(0 | 1), %eax\n\t"
//	"movl     $(0 | MTRR_TYPE_WRCOMB), %eax\n\t"
        "wrmsr\n\t"

        "movl    $0x201, %ecx\n\t"
        "movl    $0x0000000f, %edx\n\t" 
        "movl    $((~(( 0 + 0x100000) - 1)) | 0x800), %eax\n\t"
        "wrmsr\n\t"

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
        "movl    $0x0000000f, %edx\n\t" 
        "movl    $((~(( 0 + 0x100000) - 1)) | 0x800), %eax\n\t"
        "wrmsr\n\t"


        /* enable cache */
        "movl    %cr0, %eax\n\t"
        "andl    $0x9fffffff,%eax\n\t"
        "movl    %eax, %cr0\n\t"
	"invd\n\t"

        );
}
