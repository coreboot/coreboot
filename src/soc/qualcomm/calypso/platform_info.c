/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <console/console.h>
#include <soc/platform_info.h>

/* TCSR SoC HW version Register for platform identification */
#define TCSR_SOC_HW_VERSION 0x1FC8000

enum qclib_soc_id platform_get_soc_id(void)
{
	union tcsr_soc_hw_version version;

	version.data = read32((void *)TCSR_SOC_HW_VERSION);

	switch (version.device_number) {
	case TCSR_SOC_HW_VERSION_DEVICE_NUM_CALYPSO:
		return SOC_ID_CALYPSO;
	default:
		printk(BIOS_WARNING, "Unknown Qualcomm SoC device number: 0x%03x\n",
		       version.device_number);
		return SOC_ID_UNKNOWN;
	}
}
