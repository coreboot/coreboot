/*
 *
 * By
 * Dmitry Borisov( jbors@mail.sourceforge.net
 *
 */

#include <video_subr.h>
#include <subr.h>
#include <string.h>
#include <pc80/vga.h>
#include <cpu/p5/io.h>
#include <subr.h>
#include <printk.h>

// splash_done is a global to avoid putting up splash screen twice.
// Kind of a hack, but the problem is that the vga pci resources
// are sometimes ready early, sort of, and the initial call to
// vga_hardware_fixup puts up the splash screen; then a later call
// to it from hardware_main does it again; but this does not always
// happen, sometimes it fails the first call.  It is either a timing or initialization
// problem that needs to be tracked down and fixed. Note that both calls (fixup) are necessary
// since some vga boards are not ready early, but some are, and of course, the epia is sometimes ready
// and sometimes not ready.
//
int splash_done = 0;

void delay(int secs);
int vga_decode_var(struct screeninfo *var, struct vga_par *par);
int vga_load_pcx( char * pcx_file, int pcx_file_length);
int vga_set_regs(struct vga_par *par);
extern struct screeninfo vga_settings;
void vga_set_gmode(void);

#ifdef VGA_HARDWARE_FIXUP

void vga_hardware_fixup(void)
{
	u8 *pcx_file;
	int *file_size;
	int res;
	struct vga_par vga_params;

	// convert the general vga parameters in screeninfo structure
	// to actual vga register settings
	res = vga_decode_var(&vga_settings, &vga_params);
	if ( res < 0 ) { post_code (0xFD); } //no error return for now

	// write the registers
	res = vga_set_regs( &vga_params );

#ifdef PCX_FILE_LOCATION
	pcx_file = (u8 *) PCX_FILE_LOCATION;
#else
	pcx_file = (u8 *) 0xfffd0000;
#endif
	file_size = (int *) pcx_file;

	if (!splash_done) {
		printk_debug("Setting graphics mode...\n");
		//setmode();
		vga_set_gmode();

#ifdef VIDEO_SHOW_LOGO
		//
		// the pcx_file is in flash at an address set
		// in the config file with PCX_FILE_LOCATION
		// the length of the file is at offset 0, file starts at 4
		//

		printk_debug("pcx file at %x length %d\n",&pcx_file[4], *file_size);
		vga_load_pcx( &pcx_file[4], *file_size);
#endif
		splash_done++; // mark done
	}
}
#endif
