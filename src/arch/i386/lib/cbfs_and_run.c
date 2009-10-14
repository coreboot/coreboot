/* by yhlu 6.2005
	moved from nrv2v.c and some lines from crt0.S
   2006/05/02 - stepan: move nrv2b to an extra file.
*/

#include <console/console.h>
#include <cbfs.h>

void cbfs_and_run_core(char *filename, unsigned ebp)
{
	u8 *dst;
	print_debug("Jumping to image.\r\n");
	dst = cbfs_load_stage(filename);
	if (dst == (void *) -1) {
		/* We should use die() here. */
		print_emerg("Loading stage failed!\n");
		for (;;);
	}
	print_debug("Jumping to image.\r\n");

	__asm__ volatile (
		"movl %%eax, %%ebp\n\t"
		"cli\n\t"
		"jmp     *%%edi\n\t"
		:: "a"(ebp), "D"(dst)
	);

}
