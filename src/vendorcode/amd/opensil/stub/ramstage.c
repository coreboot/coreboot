/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <device/device.h>
#include "../opensil.h"

void add_opensil_memmap(struct device *dev, unsigned long *idx)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
}

void opensil_fill_fadt_io_ports(acpi_fadt_t *fadt)
{
	printk(BIOS_NOTICE, "openSIL stub: %s\n", __func__);
}

unsigned long acpi_add_opensil_tables(unsigned long current, acpi_rsdp_t *rsdp)
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
