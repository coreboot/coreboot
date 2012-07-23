/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <boot/tables.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <arch/acpi.h>
#include "BiosCallOuts.h"
#include <cpu/amd/agesa/s3_resume.h>
#include "chip.h"
#include "agesawrapper.h"

/*************************************************
 * enable the dedicated function in parmer board.
 *************************************************/
static void parmer_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");
	/*
	 * The mainboard is the first place that we get control in ramstage. Check
	 * for S3 resume and call the approriate AGESA/CIMx resume functions.
	 */
#if CONFIG_HAVE_ACPI_RESUME == 1
	acpi_slp_type = acpi_get_sleep_type();
	if (acpi_slp_type == 3)
		agesawrapper_fchs3earlyrestore();

#endif
	setup_uma_memory();
}

int add_mainboard_resources(struct lb_memory *mem)
{
	/* UMA is removed from system memory in the northbridge code, but
	 * in some circumstances we want the memory mentioned as reserved.
	 */
	/* TODO: Check out why it was commented. */
#if CONFIG_GFXUMA
	printk(BIOS_INFO, "uma_memory_start=0x%llx, uma_memory_size=0x%llx \n",
	       uma_memory_base, uma_memory_size);
	lb_add_memory_range(mem, LB_MEM_RESERVED, uma_memory_base,
			    uma_memory_size);
#endif
	return 0;
}
struct chip_operations mainboard_ops = {
	CHIP_NAME(CONFIG_MAINBOARD_VENDOR " " CONFIG_MAINBOARD_PART_NUMBER " Mainboard")
	.enable_dev = parmer_enable,
};
