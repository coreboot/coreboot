/*
 *
 * By 
 * Steve M. Gehlbach <steve@kesa.com>
 *
 * vga initialization specific for 
 * stpc consumerII
 *
 */

#include <video_subr.h>
#include <string.h>
#include <pc80/vga.h>
#include <cpu/stpc/consumer2/vga_stpc.h>
#include <cpu/p5/io.h>
#include <subr.h>
#include <printk.h>

//#define VID_M_BUFFER 0x8800000;
#define CHAR_HEIGHT 16
#define VGA_FONT_BASE 0xa0000;
#define VGA_GRAFIX_BASE 0xa0000;


extern unsigned char fontdata_8x16[];

// screeninfo is in pc80/vga_load_regs.c and has the vga
// parameters for screen size etc.  Change vga horiz,vert there.
// This is _not_ the structure used in the zero_page
// for linux init.
extern struct screeninfo vga_settings;

// prototypes
int vga_decode_var(struct screeninfo *var, struct vga_par *par);
int vga_set_regs(struct vga_par *par);
void vga_font_load(unsigned char *vidmem, unsigned char *font, int height, int num_chars);

void stpc_vga_init(void) {
	int res;
	struct vga_par vga_params;

	// convert the general vga parameters in screeninfo structure 
	// to actual vga register settings

	res = vga_decode_var(&vga_settings, &vga_params);
	if ( res < 0 ) { post_code (0xFD); } //no error return for now

	// enable access to vga registers
	// STPC specific settings
	outb(0x0, 0x94);  // enable access to port 0x102
	outb(0x01, 0x102); // enable VGA
	outb(0x01, 0x3c3); // enable VGA
	outb(0x28, 0x94);  // disable access to port 0x102

	// extended access STPC GE registers
	// set seq reg 6 to 0x57 to unlock these
	outb(6, SEQ_I);
	outb(0x57, SEQ_D);

	// write the registers
	res = vga_set_regs( &vga_params );
	if ( res < 0 ) { post_code(0xFE); } //no error return for now

	// very important to get access to the FB
	write_crtc(0x80, CRTC_GE_ENABLE); // en ext graphics AND enable access to FB

	write_crtc(0x0, CRTC_DISP_YOFFS_L); //set scan offset
	write_crtc(0x0, CRTC_DISP_YOFFS_H);

	write_crtc(0x0, CRTC_REPAINT_C4); // high bits of CRTC counter regs

	write_crtc(0x3e, CRTC_REPAINT_C1); // compatible text, b0 MUST BE 0 or reads dont work in mode 3

	write_crtc(0x0, CRTC_PAGE_R0);
	write_crtc(0x0, CRTC_PAGE_R1);

	write_crtc(0xff, CRTC_GE_APER); // ext aperture

	write_crtc(0x0, CRTC_REPAINT_C3); // Page select, Seq C4 off
	write_crtc(0xff, CRTC_URGENT_START); // always urgent setting
	write_crtc(0x0, CRTC_REPAINT_C0); // upper bits of CRTC regs

	write_crtc (0x10, CRTC_PALLETTE_C); // set LUT on
	write_crtc(0x0, CRTC_GE_GBASE); // GBASE address (default 0x800000)

}

extern void vga_set_amode(void);
extern void vga_set_gmode(void);
extern void delay(int secs);
extern void mdelay(int msecs);
extern int vga_load_pcx( char * pcx_file, int pcx_file_length);

#ifdef VGA_HARDWARE_FIXUP
void vga_hardware_fixup(void) {
	unsigned char* font_mem, *vga_mem, *pcx_file;
	int *file_size;
	vga_mem = (u8 *) VGA_GRAFIX_BASE;
	font_mem = (u8 *) VGA_FONT_BASE;
#ifdef PCX_FILE_LOCATION
	pcx_file = (u8 *) PCX_FILE_LOCATION;
#else
	pcx_file = (u8 *) 0xfffe0000;
#endif
	file_size = (int *) pcx_file;

	printk_info("Initializing stpc vga...");
	post_code(0xa0);

	stpc_vga_init();

#ifdef VIDEO_SHOW_LOGO
	// mdelay(500);
	printk_debug("Setting graphics mode...\n");
	vga_set_gmode(); // set graphics mode
//
// the pcx_file is in flash at an address set 
// in the config file with PCX_FILE_LOCATION
// the length of the file is at offset 0, file start at 4
//
	printk_debug("pcx file at %x length %d\n",&pcx_file[4], *file_size);
	vga_load_pcx( &pcx_file[4], *file_size);
	delay(VIDEO_SHOW_LOGO);
#endif

	vga_set_amode();
	printk_debug("alpha mode set.\n");

	vga_font_load(font_mem,fontdata_8x16,CHAR_HEIGHT,256);

	post_code(0xa1);
}
#endif
