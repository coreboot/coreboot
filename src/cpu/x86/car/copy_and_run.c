/* by yhlu 6.2005 
	moved from nrv2v.c and some lines from crt0.S
   2006/05/02 - stepan: move nrv2b to an extra file.
*/

#if CONFIG_COMPRESS
#include "lib/nrv2b.c"
#endif

static void copy_and_run(unsigned cpu_reset)
{
	uint8_t *src, *dst; 
	unsigned long dst_len;
        unsigned long ilen = 0, olen = 0, last_m_off =  1;
        uint32_t bb = 0;
        unsigned bc = 0;

	print_debug("Copying LinuxBIOS to ram.\r\n");

#if !CONFIG_COMPRESS 
	__asm__ volatile (
		"leal _liseg, %0\n\t"
		"leal _iseg, %1\n\t"
		"leal _eiseg, %2\n\t"
		"subl %1, %2\n\t"
		: "=a" (src), "=b" (dst), "=c" (dst_len)
	);
	memcpy(src, dst, dst_len);
#else 

        __asm__ volatile (
	        "leal  4+_liseg, %0\n\t"
	        "leal    _iseg,  %1\n\t"
                : "=a" (src) , "=b" (dst)
        );

#if CONFIG_USE_INIT		
	printk_debug("src=%08x\r\n",src); 
	printk_debug("dst=%08x\r\n",dst);
#else
        print_debug("src="); print_debug_hex32(src); print_debug("\r\n");
        print_debug("dst="); print_debug_hex32(dst); print_debug("\r\n");
#endif
	
//	dump_mem(src, src+0x100);

	unrv2b(src, dst);

#endif
//	dump_mem(dst, dst+0x100);
#if CONFIG_USE_INIT
	printk_debug("linxbios_ram.bin length = %08x\r\n", olen);
#else
	print_debug("linxbios_ram.bin length = "); print_debug_hex32(olen); print_debug("\r\n");
#endif
	print_debug("Jumping to LinuxBIOS.\r\n");

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
