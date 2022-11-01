/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <stdint.h>
#include <AGESA.h>
#include <AMD.h>

/* eventlog */
void agesawrapper_trace(AGESA_STATUS ret, AMD_CONFIG_PARAMS *StdHeader, const char *func);
AGESA_STATUS agesawrapper_amdreadeventlog(UINT8 HeapStatus);

/* For suspend-to-ram support. */

#if !CONFIG(CPU_AMD_PI)
/* TODO: With binaryPI we need different interface. */
AGESA_STATUS OemInitResume(AMD_S3_PARAMS *dataBlock);
AGESA_STATUS OemS3LateRestore(AMD_S3_PARAMS *dataBlock);
AGESA_STATUS OemS3Save(AMD_S3_PARAMS *dataBlock);
#endif

/* For FCH */
AGESA_STATUS fchs3earlyrestore(AMD_CONFIG_PARAMS *StdHeader);
AGESA_STATUS fchs3laterestore(AMD_CONFIG_PARAMS *StdHeader);

struct sysinfo
{
	AMD_CONFIG_PARAMS StdHeader;

	int s3resume;
};

void board_BeforeAgesa(struct sysinfo *cb);

void agesa_set_interface(struct sysinfo *cb);

struct agesa_state {
	u8 apic_id;

	AGESA_STRUCT_NAME func;
	const char *function_name;
	uint32_t ts_entry_id;
	uint32_t ts_exit_id;
};

void agesa_state_on_entry(struct agesa_state *task, AGESA_STRUCT_NAME func);
void agesa_state_on_exit(struct agesa_state *task,
	AMD_CONFIG_PARAMS *StdHeader);
int agesa_execute_state(struct sysinfo *cb, AGESA_STRUCT_NAME func);

/* AGESA dispatchers */

AGESA_STATUS module_dispatch(AGESA_STRUCT_NAME func, AMD_CONFIG_PARAMS *StdHeader);

void platform_BeforeInitReset(struct sysinfo *cb, AMD_RESET_PARAMS *Reset);
void board_BeforeInitReset(struct sysinfo *cb, AMD_RESET_PARAMS *Reset);

void platform_BeforeInitEarly(struct sysinfo *cb, AMD_EARLY_PARAMS *Early);
void board_BeforeInitEarly(struct sysinfo *cb, AMD_EARLY_PARAMS *Early);

/* Normal boot */
void platform_BeforeInitPost(struct sysinfo *cb, AMD_POST_PARAMS *Post);
void board_BeforeInitPost(struct sysinfo *cb, AMD_POST_PARAMS *Post);
void platform_AfterInitPost(struct sysinfo *cb, AMD_POST_PARAMS *Post);

void platform_BeforeInitEnv(struct sysinfo *cb, AMD_ENV_PARAMS *Env);
void board_BeforeInitEnv(struct sysinfo *cb, AMD_ENV_PARAMS *Env);
void platform_AfterInitEnv(struct sysinfo *cb, AMD_ENV_PARAMS *Env);

void platform_BeforeInitMid(struct sysinfo *cb, AMD_MID_PARAMS *Mid);
void board_BeforeInitMid(struct sysinfo *cb, AMD_MID_PARAMS *Mid);

void platform_BeforeInitLate(struct sysinfo *cb, AMD_LATE_PARAMS *Late);
void board_BeforeInitLate(struct sysinfo *cb, AMD_LATE_PARAMS *Late);
void platform_AfterInitLate(struct sysinfo *cb, AMD_LATE_PARAMS *Late);
void completion_InitLate(struct sysinfo *cb, AMD_LATE_PARAMS *Late);

/* S3 Resume */
void platform_BeforeInitResume(struct sysinfo *cb, AMD_RESUME_PARAMS *Resume);
void platform_AfterInitResume(struct sysinfo *cb, AMD_RESUME_PARAMS *Resume);

void platform_BeforeS3LateRestore(struct sysinfo *cb, AMD_S3LATE_PARAMS *S3Late);
void platform_AfterS3LateRestore(struct sysinfo *cb, AMD_S3LATE_PARAMS *S3Late);

void platform_AfterS3Save(struct sysinfo *cb, AMD_S3SAVE_PARAMS *S3Save);

/* FCH callouts, not used with CIMx. */
#define HAS_AGESA_FCH_OEM_CALLOUT \
	CONFIG(SOUTHBRIDGE_AMD_PI_AVALON) || \
	CONFIG(SOUTHBRIDGE_AMD_PI_KERN)

#if HAS_AGESA_FCH_OEM_CALLOUT
/* FIXME:  Structures included here were supposed to be private to AGESA. */
#include <FchCommonCfg.h>
void agesa_fch_oem_config(uintptr_t Data, AMD_CONFIG_PARAMS *StdHeader);
void board_FCH_InitReset(struct sysinfo *cb, FCH_RESET_DATA_BLOCK *FchReset);
void board_FCH_InitEnv(struct sysinfo *cb, FCH_DATA_BLOCK *FchEnv);
#endif

#endif /* _STATE_MACHINE_H_ */
