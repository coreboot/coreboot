/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/biosram.h>

#include "Porting.h"
#include "AGESA.h"

#include <device/device.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/pi/nb_common.h>

void platform_BeforeInitReset(struct sysinfo *cb, AMD_RESET_PARAMS *Reset)
{
}

void platform_BeforeInitEarly(struct sysinfo *cb, AMD_EARLY_PARAMS *Early)
{
	Early->GnbConfig.PsppPolicy = PsppDisabled;
}

void platform_BeforeInitPost(struct sysinfo *cb, AMD_POST_PARAMS *Post)
{
	Post->MemConfig.UmaMode = CONFIG(GFXUMA) ? UMA_AUTO : UMA_NONE;
	Post->MemConfig.UmaSize = 0;
	Post->MemConfig.BottomIo = (UINT16)(CONFIG_BOTTOMIO_POSITION >> 24);
}

void platform_AfterInitPost(struct sysinfo *cb, AMD_POST_PARAMS *Post)
{
	/* If UMA is enabled we currently have it below TOP_MEM as well.
	 * UMA may or may not be cacheable, so Sub4GCacheTop could be
	 * higher than UmaBase. With UMA_NONE we see UmaBase==0. */
	if (Post->MemConfig.UmaBase)
		backup_top_of_low_cacheable(Post->MemConfig.UmaBase << 16);
	else
		backup_top_of_low_cacheable(Post->MemConfig.Sub4GCacheTop);
}

void platform_BeforeInitEnv(struct sysinfo *cb, AMD_ENV_PARAMS *Env)
{
	EmptyHeap();
}

void platform_AfterInitEnv(struct sysinfo *cb, AMD_ENV_PARAMS *Env)
{
}

void platform_BeforeInitMid(struct sysinfo *cb, AMD_MID_PARAMS *Mid)
{
	amd_initcpuio();

	/* 0 iGpuVgaAdapter, 1 iGpuVgaNonAdapter; */
	Mid->GnbMidConfiguration.iGpuVgaMode = 0;
	Mid->GnbMidConfiguration.GnbIoapicAddress = IO_APIC2_ADDR;
}

void platform_BeforeInitLate(struct sysinfo *cb, AMD_LATE_PARAMS *Late)
{
	const struct device *iommu_dev = pcidev_on_root(0, 2);

	if (iommu_dev && iommu_dev->enabled) {
		/* According to AGESA headers these must be set to sane values
		 * when IOMMU build config is enabled otherwise AGESA will skip
		 * it during IOMMU init and IVRS generation.
		 */
		Late->GnbLateConfiguration.GnbIoapicId = CONFIG_MAX_CPUS + 1;
		Late->GnbLateConfiguration.FchIoapicId = CONFIG_MAX_CPUS;
	}

	/* Code for creating CDIT requires hop count table. If it is not
	 * present AGESA_ERROR is returned, which confuses users. CDIT is not
	 * written to the ACPI tables anyway. */
	Late->PlatformConfig.UserOptionCdit = 0;
}

void platform_AfterInitLate(struct sysinfo *cb, AMD_LATE_PARAMS *Late)
{
}

void platform_BeforeInitResume(struct sysinfo *cb, AMD_RESUME_PARAMS *Resume)
{
}

void platform_AfterInitResume(struct sysinfo *cb, AMD_RESUME_PARAMS *Resume)
{
}

void platform_BeforeS3LateRestore(struct sysinfo *cb, AMD_S3LATE_PARAMS *S3Late)
{
}

void platform_AfterS3LateRestore(struct sysinfo *cb, AMD_S3LATE_PARAMS *S3Late)
{
	amd_initcpuio();
}

void platform_AfterS3Save(struct sysinfo *cb, AMD_S3SAVE_PARAMS *S3Save)
{
}
