/* by yhlu 6.2005 
	moved from nrv2v.c and some lines from crt0.S
   2006/05/02 - stepan: move nrv2b to an extra file.
*/
static inline void print_debug_cp_run(const char *strval, uint32_t val)
{
#if CONFIG_USE_PRINTK_IN_CAR
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

	print_debug("Copying LinuxBIOS to RAM.\r\n");

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
	        "leal _liseg, %0\n\t"
	        "leal _iseg,  %1\n\t"
                : "=a" (src) , "=b" (dst)
        );

	print_debug_cp_run("src=",(uint32_t)src); 
	print_debug_cp_run("dst=",(uint32_t)dst);

//	dump_mem(src, src+0x100);

	olen = unrv2b(src, dst, &ilen);
	print_debug_cp_run("linxbios_ram.nrv2b length = ", ilen);

#endif
//	dump_mem(dst, dst+0x100);

	print_debug_cp_run("linxbios_ram.bin   length = ", olen);

	print_debug("Jumping to LinuxBIOS.\r\n");

        __asm__ volatile (
                "xorl %ebp, %ebp\n\t" /* cpu_reset for hardwaremain dummy */
		"cli\n\t"
		"leal    _iseg, %edi\n\t"
		"jmp     *%edi\n\t"
	);

}

#if CONFIG_AP_CODE_IN_CAR == 1

static void copy_and_run_ap_code_in_car(unsigned ret_addr)
{
        uint8_t *src, *dst;
        unsigned long ilen, olen;

//        print_debug("Copying LinuxBIOS AP code to CAR.\r\n");

#if !CONFIG_COMPRESS
        __asm__ volatile (
                "leal _liseg_apc, %0\n\t"
                "leal _iseg_apc, %1\n\t"
                "leal _eiseg_apc, %2\n\t"
                "subl %1, %2\n\t"
                : "=a" (src), "=b" (dst), "=c" (olen)
        );
        memcpy(dst, src, olen);
#else

        __asm__ volatile (
                "leal _liseg_apc, %0\n\t"
                "leal _iseg_apc,  %1\n\t"
                : "=a" (src) , "=b" (dst)
        );

//        print_debug_cp_run("src=",(uint32_t)src);
//        print_debug_cp_run("dst=",(uint32_t)dst);

//      dump_mem(src, src+0x100);

        olen = unrv2b(src, dst, &ilen);
//        print_debug_cp_run("linxbios_apc.nrv2b length = ", ilen);

#endif
//      dump_mem(dst, dst+0x100);

//        print_debug_cp_run("linxbios_apc.bin   length = ", olen);

//        print_debug("Jumping to LinuxBIOS AP code in CAR.\r\n");

        __asm__ volatile (
                "movl %0, %%ebp\n\t" /* cpu_reset for hardwaremain dummy */
                "cli\n\t"
                "leal    _iseg_apc, %%edi\n\t"
                "jmp     *%%edi\n\t"
		:: "a"(ret_addr)
        );

}
#endif
