/* main.c

   Taken from OpenBIOS 0.0.1/boot/boot32/main.c which was in turn taken
   from /usr/src/linux/arch/i386/boot/compressed/misc.c

   Used for start32, 1/11/2000
   James Hendricks, Dale Webster */


/* misc.c
 * 
 * This is a collection of several routines from gzip-1.0.3 
 * adapted for Linux.
 *
 * malloc by Hannu Savolainen 1993 and Matthias Urlichs 1994
 * puts by Nick Holloway 1993, better puts by Martin Mares 1995
 * High loaded stuff by Hans Lermen & Werner Almesberger, Feb. 1996
 */

#include <printk.h>
#include <part/fallback_boot.h>
#include <params.h>
#include <subr.h>
#include <rom/read_bytes.h>
#include <string.h>
#include <stdlib.h>

#if USE_ELF_BOOT
#include <boot/elf.h>
#endif

#include "do_inflate.h"

#ifdef USE_TFTP
extern void netboot_init(void);
extern int tftp_init(const char *name);
extern int tftp_fetchone(char *buffer);
#endif

int linuxbiosmain(unsigned long base, unsigned long totalram)
{
	unsigned char *empty_zero_page;

	unsigned char *cmd_line;
	unsigned long initrd_start, initrd_size;

#ifdef USE_TFTP
	char buffer[256];
	char *bufptr;
	int buflen;
#endif /* USE_TFTP */

#if USE_ELF_BOOT
	return elfboot();
#else /* !ELF_BOOT */
	printk_info("\n");
	printk_info("Welcome to start32, the open sourced starter.\n");
	printk_info("This space will eventually hold more diagnostic information.\n");
	printk_info("\n");
	printk_info("January 2000, James Hendricks, Dale Webster, and Ron Minnich.\n");
	printk_info("Version 0.1\n");
	printk_info("\n");

	initrd_start = 0;
	initrd_size  = 0;
#ifdef CMD_LINE
	cmd_line = CMD_LINE;
#else
	cmd_line = "root=/dev/hda1 single";
#endif /* CMD_LINE */

#ifdef LOADER_SETUP
	loader_setup(base,
		     totalram,
		     &initrd_start,
		     &initrd_size,
		     &cmd_line,
		     &zkernel_start,
		     &zkernel_mask);
#endif /* LOADER_SETUP */


	post_code(0xf1);

#ifdef PYRO_TEST1
	printk_notice( "LiLa loader, press a key to test netboot_init:");
	buflen = sizeof(buffer);
	ttys0_rx_line(buffer, &buflen);
#endif

#ifdef USE_TFTP
	netboot_init();
	printk_notice("\nnetboot_init test complete, all is well (I hope!)\n");
#endif /* USE_TFTP */
		
	streams->init();
	printk_debug("Gunzip setup\n");
	gunzip_setup();
	printk_debug("Gunzipping boot code\n");
	if (gunzip() != 0) {
		printk_notice("gunzip failed\n");
		post_code(0xff);
		return 0;
	}
	streams->fini();
	post_code(0xf8);

#ifdef TFTP_INITRD
	printk_notice("Loading initrd now\n");

	buflen = tftp_init("initrd");
	printk_notice("TFTP init complete (%d)\n",buflen);
	buflen = 512;	// I know, not it's purpose, 
				// but it isn't being used at this point.
	bufptr = (char *) initrd_start = 0x0400000;
	while (buflen == 512) {
		buflen = tftp_fetchone(bufptr);
#ifdef DEBUG_TFTP
		printk_spew("Got block, bufptr = %lu, size= %u\n",bufptr, buflen);
#endif
		bufptr += buflen;
	}
 	initrd_size = (unsigned long) bufptr - initrd_start;

	printk_notice("Initrd loaded\n");

	if(tftp_init("cmdline") >=0) {
                buflen = tftp_fetchone(buffer);
                buffer[buflen] = '\0';
		cmd_line=buffer;
	}

	printk_notice("Booting with command line: %s\n",cmd_line);


#endif /* TFTP_INITRD */

	/* parameter passing to linux. You have to get the pointer to the
	 * empty_zero_page, then fill it in. 
	 */
	/* should we do this in intel_main(). If we did we would have to save it
	 * maybe -- 0x90000 might get stomped. We do it here as the last step.
	 */
	/* yes I know we need prototypes -- we'll do it soon */
	empty_zero_page = get_empty_zero_page();
	init_params(empty_zero_page);
	post_code(0xf9);

	/* the ram address should be the last mbyte, AFAIK. Subtract one for the
	 * low 1 MB. So subtract 2K total 
	 */
	set_memory_size(empty_zero_page, 0x3c00, totalram - 2048);
	post_code(0xfa);

	printk_notice("command line - [%s]\n", cmd_line);

	set_command_line(empty_zero_page, cmd_line);
	set_root_rdonly(empty_zero_page);
	set_display(empty_zero_page, 25, 80);
	set_initrd(empty_zero_page, initrd_start, initrd_size);

	/* Reset to booting from this image as late as possible */
	boot_successful();

	printk_debug("Jumping to boot code\n");
	post_code(0xfe);

	/* there seems to be a bug in gas? it's generating wrong bit-patterns ...
	   typedef void (*kernel) ();
	   kernel v;
	   v = (kernel)KERNEL_START;
	   v();
	*/
	/* move 0x90000 to into esi (This is the address of the linux parameter page)
	 * Linux then copies this page into it's ``empty_zero_page'' so it isn't
	 * stomped while things are being setup.
	 * Later the ``empty_zero_page'' is zeroed and used fulfill read demands
	 * on memory mappings of file holes and the like.
	 * As of 2.4.0-test4 the linux parameter page isn't hardwired to be
	 * at 0x90000 anymore.
	 */
	/* move 0 to ebx. This is for SMP support. Jump to kernel */
	__asm__ __volatile__("movl $0x90000, %%esi\n\t"
			     "movl $0, %%ebx\n\t"
			     "ljmp $0x10, %0\n\t"
			     :: "i" (0x100000));

	return 0;		/* It should not ever return */
#endif /* ELF_BOOT */
}
