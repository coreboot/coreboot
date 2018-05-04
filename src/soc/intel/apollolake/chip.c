/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 - 2017 Intel Corp.
 * Copyright (C) 2017 Siemens AG
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <bootstate.h>
#include <cbmem.h>
#include <compiler.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/acpi.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/msr.h>
#include <intelblocks/xdci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/itss.h>
#include <intelblocks/pmclib.h>
#include <romstage_handoff.h>
#include <soc/iomap.h>
#include <soc/itss.h>
#include <soc/intel/common/vbt.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <spi-generic.h>
#include <soc/cpu.h>
#include <soc/pm.h>
#include <soc/systemagent.h>

#include "chip.h"

static const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	/* DSDT: acpi/northbridge.asl */
	case SA_DEVFN_ROOT:
		return "MCHC";
	/* DSDT: acpi/lpc.asl */
	case PCH_DEVFN_LPC:
		return "LPCB";
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

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources = pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.enable_resources = NULL,
	.init = NULL,
	.scan_bus = pci_domain_scan_bus,
	.acpi_name = &soc_acpi_name,
};

static struct device_operations cpu_bus_ops = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init = apollolake_init_cpus,
	.scan_bus = NULL,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
};

static void enable_dev(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
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
	device_t func0;
	unsigned int devfn;
	int i;
	unsigned int inc = PCI_DEVFN(0, 1);

	func0 = dev_find_slot(0, devfn0);
	if (func0 == NULL)
		return;

	/* No more functions if function 0 is disabled. */
	if (pci_read_config32(func0, PCI_VENDOR_ID) == 0xffffffff)
		return;

	devfn = devfn0 + inc;

	/*
	 * Increase funtion by 1.
	 * Then find first enabled device to replace func0
	 * as that port was move to func0.
	 */
	for (i = 1; i < num_funcs; i++, devfn += inc) {
		device_t dev = dev_find_slot(0, devfn);
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

/* Configure package power limits */
static void set_power_limits(void)
{
	static struct soc_intel_apollolake_config *cfg;
	struct device *dev = SA_DEV_ROOT;
	msr_t rapl_msr_reg, limit;
	uint32_t power_unit;
	uint32_t tdp, min_power, max_power;
	uint32_t pl2_val;

	if (IS_ENABLED(CONFIG_APL_SKIP_SET_POWER_LIMITS)) {
		printk(BIOS_INFO, "Skip the RAPL settings.\n");
		return;
	}

	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}

	cfg = dev->chip_info;

	/* Get units */
	rapl_msr_reg = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 1 << (rapl_msr_reg.lo & 0xf);

	/* Get power defaults for this SKU */
	rapl_msr_reg = rdmsr(MSR_PKG_POWER_SKU);
	tdp = rapl_msr_reg.lo & PKG_POWER_LIMIT_MASK;
	pl2_val = rapl_msr_reg.hi & PKG_POWER_LIMIT_MASK;
	min_power = (rapl_msr_reg.lo >> 16) & PKG_POWER_LIMIT_MASK;
	max_power = rapl_msr_reg.hi & PKG_POWER_LIMIT_MASK;

	if (min_power > 0 && tdp < min_power)
		tdp = min_power;

	if (max_power > 0 && tdp > max_power)
		tdp = max_power;

	/* Set PL1 override value */
	tdp = (cfg->tdp_pl1_override_mw == 0) ?
		tdp : (cfg->tdp_pl1_override_mw * power_unit) / 1000;
	/* Set PL2 override value */
	pl2_val = (cfg->tdp_pl2_override_mw == 0) ?
		pl2_val : (cfg->tdp_pl2_override_mw * power_unit) / 1000;

	/* Set long term power limit to TDP */
	limit.lo = tdp & PKG_POWER_LIMIT_MASK;
	/* Set PL1 Pkg Power clamp bit */
	limit.lo |= PKG_POWER_LIMIT_CLAMP;

	limit.lo |= PKG_POWER_LIMIT_EN;
	limit.lo |= (MB_POWER_LIMIT1_TIME_DEFAULT &
		PKG_POWER_LIMIT_TIME_MASK) << PKG_POWER_LIMIT_TIME_SHIFT;

	/* Set short term power limit PL2 */
	limit.hi = pl2_val & PKG_POWER_LIMIT_MASK;
	limit.hi |= PKG_POWER_LIMIT_EN;

	/* Program package power limits in RAPL MSR */
	wrmsr(MSR_PKG_POWER_LIMIT, limit);
	printk(BIOS_INFO, "RAPL PL1 %d.%dW\n", tdp / power_unit,
				100 * (tdp % power_unit) / power_unit);
	printk(BIOS_INFO, "RAPL PL2 %d.%dW\n", pl2_val / power_unit,
				100 * (pl2_val % power_unit) / power_unit);

	/* Setting RAPL MMIO register for Power limits.
	* RAPL driver is using MSR instead of MMIO.
	* So, disabled LIMIT_EN bit for MMIO. */
	MCHBAR32(MCHBAR_RAPL_PPL) = limit.lo & ~PKG_POWER_LIMIT_EN;
	MCHBAR32(MCHBAR_RAPL_PPL + 4) =  limit.hi & ~PKG_POWER_LIMIT_EN;
}

/* Overwrites the SCI IRQ if another IRQ number is given by device tree. */
static void set_sci_irq(void)
{
	static struct soc_intel_apollolake_config *cfg;
	struct device *dev = SA_DEV_ROOT;
	uint32_t scis;

	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}

	cfg = dev->chip_info;

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
	struct global_nvs_t *gnvs;

	/* Snapshot the current GPIO IRQ polarities. FSP is setting a
	 * default policy that doesn't honor boards' requirements. */
	itss_snapshot_irq_polarities(GPIO_IRQ_START, GPIO_IRQ_END);

	fsp_silicon_init(romstage_handoff_is_resume());

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

	/* Allocate ACPI NVS in CBMEM */
	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(*gnvs));

	/* Set RAPL MSR for Package power limits*/
	set_power_limits();

	/*
	* FSP-S routes SCI to IRQ 9. With the help of this function you can
	* select another IRQ for SCI.
	*/
	set_sci_irq();
}

static void soc_final(void *data)
{
	/* Disable global reset, just in case */
	pmc_global_reset_enable(0);
	/* Make sure payload/OS can't trigger global reset */
	pmc_global_reset_lock();
}

static void disable_dev(struct device *dev, FSP_S_CONFIG *silconfig)
{
	switch (dev->path.pci.devfn) {
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
	default:
		printk(BIOS_WARNING, "PCI:%02x.%01x: Could not disable the device\n",
			PCI_SLOT(dev->path.pci.devfn),
			PCI_FUNC(dev->path.pci.devfn));
		break;
	}
}

static void parse_devicetree(FSP_S_CONFIG *silconfig)
{
	struct device *dev = SA_DEV_ROOT;

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
#if !IS_ENABLED(CONFIG_SOC_INTEL_GLK) /* GLK FSP does not have these
					 fields in FspsUpd.h yet */
	uint8_t port;

	for (port = 0; port < APOLLOLAKE_USB2_PORT_MAX; port++) {
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
#endif
}

static void glk_fsp_silicon_init_params_cb(
	struct soc_intel_apollolake_config *cfg, FSP_S_CONFIG *silconfig)
{
	silconfig->Gmm = 0;

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
}

void __weak mainboard_devtree_update(struct device *dev)
{
       /* Override dev tree settings per board */
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	FSP_S_CONFIG *silconfig = &silupd->FspsConfig;
	static struct soc_intel_apollolake_config *cfg;

	/* Load VBT before devicetree-specific config. */
	silconfig->GraphicsConfigPtr = (uintptr_t)vbt_get();

	struct device *dev = SA_DEV_ROOT;

	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}

	mainboard_devtree_update(dev);

	cfg = dev->chip_info;

	/* Parse device tree and disable unused device*/
	parse_devicetree(silconfig);

	memcpy(silconfig->PcieRpClkReqNumber, cfg->pcie_rp_clkreq_pin,
	       sizeof(silconfig->PcieRpClkReqNumber));

	memcpy(silconfig->PcieRpHotPlug, cfg->pcie_rp_hotplug_enable,
	       sizeof(silconfig->PcieRpHotPlug));

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

	silconfig->LPSS_S0ixEnable = cfg->lpss_s0ix_enable;

	/* Disable monitor mwait since it is broken due to a hardware bug
	 * without a fix. Specific to Apollolake.
	 */
	if (!IS_ENABLED(CONFIG_SOC_INTEL_GLK))
		silconfig->MonitorMwaitEnable = 0;

	silconfig->SkipMpInit = 1;

	/* Disable setting of EISS bit in FSP. */
	silconfig->SpiEiss = 0;

	/* Disable FSP from locking access to the RTC NVRAM */
	silconfig->RtcLock = 0;

	/* Enable Audio clk gate and power gate */
	silconfig->HDAudioClkGate = cfg->hdaudio_clk_gate_enable;
	silconfig->HDAudioPwrGate = cfg->hdaudio_pwr_gate_enable;
	/* Bios config lockdown Audio clk and power gate */
	silconfig->BiosCfgLockDown = cfg->hdaudio_bios_config_lockdown;
	if (IS_ENABLED(CONFIG_SOC_INTEL_GLK))
		glk_fsp_silicon_init_params_cb(cfg, silconfig);
	else
		apl_fsp_silicon_init_params_cb(cfg, silconfig);

	/* Enable xDCI controller if enabled in devicetree and allowed */
	dev = dev_find_slot(0, PCH_DEVFN_XDCI);
	if (!xdci_can_enable())
		dev->enabled = 0;
	silconfig->UsbOtg = dev->enabled;
}

struct chip_operations soc_intel_apollolake_ops = {
	CHIP_NAME("Intel Apollolake SOC")
	.enable_dev = &enable_dev,
	.init = &soc_init,
	.final = &soc_final
};

static void drop_privilege_all(void)
{
	/* Drop privilege level on all the CPUs */
	if (mp_run_on_all_cpus(&cpu_enable_untrusted_mode, 1000) < 0)
		printk(BIOS_ERR, "failed to enable untrusted mode\n");
}

void platform_fsp_notify_status(enum fsp_notify_phase phase)
{
	if (phase == END_OF_FIRMWARE) {
		/* Hide the P2SB device to align with previous behavior. */
		p2sb_hide();
		/*
		 * As per guidelines BIOS is recommended to drop CPU privilege
		 * level to IA_UNTRUSTED. After that certain device registers
		 * and MSRs become inaccessible supposedly increasing system
		 * security.
		 */
		drop_privilege_all();
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

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, spi_flash_init_cb, NULL);
