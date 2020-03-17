/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cfg.h>
#include <intelblocks/systemagent.h>
#include <smbios.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include "systemagent_def.h"

/* SoC override function */
__weak void soc_systemagent_init(struct device *dev)
{
	/* no-op */
}

__weak void soc_add_fixed_mmio_resources(struct device *dev,
		int *resource_cnt)
{
	/* no-op */
}

__weak int soc_get_uncore_prmmr_base_and_mask(uint64_t *base,
		uint64_t *mask)
{
	/* return failure for this dummy API */
	return -1;
}

__weak unsigned long sa_write_acpi_tables(struct device *dev,
					  unsigned long current,
					  struct acpi_rsdp *rsdp)
{
	return current;
}

/*
 * This function will get above 4GB mmio enable config specific to soc.
 *
 * Return values:
 *  0 = Above 4GB memory is not enable
 *  1 = Above 4GB memory is enable
 */
static int get_enable_above_4GB_mmio(void)
{
	const struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	return common_config->enable_above_4GB_mmio;
}

/* Fill MMIO resource above 4GB into GNVS */
void sa_fill_gnvs(global_nvs_t *gnvs)
{
	if (get_enable_above_4GB_mmio()) {
		gnvs->e4gm = 1;
		gnvs->a4gb = ABOVE_4GB_MEM_BASE_ADDRESS;
		gnvs->a4gs = ABOVE_4GB_MEM_BASE_SIZE;
		printk(BIOS_DEBUG,
			"PCI space above 4GB MMIO is from 0x%llx  to len = 0x%llx\n",
				gnvs->a4gb, gnvs->a4gs);
	}
}

/*
 * Add all known fixed MMIO ranges that hang off the host bridge/memory
 * controller device.
 */
void sa_add_fixed_mmio_resources(struct device *dev, int *resource_cnt,
	const struct sa_mmio_descriptor *sa_fixed_resources, size_t count)
{
	int i;
	int index = *resource_cnt;

	for (i = 0; i < count; i++) {
		uintptr_t base;
		size_t size;

		size = sa_fixed_resources[i].size;
		base = sa_fixed_resources[i].base;

		mmio_resource(dev, index++, base / KiB, size / KiB);
	}

	*resource_cnt = index;
}

/*
 * DRAM memory mapped register
 *
 * TOUUD: This 64 bit register defines the Top of Upper Usable DRAM
 * TOLUD: This 32 bit register defines the Top of Low Usable DRAM
 * BGSM: This register contains the base address of stolen DRAM memory for GTT
 * TSEG: This register contains the base address of TSEG DRAM memory
 */
static const struct sa_mem_map_descriptor sa_memory_map[MAX_MAP_ENTRIES] = {
	{ TOUUD, true, "TOUUD" },
	{ TOLUD, false, "TOLUD" },
	{ BGSM, false, "BGSM" },
	{ TSEG, false, "TSEG" },
};

/* Read DRAM memory map register value through PCI configuration space */
static void sa_read_map_entry(struct device *dev,
		const struct sa_mem_map_descriptor *entry, uint64_t *result)
{
	uint64_t value = 0;

	if (entry->is_64_bit) {
		value = pci_read_config32(dev, entry->reg + 4);
		value <<= 32;
	}

	value |= pci_read_config32(dev, entry->reg);
	/* All registers are on a 1MiB granularity. */
	value = ALIGN_DOWN(value, 1 * MiB);

	*result = value;
}

static void sa_get_mem_map(struct device *dev, uint64_t *values)
{
	int i;
	for (i = 0; i < MAX_MAP_ENTRIES; i++)
		sa_read_map_entry(dev, &sa_memory_map[i], &values[i]);
}

/*
 * These are the host memory ranges that should be added:
 * - 0 -> 0xa0000: cacheable
 * - 0xc0000 -> top_of_ram : cacheable
 * - top_of_ram -> BGSM: cacheable with standard MTRRs and reserved
 * - BGSM -> TOLUD: not cacheable with standard MTRRs and reserved
 * - 4GiB -> TOUUD: cacheable
 *
 * The default SMRAM space is reserved so that the range doesn't
 * have to be saved during S3 Resume. Once marked reserved the OS
 * cannot use the memory. This is a bit of an odd place to reserve
 * the region, but the CPU devices don't have dev_ops->read_resources()
 * called on them.
 *
 * The range 0xa0000 -> 0xc0000 does not have any resources
 * associated with it to handle legacy VGA memory. If this range
 * is not omitted the mtrr code will setup the area as cacheable
 * causing VGA access to not work.
 *
 * The TSEG region is mapped as cacheable so that one can perform
 * SMRAM relocation faster. Once the SMRR is enabled the SMRR takes
 * precedence over the existing MTRRs covering this region.
 *
 * It should be noted that cacheable entry types need to be added in
 * order. The reason is that the current MTRR code assumes this and
 * falls over itself if it isn't.
 *
 * The resource index starts low and should not meet or exceed
 * PCI_BASE_ADDRESS_0.
 */
static void sa_add_dram_resources(struct device *dev, int *resource_count)
{
	uintptr_t base_k, touud_k;
	size_t size_k;
	uint64_t sa_map_values[MAX_MAP_ENTRIES];
	uintptr_t top_of_ram;
	int index = *resource_count;

	top_of_ram = (uintptr_t)cbmem_top();

	/* 0 - > 0xa0000 */
	base_k = 0;
	size_k = (0xa0000 / KiB) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* 0xc0000 -> top_of_ram */
	base_k = 0xc0000 / KiB;
	size_k = (top_of_ram / KiB) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	sa_get_mem_map(dev, &sa_map_values[0]);

	/* top_of_ram -> BGSM */
	base_k = top_of_ram;
	size_k = sa_map_values[SA_BGSM_REG] - base_k;
	reserved_ram_resource(dev, index++, base_k / KiB, size_k / KiB);

	/* BGSM -> TOLUD */
	base_k = sa_map_values[SA_BGSM_REG];
	size_k = sa_map_values[SA_TOLUD_REG] - base_k;
	mmio_resource(dev, index++, base_k / KiB, size_k / KiB);

	/* 4GiB -> TOUUD */
	base_k = 4 * (GiB / KiB); /* 4GiB */
	touud_k = sa_map_values[SA_TOUUD_REG] / KiB;
	size_k = touud_k - base_k;
	if (touud_k > base_k)
		ram_resource(dev, index++, base_k, size_k);

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	mmio_resource(dev, index++, 0xa0000 / KiB, (0xc0000 - 0xa0000) / KiB);
	reserved_ram_resource(dev, index++, 0xc0000 / KiB,
			(1*MiB - 0xc0000) / KiB);

	*resource_count = index;
}

static bool is_imr_enabled(uint32_t imr_base_reg)
{
	return !!(imr_base_reg & (1 << 31));
}

static void imr_resource(struct device *dev, int idx, uint32_t base,
			 uint32_t mask)
{
	uint32_t base_k, size_k;
	/* Bits 28:0 encode the base address bits 38:10, hence the KiB unit. */
	base_k = (base & 0x0fffffff);
	/* Bits 28:0 encode the AND mask used for comparison, in KiB. */
	size_k = ((~mask & 0x0fffffff) + 1);
	/*
	 * IMRs sit in lower DRAM. Mark them cacheable, otherwise we run
	 * out of MTRRs. Memory reserved by IMRs is not usable for host
	 * so mark it reserved.
	 */
	reserved_ram_resource(dev, idx, base_k, size_k);
}

/*
 * Add IMR ranges that hang off the host bridge/memory
 * controller device in case CONFIG_SA_ENABLE_IMR is selected by SoC.
 */
static void sa_add_imr_resources(struct device *dev, int *resource_cnt)
{
	size_t i, imr_offset;
	uint32_t base, mask;
	int index = *resource_cnt;

	for (i = 0; i < MCH_NUM_IMRS; i++) {
		imr_offset = i * MCH_IMR_PITCH;
		base = MCHBAR32(imr_offset + MCH_IMR0_BASE);
		mask = MCHBAR32(imr_offset + MCH_IMR0_MASK);

		if (is_imr_enabled(base))
			imr_resource(dev, index++, base, mask);
	}

	*resource_cnt = index;
}

static void systemagent_read_resources(struct device *dev)
{
	int index = 0;

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* Add all fixed MMIO resources. */
	soc_add_fixed_mmio_resources(dev, &index);
	/* Calculate and add DRAM resources. */
	sa_add_dram_resources(dev, &index);
	if (CONFIG(SA_ENABLE_IMR))
		/* Add the isolated memory ranges (IMRs). */
		sa_add_imr_resources(dev, &index);
}

#if CONFIG(GENERATE_SMBIOS_TABLES)
static int sa_smbios_write_type_16(struct device *dev, int *handle,
		unsigned long *current)
{
	struct smbios_type16 *t = (struct smbios_type16 *)*current;
	int len = sizeof(struct smbios_type16);

	struct memory_info *meminfo;
	meminfo = cbmem_find(CBMEM_ID_MEMINFO);
	if (meminfo == NULL)
		return 0;	/* can't find mem info in cbmem */

	memset(t, 0, sizeof(struct smbios_type16));
	t->type = SMBIOS_PHYS_MEMORY_ARRAY;
	t->handle = *handle;
	t->length = len - 2;
	t->location = MEMORY_ARRAY_LOCATION_SYSTEM_BOARD;
	t->use = MEMORY_ARRAY_USE_SYSTEM;
	/* TBD, meminfo hob have information about ECC */
	t->memory_error_correction = MEMORY_ARRAY_ECC_NONE;
	/* no error information handle available */
	t->memory_error_information_handle = 0xFFFE;
	t->maximum_capacity = 32 * (GiB / KiB); /* 32GB as default */
	t->number_of_memory_devices = meminfo->dimm_cnt;

	*current += len;
	*handle += 1;
	return len;
}
#endif

void enable_power_aware_intr(void)
{
	uint8_t pair;

	/* Enable Power Aware Interrupt Routing */
	pair = MCHBAR8(MCH_PAIR);
	pair &= ~0x7;	/* Clear 2:0 */
	pair |= 0x4;	/* Fixed Priority */
	MCHBAR8(MCH_PAIR) = pair;
}

static struct device_operations systemagent_ops = {
	.read_resources   = systemagent_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = soc_systemagent_init,
	.ops_pci          = &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = sa_write_acpi_tables,
#endif
#if CONFIG(GENERATE_SMBIOS_TABLES)
	.get_smbios_data = sa_smbios_write_type_16,
#endif
};

static const unsigned short systemagent_ids[] = {
	PCI_DEVICE_ID_INTEL_GLK_NB,
	PCI_DEVICE_ID_INTEL_APL_NB,
	PCI_DEVICE_ID_INTEL_CNL_ID_U,
	PCI_DEVICE_ID_INTEL_CNL_ID_Y,
	PCI_DEVICE_ID_INTEL_SKL_ID_U,
	PCI_DEVICE_ID_INTEL_SKL_ID_Y,
	PCI_DEVICE_ID_INTEL_SKL_ID_ULX,
	PCI_DEVICE_ID_INTEL_SKL_ID_H_4,
	PCI_DEVICE_ID_INTEL_SKL_ID_H_2,
	PCI_DEVICE_ID_INTEL_SKL_ID_S_2,
	PCI_DEVICE_ID_INTEL_SKL_ID_S_4,
	PCI_DEVICE_ID_INTEL_WHL_ID_W_2,
	PCI_DEVICE_ID_INTEL_WHL_ID_W_4,
	PCI_DEVICE_ID_INTEL_KBL_ID_S,
	PCI_DEVICE_ID_INTEL_SKL_ID_H_EM,
	PCI_DEVICE_ID_INTEL_KBL_ID_U,
	PCI_DEVICE_ID_INTEL_KBL_ID_Y,
	PCI_DEVICE_ID_INTEL_KBL_ID_H,
	PCI_DEVICE_ID_INTEL_KBL_U_R,
	PCI_DEVICE_ID_INTEL_KBL_ID_DT,
	PCI_DEVICE_ID_INTEL_KBL_ID_DT_2,
	PCI_DEVICE_ID_INTEL_CFL_ID_U,
	PCI_DEVICE_ID_INTEL_CFL_ID_U_2,
	PCI_DEVICE_ID_INTEL_CFL_ID_H,
	PCI_DEVICE_ID_INTEL_CFL_ID_H_4,
	PCI_DEVICE_ID_INTEL_CFL_ID_H_8,
	PCI_DEVICE_ID_INTEL_CFL_ID_S,
	PCI_DEVICE_ID_INTEL_CFL_ID_S_DT_2,
	PCI_DEVICE_ID_INTEL_CFL_ID_S_DT_4,
	PCI_DEVICE_ID_INTEL_CFL_ID_S_DT_8,
	PCI_DEVICE_ID_INTEL_CFL_ID_S_WS_4,
	PCI_DEVICE_ID_INTEL_CFL_ID_S_WS_6,
	PCI_DEVICE_ID_INTEL_CFL_ID_S_WS_8,
	PCI_DEVICE_ID_INTEL_CFL_ID_S_S_4,
	PCI_DEVICE_ID_INTEL_CFL_ID_S_S_6,
	PCI_DEVICE_ID_INTEL_CFL_ID_S_S_8,
	PCI_DEVICE_ID_INTEL_ICL_ID_U,
	PCI_DEVICE_ID_INTEL_ICL_ID_U_2_2,
	PCI_DEVICE_ID_INTEL_ICL_ID_Y,
	PCI_DEVICE_ID_INTEL_ICL_ID_Y_2,
	PCI_DEVICE_ID_INTEL_CML_ULT,
	PCI_DEVICE_ID_INTEL_CML_ULT_2_2,
	PCI_DEVICE_ID_INTEL_CML_ULT_6_2,
	PCI_DEVICE_ID_INTEL_CML_ULX,
	PCI_DEVICE_ID_INTEL_CML_S,
	PCI_DEVICE_ID_INTEL_CML_S_G0G1_P0P1_6_2,
	PCI_DEVICE_ID_INTEL_CML_S_P0P1_8_2,
	PCI_DEVICE_ID_INTEL_CML_S_P0P1_10_2,
	PCI_DEVICE_ID_INTEL_CML_S_G0G1_4,
	PCI_DEVICE_ID_INTEL_CML_S_G0G1_2,
	PCI_DEVICE_ID_INTEL_CML_H,
	PCI_DEVICE_ID_INTEL_CML_H_4_2,
	PCI_DEVICE_ID_INTEL_CML_H_8_2,
	PCI_DEVICE_ID_INTEL_TGL_ID_U,
	PCI_DEVICE_ID_INTEL_TGL_ID_U_1,
	PCI_DEVICE_ID_INTEL_TGL_ID_U_2_2,
	PCI_DEVICE_ID_INTEL_TGL_ID_Y,
	PCI_DEVICE_ID_INTEL_JSL_EHL,
	PCI_DEVICE_ID_INTEL_EHL_ID_1,
	PCI_DEVICE_ID_INTEL_JSL_ID_1,
	0
};

static const struct pci_driver systemagent_driver __pci_driver = {
	.ops     = &systemagent_ops,
	.vendor  = PCI_VENDOR_ID_INTEL,
	.devices = systemagent_ids
};
