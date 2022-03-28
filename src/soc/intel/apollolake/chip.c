/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <bootsplash.h>
#include <bootstate.h>
#include <console/console.h>
#include <cpu/x86/mp.h>
#include <device/mmio.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cfg.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/msr.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/xdci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/gpio.h>
#include <intelblocks/itss.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/systemagent.h>
#include <option.h>
#include <soc/cpu.h>
#include <soc/heci.h>
#include <soc/intel/common/vbt.h>
#include <soc/iomap.h>
#include <soc/itss.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/systemagent.h>
#include <spi-generic.h>
#include <timer.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <types.h>

#include "chip.h"

#define DUAL_ROLE_CFG0          0x80d8
#define SW_VBUS_VALID_MASK      (1 << 24)
#define SW_IDPIN_EN_MASK        (1 << 21)
#define SW_IDPIN_MASK           (1 << 20)
#define SW_IDPIN_HOST           (0 << 20)
#define DUAL_ROLE_CFG1          0x80dc
#define DRD_MODE_MASK           (1 << 29)
#define DRD_MODE_HOST           (1 << 29)

#define CFG_XHCLKGTEN		0x8650
/* Naking USB2.0 EPs for Backbone Clock Gating and PLL Shutdown */
#define NUEFBCGPS		(1 << 28)
/* SRAM Power Gate Enable */
#define SRAMPGTEN		(1 << 27)
/* SS Link PLL Shutdown Enable */
#define SSLSE			(1 << 26)
/* USB2 PLL Shutdown Enable */
#define USB2PLLSE		(1 << 25)
/* IOSF Sideband Trunk Clock Gating Enable */
#define IOSFSTCGE		(1 << 24)
/* BIT[23:20] HS Backbone PXP Trunk Clock Gate Enable */
#define HSTCGE			(1 << 23 | 1 << 22)
/* BIT[19:16] SS Backbone PXP Trunk Clock Gate Enable */
#define SSTCGE			(1 << 19 | 1 << 18 | 1 << 17)
/* XHC Ignore_EU3S */
#define XHCIGEU3S		(1 << 15)
/* XHC Frame Timer Clock Shutdown Enable */
#define XHCFTCLKSE		(1 << 14)
/* XHC Backbone PXP Trunk Clock Gate In Presence of ISOCH EP */
#define XHCBBTCGIPISO		(1 << 13)
/* XHC HS Backbone PXP Trunk Clock Gate U2 non RWE */
#define XHCHSTCGU2NRWE		(1 << 12)
/* BIT[11:10] XHC USB2 PLL Shutdown Lx Enable */
#define XHCUSB2PLLSDLE		(1 << 11 | 1 << 10)
/* BIT[9:8] HS Backbone PXP PLL Shutdown Ux Enable */
#define HSUXDMIPLLSE		(1 << 9)
/* BIT[7:5] SS Backbone PXP PLL shutdown Ux Enable */
#define SSPLLSUE		(1 << 6)
/* XHC Backbone Local Clock Gating Enable */
#define XHCBLCGE		(1 << 4)
/* HS Link Trunk Clock Gating Enable */
#define HSLTCGE			(1 << 3)
/* SS Link Trunk Clock Gating Enable */
#define SSLTCGE			(1 << 2)
/* IOSF Backbone Trunk Clock Gating Enable */
#define IOSFBTCGE		(1 << 1)
/* IOSF Gasket Backbone Local Clock Gating Enable */
#define IOSFGBLCGE		(1 << 0)

#define CFG_XHCPMCTRL		0x80a4
/* BIT[7:4] LFPS periodic sampling for USB3 Ports */
#define LFPS_PM_DISABLE_MASK    0xFFFFFF0F

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
			case 8:
				if (CONFIG(SOC_INTEL_GEMINILAKE))
					return "HS09";
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
	/* DSDT: acpi/northbridge.asl */
	case SA_DEVFN_ROOT:
		return "MCHC";
	/* DSDT: acpi/xhci.asl */
	case PCH_DEVFN_XHCI:
		return "XHCI";
	/* DSDT: acpi/pch_hda.asl */
	case PCH_DEVFN_HDA:
		return "HDAS";
	/* DSDT: acpi/lpss.asl */
	case PCH_DEVFN_UART0:
		return "URT1";
	case PCH_DEVFN_UART1:
		return "URT2";
	case PCH_DEVFN_UART2:
		return "URT3";
	case PCH_DEVFN_UART3:
		return "URT4";
	case PCH_DEVFN_SPI0:
		return "SPI1";
	case PCH_DEVFN_SPI1:
		return "SPI2";
	case PCH_DEVFN_SPI2:
		return "SPI3";
	case PCH_DEVFN_PWM:
		return "PWM";
	case PCH_DEVFN_I2C0:
		return "I2C0";
	case PCH_DEVFN_I2C1:
		return "I2C1";
	case PCH_DEVFN_I2C2:
		return "I2C2";
	case PCH_DEVFN_I2C3:
		return "I2C3";
	case PCH_DEVFN_I2C4:
		return "I2C4";
	case PCH_DEVFN_I2C5:
		return "I2C5";
	case PCH_DEVFN_I2C6:
		return "I2C6";
	case PCH_DEVFN_I2C7:
		return "I2C7";
	/* Storage */
	case PCH_DEVFN_SDCARD:
		return "SDCD";
	case PCH_DEVFN_EMMC:
		return "EMMC";
	case PCH_DEVFN_SDIO:
		return "SDIO";
	/* PCIe */
	case PCH_DEVFN_PCIE1:
		return "RP03";
	case PCH_DEVFN_PCIE5:
		return "RP01";
	}

	return NULL;
}

static struct device_operations pci_domain_ops = {
	.read_resources = pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.scan_bus = pci_domain_scan_bus,
	.acpi_name = &soc_acpi_name,
	.acpi_fill_ssdt = ssdt_set_above_4g_pci,
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = apollolake_init_cpus,
	.acpi_fill_ssdt = generate_cpu_entries,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
	else if (dev->path.type == DEVICE_PATH_GPIO)
		block_gpio_enable(dev);
}

/*
 * If the PCIe root port at function 0 is disabled,
 * the PCIe root ports might be coalesced after FSP silicon init.
 * The below function will swap the devfn of the first enabled device
 * in devicetree and function 0 resides a pci device
 * so that it won't confuse coreboot.
 */
static void pcie_update_device_tree(unsigned int devfn0, int num_funcs)
{
	struct device *func0;
	unsigned int devfn;
	int i;
	unsigned int inc = PCI_DEVFN(0, 1);

	func0 = pcidev_path_on_root(devfn0);
	if (func0 == NULL)
		return;

	/* No more functions if function 0 is disabled. */
	if (pci_read_config32(func0, PCI_VENDOR_ID) == 0xffffffff)
		return;

	devfn = devfn0 + inc;

	/*
	 * Increase function by 1.
	 * Then find first enabled device to replace func0
	 * as that port was move to func0.
	 */
	for (i = 1; i < num_funcs; i++, devfn += inc) {
		struct device *dev = pcidev_path_on_root(devfn);
		if (dev == NULL)
			continue;

		if (!dev->enabled)
			continue;
		/* Found the first enabled device in given dev number */
		func0->path.pci.devfn = dev->path.pci.devfn;
		dev->path.pci.devfn = devfn0;
		break;
	}
}

static void pcie_override_devicetree_after_silicon_init(void)
{
	pcie_update_device_tree(PCH_DEVFN_PCIE1, 4);
	pcie_update_device_tree(PCH_DEVFN_PCIE5, 2);
}

/* Overwrites the SCI IRQ if another IRQ number is given by device tree. */
static void set_sci_irq(void)
{
	struct soc_intel_apollolake_config *cfg;
	uint32_t scis;

	cfg = config_of_soc();

	/* Change only if a device tree entry exists. */
	if (cfg->sci_irq) {
		scis = soc_read_sci_irq_select();
		scis &= ~SCI_IRQ_SEL;
		scis |= (cfg->sci_irq << SCI_IRQ_ADJUST) & SCI_IRQ_SEL;
		soc_write_sci_irq_select(scis);
	}
}

static void soc_init(void *data)
{
	struct soc_power_limits_config *soc_config;
	config_t *config;

	/* Snapshot the current GPIO IRQ polarities. FSP is setting a
	 * default policy that doesn't honor boards' requirements. */
	itss_snapshot_irq_polarities(GPIO_IRQ_START, GPIO_IRQ_END);

	/*
	 * Clear the GPI interrupt status and enable registers. These
	 * registers do not get reset to default state when booting from S5.
	 */
	gpi_clear_int_cfg();

	fsp_silicon_init();

	/* Restore GPIO IRQ polarities back to previous settings. */
	itss_restore_irq_polarities(GPIO_IRQ_START, GPIO_IRQ_END);

	/* override 'enabled' setting in device tree if needed */
	pcie_override_devicetree_after_silicon_init();

	/*
	 * Keep the P2SB device visible so it and the other devices are
	 * visible in coreboot for driver support and PCI resource allocation.
	 * There is a UPD setting for this, but it's more consistent to use
	 * hide and unhide symmetrically.
	 */
	p2sb_unhide();

	if (CONFIG(SOC_INTEL_DISABLE_POWER_LIMITS)) {
		printk(BIOS_INFO, "Skip setting RAPL per configuration\n");
	} else {
		config = config_of_soc();
		/* Set RAPL MSR for Package power limits */
		soc_config = &config->power_limits_config;
		set_power_limits(MOBILE_SKU_PL1_TIME_SEC, soc_config);
	}

	/*
	* FSP-S routes SCI to IRQ 9. With the help of this function you can
	* select another IRQ for SCI.
	*/
	set_sci_irq();
}

static void soc_final(void *data)
{
	/* Make sure payload/OS can't trigger global reset */
	pmc_global_reset_disable_and_lock();
}

static void disable_dev(struct device *dev, FSP_S_CONFIG *silconfig)
{
	switch (dev->path.pci.devfn) {
	case PCH_DEVFN_NPK:
		/*
		 * Disable this device in the parse_devicetree_setting() function
		 * in romstage.c
		 */
		break;
	case PCH_DEVFN_ISH:
		silconfig->IshEnable = 0;
		break;
	case PCH_DEVFN_SATA:
		silconfig->EnableSata = 0;
		break;
	case PCH_DEVFN_PCIE5:
		silconfig->PcieRootPortEn[0] = 0;
		silconfig->PcieRpHotPlug[0] = 0;
		break;
	case PCH_DEVFN_PCIE6:
		silconfig->PcieRootPortEn[1] = 0;
		silconfig->PcieRpHotPlug[1] = 0;
		break;
	case PCH_DEVFN_PCIE1:
		silconfig->PcieRootPortEn[2] = 0;
		silconfig->PcieRpHotPlug[2] = 0;
		break;
	case PCH_DEVFN_PCIE2:
		silconfig->PcieRootPortEn[3] = 0;
		silconfig->PcieRpHotPlug[3] = 0;
		break;
	case PCH_DEVFN_PCIE3:
		silconfig->PcieRootPortEn[4] = 0;
		silconfig->PcieRpHotPlug[4] = 0;
		break;
	case PCH_DEVFN_PCIE4:
		silconfig->PcieRootPortEn[5] = 0;
		silconfig->PcieRpHotPlug[5] = 0;
		break;
	case PCH_DEVFN_XHCI:
		silconfig->Usb30Mode = 0;
		break;
	case PCH_DEVFN_XDCI:
		silconfig->UsbOtg = 0;
		break;
	case PCH_DEVFN_I2C0:
		silconfig->I2c0Enable = 0;
		break;
	case PCH_DEVFN_I2C1:
		silconfig->I2c1Enable = 0;
		break;
	case PCH_DEVFN_I2C2:
		silconfig->I2c2Enable = 0;
		break;
	case PCH_DEVFN_I2C3:
		silconfig->I2c3Enable = 0;
		break;
	case PCH_DEVFN_I2C4:
		silconfig->I2c4Enable = 0;
		break;
	case PCH_DEVFN_I2C5:
		silconfig->I2c5Enable = 0;
		break;
	case PCH_DEVFN_I2C6:
		silconfig->I2c6Enable = 0;
		break;
	case PCH_DEVFN_I2C7:
		silconfig->I2c7Enable = 0;
		break;
	case PCH_DEVFN_UART0:
		silconfig->Hsuart0Enable = 0;
		break;
	case PCH_DEVFN_UART1:
		silconfig->Hsuart1Enable = 0;
		break;
	case PCH_DEVFN_UART2:
		silconfig->Hsuart2Enable = 0;
		break;
	case PCH_DEVFN_UART3:
		silconfig->Hsuart3Enable = 0;
		break;
	case PCH_DEVFN_SPI0:
		silconfig->Spi0Enable = 0;
		break;
	case PCH_DEVFN_SPI1:
		silconfig->Spi1Enable = 0;
		break;
	case PCH_DEVFN_SPI2:
		silconfig->Spi2Enable = 0;
		break;
	case PCH_DEVFN_SDCARD:
		silconfig->SdcardEnabled = 0;
		break;
	case PCH_DEVFN_EMMC:
		silconfig->eMMCEnabled = 0;
		break;
	case PCH_DEVFN_SDIO:
		silconfig->SdioEnabled = 0;
		break;
	case PCH_DEVFN_SMBUS:
		silconfig->SmbusEnable = 0;
		break;
#if !CONFIG(SOC_INTEL_GEMINILAKE)
	case SA_DEVFN_IPU:
		silconfig->IpuEn = 0;
		break;
#else
	case PCH_DEVFN_CNVI:
		silconfig->CnviMode = 0;
		break;
#endif
	case PCH_DEVFN_HDA:
		silconfig->HdaEnable = 0;
		break;
	default:
		printk(BIOS_WARNING, "PCI:%02x.%01x: Could not disable the device\n",
			PCI_SLOT(dev->path.pci.devfn),
			PCI_FUNC(dev->path.pci.devfn));
		break;
	}
}

static void parse_devicetree(FSP_S_CONFIG *silconfig)
{
	struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);

	if (!dev) {
		printk(BIOS_ERR, "Could not find root device\n");
		return;
	}
	/* Only disable bus 0 devices. */
	for (dev = dev->bus->children; dev; dev = dev->sibling) {
		if (!dev->enabled)
			disable_dev(dev, silconfig);
	}
}

static void apl_fsp_silicon_init_params_cb(struct soc_intel_apollolake_config
	*cfg, FSP_S_CONFIG *silconfig)
{
#if !CONFIG(SOC_INTEL_GEMINILAKE) /* GLK FSP does not have these fields in FspsUpd.h yet */
	uint8_t port;

	for (port = 0; port < APOLLOLAKE_USB2_PORT_MAX; port++) {
		if (cfg->usb_config_override) {
			if (!cfg->usb2_port[port].enable)
				continue;

			silconfig->PortUsb20Enable[port] = 1;
			silconfig->PortUs20bOverCurrentPin[port] = cfg->usb2_port[port].oc_pin;
		}

		if (cfg->usb2eye[port].Usb20PerPortTxPeHalf != 0)
			silconfig->PortUsb20PerPortTxPeHalf[port] =
				cfg->usb2eye[port].Usb20PerPortTxPeHalf;

		if (cfg->usb2eye[port].Usb20PerPortPeTxiSet != 0)
			silconfig->PortUsb20PerPortPeTxiSet[port] =
				cfg->usb2eye[port].Usb20PerPortPeTxiSet;

		if (cfg->usb2eye[port].Usb20PerPortTxiSet != 0)
			silconfig->PortUsb20PerPortTxiSet[port] =
				cfg->usb2eye[port].Usb20PerPortTxiSet;

		if (cfg->usb2eye[port].Usb20HsSkewSel != 0)
			silconfig->PortUsb20HsSkewSel[port] =
				cfg->usb2eye[port].Usb20HsSkewSel;

		if (cfg->usb2eye[port].Usb20IUsbTxEmphasisEn != 0)
			silconfig->PortUsb20IUsbTxEmphasisEn[port] =
				cfg->usb2eye[port].Usb20IUsbTxEmphasisEn;

		if (cfg->usb2eye[port].Usb20PerPortRXISet != 0)
			silconfig->PortUsb20PerPortRXISet[port] =
				cfg->usb2eye[port].Usb20PerPortRXISet;

		if (cfg->usb2eye[port].Usb20HsNpreDrvSel != 0)
			silconfig->PortUsb20HsNpreDrvSel[port] =
				cfg->usb2eye[port].Usb20HsNpreDrvSel;
	}

	if (cfg->usb_config_override) {
		for (port = 0; port < APOLLOLAKE_USB3_PORT_MAX; port++) {
			if (!cfg->usb3_port[port].enable)
				continue;

			silconfig->PortUsb30Enable[port] = 1;
			silconfig->PortUs30bOverCurrentPin[port] = cfg->usb3_port[port].oc_pin;
		}
	}
#endif
}

static void glk_fsp_silicon_init_params_cb(
	struct soc_intel_apollolake_config *cfg, FSP_S_CONFIG *silconfig)
{
#if CONFIG(SOC_INTEL_GEMINILAKE)
	uint8_t port;

	/*
	 * UsbPerPortCtl was retired in Fsp 2.0.0+, so PDO programming must be
	 * enabled to configure individual ports in what Fsp thinks is PEI.
	 */
	silconfig->UsbPdoProgramming = cfg->usb_config_override;

	for (port = 0; port < APOLLOLAKE_USB2_PORT_MAX; port++) {
		if (cfg->usb_config_override) {
			silconfig->PortUsb20Enable[port] = cfg->usb2_port[port].enable;
			silconfig->PortUs20bOverCurrentPin[port] = cfg->usb2_port[port].oc_pin;
		}

		if (!cfg->usb2eye[port].Usb20OverrideEn)
			continue;

		silconfig->Usb2AfePehalfbit[port] =
			cfg->usb2eye[port].Usb20PerPortTxPeHalf;
		silconfig->Usb2AfePetxiset[port] =
			cfg->usb2eye[port].Usb20PerPortPeTxiSet;
		silconfig->Usb2AfeTxiset[port] =
			cfg->usb2eye[port].Usb20PerPortTxiSet;
		silconfig->Usb2AfePredeemp[port] =
			cfg->usb2eye[port].Usb20IUsbTxEmphasisEn;
	}

	if (cfg->usb_config_override) {
		for (port = 0; port < APOLLOLAKE_USB3_PORT_MAX; port++) {
			silconfig->PortUsb30Enable[port] = cfg->usb3_port[port].enable;
			silconfig->PortUs30bOverCurrentPin[port] = cfg->usb3_port[port].oc_pin;
		}
	}

	silconfig->Gmm = is_devfn_enabled(SA_GLK_DEVFN_GMM);

	/* On Geminilake, we need to override the default FSP PCIe de-emphasis
	 * settings using the device tree settings. This is because PCIe
	 * de-emphasis is enabled by default and Thunderpeak PCIe WiFi detection
	 * requires de-emphasis disabled. If we make this change common to both
	 * Apollolake and Geminilake, then we need to add mainboard device tree
	 * de-emphasis settings of 1 to Apollolake systems.
	 */
	memcpy(silconfig->PcieRpSelectableDeemphasis,
		cfg->pcie_rp_deemphasis_enable,
		sizeof(silconfig->PcieRpSelectableDeemphasis));
	/*
	 * FSP does not know what the clock requirements are for the
	 * device on SPI bus, hence it should not modify what coreboot
	 * has set up. Hence skipping in FSP.
	 */
	silconfig->SkipSpiPCP = 1;

	/*
	 * FSP provides UPD interface to execute IPC command. In order to
	 * improve boot performance, configure PmicPmcIpcCtrl for PMC to program
	 * PMIC PCH_PWROK delay.
	 */
	silconfig->PmicPmcIpcCtrl = cfg->PmicPmcIpcCtrl;

	/*
	 * Options to disable XHCI Link Compliance Mode.
	 */
	silconfig->DisableComplianceMode = cfg->DisableComplianceMode;

	/*
	 * Options to change USB3 ModPhy setting for Integrated Filter value.
	 */
	silconfig->ModPhyIfValue = cfg->ModPhyIfValue;

	/*
	 * Options to bump USB3 LDO voltage with 40mv.
	 */
	silconfig->ModPhyVoltageBump = cfg->ModPhyVoltageBump;

	/*
	 * Options to adjust PMIC Vdd2 voltage.
	 */
	silconfig->PmicVdd2Voltage = cfg->PmicVdd2Voltage;
#endif
}

void __weak mainboard_devtree_update(struct device *dev)
{
	/* Override dev tree settings per board */
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	FSP_S_CONFIG *silconfig = &silupd->FspsConfig;
	struct soc_intel_apollolake_config *cfg;
	struct device *dev;

	/* Load VBT before devicetree-specific config. */
	silconfig->GraphicsConfigPtr = (uintptr_t)vbt_get();

	dev = pcidev_path_on_root(SA_DEVFN_ROOT);
	cfg = config_of(dev);

	mainboard_devtree_update(dev);

	/* Parse device tree and disable unused device*/
	parse_devicetree(silconfig);

	memcpy(silconfig->PcieRpClkReqNumber, cfg->pcie_rp_clkreq_pin,
	       sizeof(silconfig->PcieRpClkReqNumber));

	memcpy(silconfig->PcieRpHotPlug, cfg->pcie_rp_hotplug_enable,
	       sizeof(silconfig->PcieRpHotPlug));

	switch (cfg->serirq_mode) {
	case SERIRQ_QUIET:
		silconfig->SirqEnable = 1;
		silconfig->SirqMode = 0;
		break;
	case SERIRQ_CONTINUOUS:
		silconfig->SirqEnable = 1;
		silconfig->SirqMode = 1;
		break;
	case SERIRQ_OFF:
	default:
		silconfig->SirqEnable = 0;
		break;
	}

	if (cfg->emmc_tx_cmd_cntl != 0)
		silconfig->EmmcTxCmdCntl = cfg->emmc_tx_cmd_cntl;
	if (cfg->emmc_tx_data_cntl1 != 0)
		silconfig->EmmcTxDataCntl1 = cfg->emmc_tx_data_cntl1;
	if (cfg->emmc_tx_data_cntl2 != 0)
		silconfig->EmmcTxDataCntl2 = cfg->emmc_tx_data_cntl2;
	if (cfg->emmc_rx_cmd_data_cntl1 != 0)
		silconfig->EmmcRxCmdDataCntl1 = cfg->emmc_rx_cmd_data_cntl1;
	if (cfg->emmc_rx_strobe_cntl != 0)
		silconfig->EmmcRxStrobeCntl = cfg->emmc_rx_strobe_cntl;
	if (cfg->emmc_rx_cmd_data_cntl2 != 0)
		silconfig->EmmcRxCmdDataCntl2 = cfg->emmc_rx_cmd_data_cntl2;
	if (cfg->emmc_host_max_speed != 0)
		silconfig->eMMCHostMaxSpeed = cfg->emmc_host_max_speed;

	silconfig->LPSS_S0ixEnable = cfg->lpss_s0ix_enable;

	/* Disable monitor mwait since it is broken due to a hardware bug
	 * without a fix. Specific to Apollolake.
	 */
	if (!CONFIG(SOC_INTEL_GEMINILAKE))
		silconfig->MonitorMwaitEnable = 0;

	silconfig->SkipMpInit = !CONFIG(USE_INTEL_FSP_MP_INIT);

	/* Disable setting of EISS bit in FSP. */
	silconfig->SpiEiss = 0;

	/* Disable FSP from locking access to the RTC NVRAM */
	silconfig->RtcLock = 0;

	/* Enable Audio clk gate and power gate */
	silconfig->HDAudioClkGate = cfg->hdaudio_clk_gate_enable;
	silconfig->HDAudioPwrGate = cfg->hdaudio_pwr_gate_enable;
	/* BIOS config lockdown Audio clk and power gate */
	silconfig->BiosCfgLockDown = cfg->hdaudio_bios_config_lockdown;
	if (CONFIG(SOC_INTEL_GEMINILAKE))
		glk_fsp_silicon_init_params_cb(cfg, silconfig);
	else
		apl_fsp_silicon_init_params_cb(cfg, silconfig);

	silconfig->UsbOtg = xdci_can_enable(PCH_DEVFN_XDCI);

	silconfig->VmxEnable = CONFIG(ENABLE_VMX);

	/* Set VTD feature according to devicetree */
	silconfig->VtdEnable = get_uint_option("vtd", cfg->enable_vtd);

	silconfig->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_devfn_enabled(SA_DEVFN_IGD);

	silconfig->PavpEnable = CONFIG(PAVP);

	/* SATA config */
	if (is_devfn_enabled(PCH_DEVFN_SATA))
		silconfig->SataSalpSupport = !(cfg->DisableSataSalpSupport);

	mainboard_silicon_init_params(silconfig);
}

struct chip_operations soc_intel_apollolake_ops = {
	CHIP_NAME("Intel Apollolake SOC")
	.enable_dev = &enable_dev,
	.init = &soc_init,
	.final = &soc_final
};

static void soc_enable_untrusted_mode(void *unused)
{
	/*
	 * Set Bit 6 (ENABLE_IA_UNTRUSTED_MODE) of MSR 0x120
	 * UCODE_PCR_POWER_MISC MSR to enter IA Untrusted Mode.
	 */
	msr_set(MSR_POWER_MISC, ENABLE_IA_UNTRUSTED);
}

static void drop_privilege_all(void)
{
	/* Drop privilege level on all the CPUs */
	if (mp_run_on_all_cpus(&soc_enable_untrusted_mode, NULL) != CB_SUCCESS)
		printk(BIOS_ERR, "failed to enable untrusted mode\n");
}

static void configure_xhci_host_mode_port0(void)
{
	uint32_t *cfg0;
	uint32_t *cfg1;
	const struct resource *res;
	uint32_t reg;
	struct stopwatch sw;
	struct device *xhci_dev = PCH_DEV_XHCI;

	printk(BIOS_INFO, "Putting xHCI port 0 into host mode.\n");
	res = find_resource(xhci_dev, PCI_BASE_ADDRESS_0);
	cfg0 = (void *)(uintptr_t)(res->base + DUAL_ROLE_CFG0);
	cfg1 = (void *)(uintptr_t)(res->base + DUAL_ROLE_CFG1);
	reg = read32(cfg0);
	if (!(reg & SW_IDPIN_EN_MASK))
		return;

	reg &= ~(SW_IDPIN_MASK | SW_VBUS_VALID_MASK);
	write32(cfg0, reg);

	stopwatch_init_msecs_expire(&sw, 10);
	/* Wait for the host mode status bit. */
	while ((read32(cfg1) & DRD_MODE_MASK) != DRD_MODE_HOST) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "Timed out waiting for host mode.\n");
			return;
		}
	}

	printk(BIOS_INFO, "xHCI port 0 host switch over took %lu ms\n",
		stopwatch_duration_msecs(&sw));
}

static int check_xdci_enable(void)
{
	return is_dev_enabled(pcidev_path_on_root(PCH_DEVFN_XDCI));
}

static void disable_xhci_lfps_pm(void)
{
	struct soc_intel_apollolake_config *cfg;

	cfg = config_of_soc();

	if (cfg->disable_xhci_lfps_pm) {
		void *addr;
		const struct resource *res;
		uint32_t reg;
		struct device *xhci_dev = PCH_DEV_XHCI;

		res = find_resource(xhci_dev, PCI_BASE_ADDRESS_0);
		addr = (void *)(uintptr_t)(res->base + CFG_XHCPMCTRL);
		reg = read32(addr);
		printk(BIOS_DEBUG, "XHCI PM: control reg=0x%x.\n", reg);
		if (reg) {
			reg &= LFPS_PM_DISABLE_MASK;
			write32(addr, reg);
			printk(BIOS_INFO, "XHCI PM: Disable xHCI LFPS as configured in devicetree.\n");
		}
	}
}

void platform_fsp_notify_status(enum fsp_notify_phase phase)
{
	if (phase == END_OF_FIRMWARE) {

		/*
		 * Before hiding P2SB device and dropping privilege level,
		 * dump CSE status and disable HECI1 interface.
		 */
		heci_cse_lockdown();

		/* Hide the P2SB device to align with previous behavior. */
		p2sb_hide();

		/*
		 * As per guidelines BIOS is recommended to drop CPU privilege
		 * level to IA_UNTRUSTED. After that certain device registers
		 * and MSRs become inaccessible supposedly increasing system
		 * security.
		 */
		drop_privilege_all();

		/*
		 * When USB OTG is set, GLK FSP enables xHCI SW ID pin and
		 * configures USB-C as device mode. Force USB-C into host mode.
		 */
		if (check_xdci_enable())
			configure_xhci_host_mode_port0();

		/*
		 * Override GLK xhci clock gating register(XHCLKGTEN) to
		 * mitigate USB device suspend and resume failure.
		 */
		if (CONFIG(SOC_INTEL_GEMINILAKE)) {
			uint32_t *cfg;
			const struct resource *res;
			uint32_t reg;
			struct device *xhci_dev = PCH_DEV_XHCI;

			res = find_resource(xhci_dev, PCI_BASE_ADDRESS_0);
			cfg = (void *)(uintptr_t)(res->base + CFG_XHCLKGTEN);
			reg = SRAMPGTEN | SSLSE | USB2PLLSE | IOSFSTCGE |
				HSTCGE | HSUXDMIPLLSE | SSTCGE | XHCFTCLKSE |
				XHCBBTCGIPISO | XHCUSB2PLLSDLE | SSPLLSUE |
				XHCBLCGE | HSLTCGE | SSLTCGE | IOSFBTCGE |
				IOSFGBLCGE;
			write32(cfg, reg);
		}

		/* Disable XHCI LFPS power management if the option in dev tree is set. */
		disable_xhci_lfps_pm();
	}
}

/*
 * spi_flash init() needs to run unconditionally on every boot (including
 * resume) to allow write protect to be disabled for eventlog and nvram
 * updates. This needs to be done as early as possible in ramstage. Thus, add a
 * callback for entry into BS_PRE_DEVICE.
 */
static void spi_flash_init_cb(void *unused)
{
	fast_spi_init();
}

__weak
void mainboard_silicon_init_params(FSP_S_CONFIG *silconfig)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Handle FSP logo params */
void soc_load_logo(FSPS_UPD *supd)
{
	bmp_load_logo(&supd->FspsConfig.LogoPtr, &supd->FspsConfig.LogoSize);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, spi_flash_init_cb, NULL);
