/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "amdfwtool.h"

struct psp_fw_name_entry {
	const char *name;
	amd_fw_type type;
	uint8_t subprog;
	uint8_t instance;
};

static const struct psp_fw_name_entry psp_fw_name_table[] = {
	/* Name					, type			, subprog, instance */
	{ "PSPBTLDR_WL_FILE",			AMD_FW_PSP_BOOTLOADER_AB,	0, 0 },
	{ "PSPBTLDR_AB_STAGE1_FILE",		AMD_FW_PSP_BOOTLOADER,		0, 0 },
	{ "PSPBTLDR_FILE",			AMD_FW_PSP_BOOTLOADER,		0, 0 },
	{ "AMD_PUBKEY_FILE",			AMD_FW_PSP_PUBKEY,		0, 0 },
	{ "AMD_FUSE_CHAIN",			AMD_FW_PSP_FUSE_CHAIN,		0, 0 },
	{ "PSPRCVR_FILE",			AMD_FW_PSP_RECOVERY,		0, 0 },
	{ "PUBSIGNEDKEY_FILE",			AMD_FW_PSP_RTM_PUBKEY,		0, 0 },
	{ "PSPNVRAM_FILE",			AMD_FW_PSP_NVRAM,		0, 0 },
	{ "SMUSCS_FILE",			AMD_FW_PSP_SMUSCS,		0, 0 },
	{ "PSPTRUSTLETS_FILE",			AMD_FW_PSP_TRUSTLETS,		0, 0 },
	{ "PSPSECUREDEBUG_FILE",		AMD_FW_PSP_SECURED_DEBUG,	0, 0 },
	{ "PSP_SMUFW1_SUB0_FILE",		AMD_FW_PSP_SMU_FIRMWARE,	0, 0 },
	{ "PSP_HW_IPCFG_FILE_SUB0",		AMD_FW_PSP_HW_IPCFG,			0, 0 },
	{ "PSP_HW_IPCFG_FILE_SUB1",		AMD_FW_PSP_HW_IPCFG,			1, 0 },
	{ "PSP_HW_IPCFG_FILE_SUB2",		AMD_FW_PSP_HW_IPCFG,			2, 0 },
	{ "PSP_SMUFW1_SUB1_FILE",		AMD_FW_PSP_SMU_FIRMWARE,	1, 0 },
	{ "PSP_SMUFW1_SUB2_FILE",		AMD_FW_PSP_SMU_FIRMWARE,	2, 0 },
	{ "PSP_SMUFW2_SUB0_FILE",		AMD_FW_PSP_SMU_FIRMWARE2,	0, 0 },
	{ "PSP_SMUFW2_SUB1_FILE",		AMD_FW_PSP_SMU_FIRMWARE2,	1, 0 },
	{ "PSP_TEEIPKEY_FILE",			AMD_FW_PSP_TEEIPKEY,		0, 0 },
	{ "PSP_SMUFW2_SUB2_FILE",		AMD_FW_PSP_SMU_FIRMWARE2,	2, 0 },
	{ "PSP_BOOT_DRIVER_FILE",		AMD_FW_PSP_BOOT_DRIVER,		0, 0 },
	{ "PSP_SOC_DRIVER_FILE",		AMD_FW_PSP_SOC_DRIVER,			0, 0 },
	{ "PSP_DEBUG_DRIVER_FILE",		AMD_FW_PSP_DEBUG_DRIVER,		0, 0 },
	{ "PSP_INTERFACE_DRIVER_FILE",		AMD_FW_PSP_INTERFACE_DRIVER,		0, 0 },
	{ "PSP_SEC_DBG_KEY_FILE",		AMD_FW_PSP_SECURED_DEBUG,	0, 0 },
	{ "PSP_SEC_DEBUG_FILE",			AMD_FW_PSP_DEBUG_UNLOCK,		0, 0 },
	{ "PSP_ABL0_FILE",			AMD_FW_PSP_ABL0,			0, 0 },
	{ "PSP_ABL1_FILE",			AMD_FW_PSP_ABL1,			0, 0 },
	{ "PSP_ABL2_FILE",			AMD_FW_PSP_ABL2,			0, 0 },
	{ "PSP_ABL3_FILE",			AMD_FW_PSP_ABL3,			0, 0 },
	{ "PSP_ABL4_FILE",			AMD_FW_PSP_ABL4,			0, 0 },
	{ "PSP_ABL5_FILE",			AMD_FW_PSP_ABL5,			0, 0 },
	{ "PSP_ABL6_FILE",			AMD_FW_PSP_ABL6,			0, 0 },
	{ "PSP_ABL7_FILE",			AMD_FW_PSP_ABL7,			0, 0 },
	{ "PSPSECUREOS_FILE",			AMD_FW_PSP_SECURED_OS,		0, 0 },
	{ "TRUSTLETKEY_FILE",			AMD_FW_PSP_TRUSTLETKEY,		0, 0 },
	{ "PSP_IKEK_FILE",			AMD_FW_PSP_WRAPPED_IKEK,		0, 0 },
	{ "PSP_SECG0_FILE",			AMD_FW_PSP_SEC_GASKET,			0, 0 },
	{ "PSP_SECG1_FILE",			AMD_FW_PSP_SEC_GASKET,			1, 0 },
	{ "PSP_SECG2_FILE",			AMD_FW_PSP_SEC_GASKET,			2, 0 },
	{ "PSP_MP2FW0_FILE",			AMD_FW_PSP_MP2_FW,			0, 0 },
	{ "PSP_MP2FW1_FILE",			AMD_FW_PSP_MP2_FW,			1, 0 },
	{ "PSP_MP2FW2_FILE",			AMD_FW_PSP_MP2_FW,			2, 0 },
	{ "PSP_C20MP_FILE",			AMD_FW_PSP_C20_MP,			0, 0 },
	{ "AMF_SRAM_FILE",			AMD_FW_PSP_AMF_SRAM,		0, 0 },
	{ "AMF_DRAM_FILE_INS0",			AMD_FW_PSP_AMF_DRAM,		0, 0 },
	{ "AMF_DRAM_FILE_INS1",			AMD_FW_PSP_AMF_DRAM,		0, 1 },
	{ "MFD_MPM_TEE_INS0",			AMD_FW_PSP_MFD_MPM,			0, 0 },
	{ "MFD_MPM_TEE_INS1",			AMD_FW_PSP_MFD_MPM,			0, 1 },
	{ "AMF_WLAN_FILE_INS0",			AMD_FW_PSP_AMF_WLAN,		0, 0 },
	{ "AMF_WLAN_FILE_INS1",			AMD_FW_PSP_AMF_WLAN,		0, 1 },
	{ "AMF_WLAN_FILE_INS2",			AMD_FW_PSP_AMF_WLAN,		0, 2 },
	{ "AMF_WLAN_FILE_INS3",			AMD_FW_PSP_AMF_WLAN,		0, 3 },
	{ "AMF_MFD_FILE",			AMD_FW_PSP_AMF_MFD,			0, 0 },
	{ "MPCCX_FILE",				AMD_FW_PSP_MPCCX,			0, 0 },
	{ "MPCCX_FILE_SUB1_FILE",		AMD_FW_PSP_MPCCX,			1, 0 },
	{ "LSDMA_FILE",				AMD_FW_PSP_LSDMA,			0, 0 },
	{ "MINIMSMU_FILE",			AMD_FW_PSP_MINIMSMU,		0, 0 },
	{ "MINIMSMU_FILE_SUB1_FILE",		AMD_FW_PSP_MINIMSMU,		1, 0 },
	{ "PSP_GFX_IMMU_FILE_0",		AMD_FW_PSP_GFXIMU_0,		0, 0 },
	{ "PSP_GFX_IMMU_FILE_0_SUB1",		AMD_FW_PSP_GFXIMU_0,		1, 0 },
	{ "PSP_GFX_IMMU_FILE_1",		AMD_FW_PSP_GFXIMU_1,		0, 0 },
	{ "PSP_GFX_IMMU_FILE_1_SUB1",		AMD_FW_PSP_GFXIMU_1,		1, 0 },
	{ "MINIMSMU_FILE_INS1",			AMD_FW_PSP_MINIMSMU,		0, 1 },
	{ "SRAM_FW_EXT_FILE",			AMD_FW_PSP_SRAM_FW_EXT,		0, 0 },
	{ "PSP_DRIVERS_FILE",			AMD_FW_PSP_DRIVER_ENTRIES,		0, 0 },
	{ "PSP_S0I3_FILE",			AMD_FW_PSP_S0I3_DRIVER,		0, 0 },
	{ "AMD_DRIVER_ENTRIES",			AMD_FW_PSP_DRIVER_ENTRIES,		0, 0 },
	{ "VBIOS_BTLOADER_FILE",		AMD_FW_PSP_VBIOS_BTLOADER,		0, 0 },
	{ "SECURE_POLICY_L1_FILE",		AMD_FW_PSP_TOS_SEC_POLICY,		0, 0 },
	{ "UNIFIEDUSB_FILE",			AMD_FW_PSP_USB_PHY,			0, 0 },
	{ "DRTMTA_FILE",			AMD_FW_PSP_DRTM_TA,			0, 0 },
	{ "KEYDBBL_FILE",			AMD_FW_PSP_KEYDB_BL,		0, 0 },
	{ "KEYDB_TOS_FILE",			AMD_FW_PSP_KEYDB_TOS,		0, 0 },
	{ "SPL_TABLE_FILE",			AMD_FW_PSP_SPL,			0, 0 },
	{ "DMCUERAMDCN21_FILE",			AMD_FW_PSP_DMCU_ERAM,		0, 0 },
	{ "DMCUINTVECTORSDCN21_FILE",		AMD_FW_PSP_DMCU_ISR,		0, 0 },
	{ "MSMU_FILE",				AMD_FW_PSP_MSMU,			0, 0 },
	{ "MSMU_FILE_SUB1_FILE",		AMD_FW_PSP_MSMU,			1, 0 },
	{ "DMCUB_FILE",				AMD_FW_PSP_DMCUB,			0, 0 },
	{ "SPIROM_CONFIG_FILE",			AMD_FW_PSP_SPIROM_CFG,		0, 0 },
	{ "MPIO_FILE",				AMD_FW_PSP_MPIO,			0, 0 },
	{ "TPMLITE_FILE",			AMD_FW_PSP_TPMLITE,			0, 0 },
	{ "PSP_KVM_ENGINE_DUMMY_FILE",		AMD_FW_PSP_KVM_IMAGE,		0, 0 },
	{ "RPMC_FILE",				AMD_FW_PSP_RPMC_NVRAM,			0, 0 },
	{ "PSPBTLDR_AB_FILE",			AMD_FW_PSP_BOOTLOADER_AB,	0, 0 },
	{ "TA_IKEK_FILE",			AMD_FW_PSP_TA_IKEK,			0, 0 },
	{ "SFDR_FILE",				AMD_FW_PSP_SFDR,			0, 0 },
	{ "UMSMU_FILE",				AMD_FW_PSP_UMSMU,			0, 0 },
	{ "PSP_S3_IMG",				AMD_FW_PSP_S3IMG,			0, 0 },
	{ "PSP_USB_DP",				AMD_FW_PSP_USBDP,			0, 0 },
	{ "PSP_USB_SS",				AMD_FW_PSP_USBSS,			0, 0 },
	{ "PSP_USB_4",				AMD_FW_PSP_USB4,			0, 0 },
	{ "PSP_OEM_ABL_KEY_FILE",		AMD_FW_PSP_ABL_PUBKEY,		0, 0 },
	{ "PSP_MP5FW_SUB0_FILE",		AMD_FW_PSP_MP5,			0, 0 },
	{ "PSP_MP5FW_SUB1_FILE",		AMD_FW_PSP_MP5,			1, 0 },
	{ "PSP_MP5FW_SUB2_FILE",		AMD_FW_PSP_MP5,			2, 0 },
	{ "PSP_DXIOFW_FILE",			AMD_FW_PSP_DXIO,			0, 0 },
	{ "PSP_MPIOFW_FILE",			AMD_FW_PSP_MPIO,			0, 0 },
	{ "PSP_RIB_FILE_SUB0",			AMD_FW_PSP_RIB,			0, 0 },
	{ "PSP_RIB_FILE_SUB1",			AMD_FW_PSP_RIB,			1, 0 },
	{ "PSP_RIB_FILE_SUB2",			AMD_FW_PSP_RIB,			2, 0 },
	{ "FEATURE_TABLE_FILE",			AMD_FW_PSP_FCFG_TABLE,		0, 0 },
	{ "PSP_MPDMATFFW_FILE",			AMD_FW_PSP_MPDMA_TF,		0, 0 },
	{ "PSP_GMI3PHYFW_FILE",			AMD_FW_PSP_GMI3_PHY,		0, 0 },
	{ "PSP_MPDMAPMFW_FILE",			AMD_FW_PSP_MPDMA_PM,		0, 0 },
	{ "PSP_TOKEN_UNLOCK_FILE",		AMD_FW_PSP_TOKEN_UNLOCK,		0, 0 },
	{ "SEV_DATA_FILE",			AMD_FW_PSP_SEV_DATA,			0, 0 },
	{ "SEV_CODE_FILE",			AMD_FW_PSP_SEV_CODE,			0, 0 },
	{ "PSP_SEV_DRIVER_FILE",		AMD_FW_PSP_SEV_DRIVER,			0, 0 },
	{ "PSP_RAS_DRIVER_FILE",		AMD_FW_PSP_RAS_DRIVER,			0, 0 },
	{ "PSP_RAS_TA_FILE",			AMD_FW_PSP_RAS_TA,			0, 0 },
	{ "PSP_FHP_DRIVER_FILE",		AMD_FW_PSP_FHP_DRIVER,			0, 0 },
	{ "PSP_SPDM_DRIVER_FILE",		AMD_FW_PSP_SPDM_DRIVER,			0, 0 },
	{ "PSP_DPE_DRIVER_FILE",		AMD_FW_PSP_DPE_DRIVER,			0, 0 },
	{ "PSP_TOS_WHITELIST",			AMD_FW_PSP_TOS_WHITELIST,		0, 0 },
	{ "SECURE_POLICY_L3_FILE",		AMD_FW_PSP_TOS_SEC_POLICY,		2, 0 },
	{ "CPU_S3_IMAGE_INS0",			AMD_FW_PSP_S3IMG,		0, 0 },
	{ "CPU_S3_IMAGE_INS1",			AMD_FW_PSP_S3IMG,		0, 1 },
	{ "CPU_S3_IMAGE_INS2",			AMD_FW_PSP_S3IMG,		0, 2 },
	{ "CPU_S3_IMAGE_INS3",			AMD_FW_PSP_S3IMG,		0, 3 },
	{ "CPU_S3_IMAGE_INS4",			AMD_FW_PSP_S3IMG,		0, 4 },
	{ "CPU_S3_IMAGE_INS5",			AMD_FW_PSP_S3IMG,		0, 5 },
	{ "CPU_S3_IMAGE_INS6",			AMD_FW_PSP_S3IMG,		0, 6 },
	{ NULL,					AMD_FW_PSP_INVALID,			0, 0 },
};

static amd_fw_type psp_fw_type_lookup(const char *fw_name, uint8_t *subprog, uint8_t *instance)
{
	const struct psp_fw_name_entry *entry = psp_fw_name_table;

	while (entry->name != NULL) {
		if (strcmp(fw_name, entry->name) == 0) {
			*subprog = entry->subprog;
			*instance = entry->instance;
			return entry->type;
		}
		entry++;
	}

	*subprog = 0;
	*instance = 0;
	return AMD_FW_PSP_INVALID;
}

/* TODO: a empty line does not matched. */
static const char blank_or_comment_regex[] =
	/* a blank line */
	"(^[[:space:]]*$)"
	"|"	/* or ... */
	/* a line consisting of: optional whitespace followed by */
	"(^[[:space:]]*"
	/* a '#' character and optionally, additional characters */
	"#.*$)";
static regex_t blank_or_comment_expr;

static const char entries_line_regex[] =
	/* optional whitespace */
	"^[[:space:]]*"
	/* followed by a chunk of nonwhitespace for macro field */
	"([^[:space:]]+)"
	/* followed by one or more whitespace characters */
	"[[:space:]]+"
	/* followed by a chunk of nonwhitespace for filename field */
	"([^[:space:]]+)"
	/* followed by an optional whitespace + chunk of nonwhitespace for level field
	   1st char L: Indicator of field "level"
	   2nd char:
	      Directory level to be dropped in.
	      1: Level 1
	      2: Level 2
	      b: Level both 1&2
	      x: use default value hardcoded in table
	   3rd char:
	      For A/B recovery. Defined same as 2nd char.

	   Examples:
	      L2: Level 2 for normal mode
	      L12: Level 1 for normal mode, level 2 for A/B mode
	      Lx1: Use default value for normal mode, level 1 for A/B mode
	 */
	"([[:space:]]+([Ll][12bxBX]{1,2}))?"
	/* followed by an optional whitespace + chunk of nonwhitespace for hash table field
	   1st char H: Indicator of field "Hash Table ID"
	   2nd char:
	      Table ID to be dropped in.
	      0: Table 0 / Default Unified Table
	      1: Table 1
	      ...
	      9: Table 9

	   Examples:
	      H2: Put the hash for the concerned entry in Hash Table 2
	 */
	"([[:space:]]+([Hh][0-9]+))?"
	/* followed by an optional whitespace + "UUID" to indicate the binary is using 16 bytes
	   UUID as firmware identity. In the absence of this field, the binary is using 2 bytes
	   FWID as firmware identity.
	*/
	"([[:space:]]+(UUID))?"
	/* followed by optional whitespace */
	"[[:space:]]*$";
static regex_t entries_line_expr;

enum match_id {
	FW_TYPE = 1,
	FW_FILE,
	OPT_SPACE1,
	OPT_LEVEL,
	OPT_SPACE2,
	OPT_HASH_TABLE_ID,
	OPT_SPACE3,
	OPT_FWID_TYPE,
	N_MATCHES,
};

void compile_reg_expr(int cflags, const char *expr, regex_t *reg)
{
	static const size_t ERROR_BUF_SIZE = 256;
	char error_msg[ERROR_BUF_SIZE];
	int result;

	result = regcomp(reg, expr, cflags);
	if (result != 0) {
		regerror(result, reg, error_msg, ERROR_BUF_SIZE);
		fprintf(stderr, "%s\n", error_msg);
	}
}

#define SET_LEVEL(tableptr, l, TABLE, ab)     \
	do {                                             \
		switch ((l)) {                           \
		case '1':				 \
			(tableptr)->level = ab ? TABLE##_LVL1_AB : TABLE##_LVL1; \
			break;                           \
		case '2':                                \
			(tableptr)->level = ab ? TABLE##_LVL2_AB : TABLE##_LVL2; \
			break;                           \
		case 'b':                                \
		case 'B':                                \
			(tableptr)->level = ab ? TABLE##_BOTH_AB : TABLE##_BOTH; \
			break;                           \
		default:                                 \
			/* use default value */          \
			break;                           \
		}                                        \
	} while (0)

extern amd_fw_entry amd_psp_fw_table[];
extern amd_bios_entry amd_bios_table[];

static uint8_t find_register_fw_filename_psp_dir(char *fw_name, char *filename,
						 char level_to_set, uint8_t hash_tbl_id,
						 fwid_type_t fwid_type, amd_cb_config *cb_config)
{
	amd_fw_type fw_type;
	amd_fw_entry *psp_tableptr;
	uint8_t subprog;
	uint8_t instance;

	fw_type = psp_fw_type_lookup(fw_name, &subprog, &instance);
	if (fw_type == AMD_FW_PSP_INVALID)
		return 0;

	/* Apply quirks based on cb_config. Returning 1 means skip the entry. */
	if (fw_type == AMD_FW_PSP_BOOTLOADER_AB) {
		if (strcmp(fw_name, "PSPBTLDR_AB_FILE") == 0 &&
		    cb_config->have_whitelist &&
		    !cb_config->recovery_ab)
			return 1;
		if (strcmp(fw_name, "PSPBTLDR_WL_FILE") == 0 &&
		    !cb_config->have_whitelist)
			return 1;
	} else if (fw_type == AMD_FW_PSP_BOOTLOADER) {
		if (strcmp(fw_name, "PSPBTLDR_AB_STAGE1_FILE") == 0 &&
		    !cb_config->recovery_ab)
			return 1;
		if (strcmp(fw_name, "PSPBTLDR_FILE") == 0 &&
		    cb_config->recovery_ab)
			return 1;
	} else if (fw_type == AMD_FW_PSP_SECURED_DEBUG) {
		if (strcmp(fw_name, "PSP_SEC_DBG_KEY_FILE") == 0 &&
		    !cb_config->unlock_secure)
			return 1;
	} else if (fw_type == AMD_FW_PSP_DEBUG_UNLOCK) {
		if (!cb_config->unlock_secure)
			return 1;
	} else if (fw_type == AMD_FW_PSP_SECURED_OS) {
		if (!cb_config->use_secureos)
			return 1;
	} else if (fw_type == AMD_FW_PSP_TRUSTLETKEY) {
		if (!cb_config->use_secureos)
			return 1;
	} else if (fw_type == AMD_FW_PSP_TRUSTLETS) {
		if (!cb_config->use_secureos)
			return 1;
	} else if (fw_type == AMD_FW_PSP_MP2_FW) {
		if (!cb_config->load_mp2_fw)
			return 1;
	} else if (fw_type == AMD_FW_PSP_S0I3_DRIVER) {
		if (!cb_config->s0i3)
			return 1;
	} else if (fw_type == AMD_FW_PSP_SPL) {
		if (cb_config->have_mb_spl)
			return 1;
	}

	/* Search and fill the filename */
	psp_tableptr = &amd_psp_fw_table[0];
	while (psp_tableptr->type != AMD_FW_PSP_INVALID) {
		/* instance are not used in PSP table */
		if (psp_tableptr->type == fw_type &&
		    psp_tableptr->subprog == subprog &&
		    psp_tableptr->inst  == instance) {
			if (psp_tableptr->type != AMD_FW_PSP_FUSE_CHAIN) {
				psp_tableptr->filename = filename;
				psp_tableptr->hash_tbl_id = hash_tbl_id;
				psp_tableptr->fwid_type = fwid_type;
			}
			SET_LEVEL(psp_tableptr, level_to_set, PSP,
				  cb_config->recovery_ab);
			break;
		}
		psp_tableptr++;
	}

	return 1;
}

#define PMUI_STR_BASE	"PSP_PMUI_FILE"
#define PMUD_STR_BASE	"PSP_PMUD_FILE"
#define PMU_STR_BASE_LEN strlen(PMUI_STR_BASE)
#define PMU_STR_SUB_INDEX strlen(PMUI_STR_BASE"_SUB")
#define PMU_STR_INS_INDEX strlen(PMUI_STR_BASE"_SUBx_INS")
#define PMU_STR_ALL_LEN  strlen(PMUI_STR_BASE"_SUBx_INSx")

static amd_bios_type bios_fw_type_lookup(char *fw_name, uint8_t *subprog, uint8_t *instance)
{
	if (strncmp(fw_name, PMUI_STR_BASE, PMU_STR_BASE_LEN) == 0) {
		assert(strlen(fw_name) == PMU_STR_ALL_LEN);
		*subprog = strtol(&fw_name[PMU_STR_SUB_INDEX], NULL, 16);
		*instance = strtol(&fw_name[PMU_STR_INS_INDEX], NULL, 16);
		return AMD_BIOS_PMUI;
	} else if (strncmp(fw_name, PMUD_STR_BASE, PMU_STR_BASE_LEN) == 0) {
		assert(strlen(fw_name) == PMU_STR_ALL_LEN);
		*subprog = strtol(&fw_name[PMU_STR_SUB_INDEX], NULL, 16);
		*instance = strtol(&fw_name[PMU_STR_INS_INDEX], NULL, 16);
		return AMD_BIOS_PMUD;
	} else if (strcmp(fw_name, "RTM_PUBKEY_FILE") == 0) {
		*subprog = 0;
		*instance = 0;
		return AMD_BIOS_RTM_PUBKEY;
	} else if (strcmp(fw_name, "PSP_MP2CFG_FILE") == 0) {
		*subprog = 0;
		*instance = 0;
		return AMD_BIOS_MP2_CFG;
	}

	*subprog = 0;
	*instance = 0;
	return AMD_BIOS_INVALID;
}

static uint8_t find_register_fw_filename_bios_dir(char *fw_name, char *filename,
		char level_to_set, amd_cb_config *cb_config)
{
	amd_bios_type fw_type = AMD_BIOS_INVALID;
	amd_bios_entry *bhd_tableptr;
	uint8_t subprog = 0;
	uint8_t instance = 0;

	fw_type = bios_fw_type_lookup(fw_name, &subprog, &instance);
	if (fw_type == AMD_BIOS_INVALID)
		return 0;

	/* Apply quirks based on cb_config */
	if (fw_type == AMD_BIOS_MP2_CFG && !cb_config->load_mp2_fw)
		return 1;

	bhd_tableptr = amd_bios_table;
	while (bhd_tableptr->type != AMD_BIOS_INVALID) {
		if (bhd_tableptr->type == fw_type &&
		    bhd_tableptr->subpr == subprog &&
		    bhd_tableptr->inst  == instance) {
			bhd_tableptr->filename = filename;
			SET_LEVEL(bhd_tableptr, level_to_set, BDT,
				cb_config->recovery_ab);
			break;
		}
		bhd_tableptr++;
	}

	return 1;
}

#define MAX_LINE_SIZE 1024

int get_input_file_line(FILE *f, char line[], int line_buf_size)
{
	if (fgets(line, line_buf_size, f) == NULL)
		return LINE_EOF;

	/* If the file contains a line that is too long, then it's best
	 * to let the user know right away rather than passing back a
	 * truncated result that will lead to problems later on.
	 */
	line[strlen(line) - 1] = '\0';

	if (strlen(line) == ((size_t) (line_buf_size - 1))) {
		fprintf(stderr, "The line size in config file should be lower than %d bytes.\n",
			MAX_LINE_SIZE);
		exit(1);
	}

	return OK;
}

static int is_valid_entry(char *oneline, regmatch_t match[N_MATCHES])
{
	int retval, index;

	for (index = 0; index < N_MATCHES; index++) {
		match[index].rm_so = -1;
		match[index].rm_eo = -1;
	}
	if (regexec(&entries_line_expr, oneline, N_MATCHES, match, 0) == 0) {
		/* match[1]: FW type
		   match[2]: FW filename
		   match[4]: Optional directory level to be dropped
		   match[6]: Optional hash table ID to put the hash for the entry
		 */
		if (match[FW_TYPE].rm_eo != -1)
			oneline[match[FW_TYPE].rm_eo] = '\0';
		if (match[FW_FILE].rm_eo != -1)
			oneline[match[FW_FILE].rm_eo] = '\0';
		if (match[OPT_LEVEL].rm_eo != -1)
			oneline[match[OPT_LEVEL].rm_eo] = '\0';
		if (match[OPT_HASH_TABLE_ID].rm_eo != -1)
			oneline[match[OPT_HASH_TABLE_ID].rm_eo] = '\0';
		if (match[OPT_FWID_TYPE].rm_eo != -1)
			oneline[match[OPT_FWID_TYPE].rm_eo] = '\0';
		retval = 1;
	} else {
		retval = 0;
	}

	return retval;
}

static int skip_comment_blank_line(char *oneline)
{
	int retval;

	if (regexec(&blank_or_comment_expr, oneline, 0, NULL, 0) == 0) {
		/* skip comment and blank */
		retval = 1;
	} else {
		/* no match */
		retval = 0;
	}

	return retval;
}

static char get_level_from_config(char *line, regoff_t level_index, amd_cb_config *cb_config)
{
	char lvl = 'x';
	/* If the optional level field is present, extract the level char. */
	if (level_index != -1) {
		if (cb_config->recovery_ab == 0)
			lvl = line[level_index + 1];
		else if (strlen(&line[level_index]) >= 3)
			lvl = line[level_index + 2];
	}

	assert(lvl == 'x' || lvl == 'X' ||
		lvl == 'b' || lvl == 'B' ||
		lvl == '1' || lvl == '2');

	return lvl;
}

static uint8_t get_hash_tbl_id(char *line, regoff_t hash_tbl_index)
{
	uint8_t tbl = 0;
	/* If the optional hash table field is present, extract the table id char. */
	if (hash_tbl_index != -1)
		tbl = (uint8_t)atoi(&line[hash_tbl_index + 1]);

	assert(tbl < MAX_NUM_HASH_TABLES);
	return tbl;
}

static fwid_type_t get_fwid_type(char *line, regoff_t fwid_type_index)
{
	if (fwid_type_index != -1 && !strncmp(&line[fwid_type_index], "UUID", strlen("UUID")))
		return FWID_TYPE_UUID;

	return FWID_TYPE_FWID;
}

static uint8_t process_one_line(char *oneline, regmatch_t *match, char *dir,
	amd_cb_config *cb_config)
{
	char *path_filename, *fn = &(oneline[match[FW_FILE].rm_so]);
	char *fw_type_str = &(oneline[match[FW_TYPE].rm_so]);
	regoff_t ch_lvl_index = match[OPT_LEVEL].rm_so == match[OPT_LEVEL].rm_eo ?
								-1 : match[OPT_LEVEL].rm_so;
	regoff_t ch_hash_tbl_index =
		match[OPT_HASH_TABLE_ID].rm_so == match[OPT_HASH_TABLE_ID].rm_eo ?
							-1 : match[OPT_HASH_TABLE_ID].rm_so;
	regoff_t ch_fwid_type_index = match[OPT_FWID_TYPE].rm_so == match[OPT_FWID_TYPE].rm_eo ?
								-1 : match[OPT_FWID_TYPE].rm_so;
	char ch_lvl = get_level_from_config(oneline, ch_lvl_index, cb_config);
	uint8_t ch_hash_tbl = get_hash_tbl_id(oneline, ch_hash_tbl_index);
	fwid_type_t ch_fwid_type = get_fwid_type(oneline, ch_fwid_type_index);

	path_filename = malloc(MAX_LINE_SIZE * 2 + 2);
	if (strchr(fn, '/'))
		snprintf(path_filename, MAX_LINE_SIZE * 2 + 2, "%.*s",
				MAX_LINE_SIZE, fn);
	else
		snprintf(path_filename, MAX_LINE_SIZE * 2 + 2, "%.*s/%.*s",
				MAX_LINE_SIZE, dir, MAX_LINE_SIZE, fn);

	if (find_register_fw_filename_psp_dir(fw_type_str, path_filename,
				ch_lvl, ch_hash_tbl, ch_fwid_type, cb_config) == 0) {
		if (find_register_fw_filename_bios_dir(fw_type_str, path_filename,
								ch_lvl, cb_config) == 0) {
			fprintf(stderr, "Module's name \"%s\" is not valid\n", fw_type_str);
			return 0; /* Stop parsing. */
		}
	}
	return 1;
}

#define FW_LOCATION "FIRMWARE_LOCATION"
#define SOC_NAME "SOC_NAME"
/*
  return value:
	0: The config file can not be parsed correctly.
	1: The config file can be parsed correctly.
 */
uint8_t process_config(FILE *config, amd_cb_config *cb_config)
{
	char oneline[MAX_LINE_SIZE];
	regmatch_t match[N_MATCHES];
	char dir[MAX_LINE_SIZE] = {'\0'};
	uint32_t dir_len;
	char *platform_name = NULL;
	int index;

	for (index = 0; index < N_MATCHES; index++) {
		match[index].rm_so = -1;
		match[index].rm_eo = -1;
	}

	compile_reg_expr(REG_EXTENDED | REG_NEWLINE,
		blank_or_comment_regex, &blank_or_comment_expr);
	compile_reg_expr(REG_EXTENDED | REG_NEWLINE,
		entries_line_regex, &entries_line_expr);

	/* Get a line */
	/* Get FIRMWARE_LOCATION in the first loop */
	while (get_input_file_line(config, oneline, MAX_LINE_SIZE) == OK) {
		/* get a line */
		if (skip_comment_blank_line(oneline))
			continue;
		if (is_valid_entry(oneline, match)) {
			if (strcmp(&(oneline[match[FW_TYPE].rm_so]), FW_LOCATION) == 0) {
				dir_len = match[FW_FILE].rm_eo - match[FW_FILE].rm_so;
				assert(dir_len < MAX_LINE_SIZE);
				snprintf(dir, MAX_LINE_SIZE, "%.*s", dir_len,
					&(oneline[match[FW_FILE].rm_so]));
			} else if (strcmp(&(oneline[match[FW_TYPE].rm_so]), SOC_NAME) == 0) {
				platform_name = strdup(&(oneline[match[FW_FILE].rm_so]));
				cb_config->soc_id = platform_identify(platform_name);
			}
		}
	}

	if (!platform_name) {
		fprintf(stderr, "AMDFWTOOL: Platform not specified in config\n");
		return 0;
	} else if (cb_config->soc_id == PLATFORM_UNKNOWN) {
		fprintf(stderr, "AMDFWTOOL: Unknown platform '%s'\n", platform_name);
		free(platform_name);
		return 0;
	}
	free(platform_name);

	if (platform_needs_ish(cb_config->soc_id))
		cb_config->recovery_ab = true;

	if (dir[0] == '\0') {
		fprintf(stderr, "No line with FIRMWARE_LOCATION\n");
		return 0;
	}

	fseek(config, 0, SEEK_SET);
	/* Get a line */
	while (get_input_file_line(config, oneline, MAX_LINE_SIZE) == OK) {
		/* get a line */
		if (skip_comment_blank_line(oneline))
			continue;
		if (is_valid_entry(oneline, match)) {
			if (strcmp(&(oneline[match[FW_TYPE].rm_so]), FW_LOCATION) == 0 ||
				strcmp(&(oneline[match[FW_TYPE].rm_so]), SOC_NAME) == 0) {
				continue;
			} else {
				if (process_one_line(oneline, match, dir,
						cb_config) == 0)
					return 0;
			}
		} else {
			fprintf(stderr, "AMDFWTOOL config file line can't be parsed \"%s\"\n", oneline);
			return 0;
		}
	}
	return 1;
}
