/* by yhlu 6.2005 
	moved from nrv2v.c and some lines from crt0.S
   2006/05/02 - stepan: move nrv2b to an extra file.
*/

#if CONFIG_COMPRESS
#define ENDIAN   0
#define BITSIZE 32
#include "lib/nrv2b.c"
#endif

static void copy_and_run(unsigned cpu_reset)
{
	uint8_t *src, *dst; 
#if !CONFIG_COMPRESS
	unsigned long dst_len;
#endif
        unsigned long ilen, olen;


#if !CONFIG_COMPRESS 
	print_debug("Copying coreboot to RAM.\r\n");
	__asm__ volatile (
		"leal _liseg, %0\n\t"
		"leal _iseg, %1\n\t"
		"leal _eiseg, %2\n\t"
		"subl %1, %2\n\t"
		: "=a" (src), "=b" (dst), "=c" (dst_len)
	);
	memcpy(src, dst, dst_len);
#else 
	print_debug("Uncompressing coreboot to RAM.\r\n");

        __asm__ volatile (
	        "leal _liseg, %0\n\t"
	        "leal _iseg,  %1\n\t"
                : "=a" (src) , "=b" (dst)
        );

#if CONFIG_USE_INIT		
	printk_spew("src=%08x\r\n",src); 
	printk_spew("dst=%08x\r\n",dst);
#else
        print_spew("src="); print_spew_hex32((uint32_t)src); print_spew("\r\n");
        print_spew("dst="); print_spew_hex32((uint32_t)dst); print_spew("\r\n");
#endif
	
//	dump_mem(src, src+0x100);

	olen = unrv2b(src, dst, &ilen);

#endif
//	dump_mem(dst, dst+0x100);
#if CONFIG_USE_INIT
	printk_spew("coreboot_ram.bin length = %08x\r\n", olen);
#else
	print_spew("coreboot_ram.bin length = "); print_spew_hex32(olen); print_spew("\r\n");
#endif
	print_debug("Jumping to coreboot.\r\n");

	if(cpu_reset == 1 ) {
		__asm__ volatile (
			"movl $0xffffffff, %ebp\n\t"
		);
	}
	else {
                __asm__ volatile (
                        "xorl %ebp, %ebp\n\t"
                );
	}
	
	__asm__ volatile (
		"cli\n\t"
		"leal    _iseg, %edi\n\t"
		"jmp     %edi\n\t"
	);

}
