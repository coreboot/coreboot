/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <console/console.h>
#include <soc/platform_info.h>

/* TCSR SoC HW version Register for platform identification */
#define TCSR_SOC_HW_VERSION		0x1FC8000

uint32_t platform_get_soc_hw_id(void)
{
	union tcsr_soc_hw_version version;

	version.data = read32((void *)TCSR_SOC_HW_VERSION);
	return version.device_number;
}

enum qclib_soc_id platform_get_soc_id(void)
{
	uint32_t dev_num = platform_get_soc_hw_id();

	switch (dev_num) {
	case TCSR_SOC_HW_VERSION_DEVICE_NUM_HAMOA:
		return SOC_ID_HAMOA;
	case TCSR_SOC_HW_VERSION_DEVICE_NUM_X1P42100:
		return SOC_ID_X1P42100;
	default:
		printk(BIOS_WARNING, "Unknown Qualcomm SoC device number: 0x%03x\n",
		       dev_num);
		return SOC_ID_UNKNOWN;
	}
}
