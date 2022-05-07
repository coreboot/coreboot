/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci_ops.h>
#include <northbridge/intel/haswell/haswell.h>
#include <southbridge/intel/lynxpoint/me.h>
#include <types.h>

#include "raminit_native.h"

/* GDXCBAR */
#define MPCOHTRK_GDXC_MOT_ADDRESS_LO		0x10
#define MPCOHTRK_GDXC_MOT_ADDRESS_HI		0x14
#define MPCOHTRK_GDXC_MOT_REGION		0x18

#define MPCOHTRK_GDXC_OCLA_ADDRESS_LO		0x20
#define MPCOHTRK_GDXC_OCLA_ADDRESS_HI		0x24
#define MPCOHTRK_GDXC_OCLA_REGION		0x28

/* This lock bit made me lose what little sanity I had left. - Angel Pons */
#define MPCOHTRK_GDXC_OCLA_ADDRESS_HI_LOCK	BIT(2)

static inline uint32_t gdxcbar_read32(const uintptr_t offset)
{
	return read32p((mchbar_read32(GDXCBAR) & ~1) + offset);
}

static inline void gdxcbar_write32(const uintptr_t offset, const uint32_t value)
{
	write32p((mchbar_read32(GDXCBAR) & ~1) + offset, value);
}

static inline void gdxcbar_clrsetbits32(const uintptr_t offset, uint32_t clear, uint32_t set)
{
	const uintptr_t address = (mchbar_read32(GDXCBAR) & ~1) + offset;
	clrsetbits32((void *)address, clear, set);
}

#define gdxcbar_setbits32(offset, set)		gdxcbar_clrsetbits32(offset, 0, set)
#define gdxcbar_clrbits32(offset, clear)	gdxcbar_clrsetbits32(offset, clear, 0)

/* All values stored in here (except the bool) are specified in MiB */
struct memory_map_data {
	uint32_t dpr_size;
	uint32_t tseg_size;
	uint32_t gtt_size;
	uint32_t gms_size;
	uint32_t me_stolen_size;
	uint32_t mmio_size;
	uint32_t touud;
	uint32_t remaplimit;
	uint32_t remapbase;
	uint32_t tom;
	uint32_t tom_minus_me;
	uint32_t tolud;
	uint32_t bdsm_base;
	uint32_t gtt_base;
	uint32_t tseg_base;
	bool reclaim_possible;
};

static void compute_memory_map(struct memory_map_data *map)
{
	map->tom_minus_me = map->tom - map->me_stolen_size;

	/*
	 * MMIO size will actually be slightly smaller than computed,
	 * but matches what MRC does and is more MTRR-friendly given
	 * that TSEG is treated as WB, but SMRR makes TSEG UC anyway.
	 */
	const uint32_t mmio_size = MIN(map->tom_minus_me, 4096) / 2;
	map->gtt_base = ALIGN_DOWN(mmio_size, map->tseg_size);
	map->tseg_base = map->gtt_base - map->tseg_size;
	map->bdsm_base = map->gtt_base + map->gtt_size;
	map->tolud = map->bdsm_base + map->gms_size;
	map->reclaim_possible = map->tom_minus_me > map->tolud;

	if (map->reclaim_possible) {
		map->remapbase  = MAX(4096, map->tom_minus_me);
		map->touud      = MIN(4096, map->tom_minus_me) + map->remapbase - map->tolud;
		map->remaplimit = map->touud - 1;
	} else {
		map->remapbase  = 0;
		map->remaplimit = 0;
		map->touud = map->tom_minus_me;
	}
}

static void display_memory_map(const struct memory_map_data *map)
{
	if (!CONFIG(DEBUG_RAM_SETUP))
		return;

	printk(BIOS_DEBUG, "============ MEMORY MAP ============\n");
	printk(BIOS_DEBUG, "\n");
	printk(BIOS_DEBUG, "dpr_size       = %u MiB\n", map->dpr_size);
	printk(BIOS_DEBUG, "tseg_size      = %u MiB\n", map->tseg_size);
	printk(BIOS_DEBUG, "gtt_size       = %u MiB\n", map->gtt_size);
	printk(BIOS_DEBUG, "gms_size       = %u MiB\n", map->gms_size);
	printk(BIOS_DEBUG, "me_stolen_size = %u MiB\n", map->me_stolen_size);
	printk(BIOS_DEBUG, "\n");
	printk(BIOS_DEBUG, "touud          = %u MiB\n", map->touud);
	printk(BIOS_DEBUG, "remaplimit     = %u MiB\n", map->remaplimit);
	printk(BIOS_DEBUG, "remapbase      = %u MiB\n", map->remapbase);
	printk(BIOS_DEBUG, "tom            = %u MiB\n", map->tom);
	printk(BIOS_DEBUG, "tom_minus_me   = %u MiB\n", map->tom_minus_me);
	printk(BIOS_DEBUG, "tolud          = %u MiB\n", map->tolud);
	printk(BIOS_DEBUG, "bdsm_base      = %u MiB\n", map->bdsm_base);
	printk(BIOS_DEBUG, "gtt_base       = %u MiB\n", map->gtt_base);
	printk(BIOS_DEBUG, "tseg_base      = %u MiB\n", map->tseg_base);
	printk(BIOS_DEBUG, "\n");
	printk(BIOS_DEBUG, "reclaim_possible = %s\n", map->reclaim_possible ? "Yes" : "No");
}

static void map_write_reg64(const uint16_t reg, const uint64_t size)
{
	const uint64_t value = size << 20;
	pci_write_config32(HOST_BRIDGE, reg + 4, value >> 32);
	pci_write_config32(HOST_BRIDGE, reg + 0, value >>  0);
}

static void map_write_reg32(const uint16_t reg, const uint32_t size)
{
	const uint32_t value = size << 20;
	pci_write_config32(HOST_BRIDGE, reg, value);
}

static void program_memory_map(const struct memory_map_data *map)
{
	map_write_reg64(TOUUD, map->touud);
	map_write_reg64(TOM,   map->tom);
	if (map->reclaim_possible) {
		map_write_reg64(REMAPBASE,  map->remapbase);
		map_write_reg64(REMAPLIMIT, map->remaplimit);
	}
	if (map->me_stolen_size) {
		map_write_reg64(MESEG_LIMIT, 0x80000 - map->me_stolen_size);
		map_write_reg64(MESEG_BASE, map->tom_minus_me);
		pci_or_config32(HOST_BRIDGE, MESEG_LIMIT, ME_STLEN_EN);
	}
	map_write_reg32(TOLUD, map->tolud);
	map_write_reg32(BDSM,  map->bdsm_base);
	map_write_reg32(BGSM,  map->gtt_base);
	map_write_reg32(TSEG,  map->tseg_base);

	const uint32_t dpr_reg = map->tseg_base << 20 | map->dpr_size << 4;
	pci_write_config32(HOST_BRIDGE, DPR, dpr_reg);

	const uint16_t gfx_stolen_size = GGC_IGD_MEM_IN_32MB_UNITS(map->gms_size / 32);
	const uint16_t ggc = map->gtt_size << 8 | gfx_stolen_size;
	pci_write_config16(HOST_BRIDGE, GGC, ggc);

	/** TODO: Do not hardcode these? GDXC has weird alignment requirements, though. **/
	gdxcbar_write32(MPCOHTRK_GDXC_MOT_ADDRESS_LO, 0);
	gdxcbar_write32(MPCOHTRK_GDXC_MOT_ADDRESS_HI, 0);
	gdxcbar_write32(MPCOHTRK_GDXC_MOT_REGION, 0);

	gdxcbar_write32(MPCOHTRK_GDXC_OCLA_ADDRESS_LO, 0);
	gdxcbar_write32(MPCOHTRK_GDXC_OCLA_ADDRESS_HI, 0);
	gdxcbar_write32(MPCOHTRK_GDXC_OCLA_REGION, 0);

	gdxcbar_setbits32(MPCOHTRK_GDXC_OCLA_ADDRESS_HI, MPCOHTRK_GDXC_OCLA_ADDRESS_HI_LOCK);
}

enum raminit_status configure_memory_map(struct sysinfo *ctrl)
{
	struct memory_map_data memory_map = {
		.tom            = ctrl->channel_size_mb[0] + ctrl->channel_size_mb[1],
		.dpr_size       = CONFIG_INTEL_TXT_DPR_SIZE,
		.tseg_size      = CONFIG_SMM_TSEG_SIZE >> 20,
		.me_stolen_size = intel_early_me_uma_size(),
	};
	/** FIXME: MRC hardcodes iGPU parameters, but we should not **/
	const bool igpu_on = pci_read_config32(HOST_BRIDGE, DEVEN) & DEVEN_D2EN;
	if (CONFIG(ONBOARD_VGA_IS_PRIMARY) || igpu_on) {
		memory_map.gtt_size = 2;
		memory_map.gms_size = 64;
		pci_or_config32(HOST_BRIDGE, DEVEN, DEVEN_D2EN);
	}
	compute_memory_map(&memory_map);
	display_memory_map(&memory_map);
	program_memory_map(&memory_map);
	return 0;
}
