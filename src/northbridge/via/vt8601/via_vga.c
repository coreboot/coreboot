/*
 *
 * By 
 * Steve M. Gehlbach <steve @ kesa . com>
 * 
 * vga initialization specific for 
 * via vt8601 chipset
 *
 * The font load code follows technique used
 * in the tiara project, which came from
 * the Universal Talkware Boot Loader,
 * http://www.talkware.net.
 */

#include <video_subr.h>
#include <subr.h>
#include <string.h>
#include <pc80/vga.h>
#include <cpu/p5/io.h>
#include <subr.h>
#include <printk.h>

#define VGA_FONT_BASE 0xa0000;
#define VGA_GRAFIX_BASE 0xa0000;
#define CHAR_HEIGHT 16

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

extern unsigned char fontdata_8x16[];
extern void beep (int msec);
extern void udelay (int usec);

// The screeninfo structure is in pc80/vga_load_regs.c and has the vga
// parameters for screen size etc.
// This is _not_ the struct used in the zero_page
// for linux init.  It is only used for vga init.
extern struct screeninfo vga_settings;

// prototypes
int vga_decode_var(struct screeninfo *var, struct vga_par *par);
int vga_set_regs(struct vga_par *par);


void vga_set_amode(void);
void vga_set_gmode(void);
void delay(int secs);
void mdelay(int msecs);
void vga_font_load(unsigned char *vidmem, unsigned char *font, int height, int num_chars);
int vga_load_pcx( char * pcx_file, int pcx_file_length);

void vt8601_video_init(void) {
	int res;
	u8 byte;
	struct vga_par vga_params;


	// convert the general vga parameters in screeninfo structure 
	// to actual vga register settings

	res = vga_decode_var(&vga_settings, &vga_params);
	if ( res < 0 ) { post_code (0xFD); } //no error return for now

	// enable vga system
	outb(0x01, 0x3c3); // enable VGA

	// write the registers
	res = vga_set_regs( &vga_params );
	if ( res < 0 ) { post_code(0xFE); } //no error return for now
	byte = inb(MIS_R); // get 3c2 value by reading 3cc
	outb(byte & ~0xc,MIS_W); // clear last bits to set 25Mhz clock

	// enable epia extended regs
	write_seq(0x92,0x11);
	
	// setup the video clocks
	// -follows award settings
	// not all of these are necessary
	write_seq(0xbd,0x18);
	write_seq(0xcc,0x19);
	write_seq(0xff,0x1a);
	write_seq(0xff,0x1b);
	write_seq(0x46,0x1c);
	write_seq(0xbf,0x1d);
	write_seq(0xff,0x1e);
	write_seq(0xcc,0x1f);
	write_seq(0x04,0x20);
	write_seq(0x4f,0x24);

	// setup extended crtc regs
	// -follows award settings
	// not all of these are necessary
	write_crtc(0x64,0x1f);
	write_crtc(0x20,0x20);
	write_crtc(0x0,0x21);
	write_crtc(0x7,0x25);
	write_crtc(0x4,0x29);
	write_crtc(0x1f,0x2a);
	write_crtc(0x0,0x2b);
	write_crtc(0xdf,0x2f);
	write_crtc(0x10,0x38);


}

#ifdef VGA_HARDWARE_FIXUP
void vga_hardware_fixup(void) {
	u8 *font_mem, *vga_mem, *pcx_file;
	int *file_size;

#ifdef PCX_FILE_LOCATION
	pcx_file = (u8 *) PCX_FILE_LOCATION;
#else
	pcx_file = (u8 *) 0xfffe0000;
#endif
	file_size = (int *) pcx_file;

	vga_mem = (u8 *) VGA_GRAFIX_BASE;
	font_mem = (u8 *) VGA_FONT_BASE;

	outb(0x01, 0x3b8); // enable VGA
	outb(0x01, 0x3c3); // enable VGA
	outb(0x08, 0x46e8); // enable VGA (does not appear to be used)

	if (inb(0x3c3) != 1) {
	    	printk_info("VGA not ready yet.\n");
		return;
	}
	printk_info("Initializing vt8601 vga...");
	post_code(0xa0);

	vt8601_video_init();

	printk_info("done.\n");

	if (!splash_done) {
		printk_debug("Setting graphics mode...\n");
		vga_set_gmode(); // set graphics mode

#ifdef VIDEO_SHOW_LOGO
		//
		// the pcx_file is in flash at an address set 
		// in the config file with PCX_FILE_LOCATION
		// the length of the file is at offset 0, file starts at 4
		//

		printk_debug("pcx file at %x length %d\n",&pcx_file[4], *file_size);
		vga_load_pcx( &pcx_file[4], *file_size);
		delay(VIDEO_SHOW_LOGO);
	
#endif
		vga_set_amode();
		vga_font_load(font_mem,fontdata_8x16,CHAR_HEIGHT,256);
		splash_done++; // mark done
		printk_debug("alpha mode set.\n");
		post_code(0xa1);
	}
}
#endif
