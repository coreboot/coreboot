/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/data_fabric.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include "chip.h"
#include <fsp/api.h>

/* Supplied by i2c.c */
extern struct device_operations picasso_i2c_mmio_ops;
/* Supplied by uart.c */
extern struct device_operations picasso_uart_mmio_ops;

struct device_operations cpu_bus_ops = {
	.read_resources	  = noop_read_resources,
	.set_resources	  = noop_set_resources,
	.init		  = mp_cpu_bus_init,
	.acpi_fill_ssdt   = generate_cpu_entries,
};

const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	if (dev->bus->dev->path.type == DEVICE_PATH_DOMAIN) {
		switch (dev->path.pci.devfn) {
		case GNB_DEVFN:
			return "GNB";
		case IOMMU_DEVFN:
			return "IOMM";
		case LPC_DEVFN:
			return "LPCB";
		case SMBUS_DEVFN:
			return "SBUS";
		default:
			printk(BIOS_WARNING, "Unknown root PCI device: dev: %d, fn: %d\n",
			       PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
			return NULL;
		}
	}

	printk(BIOS_WARNING, "Unknown PCI device: dev: %d, fn: %d\n",
	       PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
	return NULL;
};

static struct device_operations pci_domain_ops = {
	.read_resources	  = pci_domain_read_resources,
	.set_resources	  = pci_domain_set_resources,
	.scan_bus	  = pci_domain_scan_bus,
	.acpi_name	  = soc_acpi_name,
};

static void set_mmio_dev_ops(struct device *dev)
{
	switch (dev->path.mmio.addr) {
	case APU_I2C2_BASE:
	case APU_I2C3_BASE:
	case APU_I2C4_BASE:
		dev->ops = &picasso_i2c_mmio_ops;
		break;
	case APU_UART0_BASE:
	case APU_UART1_BASE:
	case APU_UART2_BASE:
	case APU_UART3_BASE:
		dev->ops = &picasso_uart_mmio_ops;
		break;
	}
}

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_MMIO) {
		set_mmio_dev_ops(dev);
	}
}

static void soc_init(void *chip_info)
{
	default_dev_ops_root.write_acpi_tables = agesa_write_acpi_tables;

	fsp_silicon_init(acpi_is_wakeup_s3());

	data_fabric_set_mmio_np();
	southbridge_init(chip_info);
}

static void soc_final(void *chip_info)
{
	southbridge_final(chip_info);
}

struct chip_operations soc_amd_picasso_ops = {
	CHIP_NAME("AMD Picasso SOC")
	.enable_dev = enable_dev,
	.init = soc_init,
	.final = soc_final
};
