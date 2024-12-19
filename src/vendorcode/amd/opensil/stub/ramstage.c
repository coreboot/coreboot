/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/device.h>

#include "../opensil.h"

void opensil_get_hole_info(uint32_t *n_holes, uint64_t *top_of_mem, void **hole_info)
{
	*n_holes = 0;
	*top_of_mem = 0xc0000000;
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
}

const char *opensil_get_hole_info_type(uint32_t type)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
	return "";
}

void opensil_fill_fadt(acpi_fadt_t *fadt)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
}

unsigned long add_opensil_acpi_table(unsigned long current, acpi_rsdp_t *rsdp)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
	return current;
}

void setup_opensil(void)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
}

void opensil_xSIM_timepoint_1(void)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
}

void opensil_xSIM_timepoint_2(void)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
}

void opensil_xSIM_timepoint_3(void)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
}
