/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <console/console.h>
#include <console/debug.h>
#include <cpu/x86/mp.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/acpi.h>
#include <intelblocks/gpio.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/p2sb.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/cpu.h>
#include <soc/pch.h>
#include <soc/soc_pch.h>
#include <soc/ramstage.h>
#include <soc/p2sb.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <soc/pci_devs.h>

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	mainboard_silicon_init_params(silupd);
}

#if CONFIG(HAVE_ACPI_TABLES)
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
	.write_acpi_tables  = &northbridge_write_acpi_tables,
	.acpi_name        = soc_acpi_name
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = cpx_init_cpus,
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

static void iio_write_mask(u16 bus, u16 dev, u8 func)
{
	pci_devfn_t device = PCI_DEV(bus, dev, func);
	u32 val = pci_s_read_config32(device, IIO_XPUNCCERRMSK_REG);
	val |= (SENT_PCIE_UNSUPP_MASK | RCVD_PCIE_CA_STS_MASK | RCVD_PCIE_UR_STS_MASK);
	pci_s_write_config32(device, IIO_XPUNCCERRMSK_REG, val);

	val = pci_s_read_config32(device, RP_UNCERRMSK);
	val |= (SURPRISE_DWN_ERR_MSK | UNSUPPORTED_REQ_ERR_MSK);
	pci_s_write_config32(device, RP_UNCERRMSK, val);
}

static void iio_dmi_en_masks(void)
{
	pci_devfn_t device;
	u32 val;
	device = PCI_DEV(DMI_BUS_INDEX, DMI_DEV, DMI_FUNC);
	val = pci_s_read_config32(device, IIO_XPUNCCERRMSK_REG);
	val |= (SENT_PCIE_UNSUPP_MASK | RCVD_PCIE_CA_STS_MASK | RCVD_PCIE_UR_STS_MASK);
	pci_s_write_config32(device, IIO_XPUNCCERRMSK_REG, val);

	val = pci_s_read_config32(device, DMI_UNCERRMSK);
	val |= (ECRC_ERR | MLFRMD_TLP | RCV_BUF_OVRFLOW | FLOW_CNTR | POISON_TLP | DLL_PRT_ERR);
	pci_s_write_config32(device, DMI_UNCERRMSK, val);
}

static void iio_enable_masks(void)
{
	struct iiostack_resource iio = {0};
	get_iiostack_info(&iio);
	int i, k;
	for (i = 0; i < iio.no_of_stacks; i++) {
		const STACK_RES *st = &iio.res[i];
		if (st->BusBase > 0 && st->BusBase != 0xff) {
			for (k = 0; k < DEVICES_PER_IIO_STACK; k++) {
				printk(BIOS_DEBUG, "%s: bus:%x dev:%x func:%x\n", __func__,
					st->BusBase, k, 0);
				iio_write_mask(st->BusBase, k, 0);
			}
		}
	}
	iio_dmi_en_masks();
}

static void set_pcu_locks(void)
{
	for (uint32_t socket = 0; socket < soc_get_num_cpus(); ++socket) {
		uint32_t bus = get_socket_stack_busno(socket, PCU_IIO_STACK);

		/* configure PCU_CR0_FUN csrs */
		const struct device *cr0_dev = PCU_DEV_CR0(bus);
		pci_or_config32(cr0_dev, PCU_CR0_P_STATE_LIMITS, P_STATE_LIMITS_LOCK);
		pci_or_config32(cr0_dev, PCU_CR0_PACKAGE_RAPL_LIMIT_UPR, PKG_PWR_LIM_LOCK_UPR);
		pci_or_config32(cr0_dev, PCU_CR0_TURBO_ACTIVATION_RATIO, TURBO_ACTIVATION_RATIO_LOCK);


		/* configure PCU_CR1_FUN csrs */
		const struct device *cr1_dev = PCU_DEV_CR1(bus);
		pci_or_config32(cr1_dev, PCU_CR1_SAPMCTL, SAPMCTL_LOCK_MASK);

		/* configure PCU_CR2_FUN csrs */
		const struct device *cr2_dev = PCU_DEV_CR2(bus);
		pci_or_config32(cr2_dev, PCU_CR2_DRAM_PLANE_POWER_LIMIT, PP_PWR_LIM_LOCK);
		pci_or_config32(cr2_dev, PCU_CR2_DRAM_POWER_INFO_UPR, DRAM_POWER_INFO_LOCK_UPR);

		/* configure PCU_CR3_FUN csrs */
		const struct device *cr3_dev = PCU_DEV_CR3(bus);
		pci_or_config32(cr3_dev, PCU_CR3_CONFIG_TDP_CONTROL, TDP_LOCK);
		pci_or_config32(cr3_dev, PCU_CR3_FLEX_RATIO, OC_LOCK);
	}

}

static void set_imc_locks(void)
{
	struct device *dev = 0;
	while ((dev = dev_find_device(PCI_VID_INTEL, IMC_M2MEM_DEVID, dev)))
		pci_or_config32(dev, IMC_M2MEM_TIMEOUT, TIMEOUT_LOCK);
}

static void set_upi_locks(void)
{
	struct device *dev = 0;
	while ((dev = dev_find_device(PCI_VID_INTEL, UPI_LL_CR_DEVID, dev)))
		pci_or_config32(dev, UPI_LL_CR_KTIMISCMODLCK, KTIMISCMODLCK_LOCK);
}

static void chip_final(void *data)
{
	/* Lock SBI */
	pci_or_config32(PCH_DEV_P2SB, P2SBC, SBILOCK);

	/* LOCK PAM */
	pci_or_config32(pcidev_path_on_root(PCI_DEVFN(0, 0)), 0x80, 1 << 0);

	set_pcu_locks();
	set_imc_locks();
	set_upi_locks();

	p2sb_hide();
	iio_enable_masks();
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
}

struct chip_operations soc_intel_xeon_sp_cpx_ops = {
	CHIP_NAME("Intel Cooper Lake-SP")
	.enable_dev = chip_enable_dev,
	.init = chip_init,
	.final = chip_final,
};
