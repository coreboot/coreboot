/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "amdfwtool.h"

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

static enum platform identify_platform(char *soc_name)
{
	if (!strcasecmp(soc_name, "Stoneyridge"))
		return PLATFORM_STONEYRIDGE;
	else if (!strcasecmp(soc_name, "Carrizo"))
		return PLATFORM_CARRIZO;
	else if (!strcasecmp(soc_name, "Raven"))
		return PLATFORM_RAVEN;
	else if (!strcasecmp(soc_name, "Picasso"))
		return PLATFORM_PICASSO;
	else if (!strcasecmp(soc_name, "Cezanne"))
		return PLATFORM_CEZANNE;
	else if (!strcasecmp(soc_name, "Mendocino"))
		return PLATFORM_MENDOCINO;
	else if (!strcasecmp(soc_name, "Renoir"))
		return PLATFORM_RENOIR;
	else if (!strcasecmp(soc_name, "Lucienne"))
		return PLATFORM_LUCIENNE;
	else if (!strcasecmp(soc_name, "Phoenix"))
		return PLATFORM_PHOENIX;
	else if (!strcasecmp(soc_name, "Glinda"))
		return PLATFORM_GLINDA;
	else if (!strcasecmp(soc_name, "Genoa"))
		return PLATFORM_GENOA;
	else if (!strcasecmp(soc_name, "Faegan"))
		return PLATFORM_FAEGAN;
	else
		return PLATFORM_UNKNOWN;
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
		char level_to_set, uint8_t hash_tbl_id, fwid_type_t fwid_type,
		amd_cb_config *cb_config)
{
	amd_fw_type fw_type = AMD_FW_INVALID;
	amd_fw_entry *psp_tableptr;
	uint8_t subprog;
	uint8_t instance = 0;

	if (strcmp(fw_name, "PSPBTLDR_WL_FILE") == 0) {
		if (cb_config->have_whitelist) {
			fw_type = AMD_FW_PSP_BOOTLOADER_AB;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSPBTLDR_AB_STAGE1_FILE") == 0) {
		if (cb_config->recovery_ab) {
			fw_type = AMD_FW_PSP_BOOTLOADER;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSPBTLDR_FILE") == 0) {
		if (!cb_config->recovery_ab) {
			fw_type = AMD_FW_PSP_BOOTLOADER;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "AMD_PUBKEY_FILE") == 0) {
		fw_type = AMD_FW_PSP_PUBKEY;
		subprog = 0;
	} else if (strcmp(fw_name, "AMD_FUSE_CHAIN") == 0) {
		fw_type = AMD_PSP_FUSE_CHAIN;
		subprog = 0;
	} else if (strcmp(fw_name, "PSPRCVR_FILE") == 0) {
		fw_type = AMD_FW_PSP_RECOVERY;
		subprog = 0;
	} else if (strcmp(fw_name, "PUBSIGNEDKEY_FILE") == 0) {
		fw_type = AMD_FW_PSP_RTM_PUBKEY;
		subprog = 0;
	} else if (strcmp(fw_name, "PSPNVRAM_FILE") == 0) {
		fw_type = AMD_FW_PSP_NVRAM;
		subprog = 0;
	} else if (strcmp(fw_name, "SMUSCS_FILE") == 0) {
		fw_type = AMD_FW_PSP_SMUSCS;
		subprog = 0;
	} else if (strcmp(fw_name, "PSPTRUSTLETS_FILE") == 0) {
		fw_type = AMD_FW_PSP_TRUSTLETS;
		subprog = 0;
	} else if (strcmp(fw_name, "PSPSECUREDEBUG_FILE") == 0) {
		fw_type = AMD_FW_PSP_SECURED_DEBUG;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_SMUFW1_SUB0_FILE") == 0) {
		fw_type = AMD_FW_PSP_SMU_FIRMWARE;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_HW_IPCFG_FILE_SUB0") == 0) {
		fw_type = AMD_HW_IPCFG;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_HW_IPCFG_FILE_SUB1") == 0) {
		fw_type = AMD_HW_IPCFG;
		subprog = 1;
	} else if (strcmp(fw_name, "PSP_HW_IPCFG_FILE_SUB2") == 0) {
		fw_type = AMD_HW_IPCFG;
		subprog = 2;
	} else if (strcmp(fw_name, "PSP_SMUFW1_SUB1_FILE") == 0) {
		fw_type = AMD_FW_PSP_SMU_FIRMWARE;
		subprog = 1;
	} else if (strcmp(fw_name, "PSP_SMUFW1_SUB2_FILE") == 0) {
		fw_type = AMD_FW_PSP_SMU_FIRMWARE;
		subprog = 2;
	} else if (strcmp(fw_name, "PSP_SMUFW2_SUB0_FILE") == 0) {
		fw_type = AMD_FW_PSP_SMU_FIRMWARE2;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_SMUFW2_SUB1_FILE") == 0) {
		fw_type = AMD_FW_PSP_SMU_FIRMWARE2;
		subprog = 1;
	} else if (strcmp(fw_name, "PSP_TEEIPKEY_FILE") == 0) {
		fw_type = AMD_FW_PSP_TEEIPKEY;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_SMUFW2_SUB2_FILE") == 0) {
		fw_type = AMD_FW_PSP_SMU_FIRMWARE2;
		subprog = 2;
	} else if (strcmp(fw_name, "PSP_BOOT_DRIVER_FILE") == 0) {
		fw_type = AMD_BOOT_DRIVER;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_SOC_DRIVER_FILE") == 0) {
		fw_type = AMD_SOC_DRIVER;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_DEBUG_DRIVER_FILE") == 0) {
		fw_type = AMD_DEBUG_DRIVER;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_INTERFACE_DRIVER_FILE") == 0) {
		fw_type = AMD_INTERFACE_DRIVER;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_SEC_DBG_KEY_FILE") == 0) {
		if (cb_config->unlock_secure) {
			fw_type = AMD_FW_PSP_SECURED_DEBUG;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_SEC_DEBUG_FILE") == 0) {
		if (cb_config->unlock_secure) {
			fw_type = AMD_DEBUG_UNLOCK;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_ABL0_FILE") == 0) {
		fw_type = AMD_ABL0;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_ABL1_FILE") == 0) {
		fw_type = AMD_ABL1;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_ABL2_FILE") == 0) {
		fw_type = AMD_ABL2;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_ABL3_FILE") == 0) {
		fw_type = AMD_ABL3;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_ABL4_FILE") == 0) {
		fw_type = AMD_ABL4;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_ABL5_FILE") == 0) {
		fw_type = AMD_ABL5;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_ABL6_FILE") == 0) {
		fw_type = AMD_ABL6;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_ABL7_FILE") == 0) {
		fw_type = AMD_ABL7;
		subprog = 0;
	} else if (strcmp(fw_name, "PSPSECUREOS_FILE") == 0) {
		if (cb_config->use_secureos) {
			fw_type = AMD_FW_PSP_SECURED_OS;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSPTRUSTLETS_FILE") == 0) {
		if (cb_config->use_secureos) {
			fw_type = AMD_FW_PSP_TRUSTLETS;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "TRUSTLETKEY_FILE") == 0) {
		if (cb_config->use_secureos) {
			fw_type = AMD_FW_PSP_TRUSTLETKEY;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_IKEK_FILE") == 0) {
		fw_type = AMD_WRAPPED_IKEK;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_SECG0_FILE") == 0) {
		fw_type = AMD_SEC_GASKET;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_SECG1_FILE") == 0) {
		fw_type = AMD_SEC_GASKET;
		subprog = 1;
	} else if (strcmp(fw_name, "PSP_SECG2_FILE") == 0) {
		fw_type = AMD_SEC_GASKET;
		subprog = 2;
	} else if (strcmp(fw_name, "PSP_MP2FW0_FILE") == 0) {
		if (cb_config->load_mp2_fw) {
			fw_type = AMD_MP2_FW;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_MP2FW1_FILE") == 0) {
		if (cb_config->load_mp2_fw) {
			fw_type = AMD_MP2_FW;
			subprog = 1;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_MP2FW2_FILE") == 0) {
		if (cb_config->load_mp2_fw) {
			fw_type = AMD_MP2_FW;
			subprog = 2;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_C20MP_FILE") == 0) {
		fw_type = AMD_FW_C20_MP;
		subprog = 0;
	} else if (strcmp(fw_name, "AMF_SRAM_FILE") == 0) {
		fw_type = AMD_FW_AMF_SRAM;
		subprog = 0;
	} else if (strcmp(fw_name, "AMF_DRAM_FILE_INS0") == 0) {
		fw_type = AMD_FW_AMF_DRAM;
		subprog = 0;
		instance = 0;
	} else if (strcmp(fw_name, "AMF_DRAM_FILE_INS1") == 0) {
		fw_type = AMD_FW_AMF_DRAM;
		subprog = 0;
		instance = 1;
	} else if (strcmp(fw_name, "MFD_MPM_TEE_INS0") == 0) {
		fw_type = AMD_FW_MFD_MPM;
		subprog = 0;
		instance = 0;
	} else if (strcmp(fw_name, "MFD_MPM_TEE_INS1") == 0) {
		fw_type = AMD_FW_MFD_MPM;
		subprog = 0;
		instance = 1;
	} else if (strcmp(fw_name, "AMF_WLAN_FILE_INS0") == 0) {
		fw_type = AMD_FW_AMF_WLAN;
		subprog = 0;
		instance = 0;
	} else if (strcmp(fw_name, "AMF_WLAN_FILE_INS1") == 0) {
		fw_type = AMD_FW_AMF_WLAN;
		subprog = 0;
		instance = 1;
	} else if (strcmp(fw_name, "AMF_WLAN_FILE_INS2") == 0) {
		fw_type = AMD_FW_AMF_WLAN;
		subprog = 0;
		instance = 2;
	} else if (strcmp(fw_name, "AMF_WLAN_FILE_INS3") == 0) {
		fw_type = AMD_FW_AMF_WLAN;
		subprog = 0;
		instance = 3;
	} else if (strcmp(fw_name, "AMF_MFD_FILE") == 0) {
		fw_type = AMD_FW_AMF_MFD;
		subprog = 0;
	} else if (strcmp(fw_name, "MPCCX_FILE") == 0) {
		fw_type = AMD_FW_MPCCX;
		subprog = 0;
	} else if (strcmp(fw_name, "MPCCX_FILE_SUB1_FILE") == 0) {
		fw_type = AMD_FW_MPCCX;
		subprog = 1;
	} else if (strcmp(fw_name, "LSDMA_FILE") == 0) {
		fw_type = AMD_FW_LSDMA;
		subprog = 0;
	} else if (strcmp(fw_name, "MINIMSMU_FILE") == 0) {
		fw_type = AMD_FW_MINIMSMU;
		instance = 0;
		subprog = 0;
	} else if (strcmp(fw_name, "MINIMSMU_FILE_SUB1_FILE") == 0) {
		fw_type = AMD_FW_MINIMSMU;
		instance = 0;
		subprog = 1;
	} else if (strcmp(fw_name, "PSP_GFX_IMMU_FILE_0") == 0) {
		fw_type = AMD_FW_GFXIMU_0;
		instance = 0;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_GFX_IMMU_FILE_01") == 0) {
		fw_type = AMD_FW_GFXIMU_0;
		instance = 0;
		subprog = 1;
	} else if (strcmp(fw_name, "PSP_GFX_IMMU_FILE_1") == 0) {
		fw_type = AMD_FW_GFXIMU_1;
		instance = 0;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_GFX_IMMU_FILE_11") == 0) {
		fw_type = AMD_FW_GFXIMU_1;
		instance = 0;
		subprog = 1;
	} else if (strcmp(fw_name, "PSP_GFX_IMMU_FILE_2") == 0) {
		fw_type = AMD_FW_GFXIMU_2;
		instance = 0;
		subprog = 0;
	} else if (strcmp(fw_name, "MINIMSMU_FILE_INS1") == 0) {
		fw_type = AMD_FW_MINIMSMU;
		instance = 1;
		subprog = 0;
	} else if (strcmp(fw_name, "SRAM_FW_EXT_FILE") == 0) {
		fw_type = AMD_FW_SRAM_FW_EXT;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_DRIVERS_FILE") == 0) {
		fw_type = AMD_DRIVER_ENTRIES;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_S0I3_FILE") == 0) {
		if (cb_config->s0i3) {
			fw_type = AMD_S0I3_DRIVER;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "AMD_DRIVER_ENTRIES") == 0) {
		fw_type = AMD_DRIVER_ENTRIES;
		subprog = 0;
	} else if (strcmp(fw_name, "VBIOS_BTLOADER_FILE") == 0) {
		fw_type = AMD_VBIOS_BTLOADER;
		subprog = 0;
	} else if (strcmp(fw_name, "SECURE_POLICY_L1_FILE") == 0) {
		fw_type = AMD_FW_TOS_SEC_POLICY;
		subprog = 0;
	} else if (strcmp(fw_name, "UNIFIEDUSB_FILE") == 0) {
		fw_type = AMD_FW_USB_PHY;
		subprog = 0;
	} else if (strcmp(fw_name, "DRTMTA_FILE") == 0) {
		fw_type = AMD_FW_DRTM_TA;
		subprog = 0;
	} else if (strcmp(fw_name, "KEYDBBL_FILE") == 0) {
		fw_type = AMD_FW_KEYDB_BL;
		subprog = 0;
	} else if (strcmp(fw_name, "KEYDB_TOS_FILE") == 0) {
		fw_type = AMD_FW_KEYDB_TOS;
		subprog = 0;
	} else if (strcmp(fw_name, "SPL_TABLE_FILE") == 0) {
		if (cb_config->have_mb_spl) {
			fw_type = AMD_FW_SKIP;
		} else {
			fw_type = AMD_FW_SPL;
			subprog = 0;
		}
	} else if (strcmp(fw_name, "DMCUERAMDCN21_FILE") == 0) {
		fw_type = AMD_FW_DMCU_ERAM;
		subprog = 0;
	} else if (strcmp(fw_name, "DMCUINTVECTORSDCN21_FILE") == 0) {
		fw_type = AMD_FW_DMCU_ISR;
		subprog = 0;
	} else if (strcmp(fw_name, "MSMU_FILE") == 0) {
		fw_type = AMD_FW_MSMU;
		subprog = 0;
	} else if (strcmp(fw_name, "MSMU_FILE_SUB1_FILE") == 0) {
		fw_type = AMD_FW_MSMU;
		subprog = 1;
	} else if (strcmp(fw_name, "DMCUB_FILE") == 0) {
		fw_type = AMD_FW_DMCUB;
		subprog = 0;
	} else if (strcmp(fw_name, "SPIROM_CONFIG_FILE") == 0) {
		fw_type = AMD_FW_SPIROM_CFG;
		subprog = 0;
	} else if (strcmp(fw_name, "MPIO_FILE") == 0) {
		fw_type = AMD_FW_MPIO;
		subprog = 0;
	} else if (strcmp(fw_name, "TPMLITE_FILE") == 0) {
		fw_type = AMD_FW_TPMLITE;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_KVM_ENGINE_DUMMY_FILE") == 0) {
		fw_type = AMD_FW_KVM_IMAGE;
		subprog = 0;
	} else if (strcmp(fw_name, "RPMC_FILE") == 0) {
		fw_type = AMD_RPMC_NVRAM;
		subprog = 0;
	} else if (strcmp(fw_name, "PSPBTLDR_AB_FILE") == 0) {
		if (!cb_config->have_whitelist || cb_config->recovery_ab) {
			fw_type = AMD_FW_PSP_BOOTLOADER_AB;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "TA_IKEK_FILE") == 0) {
		fw_type = AMD_TA_IKEK;
		subprog = 0;
	} else if (strcmp(fw_name, "UMSMU_FILE") == 0) {
		fw_type = AMD_FW_UMSMU;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_S3_IMG") == 0) {
		fw_type = AMD_FW_S3IMG;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_USB_DP") == 0) {
		fw_type = AMD_FW_USBDP;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_USB_SS") == 0) {
		fw_type = AMD_FW_USBSS;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_USB_4") == 0) {
		fw_type = AMD_FW_USB4;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_OEM_ABL_KEY_FILE") == 0) {
		fw_type = AMD_FW_ABL_PUBKEY;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_MP5FW_SUB0_FILE") == 0) {
		fw_type = AMD_FW_MP5;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_MP5FW_SUB1_FILE") == 0) {
		fw_type = AMD_FW_MP5;
		subprog = 1;
	} else if (strcmp(fw_name, "PSP_MP5FW_SUB2_FILE") == 0) {
		fw_type = AMD_FW_MP5;
		subprog = 2;
	} else if (strcmp(fw_name, "PSP_DXIOFW_FILE") == 0) {
		fw_type = AMD_FW_DXIO;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_MPIOFW_FILE") == 0) {
		fw_type = AMD_FW_MPIO;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_RIB_FILE_SUB0") == 0) {
		fw_type = AMD_RIB;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_RIB_FILE_SUB1") == 0) {
		fw_type = AMD_RIB;
		subprog = 1;
	} else if (strcmp(fw_name, "PSP_RIB_FILE_SUB2") == 0) {
		fw_type = AMD_RIB;
		subprog = 2;
	} else if (strcmp(fw_name, "FEATURE_TABLE_FILE") == 0) {
		fw_type = AMD_FW_FCFG_TABLE;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_MPDMATFFW_FILE") == 0) {
		fw_type = AMD_FW_MPDMA_TF;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_GMI3PHYFW_FILE") == 0) {
		fw_type = AMD_FW_GMI3_PHY;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_MPDMAPMFW_FILE") == 0) {
		fw_type = AMD_FW_MPDMA_PM;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_TOKEN_UNLOCK_FILE") == 0) {
		fw_type = AMD_TOKEN_UNLOCK;
		subprog = 0;
	} else if (strcmp(fw_name, "SEV_DATA_FILE") == 0) {
		fw_type = AMD_SEV_DATA;
		subprog = 0;
	} else if (strcmp(fw_name, "SEV_CODE_FILE") == 0) {
		fw_type = AMD_SEV_CODE;
		subprog = 0;
	} else {
		fw_type = AMD_FW_INVALID;
		/* TODO: Add more */
	}

	/* Search and fill the filename */
	psp_tableptr = &amd_psp_fw_table[0];
	if (fw_type != AMD_FW_SKIP && fw_type != AMD_FW_INVALID) {
		while (psp_tableptr->type != AMD_FW_INVALID) {
			/* instance are not used in PSP table */
			if (psp_tableptr->type == fw_type && psp_tableptr->subprog == subprog
				&& psp_tableptr->inst  == instance) {
				if (psp_tableptr->type != AMD_PSP_FUSE_CHAIN) {
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
	}
	if (fw_type == AMD_FW_INVALID)
		return 0;
	else
		return 1;
}
#define PMUI_STR_BASE	"PSP_PMUI_FILE"
#define PMUD_STR_BASE	"PSP_PMUD_FILE"
#define PMU_STR_BASE_LEN strlen(PMUI_STR_BASE)
#define PMU_STR_SUB_INDEX strlen(PMUI_STR_BASE"_SUB")
#define PMU_STR_INS_INDEX strlen(PMUI_STR_BASE"_SUBx_INS")
#define PMU_STR_ALL_LEN  strlen(PMUI_STR_BASE"_SUBx_INSx")

static uint8_t find_register_fw_filename_bios_dir(char *fw_name, char *filename,
		char level_to_set, amd_cb_config *cb_config)
{
	amd_bios_type fw_type = AMD_BIOS_INVALID;
	amd_bios_entry *bhd_tableptr;
	uint8_t subprog = 0;
	uint8_t instance = 0;

	(void) (cb_config);	/* Remove warning and reserved for future. */

	if (strncmp(fw_name, PMUI_STR_BASE, PMU_STR_BASE_LEN) == 0) {
		assert(strlen(fw_name) == PMU_STR_ALL_LEN);
		fw_type = AMD_BIOS_PMUI;
		subprog = strtol(&fw_name[PMU_STR_SUB_INDEX], NULL, 16);
		instance = strtol(&fw_name[PMU_STR_INS_INDEX], NULL, 16);
	} else if (strncmp(fw_name, PMUD_STR_BASE, PMU_STR_BASE_LEN) == 0) {
		assert(strlen(fw_name) == PMU_STR_ALL_LEN);
		fw_type = AMD_BIOS_PMUD;
		subprog = strtol(&fw_name[PMU_STR_SUB_INDEX], NULL, 16);
		instance = strtol(&fw_name[PMU_STR_INS_INDEX], NULL, 16);
	} else if (strcmp(fw_name, "RTM_PUBKEY_FILE") == 0) {
		fw_type = AMD_BIOS_RTM_PUBKEY;
		subprog = 0;
		instance = 0;
	} else if (strcmp(fw_name, "PSP_MP2CFG_FILE") == 0) {
		if (cb_config->load_mp2_fw) {
			fw_type = AMD_BIOS_MP2_CFG;
			subprog = 0;
		} else {
			fw_type = AMD_BIOS_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_MP2CFG_FILE") == 0) {
		fw_type = AMD_BIOS_MP2_CFG;
		subprog = 0;
	} else {
		fw_type = AMD_BIOS_INVALID;
	}

	bhd_tableptr = amd_bios_table;

	if (fw_type != AMD_BIOS_INVALID && fw_type != AMD_BIOS_SKIP) {
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
	}
	if (fw_type == AMD_BIOS_INVALID)
		return 0;
	else
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

bool needs_ish(enum platform platform_type)
{
	if (platform_type == PLATFORM_MENDOCINO || platform_type == PLATFORM_PHOENIX ||
		platform_type == PLATFORM_GLINDA || platform_type == PLATFORM_FAEGAN)
		return true;
	else
		return false;
}

static bool is_second_gen(enum platform platform_type)
{
	switch (platform_type) {
	case PLATFORM_CARRIZO:
	case PLATFORM_STONEYRIDGE:
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
		return false;
	case PLATFORM_RENOIR:
	case PLATFORM_LUCIENNE:
	case PLATFORM_CEZANNE:
	case PLATFORM_MENDOCINO:
	case PLATFORM_PHOENIX:
	case PLATFORM_GLINDA:
	case PLATFORM_GENOA:
	case PLATFORM_FAEGAN:
		return true;
	case PLATFORM_UNKNOWN:
	default:
		fprintf(stderr, "Error: Invalid SOC name.\n\n");
		return false;
	}
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
				cb_config->soc_id = identify_platform(
							&(oneline[match[FW_FILE].rm_so]));
			}
		}
	}

	cb_config->second_gen = is_second_gen(cb_config->soc_id);

	if (needs_ish(cb_config->soc_id))
		cb_config->need_ish = true;

	if (cb_config->need_ish)
		cb_config->recovery_ab = true;

	if (cb_config->recovery_ab)
		cb_config->multi_level = true;

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
