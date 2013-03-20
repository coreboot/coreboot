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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include "BiosCallOuts.h"
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <arch/acpi.h>
#include <cpu/amd/agesa/s3_resume.h>
#include "agesawrapper.h"

/*************************************************
 * enable the dedicated function in thatcher board.
 *************************************************/
static void mainboard_enable(device_t dev)
{
	msr_t msr;

	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	msr = rdmsr(0xC0011020);
	msr.lo &= ~(1 << 28);
	wrmsr(0xC0011020, msr);

	msr = rdmsr(0xC0011022);
	msr.lo &= ~(1 << 4);
	msr.lo &= ~(1 << 13);
	wrmsr(0xC0011022, msr);

	msr = rdmsr(0xC0011023);
	msr.lo &= ~(1 << 23);
	wrmsr(0xC0011023, msr);

	/*
	 * The mainboard is the first place that we get control in ramstage. Check
	 * for S3 resume and call the approriate AGESA/CIMx resume functions.
	 */
#if CONFIG_HAVE_ACPI_RESUME
	acpi_slp_type = acpi_get_sleep_type();
	if (acpi_slp_type == 3)
		agesawrapper_fchs3earlyrestore();

#endif
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
