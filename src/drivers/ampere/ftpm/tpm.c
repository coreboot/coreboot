/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Driver for the Ampere firmware TPM (fTPM), a CRB interface that is
 * submitted to TF-A (EL3) with an SMC call.
 *
 * The command flow is (see edk2-platforms AmpereAltraPkg
 * Library/Tpm2DeviceLibArmCrb):
 *
 *  1. Write the command to the CRB command buffer.
 *  2. Set the start bit of the control area.
 *  3. Make an SMC call to invoke the TPM service handler in TF-A.
 *  4. Wait for the start bit to clear.
 *  5. Read the response from the response buffer and go idle.
 *
 * Unlike a discrete CRB TPM, TF-A already populated the command and response
 * buffer addresses in the control area, so there is nothing to allocate here.
 * The address of the control area and the SMC function ID come from the SoC
 * (ampere_ftpm_get_params()), which takes them from the TF-A HOB like the
 * vendor firmware.
 */

#include <acpi/acpi.h>
#include <armv8/arch/smc.h>
#include <commonlib/endian.h>
#include <console/console.h>
#include <device/mmio.h>
#include <endian.h>
#include <string.h>
#include <timer.h>

#include "tpm.h"

/*
 * Timeout for the response. The fTPM can take long, e.g. for creating keys;
 * the vendor firmware waits up to 90 s (CRB_TIMEOUT_MAX_US).
 */
#define CRB_RESPONSE_TIMEOUT_MS		(90 * 1000)

struct control_area {
	uint32_t command_size;
	void *command_bfr;
	uint32_t response_size;
	void *response_bfr;
};

/*
 * Read the command and response buffer addresses and sizes TF-A set up in the
 * control area. Returns whether they are valid.
 */
static bool crb_read_control_area(uintptr_t control_area, struct control_area *ca)
{
	uint64_t addr;

	ca->command_size = read32p(control_area + CRB_REG_CMD_SIZE);
	addr = read32p(control_area + CRB_REG_CMD_ADDR_LOW);
	addr |= (uint64_t)read32p(control_area + CRB_REG_CMD_ADDR_HIGH) << 32;
	ca->command_bfr = (void *)(uintptr_t)addr;

	ca->response_size = read32p(control_area + CRB_REG_RESP_SIZE);
	ca->response_bfr = (void *)(uintptr_t)read64p(control_area + CRB_REG_RESP_ADDR);

	return ca->command_size && ca->response_size &&
	       ca->command_bfr && ca->response_bfr &&
	       ca->command_bfr != (void *)-1 && ca->response_bfr != (void *)-1;
}

/*
 * Whether the fTPM can be used, like Tpm2RequestUseTpm() of the vendor
 * firmware: TF-A reports a CRB interface, has set up the control area and
 * the fTPM doesn't report an error (e.g. because its NV storage is broken).
 */
static bool crb_usable(struct ampere_ftpm_params *params)
{
	struct control_area ca;

	if (ampere_ftpm_get_params(params))
		return false;

	if (!crb_read_control_area(params->control_area, &ca))
		return false;

	return !(read32p(params->control_area + CRB_REG_CONTROL_STATUS) &
		 CRB_CONTROL_STATUS_ERROR);
}

/* Wait until the masked bits of a register reach the expected value. */
static tpm_result_t crb_wait_for_reg32(uintptr_t addr, uint32_t timeout_ms,
				       uint32_t mask, uint32_t expected)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, timeout_ms);

	while (!stopwatch_expired(&sw)) {
		uint32_t value = read32p(addr);

		if ((value & mask) == expected)
			return TPM_SUCCESS;
	}

	printk(BIOS_ERR, "fTPM: timeout on register %#lx (mask %#x, expected %#x)\n",
	       addr, mask, expected);
	return TPM_CB_TIMEOUT;
}

/* Invoke the TPM service handler in TF-A. */
static tpm_result_t crb_invoke_tpm_service(uint32_t smc_function_id)
{
	uint64_t ret = smc_call0(smc_function_id);

	if (ret != SMC_SUCCESS) {
		printk(BIOS_ERR, "fTPM: TPM service SMC call failed: %llu\n", ret);
		return TPM_CB_FAIL;
	}

	return TPM_SUCCESS;
}

tpm_result_t crb_tpm_init(void)
{
	struct ampere_ftpm_params params;

	if (!crb_usable(&params)) {
		printk(BIOS_DEBUG, "fTPM: no usable CRB interface from TF-A\n");
		return TPM_CB_FAIL;
	}

	printk(BIOS_SPEW, "fTPM: CRB at 0x%lx, SMC function ID 0x%x\n",
	       params.control_area, params.smc_function_id);
	return TPM_SUCCESS;
}

size_t crb_tpm_process_command(const void *tpm2_command, size_t command_size,
			       void *tpm2_response, size_t max_response)
{
	struct ampere_ftpm_params params;
	struct control_area ca;
	uint32_t length;
	tpm_result_t rc;

	if (ampere_ftpm_get_params(&params) ||
	    !crb_read_control_area(params.control_area, &ca)) {
		printk(BIOS_ERR, "fTPM: control area not set up\n");
		return -1;
	}

	const uintptr_t base = params.control_area;

	if (command_size > ca.command_size) {
		printk(BIOS_ERR, "fTPM: command size too big: %zu > %u\n",
		       command_size, ca.command_size);
		return -1;
	}

	/* Wait for a previous command to complete. */
	rc = crb_wait_for_reg32(base + CRB_REG_CONTROL_START, 250,
				CRB_CONTROL_START_START, 0);
	if (rc)
		return -1;

	/* Write the command and ring the doorbell. */
	memcpy(ca.command_bfr, tpm2_command, command_size);
	write32p(base + CRB_REG_CONTROL_START, CRB_CONTROL_START_START);

	rc = crb_invoke_tpm_service(params.smc_function_id);
	if (rc)
		return -1;

	/* Wait for the response. */
	rc = crb_wait_for_reg32(base + CRB_REG_CONTROL_START, CRB_RESPONSE_TIMEOUT_MS,
				CRB_CONTROL_START_START, 0);
	if (rc)
		return -1;

	if (read32p(base + CRB_REG_CONTROL_STATUS) & CRB_CONTROL_STATUS_ERROR) {
		printk(BIOS_DEBUG, "fTPM: command returned an error\n");
		return -1;
	}

	/* The response length is the big endian field at offset 2. */
	length = be32_to_cpu(read32((uint8_t *)ca.response_bfr + 2));
	if (length < 6 || length > ca.response_size || length > max_response) {
		printk(BIOS_DEBUG, "fTPM: invalid response length %u\n", length);
		return -1;
	}

	memcpy(tpm2_response, ca.response_bfr, length);

	/* Transition back to idle. */
	write32p(base + CRB_REG_CONTROL_REQUEST, CRB_CONTROL_REQUEST_GO_IDLE);

	return length;
}

bool crb_tpm_is_active(void)
{
	struct ampere_ftpm_params params;

	return crb_usable(&params);
}

uintptr_t crb_tpm_base_address(void)
{
	struct ampere_ftpm_params params;

	if (ampere_ftpm_get_params(&params))
		return 0;

	return params.control_area;
}

/*
 * Start method specific parameters of start method 11 in the TPM2 ACPI table:
 * interrupt 0, flags 0 and the SMC function ID of the TPM service handler.
 */
void acpi_tpm2_fill_start_method_params(uint8_t *msp, size_t size)
{
	struct ampere_ftpm_params params;

	if (size < 12 || ampere_ftpm_get_params(&params))
		return;

	write_le32(&msp[0], 0);
	write_le32(&msp[4], 0);
	write_le32(&msp[8], params.smc_function_id);
}
