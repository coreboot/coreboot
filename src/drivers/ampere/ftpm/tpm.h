/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DRIVERS_AMPERE_FTPM_TPM_H
#define DRIVERS_AMPERE_FTPM_TPM_H

#include <security/tpm/tis.h>
#include <security/tpm/tss_errors.h>
#include <types.h>

/*
 * Driver for the Ampere firmware TPM (fTPM).
 *
 * The fTPM runs in the secure world (TF-A at EL3). The non-secure world
 * talks to it through a Command Response Buffer (CRB): the command is written
 * to the CRB command buffer, the start bit of the CRB control area is set and
 * then TF-A is invoked with an SMC call to run the TPM service handler. This
 * is start method 11 (CRB + ARM Secure Monitor) of the TPM2 ACPI table.
 *
 * TF-A sets up the control area (command/response buffer addresses and sizes)
 * and passes its address and the SMC function ID in the platform HOB, which
 * the SoC code provides through ampere_ftpm_get_params().
 */

struct ampere_ftpm_params {
	uintptr_t control_area;
	size_t control_area_size;
	uint32_t smc_function_id;	/* of the TPM service handler in TF-A */
};

/*
 * Implemented by the SoC: the parameters of the fTPM interface. Returns 0 if
 * TF-A provides a CRB interface, -1 if there is no fTPM. Called for every TPM
 * command, so it should be cheap.
 */
int ampere_ftpm_get_params(struct ampere_ftpm_params *params);

/*
 * CRB control area registers, offsets from the control area base (TPM 2.0
 * Mobile Command Response Buffer Interface, section 3).
 */
#define CRB_REG_CONTROL_REQUEST		0x00
#define CRB_REG_CONTROL_STATUS		0x04
#define CRB_REG_CONTROL_CANCEL		0x08
#define CRB_REG_CONTROL_START		0x0c
#define CRB_REG_INT_ENABLE		0x10
#define CRB_REG_INT_STATUS		0x14
#define CRB_REG_CMD_SIZE		0x18
#define CRB_REG_CMD_ADDR_LOW		0x1c
#define CRB_REG_CMD_ADDR_HIGH		0x20
#define CRB_REG_RESP_SIZE		0x24
#define CRB_REG_RESP_ADDR		0x28

/* Control Request register */
#define CRB_CONTROL_REQUEST_GO_IDLE	0x02

/* Control Status register */
#define CRB_CONTROL_STATUS_ERROR	0x01

/* Control Start register */
#define CRB_CONTROL_START_START		0x01

tpm_result_t crb_tpm_init(void);
size_t crb_tpm_process_command(const void *tpm2_command, size_t command_size,
			       void *tpm2_response, size_t max_response);
bool crb_tpm_is_active(void);

tis_sendrecv_fn crb_tis_probe(enum tpm_family *family);

uintptr_t crb_tpm_base_address(void);

#endif /* DRIVERS_AMPERE_FTPM_TPM_H */
