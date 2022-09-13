/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbfs.h>
#include <console/console.h>
#include <device/pci.h>
#include <intelblocks/acpi.h>
#include <intelblocks/gpio.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/pch.h>
#include <soc/ramstage.h>
#include <soc/soc_util.h>
#include <soc/util.h>

#if CONFIG(HAVE_ACPI_TABLES)
const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PC00";
	return NULL;
}
#endif

static struct device_operations pci_domain_ops = {
	.read_resources = &pci_domain_read_resources,
	.set_resources = &xeonsp_pci_domain_set_resources,
	.scan_bus = &xeonsp_pci_domain_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables  = &northbridge_write_acpi_tables,
	#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = soc_acpi_name
#endif
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = xeon_sp_init_cpus,
#if CONFIG(HAVE_ACPI_TABLES)
	/* defined in src/soc/intel/common/block/acpi/acpi.c */
	.acpi_fill_ssdt = generate_cpu_entries,
#endif
};

static void soc_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
		attach_iio_stacks(dev);
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_GPIO) {
		block_gpio_enable(dev);
	}
}

static void soc_init(void *data)
{
	printk(BIOS_DEBUG, "coreboot: calling fsp_silicon_init\n");
	fsp_silicon_init();
	override_hpet_ioapic_bdf();
	pch_lock_dmictl();
}

static void soc_final(void *data)
{
	// Temp Fix - should be done by FSP, in 2S bios completion
	// is not carried out on socket 2
	set_bios_init_completion();
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	const struct microcode *microcode_file;
	size_t microcode_len;

	microcode_file = cbfs_map("cpu_microcode_blob.bin", &microcode_len);

	if ((microcode_file) && (microcode_len != 0)) {
		/* Update CPU Microcode patch base address/size */
		silupd->FspsConfig.PcdCpuMicrocodePatchBase =
		       (uint32_t)microcode_file;
		silupd->FspsConfig.PcdCpuMicrocodePatchSize =
		       (uint32_t)microcode_len;
	}

	mainboard_silicon_init_params(silupd);
}

struct chip_operations soc_intel_xeon_sp_skx_ops = {
	CHIP_NAME("Intel Skylake-SP")
	.enable_dev = soc_enable_dev,
	.init = soc_init,
	.final = soc_final
};

struct pci_operations soc_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};
