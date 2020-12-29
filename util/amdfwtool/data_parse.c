#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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
	/* followed by optional whitespace */
	"[[:space:]]*$";
static regex_t entries_line_expr;

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

extern amd_fw_entry amd_psp_fw_table[];
extern amd_bios_entry amd_bios_table[];

static uint8_t find_register_fw_filename_psp_dir(char *fw_name, char *filename,
		amd_cb_config *cb_config)
{
	amd_fw_type fw_type = AMD_FW_INVALID;
	amd_fw_entry *psp_tableptr;
	uint8_t subprog;

	if (strcmp(fw_name, "PSPBTLDR_WL_FILE") == 0) {
		if (cb_config->have_whitelist == 1) {
			fw_type = AMD_FW_PSP_BOOTLOADER;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSPBTLDR_FILE") == 0) {
		if (cb_config->have_whitelist == 0) {
			fw_type = AMD_FW_PSP_BOOTLOADER;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "AMD_PUBKEY_FILE") == 0) {
		fw_type = AMD_FW_PSP_PUBKEY;
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
	} else if (strcmp(fw_name, "PSP_HW_IPCFG_FILE") == 0) {
		fw_type = AMD_HW_IPCFG;
		subprog = 0;
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
	} else if (strcmp(fw_name, "PSP_SMUFW2_SUB2_FILE") == 0) {
		fw_type = AMD_FW_PSP_SMU_FIRMWARE2;
		subprog = 2;
	} else if (strcmp(fw_name, "PSP_SEC_DBG_KEY_FILE") == 0) {
		if (cb_config->unlock_secure == 1) {
			fw_type = AMD_FW_PSP_SECURED_DEBUG;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_SEC_DEBUG_FILE") == 0) {
		if (cb_config->unlock_secure == 1) {
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
		if (cb_config->use_secureos == 1) {
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
		if (cb_config->load_mp2_fw == 1) {
			fw_type = AMD_MP2_FW;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_MP2FW1_FILE") == 0) {
		if (cb_config->load_mp2_fw == 1) {
			fw_type = AMD_MP2_FW;
			subprog = 1;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_MP2FW2_FILE") == 0) {
		if (cb_config->load_mp2_fw == 1) {
			fw_type = AMD_MP2_FW;
			subprog = 2;
		} else {
			fw_type = AMD_FW_SKIP;
		}
	} else if (strcmp(fw_name, "PSP_DRIVERS_FILE") == 0) {
		fw_type = AMD_DRIVER_ENTRIES;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_S0I3_FILE") == 0) {
		if (cb_config->s0i3 == 1) {
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
	} else if (strcmp(fw_name, "DMCUERAMDCN21_FILE") == 0) {
		fw_type = AMD_FW_DMCU_ERAM;
		subprog = 0;
	} else if (strcmp(fw_name, "DMCUINTVECTORSDCN21_FILE") == 0) {
		fw_type = AMD_FW_DMCU_ISR;
		subprog = 0;
	} else if (strcmp(fw_name, "PSP_KVM_ENGINE_DUMMY_FILE") == 0) {
		fw_type = AMD_FW_KVM_IMAGE;
		subprog = 0;
	} else if (strcmp(fw_name, "RPMC_FILE") == 0) {
		fw_type = AMD_RPMC_NVRAM;
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
			if (psp_tableptr->type == fw_type && psp_tableptr->subprog == subprog) {
				psp_tableptr->filename = filename;
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

static uint8_t find_register_fw_filename_bios_dir(char *fw_name, char *filename,
		amd_cb_config *cb_config)
{
	amd_bios_type fw_type = AMD_BIOS_INVALID;
	amd_bios_entry *bhd_tableptr;
	uint8_t subprog = 0;
	uint8_t instance = 0;

	(void) (cb_config);	/* Remove warning and reserved for future. */

	if (strcmp(fw_name, "PSP_PMUI_FILE1") == 0) {
		fw_type = AMD_BIOS_PMUI;
		subprog = 0;
		instance = 1;
	} else if (strcmp(fw_name, "PSP_PMUI_FILE2") == 0) {
		fw_type = AMD_BIOS_PMUI;
		subprog = 0;
		instance = 4;
	} else if (strcmp(fw_name, "PSP_PMUI_FILE3") == 0) {
		fw_type = AMD_BIOS_PMUI;
		subprog = 1;
		instance = 1;
	} else if (strcmp(fw_name, "PSP_PMUI_FILE4") == 0) {
		fw_type = AMD_BIOS_PMUI;
		subprog = 1;
		instance = 4;
	} else if (strcmp(fw_name, "PSP_PMUD_FILE1") == 0) {
		fw_type = AMD_BIOS_PMUD;
		subprog = 0;
		instance = 1;
	} else if (strcmp(fw_name, "PSP_PMUD_FILE2") == 0) {
		fw_type = AMD_BIOS_PMUD;
		subprog = 0;
		instance = 4;
	} else if (strcmp(fw_name, "PSP_PMUD_FILE3") == 0) {
		fw_type = AMD_BIOS_PMUD;
		subprog = 1;
		instance = 1;
	} else if (strcmp(fw_name, "PSP_PMUD_FILE4") == 0) {
		fw_type = AMD_BIOS_PMUD;
		subprog = 1;
		instance = 4;
	} else if (strcmp(fw_name, "RTM_PUBKEY_FILE") == 0) {
		fw_type = AMD_BIOS_RTM_PUBKEY;
		subprog = 0;
		instance = 0;
	} else if (strcmp(fw_name, "PSP_MP2CFG_FILE") == 0) {
		if (cb_config->load_mp2_fw == 1) {
			fw_type = AMD_BIOS_MP2_CFG;
			subprog = 0;
		} else {
			fw_type = AMD_FW_SKIP;
		}
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

static int is_valid_entry(char *oneline, regmatch_t *match)
{
	int retval;

	if (regexec(&entries_line_expr, oneline, 3, match, 0) == 0) {
		oneline[match[1].rm_eo] = '\0';
		oneline[match[2].rm_eo] = '\0';
		retval = 1;
	} else
		retval = 0;

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

#define N_MATCHES 4
/*
  return value:
	0: The config file can not be parsed correctly.
	1: The config file can be parsed correctly.
 */
uint8_t process_config(FILE *config, amd_cb_config *cb_config, uint8_t print_deps)
{
	char oneline[MAX_LINE_SIZE], *path_filename;
	regmatch_t match[N_MATCHES];
	char dir[MAX_LINE_SIZE] = {'\0'};

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
			if (strcmp(&(oneline[match[1].rm_so]), "FIRMWARE_LOCATION") == 0) {
				strcpy(dir, &(oneline[match[2].rm_so]));
				break;
			}
		}
	}

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
			if (strcmp(&(oneline[match[1].rm_so]), "FIRMWARE_LOCATION") == 0) {
				continue;
			} else {
				path_filename = malloc(MAX_LINE_SIZE);
				strcpy(path_filename, dir);
				strcat(path_filename, "/");
				strcat(path_filename, &(oneline[match[2].rm_so]));

				if (find_register_fw_filename_psp_dir(
						&(oneline[match[1].rm_so]),
						path_filename, cb_config) == 0) {
					if (find_register_fw_filename_bios_dir(
							&(oneline[match[1].rm_so]),
							path_filename, cb_config) == 0) {
						fprintf(stderr, "Module's name \"%s\" is not valid\n", oneline);
						return 0; /* Stop parsing. */
					} else {
						if (print_deps)
							printf(" %s ", path_filename);
					}
				} else {
					if (print_deps)
						printf(" %s ", path_filename);
				}
			}
		} else {
			fprintf(stderr, "AMDFWTOOL config file line can't be parsed \"%s\"\n", oneline);
			return 0;
		}
	}
	return 1;
}
