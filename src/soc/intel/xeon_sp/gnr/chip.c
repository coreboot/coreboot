/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/lpc_lib.h>
#include <intelblocks/pmclib.h>
#include <soc/pm.h>
#include <soc/chip_common.h>
#include <soc/numa.h>
#include <soc/ramstage.h>

#include "chip.h"

struct device_operations cpu_bus_ops = {
	.init = mp_cpu_bus_init,
};

struct pci_operations soc_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void chip_enable_dev(struct device *dev)
{
	switch (dev->path.type) {
	case DEVICE_PATH_GPIO:
		block_gpio_enable(dev);
		break;
	default:
		break;
	}
}

static void chip_init(void *data)
{
	printk(BIOS_DEBUG, "coreboot: calling fsp_silicon_init\n");
	fsp_silicon_init();

	setup_pds();
	attach_iio_stacks();
	pch_enable_ioapic();

	pmc_gpe_init();
	pmc_disable_all_gpe();
	pmc_write_pm1_control(pmc_read_pm1_control() | SCI_EN);
}

struct chip_operations soc_intel_xeon_sp_gnr_ops = {
	.name = "Intel GNR",
	.enable_dev = chip_enable_dev,
	.init = chip_init,
};

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	mainboard_silicon_init_params(silupd);
}
