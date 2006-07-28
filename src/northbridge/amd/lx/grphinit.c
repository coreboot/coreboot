#include <arch/io.h>
#include <stdint.h>
#include <cpu/amd/vr.h>
 
#define VIDEO_MB	8					// MB of video memory

/*
 * Write to a Virtual Register
 * AX = Class/Index
 * CX = data to write
 */
void vrWrite(uint16_t wClassIndex, uint16_t wData)
{
	outl(((uint32_t) VR_UNLOCK << 16) | wClassIndex, VRC_INDEX);
	outw(wData, VRC_DATA);
}

 /*
 * Read from a Virtual Register
 * AX = Class/Index
 * Returns a 16-bit word of data
 */
uint16_t vrRead(uint16_t wClassIndex)
{
	uint16_t wData;
	outl(((uint32_t) VR_UNLOCK << 16) | wClassIndex, VRC_INDEX);
	wData = inw(VRC_DATA);
	return wData;
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


