/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Alder Lake Processor SA Datasheet
 * Document number: 619503
 * Chapter number: 3
 */

#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <delay.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/msr.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/systemagent.h>
#include <soc/iomap.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>
#include <spi_flash.h>
#include "stddef.h"

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
		{ DMIBAR, DMI_BASE_ADDRESS, DMI_BASE_SIZE, "DMIBAR" },
		{ EPBAR, EP_BASE_ADDRESS, EP_BASE_SIZE, "EPBAR" },
		{ REGBAR, REG_BASE_ADDRESS, REG_BASE_SIZE, "REGBAR" },
		{ EDRAMBAR, EDRAM_BASE_ADDRESS, EDRAM_BASE_SIZE, "EDRAMBAR" },

		/* first field (sa_mmio_descriptor.index) is not used, setting to 0: */
		{ 0, CRAB_ABORT_BASE_ADDR, CRAB_ABORT_SIZE, "CRAB_ABORT" },
		{ 0, TPM_BASE_ADDRESS, TPM_SIZE, "TPM" },
		{ 0, LT_SECURITY_BASE_ADDR, LT_SECURITY_SIZE, "LT_SECURITY" },
		{ 0, IO_APIC_ADDR, APIC_SIZE, "APIC" },
		// PCH_PRESERVERD covers:
		// TraceHub SW BAR, SBREG, PMC MBAR, SPI BAR0, SerialIo BAR in ACPI mode
		// eSPI LGMR BAR, eSPI2 SEGMR BAR, TraceHub MTB BAR, TraceHub FW BAR
		// see fsp/ClientOneSiliconPkg/Fru/AdlPch/Include/PchReservedResourcesAdpP.h
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
 * set MMIO resource's fields
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
	size = get_mmcfg_size(dev);
	if (size > 0)
		set_mmio_resource(&(cfg_rsrc[count++]), CONFIG_ECAM_MMCONF_BASE_ADDRESS,
			size, "MMCONF");

	/* DSM */
	size = get_dsm_size(dev);
	if (size > 0) {
		base = pci_read_config32(dev, DSM_BASE_ADDR_REG) & 0xFFF00000;
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
	size = get_gsm_size(dev);
	if (size > 0) {
		base = sa_get_gsm_base();
		set_mmio_resource(&(cfg_rsrc[count++]), base, size, "GSM");
	}

	/* DPR */
	size = get_dpr_size(dev);
	if (size > 0) {
		/* DPR just below TSEG: */
		base = tseg_base - size;
		set_mmio_resource(&(cfg_rsrc[count++]), base, size, "DPR");
	}

	/* Add all the above */
	sa_add_fixed_mmio_resources(dev, resource_cnt, cfg_rsrc, count);
}

/*
 * SoC implementation
 *
 * Perform System Agent Initialization during Ramstage phase.
 */
void soc_systemagent_init(struct device *dev)
{
	struct soc_power_limits_config *soc_config;
	struct device *sa;
	uint16_t sa_pci_id;
	u8 tdp;
	size_t i;
	config_t *config;

	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	config = config_of_soc();

	/* Get System Agent PCI ID */
	sa = pcidev_path_on_root(SA_DEVFN_ROOT);
	sa_pci_id = sa ? pci_read_config16(sa, PCI_DEVICE_ID) : 0xFFFF;

	tdp = get_cpu_tdp();

	/* Choose power limits configuration based on the CPU SA PCI ID and
	 * CPU TDP value. */
	for (i = 0; i < ARRAY_SIZE(cpuid_to_adl); i++) {
		if (sa_pci_id == cpuid_to_adl[i].cpu_id &&
				tdp == cpuid_to_adl[i].cpu_tdp) {
			soc_config = &config->power_limits_config[cpuid_to_adl[i].limits];
			set_power_limits(MOBILE_SKU_PL1_TIME_SEC, soc_config);
			break;
		}
	}

	if (i == ARRAY_SIZE(cpuid_to_adl)) {
		printk(BIOS_ERR, "unknown SA ID: 0x%4x, skipped power limits configuration.\n",
			sa_pci_id);
		return;
	}
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

uint64_t get_mmcfg_size(const struct device *dev)
{
	uint32_t pciexbar_reg;
	uint64_t mmcfg_length;

	if (!dev) {
		printk(BIOS_DEBUG, "%s : device is null\n", __func__);
		return 0;
	}

	pciexbar_reg = pci_read_config32(dev, PCIEXBAR);

	if (!(pciexbar_reg & (1 << 0))) {
		printk(BIOS_DEBUG, "%s : PCIEXBAR disabled\n", __func__);
		return 0;
	}

	switch ((pciexbar_reg & MASK_PCIEXBAR_LENGTH) >> PCIEXBAR_LENGTH_LSB) {
	case PCIEXBAR_LENGTH_4096MB:
		mmcfg_length = 4 * ((uint64_t)GiB);
		break;
	case PCIEXBAR_LENGTH_2048MB:
		mmcfg_length = 2 * ((uint64_t)GiB);
		break;
	case PCIEXBAR_LENGTH_1024MB:
		mmcfg_length = 1 * GiB;
		break;
	case PCIEXBAR_LENGTH_512MB:
		mmcfg_length = 512 * MiB;
		break;
	case PCIEXBAR_LENGTH_256MB:
		mmcfg_length = 256 * MiB;
		break;
	case PCIEXBAR_LENGTH_128MB:
		mmcfg_length = 128 * MiB;
		break;
	case PCIEXBAR_LENGTH_64MB:
		mmcfg_length = 64 * MiB;
		break;
	default:
		printk(BIOS_DEBUG, "%s : PCIEXBAR - invalid length (0x%x)\n", __func__,
			pciexbar_reg & MASK_PCIEXBAR_LENGTH);
		mmcfg_length = 0x0;
		break;
	}

	return mmcfg_length;
}

uint64_t get_dsm_size(const struct device *dev)
{
	// - size : B0/D0/F0:R 50h [15:8]
	uint32_t reg32 = pci_read_config32(dev, GGC);
	uint64_t size;
	uint32_t size_field = (reg32 & MASK_DSM_LENGTH) >> MASK_DSM_LENGTH_LSB;
	if (size_field <= 0x10) { // 0x0 - 0x10
		size = size_field * 32 * MiB;
	} else if ((size_field >= 0xF0) && (size_field >= 0xFE)) {
		size = ((uint64_t)size_field - 0xEF) * 4 * MiB;
	} else {
		switch (size_field) {
		case 0x20:
			size = 1 * GiB;
			break;
		case 0x30:
			size = 1536 * MiB;
			break;
		case 0x40:
			size = 2 * (uint64_t)GiB;
			break;
		default:
			printk(BIOS_DEBUG, "%s : DSM - invalid length (0x%x)\n",
				__func__, size_field);
			size = 0x0;
			break;
		}
	}
	return size;
}

uint64_t get_gsm_size(const struct device *dev)
{
	const u32 gsm_size = pci_read_config32(dev, GGC);
	uint64_t size;
	uint32_t size_field = (gsm_size & MASK_GSM_LENGTH) >> MASK_GSM_LENGTH_LSB;
	switch (size_field) {
	case 0x0:
		size = 0;
		break;
	case 0x1:
		size = 2 * MiB;
		break;
	case 0x2:
		size = 4 * MiB;
		break;
	case 0x3:
		size = 8 * MiB;
		break;
	default:
		size = 0;
		break;
	}
	return size;
}
uint64_t get_dpr_size(const struct device *dev)
{
	uint64_t size;
	uint32_t dpr_reg = pci_read_config32(dev, DPR_REG);
	uint32_t size_field = (dpr_reg & MASK_DPR_LENGTH) >> MASK_DPR_LENGTH_LSB;
	size = (uint64_t)size_field * MiB;
	return size;
}
