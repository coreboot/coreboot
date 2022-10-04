/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/data_fabric.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <types.h>
#include "chip.h"

/* Supplied by i2c.c */
extern struct device_operations soc_amd_i2c_mmio_ops;
/* Supplied by uart.c */
extern struct device_operations mendocino_uart_mmio_ops;
/* Supplied by emmc.c */
extern struct device_operations mendocino_emmc_mmio_ops;


struct device_operations cpu_bus_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.init		= mp_cpu_bus_init,
	.acpi_fill_ssdt	= generate_cpu_entries,
};

static const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	printk(BIOS_WARNING, "Unknown PCI device: dev: %d, fn: %d\n",
	       PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
	return NULL;
};

static struct device_operations pci_domain_ops = {
	.read_resources	= pci_domain_read_resources,
	.set_resources	= pci_domain_set_resources,
	.scan_bus	= pci_domain_scan_bus,
	.acpi_name	= soc_acpi_name,
};

static void set_mmio_dev_ops(struct device *dev)
{
	switch (dev->path.mmio.addr) {
	case APU_I2C0_BASE:
	case APU_I2C1_BASE:
	case APU_I2C2_BASE:
	case APU_I2C3_BASE:
		dev->ops = &soc_amd_i2c_mmio_ops;
		break;
	case APU_UART0_BASE:
	case APU_UART1_BASE:
	case APU_UART2_BASE:
	case APU_UART3_BASE:
	case APU_UART4_BASE:
		dev->ops = &mendocino_uart_mmio_ops;
		break;
	case APU_EMMC_BASE:
		dev->ops = &mendocino_emmc_mmio_ops;
		break;
	}
}

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	switch (dev->path.type) {
	case DEVICE_PATH_DOMAIN:
		dev->ops = &pci_domain_ops;
		break;
	case DEVICE_PATH_CPU_CLUSTER:
		dev->ops = &cpu_bus_ops;
		break;
	case DEVICE_PATH_MMIO:
		set_mmio_dev_ops(dev);
		break;
	default:
		break;
	}
}

static void soc_init(void *chip_info)
{
	default_dev_ops_root.write_acpi_tables = agesa_write_acpi_tables;

	fsp_silicon_init();

	data_fabric_set_mmio_np();

	fch_init(chip_info);
}

static void soc_final(void *chip_info)
{
	fch_final(chip_info);
}

struct chip_operations soc_amd_mendocino_ops = {
	CHIP_NAME("AMD Mendocino SoC")
	.enable_dev = enable_dev,
	.init = soc_init,
	.final = soc_final
};
