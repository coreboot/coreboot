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

#include <Porting.h>
#include <AGESA.h>

#include <cbmem.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/agesa_helper.h>

void platform_BeforeInitReset(struct sysinfo *cb, AMD_RESET_PARAMS *Reset)
{
}

void platform_BeforeInitEarly(struct sysinfo *cb, AMD_EARLY_PARAMS *Early)
{
}

void platform_BeforeInitPost(struct sysinfo *cb, AMD_POST_PARAMS *Post)
{
}

void platform_AfterInitPost(struct sysinfo *cb, AMD_POST_PARAMS *Post)
{
	backup_top_of_low_cacheable(Post->MemConfig.Sub4GCacheTop);
}

void platform_BeforeInitResume(struct sysinfo *cb, AMD_RESUME_PARAMS *Resume)
{
	OemInitResume(&Resume->S3DataBlock);
}

void platform_AfterInitResume(struct sysinfo *cb, AMD_RESUME_PARAMS *Resume)
{
}

void platform_BeforeInitEnv(struct sysinfo *cb, AMD_ENV_PARAMS *Env)
{
	EmptyHeap();
}

void platform_AfterInitEnv(struct sysinfo *cb, AMD_ENV_PARAMS *Env)
{
}

void platform_BeforeS3LateRestore(struct sysinfo *cb, AMD_S3LATE_PARAMS *S3Late)
{
	OemS3LateRestore(&S3Late->S3DataBlock);
}

void platform_AfterS3LateRestore(struct sysinfo *cb, AMD_S3LATE_PARAMS *S3Late)
{
	amd_initcpuio();
}

void platform_BeforeInitMid(struct sysinfo *cb, AMD_MID_PARAMS *Mid)
{
	amd_initcpuio();
}

void platform_BeforeInitLate(struct sysinfo *cb, AMD_LATE_PARAMS *Late)
{
}

void platform_AfterInitLate(struct sysinfo *cb, AMD_LATE_PARAMS *Late)
{
}

void platform_AfterS3Save(struct sysinfo *cb, AMD_S3SAVE_PARAMS *S3Save)
{
	OemS3Save(&S3Save->S3DataBlock);
}
