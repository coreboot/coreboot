/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <console/console.h>
#include <console/debug.h>
#include <cpu/x86/lapic.h>
#include <device/pci.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/p2sb.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/cpu.h>
#include <soc/pch.h>
#include <soc/ramstage.h>
#include <soc/p2sb.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <soc/pci_devs.h>

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	mainboard_silicon_init_params(silupd);
}

#if CONFIG(HAVE_ACPI_TABLES)
static const char *soc_acpi_name(const struct device *dev)
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
	.acpi_name        = soc_acpi_name
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = cpx_init_cpus,
	.acpi_fill_ssdt = generate_cpu_entries,
};

struct pci_operations soc_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void chip_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
		attach_iio_stacks(dev);
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

static void chip_final(void *data)
{
	/* Lock SBI */
	pci_or_config32(PCH_DEV_P2SB, P2SBC, SBILOCK);
	p2sb_hide();

	set_bios_init_completion();
}

static void chip_init(void *data)
{
	printk(BIOS_DEBUG, "coreboot: calling fsp_silicon_init\n");
	fsp_silicon_init(false);
	override_hpet_ioapic_bdf();
	pch_enable_ioapic();
	setup_lapic();
	p2sb_unhide();
}

struct chip_operations soc_intel_xeon_sp_cpx_ops = {
	CHIP_NAME("Intel Cooperlake-SP")
	.enable_dev = chip_enable_dev,
	.init = chip_init,
	.final = chip_final,
};
