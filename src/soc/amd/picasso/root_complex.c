/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <assert.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <fsp/util.h>
#include <stdint.h>
#include <soc/memmap.h>
#include <soc/iomap.h>
#include "chip.h"

enum {
	ALIB_DPTCI_FUNCTION_ID = 0xc,
	THERMAL_CONTROL_LIMIT_ID = 0x3,
	SUSTAINED_POWER_LIMIT_PARAM_ID = 0x5,
	FAST_PPT_LIMIT_PARAM_ID = 0x6,
	SLOW_PPT_LIMIT_PARAM_ID = 0x7,
	DPTC_TOTAL_UPDATE_PARAMS = 4,
};

struct dptc_param {
	uint8_t id;
	uint32_t value;
} __packed;

struct dptc_input {
	uint16_t size;
	struct dptc_param params[DPTC_TOTAL_UPDATE_PARAMS];
} __packed;

#define DPTC_INPUTS(_thermctllmit, _sustained, _fast, _slow)			\
		{								\
			.size = sizeof(struct dptc_input),			\
			.params = {						\
				{						\
					.id = THERMAL_CONTROL_LIMIT_ID,		\
					.value = _thermctllmit,			\
				},						\
				{						\
					.id = SUSTAINED_POWER_LIMIT_PARAM_ID,	\
					.value = _sustained,			\
				},						\
				{						\
					.id = FAST_PPT_LIMIT_PARAM_ID,		\
					.value = _fast,				\
				},						\
				{						\
					.id = SLOW_PPT_LIMIT_PARAM_ID,		\
					.value = _slow,				\
				},						\
			},							\
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
 *                     +--------------------------------+ ROMSTAGE_ADDR
 *                     |                                | X86_RESET_VECTOR = BOOTBLOCK_ADDR + C_ENV_BOOTBLOCK_SIZE - 0x10
 *                     |           bootblock            |
 *                     |     (C_ENV_BOOTBLOCK_SIZE)     |
 *                     +--------------------------------+ BOOTBLOCK_ADDR
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
	const struct hob_header *hob = fsp_get_hob_list();
	const struct hob_resource *res;
	struct resource *gnb_apic;

	uintptr_t early_reserved_dram_start, early_reserved_dram_end;
	const struct memmap_early_dram *e = memmap_get_early_dram_usage();

	early_reserved_dram_start = e->base;
	early_reserved_dram_end = e->base + e->size;

	/* 0x0 - 0x9ffff */
	ram_resource(dev, idx++, 0, 0xa0000 / KiB);

	/* 0xa0000 - 0xbffff: legacy VGA */
	mmio_resource(dev, idx++, 0xa0000 / KiB, 0x20000 / KiB);

	/* 0xc0000 - 0xfffff: Option ROM */
	reserved_ram_resource(dev, idx++, 0xc0000 / KiB, 0x40000 / KiB);

	/* 1MB - bottom of DRAM reserved for early coreboot usage */
	ram_resource(dev, idx++, (1 * MiB) / KiB,
		     (early_reserved_dram_start - (1 * MiB)) / KiB);

	/* DRAM reserved for early coreboot usage */
	reserved_ram_resource(dev, idx++, early_reserved_dram_start / KiB,
		(early_reserved_dram_end - early_reserved_dram_start) / KiB);

	/* top of DRAM consumed early - low top usable RAM
	 * cbmem_top() accounts for low UMA and TSEG if they are used. */
	ram_resource(dev, idx++, early_reserved_dram_end / KiB,
		     (mem_usable - early_reserved_dram_end) / KiB);

	mmconf_resource(dev, MMIO_CONF_BASE);

	if (!hob) {
		printk(BIOS_ERR, "Error: %s incomplete because no HOB list was found\n",
				__func__);
		return;
	}

	for (; hob->type != HOB_TYPE_END_OF_HOB_LIST; hob = fsp_next_hob(hob)) {

		if (hob->type != HOB_TYPE_RESOURCE_DESCRIPTOR)
			continue;

		res = fsp_hob_header_to_resource(hob);

		if (res->type == EFI_RESOURCE_SYSTEM_MEMORY && res->addr < mem_usable)
			continue; /* 0 through low usable was set above */
		if (res->type == EFI_RESOURCE_MEMORY_MAPPED_IO)
			continue; /* Done separately */

		if (res->type == EFI_RESOURCE_SYSTEM_MEMORY)
			ram_resource(dev, idx++, res->addr / KiB, res->length / KiB);
		else if (res->type == EFI_RESOURCE_MEMORY_RESERVED)
			reserved_ram_resource(dev, idx++, res->addr / KiB, res->length / KiB);
		else
			printk(BIOS_ERR, "Error: failed to set resources for type %d\n",
					res->type);
	}

	/* GNB IOAPIC resource */
	gnb_apic = new_resource(dev, GNB_IO_APIC_ADDR);
	gnb_apic->base = GNB_IO_APIC_ADDR;
	gnb_apic->size = 0x00001000;
	gnb_apic->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void dptc_call_alib(const char *buf_name, uint8_t *buffer, size_t size)
{
	/* Name (buf_name, Buffer(size) {...} */
	acpigen_write_name(buf_name);
	acpigen_write_byte_buffer(buffer, size);

	/* \_SB.ALIB(0xc, buf_name) */
	acpigen_emit_namestring("\\_SB.ALIB");
	acpigen_write_integer(ALIB_DPTCI_FUNCTION_ID);
	acpigen_emit_namestring(buf_name);
}

static void acipgen_dptci(void)
{
	const struct soc_amd_picasso_config *config = config_of_soc();

	if (!config->dptc_enable)
		return;

	struct dptc_input default_input = DPTC_INPUTS(config->thermctl_limit_degreeC,
							config->sustained_power_limit_mW,
							config->fast_ppt_limit_mW,
							config->slow_ppt_limit_mW);
	struct dptc_input tablet_mode_input = DPTC_INPUTS(
					config->thermctl_limit_tablet_mode_degreeC,
					config->sustained_power_limit_tablet_mode_mW,
					config->fast_ppt_limit_tablet_mode_mW,
					config->slow_ppt_limit_tablet_mode_mW);
	/* Scope (\_SB) */
	acpigen_write_scope("\\_SB");

	/* Method(DPTC, 0, Serialized) */
	acpigen_write_method_serialized("DPTC", 0);

	/* If (LEqual ("\_SB.PCI0.LPCB.EC0.TBMD", 1)) */
	acpigen_write_if_lequal_namestr_int("\\_SB.PCI0.LPCB.EC0.TBMD", 1);

	dptc_call_alib("TABB", (uint8_t *)(void *)&tablet_mode_input,
			sizeof(tablet_mode_input));

	acpigen_pop_len(); /* If */

	/* Else */
	acpigen_write_else();

	dptc_call_alib("DEFB", (uint8_t *)(void *)&default_input, sizeof(default_input));

	acpigen_pop_len(); /* Else */

	acpigen_pop_len(); /* Method DPTC */
	acpigen_pop_len(); /* Scope \_SB */
}

/* Used by \_SB.PCI0._CRS */
static void root_complex_fill_ssdt(const struct device *device)
{
	msr_t msr;
	const char *scope;

	assert(device);

	scope = acpi_device_scope(device);
	assert(scope);
	acpigen_write_scope(scope);

	msr = rdmsr(TOP_MEM);
	acpigen_write_name_dword("TOM1", msr.lo);
	msr = rdmsr(TOP_MEM2);
	/*
	 * Since XP only implements parts of ACPI 2.0, we can't use a qword
	 * here.
	 * See http://www.acpi.info/presentations/S01USMOBS169_OS%2520new.ppt
	 * slide 22ff.
	 * Shift value right by 20 bit to make it fit into 32bit,
	 * giving us 1MB granularity and a limit of almost 4Exabyte of memory.
	 */
	acpigen_write_name_dword("TOM2", (msr.hi << 12) | msr.lo >> 20);
	acpigen_pop_len();
	acipgen_dptci();
}

static struct device_operations root_complex_operations = {
	.read_resources		= read_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_fill_ssdt		= root_complex_fill_ssdt,
};

static const struct pci_driver family17_root_complex __pci_driver = {
	.ops	= &root_complex_operations,
	.vendor	= PCI_VENDOR_ID_AMD,
	.device	= PCI_DEVICE_ID_AMD_17H_MODEL_101F_NB,
};
