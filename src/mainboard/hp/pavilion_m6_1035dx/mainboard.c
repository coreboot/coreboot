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

#include "agesawrapper.h"
#include "BiosCallOuts.h"
#include "ec.h"

#include <arch/acpi.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/agesa/s3_resume.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>

#include <southbridge/amd/agesa/hudson/smi.h>


static void pavilion_cold_boot_init(void)
{
	/* Lid SMI is only used in non-ACPI mode; leave it off in S3 resume */
	hudson_configure_gevent_smi(EC_LID_GEVENT, SMI_MODE_SMI, SMI_LVL_LOW);
	/* EC is not powered off during S3 sleep */
	pavilion_m6_1035dx_ec_init();
}

static void mainboard_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	hudson_configure_gevent_smi(EC_SMI_GEVENT, SMI_MODE_SMI, SMI_LVL_HIGH);
	hudson_enable_smi_generation();
	/*
	 * The mainboard is the first place that we get control in ramstage. Check
	 * for S3 resume and call the approriate AGESA/CIMx resume functions.
	 */
#if CONFIG_HAVE_ACPI_RESUME
	acpi_slp_type = acpi_get_sleep_type();
	if (acpi_slp_type == 3)
		agesawrapper_fchs3earlyrestore();
	else
#endif
		pavilion_cold_boot_init();

}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
