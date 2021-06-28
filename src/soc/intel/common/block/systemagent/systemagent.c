/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cfg.h>
#include <intelblocks/systemagent.h>
#include <smbios.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include <types.h>
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

__weak void soc_add_configurable_mmio_resources(struct device *dev,
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

__weak unsigned long sa_write_acpi_tables(const struct device *dev,
					  unsigned long current,
					  struct acpi_rsdp *rsdp)
{
	return current;
}

__weak uint32_t soc_systemagent_max_chan_capacity_mib(u8 capid0_a_ddrsz)
{
	return 32768;	/* 32 GiB per channel */
}

static uint8_t sa_get_ecc_type(const uint32_t capid0_a)
{
	return capid0_a & CAPID_ECCDIS ? MEMORY_ARRAY_ECC_NONE : MEMORY_ARRAY_ECC_SINGLE_BIT;
}

static size_t sa_slots_per_channel(const uint32_t capid0_a)
{
	return !(capid0_a & CAPID_DDPCD) + 1;
}

static size_t sa_number_of_channels(const uint32_t capid0_a)
{
	return !(capid0_a & CAPID_PDCD) + 1;
}

static void sa_soc_systemagent_init(struct device *dev)
{
	soc_systemagent_init(dev);

	struct memory_info *m = cbmem_find(CBMEM_ID_MEMINFO);
	if (m == NULL)
		return;

	const uint32_t capid0_a = pci_read_config32(dev, CAPID0_A);

	m->ecc_type = sa_get_ecc_type(capid0_a);
	m->max_capacity_mib = soc_systemagent_max_chan_capacity_mib(CAPID_DDRSZ(capid0_a)) *
			      sa_number_of_channels(capid0_a);
	m->number_of_devices = sa_slots_per_channel(capid0_a) *
			       sa_number_of_channels(capid0_a);
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

		printk(BIOS_DEBUG, "SA MMIO resource: %s ->  base = 0x%llx, size = 0x%llx\n",
			sa_fixed_resources[i].description, sa_fixed_resources[i].base,
			sa_fixed_resources[i].size);

		mmio_resource_kb(dev, index++, base / KiB, size / KiB);
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
static void sa_read_map_entry(const struct device *dev,
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
 * - top_of_ram -> TOLUD: not cacheable with standard MTRRs and reserved
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
 * Don't need to mark the entire top_of_ram till TOLUD range (used
 * for stolen memory like GFX and ME, PTT, DPR, PRMRR, TSEG etc) as
 * cacheable for OS usage as coreboot already done with mpinit w/ smm
 * relocation early.
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
	uintptr_t base_k;
	size_t size_k;
	uint64_t sa_map_values[MAX_MAP_ENTRIES];
	uintptr_t top_of_ram;
	int index = *resource_count;

	top_of_ram = (uintptr_t)cbmem_top();

	/* 0 - > 0xa0000 */
	base_k = 0;
	size_k = (0xa0000 / KiB) - base_k;
	ram_resource_kb(dev, index++, base_k, size_k);

	/* 0xc0000 -> top_of_ram */
	base_k = 0xc0000 / KiB;
	size_k = (top_of_ram / KiB) - base_k;
	ram_resource_kb(dev, index++, base_k, size_k);

	sa_get_mem_map(dev, &sa_map_values[0]);

	/* top_of_ram -> TOLUD */
	base_k = top_of_ram;
	size_k = sa_map_values[SA_TOLUD_REG] - base_k;
	mmio_resource_kb(dev, index++, base_k / KiB, size_k / KiB);

	/* 4GiB -> TOUUD */
	upper_ram_end(dev, index++, sa_map_values[SA_TOUUD_REG]);

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	mmio_resource_kb(dev, index++, 0xa0000 / KiB, (0xc0000 - 0xa0000) / KiB);
	reserved_ram_resource_kb(dev, index++, 0xc0000 / KiB,
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
	reserved_ram_resource_kb(dev, idx, base_k, size_k);
}

/*
 * Add IMR ranges that hang off the host bridge/memory
 * controller device in case CONFIG(SA_ENABLE_IMR) is selected by SoC.
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

	/* Add all configurable MMIO resources. */
	soc_add_configurable_mmio_resources(dev, &index);

	/* Calculate and add DRAM resources. */
	sa_add_dram_resources(dev, &index);
	if (CONFIG(SA_ENABLE_IMR))
		/* Add the isolated memory ranges (IMRs). */
		sa_add_imr_resources(dev, &index);

	/* Reserve the window used for extended BIOS decoding. */
	if (CONFIG(FAST_SPI_SUPPORTS_EXT_BIOS_WINDOW))
		mmio_resource_kb(dev, index++, CONFIG_EXT_BIOS_WIN_BASE / KiB,
			      CONFIG_EXT_BIOS_WIN_SIZE / KiB);
}

void enable_power_aware_intr(void)
{
	uint8_t pair;

	/* Enable Power Aware Interrupt Routing */
	pair = MCHBAR8(MCH_PAIR);
	pair &= ~0x7;	/* Clear 2:0 */
	pair |= 0x4;	/* Fixed Priority */
	MCHBAR8(MCH_PAIR) = pair;
}

void sa_lock_pam(void)
{
	const struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
	if (!dev)
		return;

	pci_or_config8(dev, PAM0, PAM_LOCK);
}

void ssdt_set_above_4g_pci(const struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_DOMAIN)
		return;

	uint64_t touud;
	sa_read_map_entry(pcidev_path_on_root(SA_DEVFN_ROOT), &sa_memory_map[SA_TOUUD_REG],
			  &touud);
	const uint64_t len = POWER_OF_2(cpu_phys_address_size()) - touud;

	const char *scope = acpi_device_path(dev);
	acpigen_write_scope(scope);
	acpigen_write_name_qword("A4GB", touud);
	acpigen_write_name_qword("A4GS", len);
	acpigen_pop_len();

	printk(BIOS_DEBUG, "PCI space above 4GB MMIO is at 0x%llx, len = 0x%llx\n", touud, len);
}

static struct device_operations systemagent_ops = {
	.read_resources   = systemagent_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = sa_soc_systemagent_init,
	.ops_pci          = &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = sa_write_acpi_tables,
#endif
};

static const unsigned short systemagent_ids[] = {
	PCI_DID_INTEL_MTL_M_ID,
	PCI_DID_INTEL_MTL_P_ID_1,
	PCI_DID_INTEL_MTL_P_ID_2,
	PCI_DID_INTEL_MTL_P_ID_3,
	PCI_DID_INTEL_GLK_NB,
	PCI_DID_INTEL_APL_NB,
	PCI_DID_INTEL_CNL_ID_U,
	PCI_DID_INTEL_CNL_ID_Y,
	PCI_DID_INTEL_SKL_ID_U,
	PCI_DID_INTEL_SKL_ID_Y,
	PCI_DID_INTEL_SKL_ID_ULX,
	PCI_DID_INTEL_SKL_ID_H_4,
	PCI_DID_INTEL_SKL_ID_H_2,
	PCI_DID_INTEL_SKL_ID_S_2,
	PCI_DID_INTEL_SKL_ID_S_4,
	PCI_DID_INTEL_WHL_ID_W_2,
	PCI_DID_INTEL_WHL_ID_W_4,
	PCI_DID_INTEL_KBL_ID_S,
	PCI_DID_INTEL_SKL_ID_H_EM,
	PCI_DID_INTEL_KBL_ID_U,
	PCI_DID_INTEL_KBL_ID_Y,
	PCI_DID_INTEL_KBL_ID_H,
	PCI_DID_INTEL_KBL_U_R,
	PCI_DID_INTEL_KBL_ID_DT,
	PCI_DID_INTEL_KBL_ID_DT_2,
	PCI_DID_INTEL_CFL_ID_U,
	PCI_DID_INTEL_CFL_ID_U_2,
	PCI_DID_INTEL_CFL_ID_H,
	PCI_DID_INTEL_CFL_ID_H_4,
	PCI_DID_INTEL_CFL_ID_H_8,
	PCI_DID_INTEL_CFL_ID_S,
	PCI_DID_INTEL_CFL_ID_S_DT_2,
	PCI_DID_INTEL_CFL_ID_S_DT_4,
	PCI_DID_INTEL_CFL_ID_S_DT_8,
	PCI_DID_INTEL_CFL_ID_S_WS_4,
	PCI_DID_INTEL_CFL_ID_S_WS_6,
	PCI_DID_INTEL_CFL_ID_S_WS_8,
	PCI_DID_INTEL_CFL_ID_S_S_4,
	PCI_DID_INTEL_CFL_ID_S_S_6,
	PCI_DID_INTEL_CFL_ID_S_S_8,
	PCI_DID_INTEL_ICL_ID_U,
	PCI_DID_INTEL_ICL_ID_U_2_2,
	PCI_DID_INTEL_ICL_ID_Y,
	PCI_DID_INTEL_ICL_ID_Y_2,
	PCI_DID_INTEL_CML_ULT,
	PCI_DID_INTEL_CML_ULT_2_2,
	PCI_DID_INTEL_CML_ULT_6_2,
	PCI_DID_INTEL_CML_ULX,
	PCI_DID_INTEL_CML_S,
	PCI_DID_INTEL_CML_S_G0G1_P0P1_6_2,
	PCI_DID_INTEL_CML_S_P0P1_8_2,
	PCI_DID_INTEL_CML_S_P0P1_10_2,
	PCI_DID_INTEL_CML_S_G0G1_4,
	PCI_DID_INTEL_CML_S_G0G1_2,
	PCI_DID_INTEL_CML_H,
	PCI_DID_INTEL_CML_H_4_2,
	PCI_DID_INTEL_CML_H_8_2,
	PCI_DID_INTEL_TGL_ID_U_2_2,
	PCI_DID_INTEL_TGL_ID_U_4_2,
	PCI_DID_INTEL_TGL_ID_Y_2_2,
	PCI_DID_INTEL_TGL_ID_Y_4_2,
	PCI_DID_INTEL_TGL_ID_H_6_1,
	PCI_DID_INTEL_TGL_ID_H_8_1,
	PCI_DID_INTEL_EHL_ID_0,
	PCI_DID_INTEL_EHL_ID_1,
	PCI_DID_INTEL_EHL_ID_1A,
	PCI_DID_INTEL_EHL_ID_2,
	PCI_DID_INTEL_EHL_ID_2_1,
	PCI_DID_INTEL_EHL_ID_3,
	PCI_DID_INTEL_EHL_ID_3A,
	PCI_DID_INTEL_EHL_ID_4,
	PCI_DID_INTEL_EHL_ID_5,
	PCI_DID_INTEL_EHL_ID_6,
	PCI_DID_INTEL_EHL_ID_7,
	PCI_DID_INTEL_EHL_ID_8,
	PCI_DID_INTEL_EHL_ID_9,
	PCI_DID_INTEL_EHL_ID_10,
	PCI_DID_INTEL_EHL_ID_11,
	PCI_DID_INTEL_EHL_ID_12,
	PCI_DID_INTEL_EHL_ID_13,
	PCI_DID_INTEL_EHL_ID_14,
	PCI_DID_INTEL_EHL_ID_15,
	PCI_DID_INTEL_JSL_ID_1,
	PCI_DID_INTEL_JSL_ID_2,
	PCI_DID_INTEL_JSL_ID_3,
	PCI_DID_INTEL_JSL_ID_4,
	PCI_DID_INTEL_JSL_ID_5,
	PCI_DID_INTEL_ADL_S_ID_1,
	PCI_DID_INTEL_ADL_S_ID_2,
	PCI_DID_INTEL_ADL_S_ID_3,
	PCI_DID_INTEL_ADL_S_ID_4,
	PCI_DID_INTEL_ADL_S_ID_5,
	PCI_DID_INTEL_ADL_S_ID_6,
	PCI_DID_INTEL_ADL_S_ID_7,
	PCI_DID_INTEL_ADL_S_ID_8,
	PCI_DID_INTEL_ADL_S_ID_9,
	PCI_DID_INTEL_ADL_S_ID_10,
	PCI_DID_INTEL_ADL_S_ID_11,
	PCI_DID_INTEL_ADL_S_ID_12,
	PCI_DID_INTEL_ADL_S_ID_13,
	PCI_DID_INTEL_ADL_S_ID_14,
	PCI_DID_INTEL_ADL_S_ID_15,
	PCI_DID_INTEL_ADL_P_ID_1,
	PCI_DID_INTEL_ADL_P_ID_3,
	PCI_DID_INTEL_ADL_P_ID_4,
	PCI_DID_INTEL_ADL_P_ID_5,
	PCI_DID_INTEL_ADL_P_ID_6,
	PCI_DID_INTEL_ADL_P_ID_7,
	PCI_DID_INTEL_ADL_P_ID_8,
	PCI_DID_INTEL_ADL_P_ID_9,
	PCI_DID_INTEL_ADL_P_ID_10,
	PCI_DID_INTEL_ADL_M_ID_1,
	PCI_DID_INTEL_ADL_M_ID_2,
	PCI_DID_INTEL_ADL_N_ID_1,
	PCI_DID_INTEL_ADL_N_ID_2,
	PCI_DID_INTEL_ADL_N_ID_3,
	PCI_DID_INTEL_ADL_N_ID_4,
	PCI_DID_INTEL_RPL_P_ID_1,
	PCI_DID_INTEL_RPL_P_ID_2,
	PCI_DID_INTEL_RPL_P_ID_3,
	0
};

static const struct pci_driver systemagent_driver __pci_driver = {
	.ops     = &systemagent_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = systemagent_ids
};
