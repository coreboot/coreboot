/* by yhlu 6.2005
	moved from nrv2v.c and some lines from crt0.S
   2006/05/02 - stepan: move nrv2b to an extra file.
*/

#include <console/console.h>
#include <stdint.h>
#include <string.h>

#if CONFIG_COMPRESS
#define ENDIAN   0
#define BITSIZE 32
#include "../lib/nrv2b.c"
#endif

void copy_and_run_core(u8 *src, u8 *dst, unsigned long ilen, unsigned ebp)
{
        unsigned long olen;

#if CONFIG_USE_INIT
	printk_spew("src=%08x\r\n",src);
	printk_spew("dst=%08x\r\n",dst);
#else
        print_spew("src="); print_spew_hex32((uint32_t)src); print_spew("\r\n");
        print_spew("dst="); print_spew_hex32((uint32_t)dst); print_spew("\r\n");
#endif

#if !CONFIG_COMPRESS
	print_debug("Copying image to RAM.\r\n");
	memcpy(src, dst, ilen);
	olen = ilen;
#else
	print_debug("Uncompressing image to RAM.\r\n");
//	dump_mem(src, src+0x100);
	olen = unrv2b(src, dst, &ilen);
#endif

//	dump_mem(dst, dst+0x100);
#if CONFIG_USE_INIT
	printk_spew("image length = %08x\r\n", olen);
#else
	print_spew("image length = "); print_spew_hex32(olen); print_spew("\r\n");
#endif
	print_debug("Jumping to image.\r\n");

	__asm__ volatile (
		"movl %0, %%ebp\n\t"
		"cli\n\t"
		"leal    _iseg, %%edi\n\t"
		"jmp     *%%edi\n\t"
		:: "a"(ebp)
	);

}
