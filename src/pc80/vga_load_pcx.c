/***********************************************************
 *
 * $Id$
 * $Source$
 *
 * load a splash image from a pcx file
 * the pcs file is stored in flash at a known address.
 * VGA must be in gmode prior to entering this routine.
 * There are some assumptions made about the pcx image
 * and the color table. See the code.
 *
 * by 
 * Steve M. Gehlbach <steve@kesa.com>
 *
 * References: 
 * PCX file spec:
 *       http://www.whisqu.se/per/docs/graphics57.htm
 *
 * pcx row decode function modified from the netpbm project
 * http://download.sourceforge.net/netpbm/
 *
 ***********************************************************/
#include <pc80/vga.h>
#include <printk.h>

#define SET_PLANE(x) write_seq(x,SEQ_PLANE_WRITE);
#define VGA_GRAFIX_BASE 0xa0000
u8 *vga = (u8 *) VGA_GRAFIX_BASE;
#define PCX_256_COLORS 0x0c

// prototypes
static void GetPCXRow(unsigned char * pcx_file, unsigned char * const pcxrow, int const bytesperline, int * file_index);
void writeVga(u8 *pcxrow);

// globals
static int rowsRead = 0;
static int xsize = 0;
static int ysize = 0;
static u8 p_offset;

struct pcx_header {
	u8 manufacturer;
	u8 version;
	u8 encoding;
	u8 bits_per_pixel;
	u16 xmin; u16 ymin; u16 xmax; u16 ymax;
	u16 HDpi;
	u16 VDpi;
	struct color {u8 red; 
			u8 blue;
			u8 green;
			} colormap[16];
	u8 reserved;
	u8 nplanes;
	u16 bytes_per_line;
	u16 palette_info;
	u16 HscreenSize;
	u16 VscreenSize;
	u8 filler[54];
};

int vga_load_pcx (char * pcx_file, int size) {
	int total_bytes_per_line,file_index,row,i;
	u8 vgarow[650];
	struct pcx_header *pcx;

/////////////////////////////////////////////////////////////////////
//  
//  Image must be 1 plane, 8 bits per pixel, run length encoding, 
//  256 colors with only 16 colors of the 256 used.  The file is 
//  assumed to be a version 5 type file with the 256 
//  color palette at the file end.  Only 4 bit planes
//  are used, however, so the image must be generated with
//  16 colors, so that the palette uses only 16
//  entries.  The image can use either the bottom or
//  top of the palette (the code checks the
//  bottom then the top of 256 entry palette).  
//  Gimp puts the 16 entry palette at the beginning of the 256 color
//  palette, and PhotoShop puts the 16 colors at the end of the palette.
//  Both are supported.  To generate a compatible pcx file in gimp,
//  right click on the image and select image->mode->indexed, 
//  change #colors to 16, and make sure "Generate Optimal Palette"
//  and appropriate dithering is selected (Floyd Steinberg Normal
//  is the default and seems to work).  Click Okay, then right
//  click and select File->Save As and select PCX as the File
//  Type and save the file with a pcx extension.
//  This routine supports variable width and height but the default
//  for linuxbios is 640 x 400 pixels.
//
/////////////////////////////////////////////////////////////////////

	pcx = (struct pcx_header *) pcx_file;
	if ( ! (pcx->manufacturer == 0x0a && pcx->version == 0x05 && pcx->encoding == 0x01) ) {
		printk_info("vga_load_pcx: not a compatible .pcx file.\n");
		return -1;
	}
	if (pcx->bits_per_pixel != 8 || pcx->nplanes != 1) {
		printk_info("vga_load_pcx: not a 1 plane 8-bits per pixel .pcx file. planes= %d; bits-per-pixel= %d\n",
				pcx->nplanes,pcx->bits_per_pixel);
		return -1;
	}
	xsize= pcx->xmax - pcx->xmin + 1;
	ysize= pcx->ymax - pcx->ymin + 1;
	total_bytes_per_line = pcx->nplanes * pcx->bytes_per_line;
	printk_debug("vga_load_pcx: bits_per_pixel= %d \n xmin= %d, xmax= %d, ymin= %d, \
 ymax= %d\n bytes_per_line= %d nplanes= %d\n image size (bytes)= %d\n",
			pcx->bits_per_pixel,
			pcx->xmin,
			pcx->xmax,
			pcx->ymin,
			pcx->ymax,
			pcx->bytes_per_line,
			pcx->nplanes,
			total_bytes_per_line*ysize		
			);

	// Read the color table at the end of the file.
	// Check the beginning of the palette to see if it
	// is non-zero.  Default to the end of the palette
	// if beginning is zero.  We only use 16 colors.

	file_index = size-769;
	// check palette signature
	if (pcx_file[file_index++] != PCX_256_COLORS) {
		// palette no good, bail out
		printk_info("vga_load_pcx: invalid color table signature at file_index %d: 0x%x\n",
				file_index-1,pcx_file[file_index-1]);
				return -1;
		}
	
	// add up 2 3-tuples at beginning of palette and see if the result == 0; if so,
	// then use top 16 entries.  If not, then use 16 entries at beginning of palette.
	p_offset = 0;
	for (i = 0; i<6; i++) p_offset += pcx_file[file_index + i];
	if (p_offset == 0) p_offset = 240;
	else p_offset = 0;

	file_index += p_offset*3; 
	i = 0;
	outb_p(240, PEL_IW);
	do {
		// put the 16 3-tuple color table at the upper end of the PEL
		// this avoids conflicting with standard palette at lower end
		// since the upper end is not used in this legacy mode
		// scale 0-255 -> 0-63 (6 bit DAC)
		outb_p(pcx_file[file_index++]>>2, PEL_D);
		outb_p(pcx_file[file_index++]>>2, PEL_D);
		outb_p(pcx_file[file_index++]>>2, PEL_D);
		i++;
	} while ( (i < 16) && (file_index < (size-2)) );
	if (i < 16)
		printk_info("vga_load_pcx: Seek past end of file in loading colortable. Only %d colors loaded.  file_index= %d size= %d\n",i,file_index,size);
	else 
		printk_debug("vga_load_pcx: %d colors read into colortable.\n",i);

	// set the color select to set b7-4 to 1111 to use high end of PEL
	// this avoids conflicting with standard palette at lower end
	write_att(read_att_b(ATC_MODE)|0x80,ATC_MODE);
	write_att(0x0f,ATC_COLOR_PAGE);
	
	// go through the pcx file and put it into one row of 1 byte per pixel
	// then call the write VGA routine to write this to vga memory
	// do this for all rows up to ysize
	file_index = 128;
	// now get the image data and put into vga memory
	for( row = 0; row < ysize; row++ ) {
		GetPCXRow(pcx_file,vgarow,pcx->bytes_per_line, &file_index);
		//printk_debug("Row %d complete; at pcx_file[%d].\n",rowsRead, file_index);
		writeVga(vgarow);
	}
	printk_debug("Row %d complete; at pcx_file[%d].\n",rowsRead, file_index);

// turn on display, disable access to attr palette
	inb(IS1_RC);
	outb(0x20, ATT_IW);
	return 0;

}

// decode pcx format
/*
 * from netpbm
 * read a single encoded row, handles encoding across rows
 * run length encoding per http://www.whisqu.se/per/docs/graphics57.htm
 *
 */
static void
GetPCXRow(unsigned char * pcx_file, unsigned char * const pcxrow, 
	int const bytesperline, int * file_index) {

/*----------------------------------------------------------------------------
 *	Read one row from the PCX raster.
 *       
 *	The PCX raster is run length encoded as follows:  If the upper two
 *	bits of a byte are 11, the lower 6 bits are a repetition count for the
 *	raster byte that follows.  If the upper two bits are not 11, the byte 
 *	_is_ a raster byte, with repetition count 1.
 *
 *	A run can't span rows, but it can span planes within a row.  That's
 *	why 'repetitionsLeft' and 'c' are static variables in this
 *	subroutine.
 *-----------------------------------------------------------------------------*/
	static int repetitionsLeft = 0;
	static int c;
	int bytesGenerated;

	bytesGenerated = 0;
	while( bytesGenerated < bytesperline ) {
		if(repetitionsLeft > 0) {
			pcxrow[bytesGenerated++] = c;
			--repetitionsLeft;
		} else {
			c = pcx_file[(*file_index)++];
			if ((c & 0xc0) != 0xc0)
/* This is a 1-shot byte, not a repetition count */
				pcxrow[bytesGenerated++] = c;
			else {
/* This is a repetition count for the following byte */
				repetitionsLeft = c & 0x3f;
				c = pcx_file[(*file_index)++];
			}
		}
	}
	rowsRead++;
}
/*---------------------------------------------------------------------------------
 * write one row of the vga raster to the memory
 * the vga memory is 1 bit per pixel and the pcxrow 
 * is 1 pixel per byte.  The vga memory is organized
 * into planes for each bit (4 planes).
 *
 *--------------------------------------------------------------------------------*/

void writeVga(u8 *pcxrow) {
	int i, j, offset;
	u8 pixel[4] = {0,0,0,0};
	u8 mask, s_pixel;

	// offset is less one since we process 8 bytes
	// before writing the first vga byte.  This way we
	// don't have to subtract one in the loop below.
	offset = (rowsRead-1) * (xsize>>3) - 1;

	for (i=0;i<=xsize;i++) {
		if ( (i != 0) && (i % 8 == 0)) {
			// write each plane
			for (j=0;j<4;j++) {
				// set plane j
				SET_PLANE(1<<j);
				
				// write byte to plane
				vga[ ( i>>3 ) + offset] = pixel[j];

				// clear the value
				pixel[j] = 0;
			}
			if (i >= xsize) return;
		}
	// if we are using upper end of palette, then
	// subtract 240 to scale value to 0-15
		s_pixel = pcxrow[i] - p_offset;
		mask = 0x80 >> (i % 8);
		for (j=0;j<4;j++)
			if (s_pixel & (1<<j) )
				pixel[j] |= mask;
			else 
				continue;
	}
}
