/* by yhlu 6.2005 
	moved from nrv2v.c and some lines from crt0.S
   2006/05/02 - stepan: move nrv2b to an extra file.
*/

static inline void print_debug_cp_run(const char *strval, uint32_t val)
{
#if CONFIG_USE_INIT
        printk_debug("%s%08x\r\n", strval, val);
#else
        print_debug(strval); print_debug_hex32(val); print_debug("\r\n");
#endif
}

#if CONFIG_COMPRESS
#define ENDIAN   0
#define BITSIZE 32
#include "lib/nrv2b.c"
#endif


static void copy_and_run(void)
{
	uint8_t *src, *dst; 
        unsigned long ilen, olen;

	print_debug("Copying LinuxBIOS to ram.\r\n");

#if !CONFIG_COMPRESS 
	__asm__ volatile (
		"leal _liseg, %0\n\t"
		"leal _iseg, %1\n\t"
		"leal _eiseg, %2\n\t"
		"subl %1, %2\n\t"
		: "=a" (src), "=b" (dst), "=c" (olen)
	);
	memcpy(dst, src, olen);
#else 

        __asm__ volatile (
	        "leal  _liseg, %0\n\t"
	        "leal  _iseg,  %1\n\t"
                : "=a" (src) , "=b" (dst)
        );

	print_debug_cp_run("src=",(uint32_t)src); 
	print_debug_cp_run("dst=",(uint32_t)dst);

	olen = unrv2b(src, dst, &ilen);
	print_debug_cp_run("linxbios_ram.nrv2b length = ", ilen);

#endif

	print_debug_cp_run("linxbios_ram.bin   length = ", olen);

	print_debug("Jumping to LinuxBIOS.\r\n");

        __asm__ volatile (
                "xorl %ebp, %ebp\n\t" /* cpu_reset for hardwaremain dummy */
		"cli\n\t"
		"leal    _iseg, %edi\n\t"
		"jmp     *%edi\n\t"
	);

}
