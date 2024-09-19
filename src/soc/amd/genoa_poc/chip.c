/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpi.h>
#include <amdblocks/data_fabric.h>
#include <device/device.h>
#include <soc/southbridge.h>
#include <soc/southbridge.h>
#include <vendorcode/amd/opensil/opensil.h>

static void soc_init(void *chip_info)
{
	default_dev_ops_root.write_acpi_tables = soc_acpi_write_tables;

	setup_opensil();
	opensil_xSIM_timepoint_1();

	data_fabric_print_mmio_conf();

	fch_init(chip_info);
}

static void soc_final(void *chip_info)
{
}

struct chip_operations soc_amd_genoa_poc_ops = {
	.name = "AMD Genoa SoC Proof of Concept",
	.init = soc_init,
	.final = soc_final,
};
