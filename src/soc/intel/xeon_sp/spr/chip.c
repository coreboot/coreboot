/* SPDX-License-Identifier: GPL-2.0-only */
#include <acpi/acpigen_pci.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <console/debug.h>
#include <cpu/x86/lapic.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pciexp.h>
#include <intelblocks/acpi.h>
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
		/* domain ops are assigned at their creation */
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_GPIO) {
		block_gpio_enable(dev);
	}
}

static void chip_final(void *data)
{
	/* Lock SBI */
	pci_or_config32(PCH_DEV_P2SB, P2SBC, SBILOCK);

	/* LOCK PAM */
	pci_or_config32(pcidev_path_on_root(PCI_DEVFN(0, 0)), 0x80, 1 << 0);

	tco_lockdown();

	p2sb_hide();

	/* Accessing xHCI CSR needs to be done after PCI enumeration. */
	lock_oc_cfg(false);
	mainboard_override_usb_oc();
	lock_oc_cfg(true);
	/* Disable CPU Crashlog to avoid conflict between CPU Crashlog and BMC ACD. */
	disable_cpu_crashlog();
}

static void chip_init(void *data)
{
	printk(BIOS_DEBUG, "coreboot: calling fsp_silicon_init\n");
	fsp_silicon_init();

	setup_pds();
	attach_iio_stacks();

	override_hpet_ioapic_bdf();
	pch_enable_ioapic();
	pch_lock_dmictl();
	p2sb_unhide();
	lock_gpio(false);
	mainboard_override_fsp_gpio();
	lock_gpio(true);
}

struct chip_operations soc_intel_xeon_sp_spr_ops = {
	.name = "Intel SapphireRapids-SP",
	.enable_dev = chip_enable_dev,
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
		if (pds.pds[i].pd_type != PD_TYPE_GENERIC_INITIATOR)
			continue;
		ep_bus = PCI_BDF(pds.pds[i].dev) >> 20;
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
