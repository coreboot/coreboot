/* SPDX-License-Identifier: BSD-3-Clause */
/* This is a driver for a Command Response Buffer Interface
 * as found on AMD fTPMs. It only implements a small subset
 * of the registers defined in the TCG specification.
 */

#include <amdblocks/psp.h>
#include <security/tpm/tis.h>
#include <security/tpm/tss_errors.h>

/* CRB hardware registers */
#define CRB_REG_STATUS			0x44
#define  CRB_REG_STATUS_ERROR		0x01
#define CRB_REG_START			0x4C
#define  CRB_REG_START_START		0x01
#define CRB_REG_CMD_SIZE		0x58
#define CRB_REG_CMD_ADDR		0x5C
#define CRB_REG_RESP_SIZE		0x64
#define CRB_REG_RESP_ADDR		0x68

/* address of locality 0 (CRB) */
#define CRB_REG(REG) ((void *)(uintptr_t)(psp_ftpm_base_address() + (REG)))

tpm_result_t crb_tpm_init(void);
size_t crb_tpm_process_command(const void *tpm2_command, size_t command_size,
			       void *tpm2_response, size_t max_response);
bool crb_tpm_is_active(void);

tis_sendrecv_fn crb_tis_probe(enum tpm_family *family);

uintptr_t crb_tpm_base_address(void);
