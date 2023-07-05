/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <amdblocks/acpi.h>
#include <amdblocks/alib.h>
#include <amdblocks/data_fabric.h>
#include <amdblocks/memmap.h>
#include <amdblocks/ioapic.h>
#include <amdblocks/iomap.h>
#include <arch/ioapic.h>
#include <arch/vga.h>
#include <assert.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/util.h>
#include <stdint.h>
#include <soc/iomap.h>
#include "chip.h"

#define DPTC_TOTAL_UPDATE_PARAMS	4

struct dptc_input {
	uint16_t size;
	struct alib_dptc_param params[DPTC_TOTAL_UPDATE_PARAMS];
} __packed;

#define DPTC_INPUTS(_thermctllmit, _sustained, _fast, _slow)			\
	{									\
		.size = sizeof(struct dptc_input),				\
		.params = {							\
			{							\
				.id = ALIB_DPTC_THERMAL_CONTROL_LIMIT_ID,	\
				.value = _thermctllmit,				\
			},							\
			{							\
				.id = ALIB_DPTC_SUSTAINED_POWER_LIMIT_ID,	\
				.value = _sustained,				\
			},							\
			{							\
				.id = ALIB_DPTC_FAST_PPT_LIMIT_ID,		\
				.value = _fast,					\
			},							\
			{							\
				.id = ALIB_DPTC_SLOW_PPT_LIMIT_ID,		\
				.value = _slow,					\
			},							\
		},								\
	}
/*
 *
 *                     +--------------------------------+
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *   reserved_dram_end +--------------------------------+
 *                     |                                |
 *                     |       verstage (if reqd)       |
 *                     |          (VERSTAGE_SIZE)       |
 *                     +--------------------------------+ VERSTAGE_ADDR
 *                     |                                |
 *                     |            FSP-M               |
 *                     |         (FSP_M_SIZE)           |
 *                     +--------------------------------+ FSP_M_ADDR
 *                     |           romstage             |
 *                     |        (ROMSTAGE_SIZE)         |
 *                     +--------------------------------+ ROMSTAGE_ADDR = BOOTBLOCK_END
 *                     |                                | X86_RESET_VECTOR = BOOTBLOCK_END  - 0x10
 *                     |           bootblock            |
 *                     |     (C_ENV_BOOTBLOCK_SIZE)     |
 *                     +--------------------------------+ BOOTBLOCK_ADDR = BOOTBLOCK_END - C_ENV_BOOTBLOCK_SIZE
 *                     |          Unused hole           |
 *                     |            (86KiB)             |
 *                     +--------------------------------+
 *                     |     FMAP cache (FMAP_SIZE)     |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE + PSP_SHAREDMEM_SIZE + PRERAM_CBMEM_CONSOLE_SIZE + 0x200
 *                     |  Early Timestamp region (512B) |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE + PSP_SHAREDMEM_SIZE + PRERAM_CBMEM_CONSOLE_SIZE
 *                     |      Preram CBMEM console      |
 *                     |   (PRERAM_CBMEM_CONSOLE_SIZE)  |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE + PSP_SHAREDMEM_SIZE
 *                     |   PSP shared (vboot workbuf)   |
 *                     |      (PSP_SHAREDMEM_SIZE)      |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE
 *                     |          APOB (64KiB)          |
 *                     +--------------------------------+ PSP_APOB_DRAM_ADDRESS
 *                     |        Early BSP stack         |
 *                     |   (EARLYRAM_BSP_STACK_SIZE)    |
 * reserved_dram_start +--------------------------------+ EARLY_RESERVED_DRAM_BASE
 *                     |              DRAM              |
 *                     +--------------------------------+ 0x100000
 *                     |           Option ROM           |
 *                     +--------------------------------+ 0xc0000
 *                     |           Legacy VGA           |
 *                     +--------------------------------+ 0xa0000
 *                     |              DRAM              |
 *                     +--------------------------------+ 0x0
 */
static void read_resources(struct device *dev)
{
	uint32_t mem_usable = (uintptr_t)cbmem_top();
	unsigned int idx = 0;
	const struct hob_header *hob_iterator;
	const struct hob_resource *res;

	uintptr_t early_reserved_dram_start, early_reserved_dram_end;
	const struct memmap_early_dram *e = memmap_get_early_dram_usage();

	early_reserved_dram_start = e->base;
	early_reserved_dram_end = e->base + e->size;

	/* The root complex has no PCI BARs implemented, so there's no need to call
	   pci_dev_read_resources for it */

	fixed_io_range_reserved(dev, idx++, PCI_IO_CONFIG_INDEX, PCI_IO_CONFIG_PORT_COUNT);

	/* 0x0 - 0x9ffff */
	ram_range(dev, idx++, 0, 0xa0000);

	/* 0xa0000 - 0xbffff: legacy VGA */
	mmio_range(dev, idx++, VGA_MMIO_BASE, VGA_MMIO_SIZE);

	/* 0xc0000 - 0xfffff: Option ROM */
	reserved_ram_from_to(dev, idx++, 0xc0000, 1 * MiB);

	/* 1MB - bottom of DRAM reserved for early coreboot usage */
	ram_from_to(dev, idx++, 1 * MiB, early_reserved_dram_start);

	/* DRAM reserved for early coreboot usage */
	reserved_ram_from_to(dev, idx++, early_reserved_dram_start, early_reserved_dram_end);

	/* top of DRAM consumed early - low top usable RAM
	 * cbmem_top() accounts for low UMA and TSEG if they are used. */
	ram_from_to(dev, idx++, early_reserved_dram_end, mem_usable);

	mmconf_resource(dev, idx++);

	/* GNB IOAPIC resource */
	mmio_range(dev, IOMMU_IOAPIC_IDX, GNB_IO_APIC_ADDR, 0x1000);

	/* Reserve fixed IOMMU MMIO region */
	mmio_range(dev, idx++, IOMMU_RESERVED_MMIO_BASE, IOMMU_RESERVED_MMIO_SIZE);

	if (fsp_hob_iterator_init(&hob_iterator) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s incomplete because no HOB list was found\n", __func__);
		return;
	}

	while (fsp_hob_iterator_get_next_resource(&hob_iterator, &res) == CB_SUCCESS) {
		if (res->type == EFI_RESOURCE_SYSTEM_MEMORY && res->addr < mem_usable)
			continue; /* 0 through low usable was set above */
		if (res->type == EFI_RESOURCE_MEMORY_MAPPED_IO)
			continue; /* Done separately */

		if (res->type == EFI_RESOURCE_SYSTEM_MEMORY)
			ram_range(dev, idx++, res->addr, res->length);
		else if (res->type == EFI_RESOURCE_MEMORY_RESERVED)
			reserved_ram_range(dev, idx++, res->addr, res->length);
		else
			printk(BIOS_ERR, "failed to set resources for type %d\n",
					res->type);
	}
}

static void root_complex_init(struct device *dev)
{
	register_new_ioapic((u8 *)GNB_IO_APIC_ADDR);
}

static void acipgen_dptci(void)
{
	const struct soc_amd_picasso_config *config = config_of_soc();

	/* Normal mode DPTC values. */
	struct dptc_input default_input = DPTC_INPUTS(config->thermctl_limit_degreeC,
							config->sustained_power_limit_mW,
							config->fast_ppt_limit_mW,
							config->slow_ppt_limit_mW);
	acpigen_write_alib_dptc_default((uint8_t *)&default_input, sizeof(default_input));

	/* Tablet Mode */
	struct dptc_input tablet_mode_input = DPTC_INPUTS(
					config->thermctl_limit_tablet_mode_degreeC,
					config->sustained_power_limit_tablet_mode_mW,
					config->fast_ppt_limit_tablet_mode_mW,
					config->slow_ppt_limit_tablet_mode_mW);
	acpigen_write_alib_dptc_tablet((uint8_t *)&tablet_mode_input,
		sizeof(tablet_mode_input));
}

static void root_complex_fill_ssdt(const struct device *device)
{
	if (CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_DPTC))
		acipgen_dptci();
}

static const char *gnb_acpi_name(const struct device *dev)
{
	return "GNB";
}

struct device_operations picasso_root_complex_operations = {
	.read_resources		= read_resources,
	.set_resources		= noop_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= root_complex_init,
	.acpi_name		= gnb_acpi_name,
	.acpi_fill_ssdt		= root_complex_fill_ssdt,
};
