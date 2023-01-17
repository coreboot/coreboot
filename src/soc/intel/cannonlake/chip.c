/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <gpio.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cfg.h>
#include <intelblocks/irq.h>
#include <intelblocks/itss.h>
#include <intelblocks/pcie_rp.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/xdci.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

#include "chip.h"

static const struct pcie_rp_group pch_lp_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 8, .lcap_port_base = 1 },
	{ 0 }
};

static const struct pcie_rp_group pch_h_rp_groups[] = {
	{ .slot = PCH_DEV_SLOT_PCIE,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_1,	.count = 8, .lcap_port_base = 1 },
	{ .slot = PCH_DEV_SLOT_PCIE_2,	.count = 8, .lcap_port_base = 1 },
	{ 0 }
};

#if CONFIG(HAVE_ACPI_TABLES)
const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type == DEVICE_PATH_USB) {
		switch (dev->path.usb.port_type) {
		case 0:
			/* Root Hub */
			return "RHUB";
		case 2:
			/* USB2 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "HS01";
			case 1: return "HS02";
			case 2: return "HS03";
			case 3: return "HS04";
			case 4: return "HS05";
			case 5: return "HS06";
			case 6: return "HS07";
			case 7: return "HS08";
			case 8: return "HS09";
			case 9: return "HS10";
			case 10: return "HS11";
			case 11: return "HS12";
			}
			break;
		case 3:
			/* USB3 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "SS01";
			case 1: return "SS02";
			case 2: return "SS03";
			case 3: return "SS04";
			case 4: return "SS05";
			case 5: return "SS06";
			}
			break;
		}
		return NULL;
	}

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case SA_DEVFN_ROOT:	return "MCHC";
	case SA_DEVFN_IGD:	return "GFX0";
	case PCH_DEVFN_ISH:	return "ISHB";
	case PCH_DEVFN_XHCI:	return "XHCI";
	case PCH_DEVFN_USBOTG:	return "XDCI";
	case PCH_DEVFN_THERMAL:	return "THRM";
	case PCH_DEVFN_I2C0:	return "I2C0";
	case PCH_DEVFN_I2C1:	return "I2C1";
	case PCH_DEVFN_I2C2:	return "I2C2";
	case PCH_DEVFN_I2C3:	return "I2C3";
	case PCH_DEVFN_CSE:	return "CSE1";
	case PCH_DEVFN_CSE_2:	return "CSE2";
	case PCH_DEVFN_CSE_IDER:	return "CSED";
	case PCH_DEVFN_CSE_KT:	return "CSKT";
	case PCH_DEVFN_CSE_3:	return "CSE3";
	case PCH_DEVFN_SATA:	return "SATA";
	case PCH_DEVFN_UART2:	return "UAR2";
	case PCH_DEVFN_I2C4:	return "I2C4";
	case PCH_DEVFN_I2C5:	return "I2C5";
	case PCH_DEVFN_PCIE1:	return "RP01";
	case PCH_DEVFN_PCIE2:	return "RP02";
	case PCH_DEVFN_PCIE3:	return "RP03";
	case PCH_DEVFN_PCIE4:	return "RP04";
	case PCH_DEVFN_PCIE5:	return "RP05";
	case PCH_DEVFN_PCIE6:	return "RP06";
	case PCH_DEVFN_PCIE7:	return "RP07";
	case PCH_DEVFN_PCIE8:	return "RP08";
	case PCH_DEVFN_PCIE9:	return "RP09";
	case PCH_DEVFN_PCIE10:	return "RP10";
	case PCH_DEVFN_PCIE11:	return "RP11";
	case PCH_DEVFN_PCIE12:	return "RP12";
	case PCH_DEVFN_PCIE13:	return "RP13";
	case PCH_DEVFN_PCIE14:	return "RP14";
	case PCH_DEVFN_PCIE15:	return "RP15";
	case PCH_DEVFN_PCIE16:	return "RP16";
	case PCH_DEVFN_PCIE17:	return "RP17";
	case PCH_DEVFN_PCIE18:	return "RP18";
	case PCH_DEVFN_PCIE19:	return "RP19";
	case PCH_DEVFN_PCIE20:	return "RP20";
	case PCH_DEVFN_PCIE21:	return "RP21";
	case PCH_DEVFN_PCIE22:	return "RP22";
	case PCH_DEVFN_PCIE23:	return "RP23";
	case PCH_DEVFN_PCIE24:	return "RP24";
	case PCH_DEVFN_UART0:	return "UAR0";
	case PCH_DEVFN_UART1:	return "UAR1";
	case PCH_DEVFN_GSPI0:	return "SPI0";
	case PCH_DEVFN_GSPI1:	return "SPI1";
	case PCH_DEVFN_GSPI2:	return "SPI2";
	case PCH_DEVFN_EMMC:	return "EMMC";
	case PCH_DEVFN_SDCARD:	return "SDXC";
	case PCH_DEVFN_P2SB:	return "P2SB";
	case PCH_DEVFN_PMC:	return "PMC_";
	case PCH_DEVFN_HDA:	return "HDAS";
	case PCH_DEVFN_SMBUS:	return "SBUS";
	case PCH_DEVFN_SPI:	return "FSPI";
	case PCH_DEVFN_GBE:	return "IGBE";
	case PCH_DEVFN_TRACEHUB:return "THUB";
	}

	return NULL;
}
#endif

void soc_init_pre_device(void *chip_info)
{
	/* Perform silicon specific init. */
	fsp_silicon_init();

	 /* Display FIRMWARE_VERSION_INFO_HOB */
	fsp_display_fvi_version_hob();

	soc_gpio_pm_configuration();

	/* swap enabled PCI ports in device tree if needed */
	if (CONFIG(SOC_INTEL_CANNONLAKE_PCH_H))
		pcie_rp_update_devicetree(pch_h_rp_groups);
	else
		pcie_rp_update_devicetree(pch_lp_rp_groups);
}

static void cpu_fill_ssdt(const struct device *dev)
{
	generate_cpu_entries(dev);

	if (!generate_pin_irq_map())
		printk(BIOS_ERR, "Failed to generate ACPI _PRT table!\n");
}

static void cpu_set_north_irqs(struct device *dev)
{
	irq_program_non_pch();
}

static struct device_operations pci_domain_ops = {
	.read_resources   = &pci_domain_read_resources,
	.set_resources    = &pci_domain_set_resources,
	.scan_bus         = &pci_domain_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = &soc_acpi_name,
	.acpi_fill_ssdt   = ssdt_set_above_4g_pci,
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.enable_resources = cpu_set_north_irqs,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt   = cpu_fill_ssdt,
#endif
};

static void soc_enable(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
	else if (dev->path.type == DEVICE_PATH_GPIO)
		block_gpio_enable(dev);
	else if (dev->path.type == DEVICE_PATH_PCI &&
		 dev->path.pci.devfn == PCH_DEVFN_PMC)
		dev->ops = &pmc_ops;
}

struct chip_operations soc_intel_cannonlake_ops = {
	CHIP_NAME("Intel Cannonlake")
	.enable_dev	= &soc_enable,
	.init		= &soc_init_pre_device,
};
