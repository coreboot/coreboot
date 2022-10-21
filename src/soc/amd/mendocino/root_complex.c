/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <acpi/acpigen.h>
#include <amdblocks/acpi.h>
#include <amdblocks/alib.h>
#include <amdblocks/ioapic.h>
#include <amdblocks/memmap.h>
#include <arch/ioapic.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/amd/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/util.h>
#include <soc/iomap.h>
#include <stdint.h>
#include "chip.h"

#define DPTC_TOTAL_UPDATE_PARAMS	13

struct dptc_input {
	uint16_t size;
	struct alib_dptc_param params[DPTC_TOTAL_UPDATE_PARAMS];
} __packed;


#define DPTC_INPUTS(_thermctllmit, _sustained, _spptTimeConst, _fast, _slow,	\
	_vrmCurrentLimit, _vrmMaxCurrentLimit, _vrmSocCurrentLimit,		\
	_sttMinLimit, _sttM1, _sttM2, _sttCApu, _sttSkinTempLimitApu)		\
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
				.id = ALIB_DPTC_SLOW_PPT_TIME_CONSTANT_ID,	\
				.value = _spptTimeConst,			\
			},							\
			{							\
				.id = ALIB_DPTC_FAST_PPT_LIMIT_ID,		\
				.value = _fast,					\
			},							\
			{							\
				.id = ALIB_DPTC_SLOW_PPT_LIMIT_ID,		\
				.value = _slow,					\
			},							\
			{							\
				.id = ALIB_DPTC_VRM_CURRENT_LIMIT_ID,		\
				.value = _vrmCurrentLimit,			\
			},							\
			{							\
				.id = ALIB_DPTC_VRM_MAXIMUM_CURRENT_LIMIT,	\
				.value = _vrmMaxCurrentLimit,			\
			},							\
			{							\
				.id = ALIB_DPTC_VRM_SOC_CURRENT_LIMIT_ID,	\
				.value = _vrmSocCurrentLimit,			\
			},							\
			{							\
				.id = ALIB_DPTC_STT_MIN_LIMIT_ID,		\
				.value = _sttMinLimit,				\
			},							\
			{							\
				.id = ALIB_DPTC_STT_M1_ID,			\
				.value = _sttM1,				\
			},							\
			{							\
				.id = ALIB_DPTC_STT_M2_ID,			\
				.value = _sttM2,				\
			},							\
			{							\
				.id = ALIB_DPTC_STT_C_APU_ID,			\
				.value = _sttCApu,				\
			},							\
			{							\
				.id = ALIB_DPTC_STT_SKIN_TEMPERATURE_LIMIT_APU_ID,	\
				.value = _sttSkinTempLimitApu,			\
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
 *                     |            (30KiB)             |
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
 *                     |          APOB (120KiB)         |
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
	struct resource *gnb_apic;

	uintptr_t early_reserved_dram_start, early_reserved_dram_end;
	const struct memmap_early_dram *e = memmap_get_early_dram_usage();

	early_reserved_dram_start = e->base;
	early_reserved_dram_end = e->base + e->size;

	/* The root complex has no PCI BARs implemented, so there's no need to call
	   pci_dev_read_resources for it */

	/* 0x0 - 0x9ffff */
	ram_resource_kb(dev, idx++, 0, 0xa0000 / KiB);

	/* 0xa0000 - 0xbffff: legacy VGA */
	mmio_resource_kb(dev, idx++, 0xa0000 / KiB, 0x20000 / KiB);

	/* 0xc0000 - 0xfffff: Option ROM */
	reserved_ram_resource_kb(dev, idx++, 0xc0000 / KiB, 0x40000 / KiB);

	/* 1MiB - bottom of DRAM reserved for early coreboot usage */
	ram_resource_kb(dev, idx++, (1 * MiB) / KiB,
		     (early_reserved_dram_start - (1 * MiB)) / KiB);

	/* DRAM reserved for early coreboot usage */
	reserved_ram_resource_kb(dev, idx++, early_reserved_dram_start / KiB,
		(early_reserved_dram_end - early_reserved_dram_start) / KiB);

	/*
	 * top of DRAM consumed early - low top usable RAM
	 * cbmem_top() accounts for low UMA and TSEG if they are used.
	 */
	ram_resource_kb(dev, idx++, early_reserved_dram_end / KiB,
		     (mem_usable - early_reserved_dram_end) / KiB);

	mmconf_resource(dev, idx++);

	/* GNB IOAPIC resource */
	gnb_apic = new_resource(dev, idx++);
	gnb_apic->base = GNB_IO_APIC_ADDR;
	gnb_apic->size = 0x00001000;
	gnb_apic->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	if (fsp_hob_iterator_init(&hob_iterator) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s incomplete because no HOB list was found\n",
				__func__);
		return;
	}

	while (fsp_hob_iterator_get_next_resource(&hob_iterator, &res) == CB_SUCCESS) {
		if (res->type == EFI_RESOURCE_SYSTEM_MEMORY && res->addr < mem_usable)
			continue; /* 0 through low usable was set above */
		if (res->type == EFI_RESOURCE_MEMORY_MAPPED_IO)
			continue; /* Done separately */

		if (res->type == EFI_RESOURCE_SYSTEM_MEMORY)
			ram_resource_kb(dev, idx++, res->addr / KiB, res->length / KiB);
		else if (res->type == EFI_RESOURCE_MEMORY_RESERVED)
			reserved_ram_resource_kb(dev, idx++, res->addr / KiB, res->length / KiB);
		else
			printk(BIOS_ERR, "Failed to set resources for type %d\n",
					res->type);
	}
}

static void root_complex_init(struct device *dev)
{
	register_new_ioapic((u8 *)GNB_IO_APIC_ADDR);
}

static void acipgen_dptci(void)
{
	const struct soc_amd_mendocino_config *config = config_of_soc();

	/* Normal mode DPTC values. */
	struct dptc_input default_input = DPTC_INPUTS(
		config->thermctl_limit_degreeC,
		config->sustained_power_limit_mW,
		config->slow_ppt_time_constant_s,
		config->fast_ppt_limit_mW,
		config->slow_ppt_limit_mW,
		config->vrm_current_limit_mA,
		config->vrm_maximum_current_limit_mA,
		config->vrm_soc_current_limit_mA,
		config->stt_min_limit,
		config->stt_m1,
		config->stt_m2,
		config->stt_c_apu,
		config->stt_skin_temp_apu);
	acpigen_write_alib_dptc_default((uint8_t *)&default_input, sizeof(default_input));

	/* Low/No Battery */
	struct dptc_input no_battery_input = DPTC_INPUTS(
		config->thermctl_limit_degreeC,
		config->sustained_power_limit_mW,
		config->slow_ppt_time_constant_s,
		config->fast_ppt_limit_mW,
		config->slow_ppt_limit_mW,
		config->vrm_current_limit_throttle_mA,
		config->vrm_maximum_current_limit_throttle_mA,
		config->vrm_soc_current_limit_throttle_mA,
		config->stt_min_limit,
		config->stt_m1,
		config->stt_m2,
		config->stt_c_apu,
		config->stt_skin_temp_apu);
	acpigen_write_alib_dptc_no_battery((uint8_t *)&no_battery_input,
		sizeof(no_battery_input));

#if (CONFIG(FEATURE_DYNAMIC_DPTC))
	/* Profile B */
	struct dptc_input thermal_B_input = DPTC_INPUTS(
		config->thermctl_limit_degreeC,
		config->sustained_power_limit_mW_B,
		config->slow_ppt_time_constant_s_B,
		config->fast_ppt_limit_mW_B,
		config->slow_ppt_limit_mW_B,
		config->vrm_current_limit_throttle_mA,
		config->vrm_maximum_current_limit_mA,
		config->vrm_soc_current_limit_mA,
		config->stt_min_limit_B,
		config->stt_m1_B,
		config->stt_m2_B,
		config->stt_c_apu_B,
		config->stt_skin_temp_apu_B);
	acpigen_write_alib_dptc_thermal_B((uint8_t *)&thermal_B_input,
		sizeof(thermal_B_input));

	/* Profile C */
	struct dptc_input thermal_C_input = DPTC_INPUTS(
		config->thermctl_limit_degreeC,
		config->sustained_power_limit_mW_C,
		config->slow_ppt_time_constant_s_C,
		config->fast_ppt_limit_mW_C,
		config->slow_ppt_limit_mW_C,
		config->vrm_current_limit_mA,
		config->vrm_maximum_current_limit_mA,
		config->vrm_soc_current_limit_mA,
		config->stt_min_limit_C,
		config->stt_m1_C,
		config->stt_m2_C,
		config->stt_c_apu_C,
		config->stt_skin_temp_apu_C);
	acpigen_write_alib_dptc_thermal_C((uint8_t *)&thermal_C_input,
		sizeof(thermal_C_input));

	/* Profile D */
	struct dptc_input thermal_D_input = DPTC_INPUTS(
		config->thermctl_limit_degreeC,
		config->sustained_power_limit_mW_D,
		config->slow_ppt_time_constant_s_D,
		config->fast_ppt_limit_mW_D,
		config->slow_ppt_limit_mW_D,
		config->vrm_current_limit_mA,
		config->vrm_maximum_current_limit_mA,
		config->vrm_soc_current_limit_mA,
		config->stt_min_limit_D,
		config->stt_m1_D,
		config->stt_m2_D,
		config->stt_c_apu_D,
		config->stt_skin_temp_apu_D);
	acpigen_write_alib_dptc_thermal_D((uint8_t *)&thermal_D_input,
		sizeof(thermal_D_input));

	/* Profile E */
	struct dptc_input thermal_E_input = DPTC_INPUTS(
		config->thermctl_limit_degreeC,
		config->sustained_power_limit_mW_E,
		config->slow_ppt_time_constant_s_E,
		config->fast_ppt_limit_mW_E,
		config->slow_ppt_limit_mW_E,
		config->vrm_current_limit_mA,
		config->vrm_maximum_current_limit_mA,
		config->vrm_soc_current_limit_mA,
		config->stt_min_limit_E,
		config->stt_m1_E,
		config->stt_m2_E,
		config->stt_c_apu_E,
		config->stt_skin_temp_apu_E);
	acpigen_write_alib_dptc_thermal_E((uint8_t *)&thermal_E_input,
		sizeof(thermal_E_input));

	/* Profile F */
	struct dptc_input thermal_F_input = DPTC_INPUTS(
		config->thermctl_limit_degreeC,
		config->sustained_power_limit_mW_F,
		config->slow_ppt_time_constant_s_F,
		config->fast_ppt_limit_mW_F,
		config->slow_ppt_limit_mW_F,
		config->vrm_current_limit_mA,
		config->vrm_maximum_current_limit_mA,
		config->vrm_soc_current_limit_mA,
		config->stt_min_limit_F,
		config->stt_m1_F,
		config->stt_m2_F,
		config->stt_c_apu_F,
		config->stt_skin_temp_apu_F);
	acpigen_write_alib_dptc_thermal_F((uint8_t *)&thermal_F_input,
		sizeof(thermal_F_input));
#endif
}

static void root_complex_fill_ssdt(const struct device *device)
{
	acpi_fill_root_complex_tom(device);
	if (CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_DPTC))
		acipgen_dptci();
}

static const char *gnb_acpi_name(const struct device *dev)
{
	return "GNB";
}

struct device_operations mendocino_root_complex_operations = {
	.read_resources		= read_resources,
	.set_resources		= noop_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= root_complex_init,
	.acpi_name		= gnb_acpi_name,
	.acpi_fill_ssdt		= root_complex_fill_ssdt,
};
