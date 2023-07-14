/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <console/console.h>
#include <console/debug.h>
#include <cpu/x86/lapic.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pciexp.h>
#include <intelblocks/gpio.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/pcr.h>
#include <intelblocks/tco.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/crashlog.h>
#include <soc/numa.h>
#include <soc/p2sb.h>
#include <soc/pch.h>
#include <soc/soc_pch.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <soc/xhci.h>

__weak void mainboard_silicon_init_params(FSPS_UPD *params)
{

}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	mainboard_silicon_init_params(silupd);
}

#if CONFIG(HAVE_ACPI_TABLES)
const char *soc_acpi_name(const struct device *dev);
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
	.write_acpi_tables = &northbridge_write_acpi_tables,
	.acpi_name = soc_acpi_name
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = mp_cpu_bus_init,
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
	} else if (dev->path.type == DEVICE_PATH_GPIO) {
		block_gpio_enable(dev);
	}
}

static void pcu_pci_or_config32(u8 bus, u8 func, u32 reg, u32 orval)
{
	u32 data;
	const uint32_t pcie_offset = PCI_DEV(bus, PCU_DEV, func);

	data = pci_s_read_config32(pcie_offset, reg);
	data |= orval;
	pci_s_write_config32(pcie_offset, reg, data);
}

static void set_pcu_locks(void)
{
	for (uint32_t socket = 0; socket < CONFIG_MAX_SOCKET; ++socket) {
		if (!soc_cpu_is_enabled(socket))
			continue;
		const uint32_t bus = get_ubox_busno(socket, UNCORE_BUS_1);

		/* configure PCU_CR0_FUN csrs */
		pcu_pci_or_config32(bus, PCU_CR0_FUN, PCU_CR0_P_STATE_LIMITS,
				    P_STATE_LIMITS_LOCK);
		pcu_pci_or_config32(bus, PCU_CR0_FUN, PCU_CR0_PACKAGE_RAPL_LIMIT_UPR,
				    PKG_PWR_LIM_LOCK_UPR);
		pcu_pci_or_config32(bus, PCU_CR0_FUN, PCU_CR0_TURBO_ACTIVATION_RATIO,
				    TURBO_ACTIVATION_RATIO_LOCK);

		/* configure PCU_CR2_FUN csrs */
		pcu_pci_or_config32(bus, PCU_CR2_FUN, PCU_CR2_DRAM_POWER_INFO_UPR,
				    DRAM_POWER_INFO_LOCK_UPR);
		pcu_pci_or_config32(bus, PCU_CR2_FUN, PCU_CR2_DRAM_PLANE_POWER_LIMIT_UPR,
				    PP_PWR_LIM_LOCK_UPR);

		/* configure PCU_CR3_FUN csrs */
		pcu_pci_or_config32(bus, PCU_CR3_FUN, PCU_CR3_CONFIG_TDP_CONTROL, TDP_LOCK);

		/* configure PCU_CR6_FUN csrs */
		pcu_pci_or_config32(bus, PCU_CR6_FUN, PCU_CR6_PLATFORM_RAPL_LIMIT_CFG_UPR,
				    PLT_PWR_LIM_LOCK_UPR);
		pcu_pci_or_config32(bus, PCU_CR6_FUN, PCU_CR6_PLATFORM_POWER_INFO_CFG_UPR,
				    PLT_PWR_INFO_LOCK_UPR);
	}
}

static void chip_final(void *data)
{
	/* Lock SBI */
	pci_or_config32(PCH_DEV_P2SB, P2SBC, SBILOCK);

	/* LOCK PAM */
	pci_or_config32(pcidev_path_on_root(PCI_DEVFN(0, 0)), 0x80, 1 << 0);

	set_pcu_locks();
	tco_lockdown();

	p2sb_hide();

	/* Accessing xHCI CSR needs to be done after PCI enumeration. */
	lock_oc_cfg(false);
	mainboard_override_usb_oc();
	lock_oc_cfg(true);
	/* Disable CPU Crashlog to avoid conflict between CPU Crashlog and BMC ACD. */
	disable_cpu_crashlog();

	set_bios_init_completion();
}

static void chip_init(void *data)
{
	printk(BIOS_DEBUG, "coreboot: calling fsp_silicon_init\n");
	fsp_silicon_init();
	override_hpet_ioapic_bdf();
	pch_enable_ioapic();
	pch_lock_dmictl();
	p2sb_unhide();
	lock_gpio(false);
	mainboard_override_fsp_gpio();
	lock_gpio(true);
}

struct chip_operations soc_intel_xeon_sp_spr_ops = {
	CHIP_NAME("Intel SapphireRapids-SP").enable_dev = chip_enable_dev,
	.init = chip_init,
	.final = chip_final,
};

void lock_gpio(bool lock)
{
	if (lock) {
		pcr_write32(gpio_get_pad_portid(GPPC_B0), PAD_CFG_LOCK_B, 0xffffffff);
		pcr_write32(gpio_get_pad_portid(GPP_D0), PAD_CFG_LOCK_D, 0xffffffff);
	} else {
		pcr_write32(gpio_get_pad_portid(GPPC_B0), PAD_CFG_LOCK_B, 0);
		pcr_write32(gpio_get_pad_portid(GPP_D0), PAD_CFG_LOCK_D, 0);
	}
}

/* Root Complex Event Collector */
static void rcec_init(struct device *dev)
{
	/* Set up RCEC EA extended capability, section 7.9.10 of PCIe 5.0 spec */
	const unsigned int rcecea_cap =
		pciexp_find_extended_cap(dev, PCIE_EXT_CAP_RCECEA_ID, 0);
	if (!rcecea_cap)
		return;

	pci_devfn_t ecrc_bdf = PCI_BDF(dev);
	uint32_t ecrc_bus = (ecrc_bdf >> 20) & 0xFFF;
	uint32_t ecrc_dev = (ecrc_bdf >> 15) & 0x1F;

	/*
	 * Find all CXL devices, and match them with RCEC.
	 * With CXL 1.1, the bus# of CXL device (RCiEP) is 1 bigger than
	 * the bus# of RCEC.
	 */
	uint32_t ep_bus;
	uint8_t i;
	for (i = 0; i < pds.num_pds; i++) {
		if (pds.pds[i].pd_type == PD_TYPE_PROCESSOR)
			continue;
		ep_bus = pds.pds[i].device_handle >> 20;
		if (ep_bus == ecrc_bus + 1)
			break;
	}
	if (i == pds.num_pds)
		return;

	printk(BIOS_DEBUG, "ep_bus: %x, ecrc_dev: %x\n", ep_bus, ecrc_dev);
	u32 rcecea_bitmap = 0x1 << ecrc_dev;
	u32 rcecea_busnum = (ep_bus << 8) | (ep_bus << 16);
	pci_write_config32(dev, rcecea_cap + PCI_RCECEA_BITMAP, rcecea_bitmap);
	pci_write_config32(dev, rcecea_cap + PCI_RCECEA_BUSNUM, rcecea_busnum);
}

#define SPR_IEH	0x0b23

static const unsigned short rcec_ids[] = {
	SPR_IEH,
	0
};

static struct device_operations rcec_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = rcec_init,
	.ops_pci = &soc_pci_ops,
};

static const struct pci_driver rcec_driver __pci_driver = {
	.ops = &rcec_ops,
	.vendor = PCI_VID_INTEL,
	.devices = rcec_ids,
};
