/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <assert.h>
#include <device/mmio.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/id.h>
#include <soc/mc.h>
#include <soc/sdram.h>
#include <symbols.h>
#include <soc/nvidia/tegra/types.h>
#include <types.h>

static uintptr_t tz_base_mib;
static const size_t tz_size_mib = CONFIG_TRUSTZONE_CARVEOUT_SIZE_MB;

/* returns total amount of DRAM (in MB) from memory controller registers */
int sdram_size_mb(void)
{
	struct tegra_mc_regs *mc = (struct tegra_mc_regs *)TEGRA_MC_BASE;
	static int total_size = 0;

	if (total_size)
		return total_size;

	/*
	 * This obtains memory size from the External Memory Aperture
	 * Configuration register. Nvidia confirmed that it is safe to assume
	 * this value represents the total physical DRAM size.
	 */
	total_size = (read32(&mc->emem_cfg) >>
			MC_EMEM_CFG_SIZE_MB_SHIFT) & MC_EMEM_CFG_SIZE_MB_MASK;

	return total_size;
}

static void carveout_from_regs(uintptr_t *base_mib, size_t *size_mib,
				uint32_t bom, uint32_t bom_hi, uint32_t size)
{

	/* All size regs of carveouts are in MiB. */
	if (size == 0)
		return;

	*size_mib = size;
	bom >>= 20;
	bom |= bom_hi << (32 - 20);

	*base_mib = bom;
}

void carveout_range(int id, uintptr_t *base_mib, size_t *size_mib)
{
	*base_mib = 0;
	*size_mib = 0;
	struct tegra_mc_regs * const mc = (struct tegra_mc_regs *)TEGRA_MC_BASE;
	size_t region_size_mb;

	switch (id) {
	case CARVEOUT_TZ:
		*base_mib = tz_base_mib;
		*size_mib = tz_size_mib;
		break;
	case CARVEOUT_SEC:
		carveout_from_regs(base_mib, size_mib,
					read32(&mc->sec_carveout_bom),
					read32(&mc->sec_carveout_adr_hi),
					read32(&mc->sec_carveout_size_mb));
		break;
	case CARVEOUT_MTS:
		carveout_from_regs(base_mib, size_mib,
					read32(&mc->mts_carveout_bom),
					read32(&mc->mts_carveout_adr_hi),
					read32(&mc->mts_carveout_size_mb));
		break;
	case CARVEOUT_VPR:
		/*
		 * A 128MB VPR carveout is felt to be sufficient as per syseng.
		 * Set it up in vpr_region_init, below.
		 */
		carveout_from_regs(base_mib, size_mib,
					read32(&mc->video_protect_bom),
					read32(&mc->video_protect_bom_adr_hi),
					read32(&mc->video_protect_size_mb));
		break;
	case CARVEOUT_GPU:
		/* These carveout regs use 128KB granularity - convert to MB */
		region_size_mb = DIV_ROUND_UP(read32(&mc->security_carveout2_size_128kb), 8);

		/* BOM address set in gpu_region_init, below */
		carveout_from_regs(base_mib, size_mib,
					read32(&mc->security_carveout2_bom),
					read32(&mc->security_carveout2_bom_hi),
					region_size_mb);
		break;
	case CARVEOUT_NVDEC:
		/* These carveout regs use 128KB granularity - convert to MB */
		region_size_mb = DIV_ROUND_UP(read32(&mc->security_carveout1_size_128kb), 8);

		/* BOM address set in nvdec_region_init, below */
		carveout_from_regs(base_mib, size_mib,
					read32(&mc->security_carveout1_bom),
					read32(&mc->security_carveout1_bom_hi),
					region_size_mb);
		break;
	case CARVEOUT_TSEC:
		/* These carveout regs use 128KB granularity - convert to MB */
		region_size_mb = DIV_ROUND_UP(read32(&mc->security_carveout4_size_128kb), 8);

		/* BOM address set in tsec_region_init, below.
		 * Since the TSEC region consumes 2 carveouts, and is
		 * expected to be split evenly between the two, size_mib
		 * is doubled here.
		 */
		region_size_mb *= 2;
		carveout_from_regs(base_mib, size_mib,
					read32(&mc->security_carveout4_bom),
					read32(&mc->security_carveout4_bom_hi),
					region_size_mb);
		break;
	default:
		break;
	}
}

void print_carveouts(void)
{
	int i;
	printk(BIOS_INFO, "Carveout ranges:\n");
	for (i = 0; i < CARVEOUT_NUM; i++) {
		uintptr_t base, end;
		size_t size;
		carveout_range(i, &base, &size);
		end = base + size;
		if (end && base)
			printk(BIOS_INFO, "ID:%d [%lx - %lx)\n", i,
			       (unsigned long)base * MiB,
			       (unsigned long)end * MiB);
	}
}

/*
 *    Memory Map is as follows
 *
 * ------------------------------   <-- Start of DRAM
 * |                            |
 * |      Available DRAM        |
 * |____________________________|
 * |                            |
 * |          CBMEM             |
 * |____________________________|
 * |                            |
 * |      Other carveouts       |
 * | (with dynamic allocation)  |
 * |____________________________|
 * |                            |
 * |    TZ carveout of size     |
 * | TRUSTZONE_CARVEOUT_SIZE_MB |
 * |____________________________|   <-- 0x100000000
 * |                            |
 * |      Available DRAM        |
 * |                            |
 * ------------------------------   <-- End of DRAM
 *
 */
static void memory_in_range(uintptr_t *base_mib, uintptr_t *end_mib,
				int ignore_carveout_id)
{
	uintptr_t base;
	uintptr_t end;
	int i;

	base = (uintptr_t)_dram / MiB;
	end = base + sdram_size_mb();

	/* Requested limits out of range. */
	if (*end_mib <= base || *base_mib >= end) {
		*end_mib = *base_mib = 0;
		return;
	}

	/* Clip region to passed in limits. */
	if (*end_mib < end)
		end = *end_mib;
	if (*base_mib > base)
		base = *base_mib;

	for (i = 0; i < CARVEOUT_NUM; i++) {
		uintptr_t carveout_base;
		uintptr_t carveout_end;
		size_t carveout_size;

		if (i == ignore_carveout_id)
			continue;

		carveout_range(i, &carveout_base, &carveout_size);
		if (carveout_size == 0)
			continue;

		carveout_end = carveout_base + carveout_size;

		/* Bypass carveouts out of requested range. */
		if (carveout_base >= end || carveout_end <= base)
			continue;

		/*
		 * This is crude, but the assumption is that carveouts live
		 * at the upper range of physical memory. Therefore, update
		 * the end address to be equal to the base of the carveout.
		 */
		end = carveout_base;
	}

	*base_mib = base;
	*end_mib = end;
}

void memory_in_range_below_4gb(uintptr_t *base_mib, uintptr_t *end_mib)
{
	*base_mib = 0;
	*end_mib = 4096;
	memory_in_range(base_mib, end_mib, CARVEOUT_NUM);
}

void memory_in_range_above_4gb(uintptr_t *base_mib, uintptr_t *end_mib)
{
	*base_mib = 4096;
	*end_mib = ~0UL;
	memory_in_range(base_mib, end_mib, CARVEOUT_NUM);
}

void trustzone_region_init(void)
{
	struct tegra_mc_regs * const mc = (void *)(uintptr_t)TEGRA_MC_BASE;
	uintptr_t end = 4096;

	/* Already has been initialized. */
	if (tz_size_mib != 0 && tz_base_mib != 0)
		return;

	/*
	 * Get memory layout below 4GiB ignoring the TZ carveout because
	 * that's the one to initialize.
	 */
	tz_base_mib = end - tz_size_mib;
	memory_in_range(&tz_base_mib, &end, CARVEOUT_TZ);

	/*
	 * IMPORTANT!!!!!
	 * We need to ensure that trustzone region is located at the end of
	 * 32-bit address space. If any carveout is allocated space before
	 * trustzone_region_init is called, then this assert will ensure that
	 * the boot flow fails. If you are here because of this assert, please
	 * move your call to initialize carveout after trustzone_region_init in
	 * romstage and ramstage.
	 */
	assert(end == 4096);

	/* AVP cannot set the TZ registers proper as it is always non-secure. */
	if (context_avp())
		return;

	/* Set the carveout region. */
	write32(&mc->security_cfg0, tz_base_mib << 20);
	write32(&mc->security_cfg1, tz_size_mib);

	/* Enable SMMU translations */
	write32(&mc->smmu_config, MC_SMMU_CONFIG_ENABLE);
}

void gpu_region_init(void)
{
	struct tegra_mc_regs * const mc = (void *)(uintptr_t)TEGRA_MC_BASE;
	uintptr_t gpu_base_mib = 0, end = 4096;
	size_t gpu_size_mib = GPU_CARVEOUT_SIZE_MB;

	/* Get memory layout below 4GiB */
	memory_in_range(&gpu_base_mib, &end, CARVEOUT_GPU);
	gpu_base_mib = end - gpu_size_mib;

	/* Set the carveout2 base address. Everything else has been set in the BCT cfg/inc */
	write32(&mc->security_carveout2_bom, gpu_base_mib << 20);
	write32(&mc->security_carveout2_bom_hi, 0);

	/* Set the locked bit. This will lock out any other writes! */
	setbits32(&mc->security_carveout2_cfg0, MC_SECURITY_CARVEOUT_LOCKED);

	/* Set the carveout3 base to 0, unused */
	write32(&mc->security_carveout3_bom, 0);
	write32(&mc->security_carveout3_bom_hi, 0);

	/* Set the locked bit. This will lock out any other writes! */
	setbits32(&mc->security_carveout3_cfg0, MC_SECURITY_CARVEOUT_LOCKED);
}

void nvdec_region_init(void)
{
	struct tegra_mc_regs * const mc = (void *)(uintptr_t)TEGRA_MC_BASE;
	uintptr_t nvdec_base_mib = 0, end = 4096;
	size_t nvdec_size_mib = NVDEC_CARVEOUT_SIZE_MB;

	/* Get memory layout below 4GiB */
	memory_in_range(&nvdec_base_mib, &end, CARVEOUT_NVDEC);
	nvdec_base_mib = end - nvdec_size_mib;

	/* Set the carveout1 base address. Everything else has been set in the BCT cfg/inc */
	write32(&mc->security_carveout1_bom, nvdec_base_mib << 20);
	write32(&mc->security_carveout1_bom_hi, 0);

	/* Set the locked bit. This will lock out any other writes! */
	setbits32(&mc->security_carveout1_cfg0, MC_SECURITY_CARVEOUT_LOCKED);
}

void tsec_region_init(void)
{
	struct tegra_mc_regs * const mc = (void *)(uintptr_t)TEGRA_MC_BASE;
	uintptr_t tsec_base_mib = 0, end = 4096;
	size_t tsec_size_mib = TSEC_CARVEOUT_SIZE_MB;

	/* Get memory layout below 4GiB */
	memory_in_range(&tsec_base_mib, &end, CARVEOUT_TSEC);
	tsec_base_mib = end - tsec_size_mib;

	/*
	 * Set the carveout4/5 base address. Everything else has been set in the BCT cfg/inc
	 * Note that the TSEC range is split evenly between the 2 carveouts (i.e. 1MB each)
	 */
	write32(&mc->security_carveout4_bom, tsec_base_mib << 20);
	write32(&mc->security_carveout4_bom_hi, 0);
	write32(&mc->security_carveout5_bom, (tsec_base_mib + (TSEC_CARVEOUT_SIZE_MB / 2)) << 20);
	write32(&mc->security_carveout5_bom_hi, 0);

	/* Set the locked bit. This will lock out any other writes! */
	setbits32(&mc->security_carveout4_cfg0, MC_SECURITY_CARVEOUT_LOCKED);
	setbits32(&mc->security_carveout5_cfg0, MC_SECURITY_CARVEOUT_LOCKED);
}

void vpr_region_init(void)
{
	struct tegra_mc_regs * const mc = (void *)(uintptr_t)TEGRA_MC_BASE;
	uintptr_t vpr_base_mib = 0, end = 4096;
	size_t vpr_size_mib = VPR_CARVEOUT_SIZE_MB;

	/* Get memory layout below 4GiB */
	memory_in_range(&vpr_base_mib, &end, CARVEOUT_VPR);
	vpr_base_mib = end - vpr_size_mib;

	/* Set the carveout base address and size */
	write32(&mc->video_protect_bom, vpr_base_mib << 20);
	write32(&mc->video_protect_bom_adr_hi, 0);
	write32(&mc->video_protect_size_mb, vpr_size_mib);

	/* Set the locked bit. This will lock out any other writes! */
	write32(&mc->video_protect_reg_ctrl, MC_VPR_WR_ACCESS_DISABLE);
}
