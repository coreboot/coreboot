/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2016 Kyösti Mälkki
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

#include <stdint.h>
#include <string.h>

#include <arch/acpi.h>
#include <bootstate.h>
#include <cbfs.h>
#include <cbmem.h>
#include <compiler.h>

#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "amdlib.h"

#include "AMD.h"

#if IS_ENABLED(CONFIG_CPU_AMD_AGESA_OPENSOURCE)
#include "Dispatcher.h"
#endif

#if ENV_ROMSTAGE
#include <PlatformMemoryConfiguration.h>
CONST PSO_ENTRY ROMDATA DefaultPlatformMemoryConfiguration[] = {PSO_END};
#endif

static void agesa_locate_image(AMD_CONFIG_PARAMS *StdHeader)
{
#if IS_ENABLED(CONFIG_CPU_AMD_AGESA_BINARY_PI)
	const char ModuleIdentifier[] = AGESA_ID;
	const void *agesa, *image;
	size_t file_size;

	agesa = cbfs_boot_map_with_leak((const char *)CONFIG_AGESA_CBFS_NAME,
			CBFS_TYPE_RAW, &file_size);
	if (agesa == NULL)
		return;

	image = LibAmdLocateImage(agesa, agesa + file_size, 4096,
		ModuleIdentifier);
	StdHeader->ImageBasePtr = (void*) image;
#endif
}

void agesa_set_interface(struct sysinfo *cb)
{
	memset(&cb->StdHeader, 0, sizeof(AMD_CONFIG_PARAMS));

	cb->StdHeader.CalloutPtr = GetBiosCallout;

	if (IS_ENABLED(CONFIG_CPU_AMD_AGESA_BINARY_PI)) {
		agesa_locate_image(&cb->StdHeader);
		AMD_IMAGE_HEADER *image =
			(void*)(uintptr_t)cb->StdHeader.ImageBasePtr;
		ASSERT(image);
		AMD_MODULE_HEADER *module =
			(void*)(uintptr_t)image->ModuleInfoOffset;
		ASSERT(module && module->ModuleDispatcher);
	}
}

AGESA_STATUS module_dispatch(AGESA_STRUCT_NAME func,
	AMD_CONFIG_PARAMS *StdHeader)
{
	MODULE_ENTRY dispatcher;

#if IS_ENABLED(CONFIG_CPU_AMD_AGESA_OPENSOURCE)
	dispatcher = AmdAgesaDispatcher;
#endif
#if IS_ENABLED(CONFIG_CPU_AMD_AGESA_BINARY_PI)
	AMD_IMAGE_HEADER *image = (void*)(uintptr_t)StdHeader->ImageBasePtr;
	AMD_MODULE_HEADER *module = (void*)(uintptr_t)image->ModuleInfoOffset;
	dispatcher = module->ModuleDispatcher;
#endif

	StdHeader->Func = func;
	return dispatcher(StdHeader);
}

static AGESA_STATUS amd_create_struct(AMD_INTERFACE_PARAMS *aip,
	AGESA_STRUCT_NAME func, void *buf, size_t len)
{
	aip->AgesaFunctionName = func;
	aip->AllocationMethod = 0;
	aip->NewStructPtr = buf;
	aip->NewStructSize = len;
	if (buf != NULL && len != 0)
		aip->AllocationMethod = ByHost;

	return module_dispatch(AMD_CREATE_STRUCT, &aip->StdHeader);
}

static AGESA_STATUS amd_release_struct(AMD_INTERFACE_PARAMS *aip)
{
	/* Cannot release AMD_LATE_PARAMS until ACPI tables are done. */
	if (aip->AgesaFunctionName == AMD_INIT_LATE)
		return AGESA_SUCCESS;

	return module_dispatch(AMD_RELEASE_STRUCT, &aip->StdHeader);
}

/* By design, for each valid AGESA_STRUCT_NAME, AMD_CONFIG_PARAMS
 * can be evaluated to apply correct typecast based on Func field.
 */

static AGESA_STATUS romstage_dispatch(struct sysinfo *cb,
	AGESA_STRUCT_NAME func, AMD_CONFIG_PARAMS *StdHeader)
{
	AGESA_STATUS status = AGESA_UNSUPPORTED;

	switch (func)
	{
		case AMD_INIT_RESET:
		{
			AMD_RESET_PARAMS *param = (void *)StdHeader;
			platform_BeforeInitReset(cb, param);
			board_BeforeInitReset(cb, param);
			status = module_dispatch(func, StdHeader);
			break;
		}

		case AMD_INIT_EARLY:
		{
			AMD_EARLY_PARAMS *param = (void *)StdHeader;
			platform_BeforeInitEarly(cb, param);
			board_BeforeInitEarly(cb, param);
			status = module_dispatch(func, StdHeader);
			break;
		}

		case AMD_INIT_POST:
		{
			AMD_POST_PARAMS *param = (void *)StdHeader;
			platform_BeforeInitPost(cb, param);
			board_BeforeInitPost(cb, param);
			status = module_dispatch(func, StdHeader);
			platform_AfterInitPost(cb, param);
			break;
		}

		case AMD_INIT_RESUME:
		{
			AMD_RESUME_PARAMS *param = (void *)StdHeader;
			platform_BeforeInitResume(cb, param);
			status = module_dispatch(func, StdHeader);
			platform_AfterInitResume(cb, param);
			break;
		}

		default:
		{
			break;
		}

	}
	return status;
}

static AGESA_STATUS ramstage_dispatch(struct sysinfo *cb,
	AGESA_STRUCT_NAME func, AMD_CONFIG_PARAMS *StdHeader)
{
	AGESA_STATUS status = AGESA_UNSUPPORTED;

	switch (func)
	{
		case AMD_INIT_ENV:
		{
			AMD_ENV_PARAMS *param = (void *)StdHeader;
			platform_BeforeInitEnv(cb, param);
			board_BeforeInitEnv(cb, param);
			status = module_dispatch(func, StdHeader);
			platform_AfterInitEnv(cb, param);
			break;
		}

		case AMD_S3LATE_RESTORE:
		{
			AMD_S3LATE_PARAMS *param = (void *)StdHeader;
			platform_BeforeS3LateRestore(cb, param);
			status = module_dispatch(func, StdHeader);
			platform_AfterS3LateRestore(cb, param);
			break;
		}

		case AMD_INIT_MID:
		{
			AMD_MID_PARAMS *param = (void *)StdHeader;
			platform_BeforeInitMid(cb, param);
			board_BeforeInitMid(cb, param);
			status = module_dispatch(func, StdHeader);
			break;
		}

		case AMD_S3_SAVE:
		{
			AMD_S3SAVE_PARAMS *param = (void *)StdHeader;
			status = module_dispatch(func, StdHeader);
			platform_AfterS3Save(cb, param);
			break;
		}

		case AMD_INIT_LATE:
		{
			AMD_LATE_PARAMS *param = (void *)StdHeader;
			status = module_dispatch(func, StdHeader);
			platform_AfterInitLate(cb, param);
			completion_InitLate(cb, param);
			break;
		}

		default:
		{
			break;
		}

	}
	return status;
}

/* DEBUG trace helper */

struct agesa_state
{
	u8 apic_id;

	AGESA_STRUCT_NAME func;
	const char *function_name;
};

static void state_on_entry(struct agesa_state *task, AGESA_STRUCT_NAME func,
	const char *struct_name)
{
	task->apic_id = (u8) (cpuid_ebx(1) >> 24);
	task->func = func;
	task->function_name = struct_name;

	printk(BIOS_DEBUG, "\nAPIC %02d: ** Enter %s [%08x]\n",
		task->apic_id, task->function_name, task->func);
}

static void state_on_exit(struct agesa_state *task,
	AMD_CONFIG_PARAMS *StdHeader)
{
	printk(BIOS_DEBUG, "APIC %02d: Heap in %s (%d) at 0x%08x\n",
		task->apic_id, heap_status_name(StdHeader->HeapStatus),
		StdHeader->HeapStatus, (u32)StdHeader->HeapBasePtr);

	printk(BIOS_DEBUG, "APIC %02d: ** Exit  %s [%08x]\n",
		task->apic_id, task->function_name, task->func);
}

int agesa_execute_state(struct sysinfo *cb, AGESA_STRUCT_NAME func)
{
	AMD_INTERFACE_PARAMS aip;
	union {
		AMD_RESET_PARAMS reset;
		AMD_S3LATE_PARAMS s3late;
	} agesa_params;
	void *buf = NULL;
	size_t len = 0;
	const char *state_name = agesa_struct_name(func);

	AGESA_STATUS status, final;

	struct agesa_state task;
	memset(&task, 0, sizeof(task));
	state_on_entry(&task, func, state_name);

	aip.StdHeader = cb->StdHeader;

	/* For these calls, heap is not available. */
	if (func == AMD_INIT_RESET || func == AMD_S3LATE_RESTORE) {
		buf = (void *) &agesa_params;
		len = sizeof(agesa_params);
		memcpy(buf, &cb->StdHeader, sizeof(cb->StdHeader));
	}

	status = amd_create_struct(&aip, func, buf, len);
	ASSERT(status == AGESA_SUCCESS);

	/* Must call the function buffer was allocated for.*/
	AMD_CONFIG_PARAMS *StdHeader = aip.NewStructPtr;
	ASSERT(StdHeader->Func == func);

	if (ENV_ROMSTAGE)
		final = romstage_dispatch(cb, func, StdHeader);

	if (ENV_RAMSTAGE)
		final = ramstage_dispatch(cb, func, StdHeader);

	agesawrapper_trace(final, StdHeader, state_name);
	ASSERT(final < AGESA_FATAL);

	status = amd_release_struct(&aip);
	ASSERT(status == AGESA_SUCCESS);

	state_on_exit(&task, &aip.StdHeader);

	return (final < AGESA_FATAL) ? 0 : -1;
}

#if ENV_RAMSTAGE

static void amd_bs_ramstage_init(void *arg)
{
	struct sysinfo *cb = arg;

	agesa_set_interface(cb);

	if (!acpi_is_wakeup_s3())
		agesa_execute_state(cb, AMD_INIT_ENV);
	else {
		agesa_execute_state(cb, AMD_S3LATE_RESTORE);
		fchs3earlyrestore(&cb->StdHeader);
	}
}

void sb_After_Pci_Restore_Init(void);

static void amd_bs_dev_enable(void *arg)
{
	struct sysinfo *cb = arg;

	if (!acpi_is_wakeup_s3())
		agesa_execute_state(cb, AMD_INIT_MID);

	/* FIXME */
	if (IS_ENABLED(CONFIG_AMD_SB_CIMX) && acpi_is_wakeup_s3())
		sb_After_Pci_Restore_Init();
}

static void amd_bs_post_device(void *arg)
{
	struct sysinfo *cb = arg;

	if (acpi_is_wakeup_s3()) {
		fchs3laterestore(&cb->StdHeader);
		return;
	}

	agesa_execute_state(cb, AMD_INIT_LATE);

	if (!acpi_s3_resume_allowed())
		return;

	agesa_execute_state(cb, AMD_S3_SAVE);
}

static struct sysinfo state_machine;

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, amd_bs_ramstage_init,
	&state_machine);

BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, amd_bs_dev_enable,
	&state_machine);

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, amd_bs_post_device,
	&state_machine);

#endif /* ENV_RAMSTAGE */

/* Empty stubs for cases board does not need to override anything. */
void __weak
board_BeforeInitReset(struct sysinfo *cb, AMD_RESET_PARAMS *Reset) { }
void __weak
board_BeforeInitEarly(struct sysinfo *cb, AMD_EARLY_PARAMS *Early) { }
void __weak
board_BeforeInitPost(struct sysinfo *cb, AMD_POST_PARAMS *Post) { }
void __weak
board_BeforeInitEnv(struct sysinfo *cb, AMD_ENV_PARAMS *Env) { }
void __weak
board_BeforeInitMid(struct sysinfo *cb, AMD_MID_PARAMS *Mid) { }

AGESA_STATUS __weak
fchs3earlyrestore(AMD_CONFIG_PARAMS *StdHeader)
{
	return AGESA_SUCCESS;
}

AGESA_STATUS __weak
fchs3laterestore(AMD_CONFIG_PARAMS *StdHeader)
{
	return AGESA_SUCCESS;
}
