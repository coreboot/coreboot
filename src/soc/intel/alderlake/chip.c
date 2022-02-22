/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cfg.h>
#include <intelblocks/cse.h>
#include <intelblocks/gpio.h>
#include <intelblocks/irq.h>
#include <intelblocks/itss.h>
#include <intelblocks/pcie_rp.h>
#include <intelblocks/xdci.h>
#include <soc/intel/common/vbt.h>
#include <soc/itss.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>

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
		return NULL;
	}
	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case SA_DEVFN_ROOT:		return "MCHC";
	case SA_DEVFN_CPU_PCIE1_0:	return "PEG2";
	case SA_DEVFN_CPU_PCIE6_0:	return "PEG0";
	case SA_DEVFN_CPU_PCIE6_2:	return "PEG1";
	case SA_DEVFN_IGD:		return "GFX0";
	case SA_DEVFN_TCSS_XHCI:	return "TXHC";
	case SA_DEVFN_TCSS_XDCI:	return "TXDC";
	case SA_DEVFN_TCSS_DMA0:	return "TDM0";
	case SA_DEVFN_TCSS_DMA1:	return "TDM1";
	case SA_DEVFN_TBT0:		return "TRP0";
	case SA_DEVFN_TBT1:		return "TRP1";
	case SA_DEVFN_TBT2:		return "TRP2";
	case SA_DEVFN_TBT3:		return "TRP3";
	case SA_DEVFN_IPU:		return "IPU0";
	case PCH_DEVFN_ISH:		return "ISHB";
	case PCH_DEVFN_XHCI:		return "XHCI";
	case PCH_DEVFN_I2C0:		return "I2C0";
	case PCH_DEVFN_I2C1:		return "I2C1";
	case PCH_DEVFN_I2C2:		return "I2C2";
	case PCH_DEVFN_I2C3:		return "I2C3";
	case PCH_DEVFN_I2C4:		return "I2C4";
	case PCH_DEVFN_I2C5:		return "I2C5";
	case PCH_DEVFN_I2C6:		return "I2C6";
	case PCH_DEVFN_I2C7:		return "I2C7";
	case PCH_DEVFN_SATA:		return "SATA";
	case PCH_DEVFN_PCIE1:		return "RP01";
	case PCH_DEVFN_PCIE2:		return "RP02";
	case PCH_DEVFN_PCIE3:		return "RP03";
	case PCH_DEVFN_PCIE4:		return "RP04";
	case PCH_DEVFN_PCIE5:		return "RP05";
	case PCH_DEVFN_PCIE6:		return "RP06";
	case PCH_DEVFN_PCIE7:		return "RP07";
	case PCH_DEVFN_PCIE8:		return "RP08";
	case PCH_DEVFN_PCIE9:		return "RP09";
	case PCH_DEVFN_PCIE10:		return "RP10";
	case PCH_DEVFN_PCIE11:		return "RP11";
	case PCH_DEVFN_PCIE12:		return "RP12";
	case PCH_DEVFN_PMC:		return "PMC";
	case PCH_DEVFN_UART0:		return "UAR0";
	case PCH_DEVFN_UART1:		return "UAR1";
	case PCH_DEVFN_UART2:		return "UAR2";
	case PCH_DEVFN_GSPI0:		return "SPI0";
	case PCH_DEVFN_GSPI1:		return "SPI1";
	case PCH_DEVFN_GSPI2:		return "SPI2";
	case PCH_DEVFN_GSPI3:		return "SPI3";
	/* Keeping ACPI device name coherent with ec.asl */
	case PCH_DEVFN_ESPI:		return "LPCB";
	case PCH_DEVFN_HDA:		return "HDAS";
	case PCH_DEVFN_SMBUS:		return "SBUS";
	case PCH_DEVFN_GBE:		return "GLAN";
#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
	case PCH_DEVFN_EMMC:		return "EMMC";
#endif
	}

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
	/* Perform silicon specific init. */
	fsp_silicon_init();

	 /* Display FIRMWARE_VERSION_INFO_HOB */
	fsp_display_fvi_version_hob();

	soc_fill_gpio_pm_configuration();

	/* Swap enabled PCI ports in device tree if needed. */
	pcie_rp_update_devicetree(get_pch_pcie_rp_table());

	/* Swap enabled TBT root ports in device tree if needed. */
	pcie_rp_update_devicetree(get_tbt_pcie_rp_table());

	/*
	 * Earlier when coreboot used to send EOP at late as possible caused
	 * issue of delayed response from CSE since CSE was busy loading payload.
	 * To resolve the issue, EOP should be sent earlier than current sequence
	 * in the boot sequence at BS_DEV_INIT.
	 * Intel CSE team recommends to send EOP close to FW init (between FSP-S exit and
	 * current boot sequence) to reduce message response time from CSE hence moving
	 * sending EOP to earlier stage.
	 */
	if (CONFIG(SOC_INTEL_CSE_SEND_EOP_EARLY)) {
		printk(BIOS_INFO, "Sending EOP early from SoC\n");
		cse_send_end_of_post();
	}
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
	/*
	 * Set the operations if it is a special bus type or a hidden PCI
	 * device.
	 */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
	else if (dev->path.type == DEVICE_PATH_PCI &&
		 dev->path.pci.devfn == PCH_DEVFN_PMC)
		dev->ops = &pmc_ops;
	else if (dev->path.type == DEVICE_PATH_GPIO)
		block_gpio_enable(dev);
}

struct chip_operations soc_intel_alderlake_ops = {
	CHIP_NAME("Intel Alderlake")
	.enable_dev	= &soc_enable,
	.init		= &soc_init_pre_device,
};
