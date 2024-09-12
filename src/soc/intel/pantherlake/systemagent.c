/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/msr.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/systemagent.h>
#include <soc/iomap.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>

/*
 * SoC implementation
 *
 * Add all known fixed memory ranges for Host Controller/Memory
 * controller.
 */
void soc_add_fixed_mmio_resources(struct device *dev, int *index)
{
	static const struct sa_mmio_descriptor soc_fixed_resources[] = {
		{ MCHBAR, MCH_BASE_ADDRESS, MCH_BASE_SIZE, "MCHBAR" },
		{ SAFBAR, SAF_BASE_ADDRESS, SAF_BASE_SIZE, "SAFBAR" },
		{ EPBAR, EP_BASE_ADDRESS, EP_BASE_SIZE, "EPBAR" },
		{ REGBAR, REG_BASE_ADDRESS, REG_BASE_SIZE, "REGBAR" },
		/* First field (sa_mmio_descriptor.index) is not used, setting to 0: */
		{ 0, CRAB_ABORT_BASE_ADDR, CRAB_ABORT_SIZE, "CRAB_ABORT" },
		{ 0, LT_SECURITY_BASE_ADDR, LT_SECURITY_SIZE, "LT_SECURITY" },
		{ 0, IO_APIC_ADDR, APIC_SIZE, "APIC" },
		{ 0, PCH_PRESERVED_BASE_ADDRESS, PCH_PRESERVED_BASE_SIZE, "PCH_RESERVED" },
	};

	sa_add_fixed_mmio_resources(dev, index, soc_fixed_resources,
			ARRAY_SIZE(soc_fixed_resources));

	/* Add Vt-d resources if VT-d is enabled */
	if ((pci_read_config32(dev, CAPID0_A) & VTD_DISABLE))
		return;

	sa_add_fixed_mmio_resources(dev, index, soc_vtd_resources,
			ARRAY_SIZE(soc_vtd_resources));
}

/*
 * Set MMIO resource's fields
 */
static void set_mmio_resource(
	struct sa_mmio_descriptor *resource,
	uint64_t base,
	uint64_t size,
	const char *description)
{
	if (resource == NULL) {
		printk(BIOS_ERR, "%s: argument resource is NULL for %s\n",
		__func__, description);
		return;
	}
	resource->base = base;
	resource->size = size;
	resource->description = description;
}

int soc_get_uncore_prmmr_base_and_mask(uint64_t *prmrr_base,
	uint64_t *prmrr_mask)
{
	msr_t msr;
	msr = rdmsr(MSR_PRMRR_BASE_0);
	*prmrr_base = (uint64_t)msr.hi << 32 | msr.lo;
	msr = rdmsr(MSR_PRMRR_PHYS_MASK);
	*prmrr_mask = (uint64_t)msr.hi << 32 | msr.lo;
	return 0;
}

/*
 * SoC implementation
 *
 * Add all known configurable memory ranges for Host Controller/Memory
 * controller.
 */
void soc_add_configurable_mmio_resources(struct device *dev, int *resource_cnt)
{
	uint64_t size, base, tseg_base;
	int count = 0;
	struct sa_mmio_descriptor cfg_rsrc[6]; /* Increase size when adding more resources */

	/* MMCONF */
	size = sa_get_mmcfg_size();
	if (size > 0)
		set_mmio_resource(&(cfg_rsrc[count++]), CONFIG_ECAM_MMCONF_BASE_ADDRESS,
			size, "MMCONF");

	/* DSM */
	size = sa_get_dsm_size();
	if (size > 0) {
		base = pci_read_config32(dev, BDSM) & 0xFFF00000;
		set_mmio_resource(&(cfg_rsrc[count++]), base, size, "DSM");
	}

	/* TSEG */
	size = sa_get_tseg_size();
	tseg_base = sa_get_tseg_base();
	if (size > 0)
		set_mmio_resource(&(cfg_rsrc[count++]), tseg_base, size, "TSEG");

	/* PMRR */
	size = get_valid_prmrr_size();
	if (size > 0) {
		uint64_t mask;
		if (soc_get_uncore_prmmr_base_and_mask(&base, &mask) == 0) {
			base &= mask;
			set_mmio_resource(&(cfg_rsrc[count++]), base, size, "PMRR");
		} else {
			printk(BIOS_ERR, "SA: Failed to get PRMRR base and mask\n");
		}
	}

	/* GSM */
	size = sa_get_gsm_size();
	if (size > 0) {
		base = sa_get_gsm_base();
		set_mmio_resource(&(cfg_rsrc[count++]), base, size, "GSM");
	}

	/* DPR */
	size = sa_get_dpr_size();
	if (size > 0) {
		/* DPR just below TSEG: */
		base = tseg_base - size;
		set_mmio_resource(&(cfg_rsrc[count++]), base, size, "DPR");
	}

	/* Add all the above */
	sa_add_fixed_mmio_resources(dev, resource_cnt, cfg_rsrc, count);
}

static void configure_tdp(struct device *dev)
{
	struct soc_power_limits_config *soc_config;
	struct device *sa;
	uint16_t sa_pci_id;
	u8 tdp;
	size_t i;
	bool config_tdp = false;
	config_t *config;

	config = config_of_soc();

	/* Get System Agent PCI ID */
	sa = pcidev_path_on_root(PCI_DEVFN_ROOT);
	sa_pci_id = sa ? pci_read_config16(sa, PCI_DEVICE_ID) : 0xFFFF;

	if (sa_pci_id == 0xFFFF) {
		printk(BIOS_WARNING, "Unknown SA PCI Device!\n");
		return;
	}

	tdp = get_cpu_tdp();

	/*
	 * Choose power limits configuration based on the CPU SA PCI ID and
	 * CPU TDP value.
	 */
	for (i = 0; i < ARRAY_SIZE(cpuid_to_ptl); i++) {
		if (sa_pci_id == cpuid_to_ptl[i].cpu_id &&
				tdp == cpuid_to_ptl[i].cpu_tdp) {
			soc_config = &config->power_limits_config[cpuid_to_ptl[i].limits];
			set_power_limits(MOBILE_SKU_PL1_TIME_SEC, soc_config);
			config_tdp = true;
			printk(BIOS_DEBUG, "Configured power limits for SA PCI ID: 0x%4x\n",
				sa_pci_id);
			break;
		}
	}

	if (!config_tdp) {
		printk(BIOS_WARNING, "Skipped power limits configuration for SA PCI ID: 0x%4x\n",
			sa_pci_id);
		return;
	}
}

/*
 * SoC implementation
 *
 * Perform System Agent Initialization during ramstage phase.
 */
void soc_systemagent_init(struct device *dev)
{
	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	/* Configure TDP */
	configure_tdp(dev);
}

uint32_t soc_systemagent_max_chan_capacity_mib(u8 capid0_a_ddrsz)
{
	switch (capid0_a_ddrsz) {
	case 1:
		return 8192;
	case 2:
		return 4096;
	case 3:
		return 2048;
	default:
		return 65536;
	}
}
