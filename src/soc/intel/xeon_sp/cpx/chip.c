/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <cbfs.h>
#include <console/console.h>
#include <cpu/x86/lapic.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <intelblocks/p2sb.h>
#include <soc/cpu.h>
#include <soc/ramstage.h>
#include <soc/pm.h>

/* C620 IOAPIC has 120 redirection entries */
#define C620_IOAPIC_REDIR_ENTRIES		120

void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	/* not implemented yet */
}

static struct device_operations pci_domain_ops = {
	.read_resources = &pci_domain_read_resources,
	.set_resources = &pci_domain_set_resources,
	.scan_bus = &pci_domain_scan_bus,
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = cpx_init_cpus,
};

static void chip_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

static void pch_enable_ioapic(const struct device *dev)
{
	uint32_t reg32;

	set_ioapic_id((void *)IO_APIC_ADDR, 2);

	/* affirm full set of redirection table entries ("write once") */
	reg32 = io_apic_read((void *)IO_APIC_ADDR, 1);

	reg32 &= ~0x00ff0000;
	reg32 |= (C620_IOAPIC_REDIR_ENTRIES - 1) << 16;

	io_apic_write((void *)IO_APIC_ADDR, 1, reg32);

	/*
	 * Select Boot Configuration register (0x03) and
	 * use Processor System Bus (0x01) to deliver interrupts.
	 */
	io_apic_write((void *)IO_APIC_ADDR, 3, 1);
}

struct pci_operations soc_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void chip_final(void *data)
{
	p2sb_hide();
}

static void chip_init(void *data)
{
	printk(BIOS_DEBUG, "coreboot: calling fsp_silicon_init\n");
	fsp_silicon_init(false);
	pch_enable_ioapic(NULL);
	setup_lapic();
	p2sb_unhide();
}

struct chip_operations soc_intel_xeon_sp_cpx_ops = {
	CHIP_NAME("Intel Cooperlake-SP")
	.enable_dev = chip_enable_dev,
	.init = chip_init,
	.final = chip_final
};
