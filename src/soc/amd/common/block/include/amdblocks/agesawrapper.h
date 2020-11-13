/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_AGESAWRAPPER_H
#define AMD_BLOCK_AGESAWRAPPER_H

#include <agesa_headers.h>

enum {
	PICK_DMI,       /* DMI Interface */
	PICK_PSTATE,    /* Acpi Pstate SSDT Table */
	PICK_SRAT,      /* SRAT Table */
	PICK_SLIT,      /* SLIT Table */
	PICK_WHEA_MCE,  /* WHEA MCE table */
	PICK_WHEA_CMC,  /* WHEA CMV table */
	PICK_ALIB,      /* SACPI SSDT table with ALIB implementation */
	PICK_IVRS,      /* IOMMU ACPI IVRS (I/O Virt. Reporting Structure) */
	PICK_CRAT,
};

/* Return current dispatcher or NULL on error. */
MODULE_ENTRY agesa_get_dispatcher(void);

AGESA_STATUS agesa_execute_state(AGESA_STRUCT_NAME func);
AGESA_STATUS amd_late_run_ap_task(AP_EXE_PARAMS *ApExeParams);

void *agesawrapper_getlateinitptr(int pick);

void OemCustomizeInitEarly(AMD_EARLY_PARAMS *InitEarly);
void amd_initcpuio(void);
const void *agesawrapper_locate_module(const char name[8]);

void SetFchResetParams(FCH_RESET_INTERFACE *params);
void OemPostParams(AMD_POST_PARAMS *PostParams);
void SetMemParams(AMD_POST_PARAMS *PostParams);
void SetFchEnvParams(FCH_INTERFACE *params);
void SetNbEnvParams(GNB_ENV_CONFIGURATION *params);
void SetFchMidParams(FCH_INTERFACE *params);
void SetNbMidParams(GNB_MID_CONFIGURATION *params);
void set_board_env_params(GNB_ENV_CONFIGURATION *params);
void soc_customize_init_early(AMD_EARLY_PARAMS *InitEarly);

#endif /* AMD_BLOCK_AGESAWRAPPER_H */
