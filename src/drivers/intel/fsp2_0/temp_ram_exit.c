/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/romstage.h>
#include <cbfs.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <mode_switch.h>
#include <types.h>

static void fsp_temp_ram_exit(void)
{
	struct fsp_header hdr;
	uint32_t status;
	temp_ram_exit_fn  temp_ram_exit;
	void *mapping;
	size_t size;
	const char *name = CONFIG_FSP_M_CBFS;

	mapping = cbfs_map(name, &size);
	if (!mapping) {
		printk(BIOS_CRIT, "Could not map %s from CBFS\n", name);
		die("FSPM not available for CAR Exit!\n");
	}

	if (fsp_validate_component(&hdr, mapping, size) != CB_SUCCESS)
		die("Invalid FSPM header!\n");

	temp_ram_exit = (void *)(uintptr_t)(hdr.image_base + hdr.temp_ram_exit_entry_offset);
	printk(BIOS_DEBUG, "Calling TempRamExit: %p\n", temp_ram_exit);
	if (ENV_X86_64 && CONFIG(PLATFORM_USES_FSP2_X86_32))
		status = protected_mode_call_1arg(temp_ram_exit, 0);
	else
		status = temp_ram_exit(NULL);

	if (status != FSP_SUCCESS)
		die("TempRamExit returned with error 0x%08x!\n", status);

	cbfs_unmap(mapping);
}

void late_car_teardown(void)
{
	fsp_temp_ram_exit();
}
