/*
*
* Copyright (C) 2007 Advanced Micro Devices
*
*/

#include <arch/io.h>
#include <stdint.h>
#include <cpu/amd/vr.h>
#include <console/console.h>


 /*
 * This function mirrors the Graphics_Init routine in GeodeROM.
 */
void graphics_init(void)
{
	uint16_t wClassIndex, wData, res;
	
	/* SoftVG initialization */
	printk_debug("Graphics init...\n");

	/* Call SoftVG with the main configuration parameters. */
	/* NOTE: SoftVG expects the memory size to be given in 2MB blocks */
	
	wClassIndex = (VRC_VG <<  8) + VG_CONFIG;
	
	/*
	 * Graphics Driver Enabled (13) 			0, NO (lets BIOS controls the GP)
	 * External Monochrome Card Support(12)		0, NO
	 * Controller Priority Select(11)			1, Primary
	 * Display Select(10:8)						0x0, CRT
	 * Graphics Memory Size(7:1)				CONFIG_VIDEO_MB >> 1,
	 *											defined in mainboard/../Options.lb
	 * PLL Reference Clock Bypass(0)			0, Default
	 */

	/* Video RAM has to be given in 2MB chunks
	 *   the value is read @ 7:1 (value in 7:0 looks like /2)
	 *   so we can add the real value in megabytes
	 */
	 
	wData =  VG_CFG_DRIVER | VG_CFG_PRIORITY | VG_CFG_DSCRT | (CONFIG_VIDEO_MB & VG_MEM_MASK);
	vrWrite(wClassIndex, wData);
	
	res = vrRead(wClassIndex);
	printk_debug("VRC_VG value: 0x%04x\n", res);
}


