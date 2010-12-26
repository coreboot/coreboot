#include <arch/io.h>
#include <stdint.h>
#include <cpu/amd/vr.h>
#include <device/device.h>
#include "chip.h"
#include "northbridge.h"

/* This function mirrors the Graphics_Init routine in GeodeROM. */
void graphics_init(void)
{
	/* SoftVG initialization */

	/* Call SoftVG with the main configuration parameters. */
	/* NOTE: SoftVG expects the memory size to be given in 512 KB pages */
	vrWrite((VRC_VG <<  8) + VG_MEM_SIZE, 0x0100 | (CONFIG_VIDEO_MB * 2));
}


