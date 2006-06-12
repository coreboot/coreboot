#include <arch/io.h>
#include <stdint.h>

#define VIDEO_MB	8					// MB of video memory

#define VRC_INDEX				0xAC1C	// Index register
#define VRC_DATA				0xAC1E	// Data register
#define VR_UNLOCK				0xFC53	// Virtual register unlock code
#define	VRC_VG					0x02	// SoftVG Class
#define	VG_MEM_SIZE				0x00	// bits 7:0 - 512K unit size, bit 8 controller priority


/*
 * Write to a Virtual Register
 * AX = Class/Index
 * CX = data to write
 */
static void vrWrite(uint16_t wClassIndex, uint16_t wData)
{
	outl(((uint32_t) VR_UNLOCK << 16) | wClassIndex, VRC_INDEX);
	outw(wData, VRC_DATA);
}


/*
 * This function mirrors the Graphics_Init routine in GeodeROM.
 */
void graphics_init(void)
{
	/* SoftVG initialization */

	/* Call SoftVG with the main configuration parameters. */
	/* NOTE: SoftVG expects the memory size to be given in 512 KB pages */
	vrWrite((VRC_VG <<  8) + VG_MEM_SIZE, 0x0100 | (VIDEO_MB * 2));
}


