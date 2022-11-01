/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <string.h>
#include <acpi/acpi.h>
#include <bootstate.h>
#include <cbfs.h>
#include <timestamp.h>

#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <amdlib.h>

#include <AMD.h>

#if ENV_RAMINIT
#include <PlatformMemoryConfiguration.h>
CONST PSO_ENTRY ROMDATA DefaultPlatformMemoryConfiguration[] = {PSO_END};
#endif

static void agesa_locate_image(AMD_CONFIG_PARAMS *StdHeader)
{
	const char ModuleIdentifier[] = AGESA_ID;
	const void *agesa, *image;
	size_t file_size;

	agesa = cbfs_map((const char *)CONFIG_AGESA_CBFS_NAME, &file_size);
	if (agesa == NULL)
		return;

	image = LibAmdLocateImage(agesa, agesa + file_size, 4096,
		ModuleIdentifier);
	StdHeader->ImageBasePtr = (void *)image;
}

void agesa_set_interface(struct sysinfo *cb)
{
	memset(&cb->StdHeader, 0, sizeof(AMD_CONFIG_PARAMS));

	cb->StdHeader.CalloutPtr = GetBiosCallout;

	agesa_locate_image(&cb->StdHeader);
	AMD_IMAGE_HEADER *image =
		(void *)(uintptr_t)cb->StdHeader.ImageBasePtr;
	ASSERT(image);
	AMD_MODULE_HEADER *module =
		(void *)(uintptr_t)image->ModuleInfoOffset;
	ASSERT(module && module->ModuleDispatcher);
}

AGESA_STATUS module_dispatch(AGESA_STRUCT_NAME func,
	AMD_CONFIG_PARAMS *StdHeader)
{
	MODULE_ENTRY dispatcher;

	AMD_IMAGE_HEADER *image = (void *)(uintptr_t)StdHeader->ImageBasePtr;
	AMD_MODULE_HEADER *module = (void *)(uintptr_t)image->ModuleInfoOffset;
	dispatcher = module->ModuleDispatcher;

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

			/* FIXME: Detect if TSC frequency really
			 * changed during raminit? */
			timestamp_rescale_table(1, 4);

			platform_AfterInitPost(cb, param);
			break;
		}

		case AMD_INIT_RESUME:
		{
			AMD_RESUME_PARAMS *param = (void *)StdHeader;
			platform_BeforeInitResume(cb, param);
			status = module_dispatch(func, StdHeader);

			/* FIXME: Detect if TSC frequency really
			 * changed during raminit? */
			timestamp_rescale_table(1, 4);

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
			platform_BeforeInitLate(cb, param);
			board_BeforeInitLate(cb, param);
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

int agesa_execute_state(struct sysinfo *cb, AGESA_STRUCT_NAME func)
{
	AMD_INTERFACE_PARAMS aip;
	union {
		AMD_RESET_PARAMS reset;
		AMD_S3LATE_PARAMS s3late;
	} agesa_params;
	void *buf = NULL;
	size_t len = 0;

	AGESA_STATUS status, final;

	struct agesa_state task;
	memset(&task, 0, sizeof(task));
	agesa_state_on_entry(&task, func);

	aip.StdHeader = cb->StdHeader;

	/* For these calls, heap is not available. */
	if (func == AMD_INIT_RESET || func == AMD_S3LATE_RESTORE) {
		buf = (void *)&agesa_params;
		len = sizeof(agesa_params);
		memcpy(buf, &cb->StdHeader, sizeof(cb->StdHeader));
	}

	status = amd_create_struct(&aip, func, buf, len);
	ASSERT(status == AGESA_SUCCESS);

	/* Must call the function buffer was allocated for.*/
	AMD_CONFIG_PARAMS *StdHeader = aip.NewStructPtr;
	ASSERT(StdHeader != NULL && StdHeader->Func == func);

	if (CONFIG(AGESA_EXTRA_TIMESTAMPS) && task.ts_entry_id)
		timestamp_add_now(task.ts_entry_id);

	if (ENV_RAMINIT)
		final = romstage_dispatch(cb, func, StdHeader);

	if (ENV_RAMSTAGE)
		final = ramstage_dispatch(cb, func, StdHeader);

	if (CONFIG(AGESA_EXTRA_TIMESTAMPS) && task.ts_exit_id)
		timestamp_add_now(task.ts_exit_id);

	agesawrapper_trace(final, StdHeader, task.function_name);
	ASSERT(final < AGESA_FATAL);

	status = amd_release_struct(&aip);
	ASSERT(status == AGESA_SUCCESS);

	agesa_state_on_exit(&task, &aip.StdHeader);

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
void __weak
board_BeforeInitLate(struct sysinfo *cb, AMD_LATE_PARAMS *Late) { }

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
