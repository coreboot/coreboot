/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <acpi/acpigen_pci.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <gpio.h>
#include <intelblocks/acpi.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/numa.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/soc_pch.h>
#include <soc/ramstage.h>
#include <soc/soc_util.h>
#include <soc/util.h>

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = mp_cpu_bus_init,
#if CONFIG(HAVE_ACPI_TABLES)
	/* defined in src/soc/intel/common/block/acpi/acpi.c */
	.acpi_fill_ssdt = generate_cpu_entries,
#endif
};

static void soc_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		/* domain ops are assigned at their creation */
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_GPIO) {
		block_gpio_enable(dev);
	}
}

static void set_imc_locks(void)
{
	struct device *dev = 0;
	while ((dev = dev_find_device(PCI_VID_INTEL, IMC_M2MEM_DEVID, dev)))
		pci_or_config32(dev, IMC_M2MEM_TIMEOUT, TIMEOUT_LOCK);
}

static void set_upi_locks(void)
{
	struct device *dev = 0;
	while ((dev = dev_find_device(PCI_VID_INTEL, UPI_LL_CR_DEVID, dev)))
		pci_or_config32(dev, UPI_LL_CR_KTIMISCMODLCK, KTIMISCMODLCK_LOCK);
}

static void soc_final(void *data)
{
	// Temp Fix - should be done by FSP, in 2S bios completion
	// is not carried out on socket 2
	set_imc_locks();
	set_upi_locks();
}

static void soc_init(void *data)
{
	printk(BIOS_DEBUG, "coreboot: calling fsp_silicon_init\n");
	fsp_silicon_init();

	setup_pds();
	attach_iio_stacks();

	override_hpet_ioapic_bdf();
	pch_lock_dmictl();
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	mainboard_silicon_init_params(silupd);
}

struct chip_operations soc_intel_xeon_sp_skx_ops = {
	.name = "Intel Skylake-SP",
	.enable_dev = soc_enable_dev,
	.init = soc_init,
	.final = soc_final
};

struct pci_operations soc_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};
