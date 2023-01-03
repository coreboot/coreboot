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
#include <intelblocks/p2sb.h>
#include <intelblocks/pcie_rp.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/tcss.h>
#include <intelblocks/xdci.h>
#include <soc/intel/common/vbt.h>
#include <soc/iomap.h>
#include <soc/itss.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <soc/tcss.h>

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
			}
			break;
		case 3:
			/* USB3 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "SS01";
			case 1: return "SS02";
			case 2: return "SS03";
			case 3: return "SS04";
			}
			break;
		}
		printk(BIOS_DEBUG, "dev->path.type=%x\n", dev->path.usb.port_type);
		return NULL;
	}
	if (dev->path.type != DEVICE_PATH_PCI) {
		printk(BIOS_DEBUG, "dev->path.type=%x\n", dev->path.type);
		return NULL;
	}

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN_ROOT:		return "MCHC";
	case PCI_DEVFN_TCSS_XHCI:	return "TXHC";
	case PCI_DEVFN_TCSS_XDCI:	return "TXDC";
	case PCI_DEVFN_TCSS_DMA0:	return "TDM0";
	case PCI_DEVFN_TCSS_DMA1:	return "TDM1";
	case PCI_DEVFN_TBT0:		return "TRP0";
	case PCI_DEVFN_TBT1:		return "TRP1";
	case PCI_DEVFN_TBT2:		return "TRP2";
	case PCI_DEVFN_TBT3:		return "TRP3";
	case PCI_DEVFN_IPU:		return "IPU0";
	case PCI_DEVFN_ISH:		return "ISHB";
	case PCI_DEVFN_XHCI:	return "XHCI";
	case PCI_DEVFN_I2C0:	return "I2C0";
	case PCI_DEVFN_I2C1:	return "I2C1";
	case PCI_DEVFN_I2C2:	return "I2C2";
	case PCI_DEVFN_I2C3:	return "I2C3";
	case PCI_DEVFN_I2C4:	return "I2C4";
	case PCI_DEVFN_I2C5:	return "I2C5";
	case PCI_DEVFN_SATA:	return "SATA";
	case PCI_DEVFN_PCIE1:	return "RP01";
	case PCI_DEVFN_PCIE2:	return "RP02";
	case PCI_DEVFN_PCIE3:	return "RP03";
	case PCI_DEVFN_PCIE4:	return "RP04";
	case PCI_DEVFN_PCIE5:	return "RP05";
	case PCI_DEVFN_PCIE6:	return "RP06";
	case PCI_DEVFN_PCIE7:	return "RP07";
	case PCI_DEVFN_PCIE8:	return "RP08";
	case PCI_DEVFN_PCIE9:	return "RP09";
	case PCI_DEVFN_PCIE10:	return "RP10";
	case PCI_DEVFN_PCIE11:	return "RP11";
	case PCI_DEVFN_PCIE12:	return "RP12";
	case PCI_DEVFN_PMC:		return "PMC";
	case PCI_DEVFN_UART0:	return "UAR0";
	case PCI_DEVFN_UART1:	return "UAR1";
	case PCI_DEVFN_UART2:	return "UAR2";
	case PCI_DEVFN_GSPI0:	return "SPI0";
	case PCI_DEVFN_GSPI1:	return "SPI1";
	case PCI_DEVFN_GSPI2:	return "SPI2";
	/* Keeping ACPI device name coherent with ec.asl */
	case PCI_DEVFN_ESPI:	return "LPCB";
	case PCI_DEVFN_HDA:	return "HDAS";
	case PCI_DEVFN_SMBUS:	return "SBUS";
	case PCI_DEVFN_GBE:	return "GLAN";
	}
	printk(BIOS_DEBUG, "dev->path.devfn=%x\n", dev->path.pci.devfn);
	return NULL;
}
#endif

/* SoC routine to fill GPIO PM mask and value for GPIO_MISCCFG register */
static void soc_fill_gpio_pm_configuration(void)
{
	uint8_t value[TOTAL_GPIO_COMM];
	const config_t *config = config_of_soc();

	if (config->gpio_override_pm)
		memcpy(value, config->gpio_pm, sizeof(value));
	else
		memset(value, MISCCFG_GPIO_PM_CONFIG_BITS, sizeof(value));

	gpio_pm_configure(value, TOTAL_GPIO_COMM);
}

void soc_init_pre_device(void *chip_info)
{
	config_t *config = config_of_soc();

	/* Validate TBT image authentication */
	config->tbt_authentication = ioe_p2sb_sbi_read(PID_IOM,
					IOM_CSME_IMR_TBT_STATUS) & TBT_VALID_AUTHENTICATION;

	/* Perform silicon specific init. */
	fsp_silicon_init();

	 /* Display FIRMWARE_VERSION_INFO_HOB */
	fsp_display_fvi_version_hob();

	soc_fill_gpio_pm_configuration();

	/* Swap enabled PCI ports in device tree if needed. */
	pcie_rp_update_devicetree(get_pcie_rp_table());
}

static void cpu_fill_ssdt(const struct device *dev)
{
	if (!generate_pin_irq_map())
		printk(BIOS_ERR, "Failed to generate ACPI _PRT table!\n");

	generate_cpu_entries(dev);
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
	.acpi_fill_ssdt = cpu_fill_ssdt,
#endif
};

static void soc_enable(struct device *dev)
{
	/*
	 * Set the operations if it is a special bus type or a hidden PCI
	 * device.
	 */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
	else if (dev->path.type == DEVICE_PATH_PCI &&
		 dev->path.pci.devfn == PCI_DEVFN_PMC)
		dev->ops = &pmc_ops;
	else if (dev->path.type == DEVICE_PATH_PCI &&
		 dev->path.pci.devfn == PCI_DEVFN_P2SB)
		dev->ops = &soc_p2sb_ops;
	else if (dev->path.type == DEVICE_PATH_PCI &&
		 dev->path.pci.devfn == PCI_DEVFN_IOE_P2SB)
		dev->ops = &ioe_p2sb_ops;
	else if (dev->path.type == DEVICE_PATH_GPIO)
		block_gpio_enable(dev);
}

struct chip_operations soc_intel_meteorlake_ops = {
	CHIP_NAME("Intel Meteorlake")
	.enable_dev	= &soc_enable,
	.init		= &soc_init_pre_device,
};
