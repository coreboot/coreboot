/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * This is a driver for a CRB Interface.
 *
 * The general flow looks like this:
 *
 * TPM starts in IDLE Mode
 *
 *   IDLE  --> READY --> Command Reception
 *    ^	                        |
 *    |	                        v
      -- Cmd Complete <-- Command Execution
 */

#include <timer.h>
#include <console/console.h>
#include <device/mmio.h>
#include <string.h>
#include <soc/pci_devs.h>
#include <device/pci_ops.h>

#include "tpm.h"

static struct control_area {
	uint32_t request;
	uint32_t status;
	uint32_t cancel;
	uint32_t start;
	uint64_t interrupt_control;
	uint32_t command_size;
	void *command_bfr;
	uint32_t response_size;
	void *response_bfr;
} control_area;

static uint8_t cur_loc = 0;

/* Read Control Area Structure back  */
static void crb_readControlArea(void)
{
	control_area.request = read32(CRB_REG(cur_loc, CRB_REG_REQUEST));
	control_area.status = read32(CRB_REG(cur_loc, CRB_REG_STATUS));
	control_area.cancel = read32(CRB_REG(cur_loc, CRB_REG_CANCEL));
	control_area.interrupt_control = read64(CRB_REG(cur_loc, CRB_REG_INT_CTRL));
	control_area.command_size = read32(CRB_REG(cur_loc, CRB_REG_CMD_SIZE));
	control_area.command_bfr =
		(void *)(uintptr_t)read64(CRB_REG(cur_loc, CRB_REG_CMD_ADDR));
	control_area.response_size = read32(CRB_REG(cur_loc, CRB_REG_RESP_SIZE));
	control_area.response_bfr =
		(void *)(uintptr_t)read64(CRB_REG(cur_loc, CRB_REG_RESP_ADDR));

	/*
	 * Intel PTT has to write the command/response address and size
	 * register before each command submission otherwise the control area
	 * is all zeroed. This has been observed on Alder Lake S CPU and may be
	 * applicable to other new microarchitectures. Update the local control
	 * area data to make crb_tpm_process_command() not fail on buffer checks.
	 * PTT command/response buffer is fixed to be at offset 0x80 and spans
	 * up to the end of 4KB region for the current locality.
	 */
	if (CONFIG(HAVE_INTEL_PTT)) {
		control_area.command_size = 0x1000 - CRB_REG_DATA_BUFF;
		control_area.response_size = control_area.command_size;
		control_area.command_bfr = CRB_REG(cur_loc, CRB_REG_DATA_BUFF);
		control_area.response_bfr = CRB_REG(cur_loc, CRB_REG_DATA_BUFF);
	}
}

/* Wait for Reg to be expected Value  */
static tpm_result_t crb_wait_for_reg32(const void *addr, uint32_t timeoutMs, uint32_t mask,
			      uint32_t expectedValue)
{
	uint32_t regValue;
	struct stopwatch sw;

	// Set up a timer which breaks the loop after timeout
	stopwatch_init_msecs_expire(&sw, timeoutMs);

	while (1) {
		// Now check if the TPM is in IDLE mode
		regValue = read32(addr);

		if ((regValue & mask) == expectedValue)
			return TPM_SUCCESS;

		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR,
			       "CRB_WAIT: Error - Timed out with RegValue: %08x, Mask: %08x, Expected: %08x\n",
			       regValue, mask, expectedValue);
			return TPM_CB_TIMEOUT;
		}
	}
}

/* CRB PROBE
 *
 * Checks if the CRB Interface is ready
 */
static tpm_result_t crb_probe(void)
{
	uint64_t tpmStatus = read64(CRB_REG(cur_loc, CRB_REG_INTF_ID));
	printk(BIOS_SPEW, "Interface ID Reg. %llx\n", tpmStatus);

	if ((tpmStatus & CRB_INTF_REG_CAP_CRB) == 0) {
		printk(BIOS_DEBUG, "TPM: CRB Interface is not supported.\n");
		return TPM_CB_FAIL;
	}

	if ((tpmStatus & (0xf)) != 1) {
		printk(BIOS_DEBUG,
		       "TPM: CRB Interface is not active. System needs reboot in order to active TPM.\n");
		write32(CRB_REG(cur_loc, CRB_REG_INTF_ID), CRB_INTF_REG_INTF_SEL);
		return TPM_CB_FAIL;
	}

	write32(CRB_REG(cur_loc, CRB_REG_INTF_ID), CRB_INTF_REG_INTF_SEL);
	write32(CRB_REG(cur_loc, CRB_REG_INTF_ID), CRB_INTF_REG_INTF_LOCK);

	return TPM_SUCCESS;
}

/*
 * Get active Locality
 *
 * Get the active locality
 */
static uint8_t crb_activate_locality(void)
{
	uint8_t locality = (read8(CRB_REG(0, CRB_REG_LOC_STATE)) >> 2) & 0x07;
	printk(BIOS_SPEW, "Active locality: %i\n", locality);

	tpm_result_t rc = crb_wait_for_reg32(CRB_REG(locality, CRB_REG_LOC_STATE), 750,
				    LOC_STATE_LOC_ASSIGN, LOC_STATE_LOC_ASSIGN);

	if (!rc && (locality == 0))
		return locality;

	if (rc)
		write8(CRB_REG(locality, CRB_REG_LOC_CTRL), LOC_CTRL_REQ_ACCESS);

	rc = crb_wait_for_reg32(CRB_REG(locality, CRB_REG_LOC_STATE), 750, LOC_STATE_LOC_ASSIGN,
				LOC_STATE_LOC_ASSIGN);
	if (rc) {
		printk(BIOS_ERR, "TPM: Error (%#x) - No Locality has been assigned TPM-wise.\n", rc);
		return 0;
	}

	rc = crb_wait_for_reg32(CRB_REG(locality, CRB_REG_LOC_STATE), 1500,
				LOC_STATE_REG_VALID_STS, LOC_STATE_REG_VALID_STS);
	if (rc) {
		printk(BIOS_ERR, "TPM: Error (%#x) - LOC_STATE Register %u contains errors.\n",
		       rc, locality);
		return 0;
	}

	return locality;
}

/* Switch Device into a Ready State */
static tpm_result_t crb_switch_to_ready(void)
{
	/* Transition into ready state */
	write8(CRB_REG(cur_loc, CRB_REG_REQUEST), 0x1);
	tpm_result_t rc = crb_wait_for_reg32(CRB_REG(cur_loc, CRB_REG_REQUEST), 200,
					CRB_REG_REQUEST_CMD_RDY, 0x0);
	if (rc) {
		printk(BIOS_ERR,
		       "TPM Error (%#x): TPM did not transition into ready state in time.\n", rc);
		return rc;
	}

	/* Check TPM_CRB_CTRL_STS[0] to be "0" - no unrecoverable error */
	rc = crb_wait_for_reg32(CRB_REG(cur_loc, CRB_REG_STATUS), 500, CRB_REG_STATUS_ERROR,
				0x0);
	if (rc) {
		printk(BIOS_ERR, "TPM Error (%#x): Could not recover.\n", rc);
		return rc;
	}

	return TPM_SUCCESS;
}

/*
 * crb_tpm_init
 *
 * Even though the TPM does not need an initialization we check
 * if the TPM responds and is in IDLE mode, which should be the
 * normal bring up mode.
 *
 */
tpm_result_t crb_tpm_init(void)
{
	tpm_result_t rc = crb_probe();
	if (rc) {
		printk(BIOS_ERR, "TPM: Probe failed.\n");
		return rc;
	}

	/* Read back control area structure */
	crb_readControlArea();

	/*
	 * PTT may have no assigned locality before startup. Request locality here to save
	 * some precious milliseconds which would be wasted in crb_activate_locality polling
	 * for LOC_STATE_LOC_ASSIGN bit for the first time.
	 */
	if (CONFIG(HAVE_INTEL_PTT)) {
		uint8_t locality = (read8(CRB_REG(0, CRB_REG_LOC_STATE)) >> 2) & 0x07;
		write8(CRB_REG(locality, CRB_REG_LOC_CTRL), LOC_CTRL_REQ_ACCESS);
	}

	/* Good to go. */
	printk(BIOS_SPEW, "TPM: CRB TPM initialized successfully\n");

	return TPM_SUCCESS;
}

static void set_ptt_cmd_resp_buffers(void)
{
	write32(CRB_REG(cur_loc, CRB_REG_CMD_ADDR + 4), 0);
	write32(CRB_REG(cur_loc, CRB_REG_CMD_ADDR),
		(uintptr_t)CRB_REG(cur_loc, CRB_REG_DATA_BUFF));
	write32(CRB_REG(cur_loc, CRB_REG_CMD_SIZE), control_area.command_size);
	write64(CRB_REG(cur_loc, CRB_REG_RESP_ADDR),
		(uintptr_t)CRB_REG(cur_loc, CRB_REG_DATA_BUFF));
	write32(CRB_REG(cur_loc, CRB_REG_RESP_SIZE), control_area.response_size);
}

/*
 * crb_tpm_process_command
 */
size_t crb_tpm_process_command(const void *tpm2_command, size_t command_size,
			       void *tpm2_response, size_t max_response)
{
	tpm_result_t rc;

	if (command_size > control_area.command_size) {
		printk(BIOS_ERR, "TPM: Command size is too big.\n");
		return -1;
	}

	if (control_area.response_size < max_response) {
		printk(BIOS_ERR, "TPM: Response size could be too big.\n");
		return -1;
	}

	cur_loc = crb_activate_locality();

	// Check if CMD bit is cleared.
	rc = crb_wait_for_reg32(CRB_REG(0, CRB_REG_START), 250, CRB_REG_START_START, 0x0);
	if (rc) {
		printk(BIOS_ERR, "TPM Error (%#x): Cmd Bit not cleared.\n", rc);
		return -1;
	}

	rc = crb_switch_to_ready();
	if (rc) {
		printk(BIOS_DEBUG, "TPM Error (%#x): Can not transition into ready state.\n", rc);
		return -1;
	}

	// Write to Command Buffer
	memcpy(control_area.command_bfr, tpm2_command, command_size);

	/*
	 * Initialize CRB addresses and sizes for PTT. It seems to be possible
	 * only after CRB is switched to ready and before writing start bit.
	 * This is also what EDK2 TPM CRB drivers do.
	 */
	if (CONFIG(HAVE_INTEL_PTT))
		set_ptt_cmd_resp_buffers();

	// Write Start Bit
	write8(CRB_REG(cur_loc, CRB_REG_START), 0x1);

	// Poll for Response
	rc = crb_wait_for_reg32(CRB_REG(cur_loc, CRB_REG_START), 3500, CRB_REG_START_START, 0);
	if (rc) {
		printk(BIOS_DEBUG, "TPM Error (%#x): Command Timed out.\n", rc);
		return -1;
	}

	// Check for errors
	rc = crb_wait_for_reg32(CRB_REG(cur_loc, CRB_REG_STATUS), 200, CRB_REG_STATUS_ERROR, 0);
	if (rc) {
		printk(BIOS_DEBUG, "TPM Error (%#x): Command errored.\n", rc);
		return -1;
	}

	// Get Response Length
	uint32_t length = be32_to_cpu(read32(control_area.response_bfr + 2));

	/* Response has to have at least 6 bytes */
	if (length < 6)
		return 1;

	// Copy Response
	memcpy(tpm2_response, control_area.response_bfr, length);

	rc = crb_switch_to_ready();
	if (rc) {
		printk(BIOS_DEBUG, "TPM Error (%#x): Can not transition into ready state again.\n", rc);
		return -1;
	}

	return length;
}

/*
 * tp2_get_info
 *
 * Returns information about the TPM
 *
 */
void crb_tpm_get_info(struct crb_tpm_info *crb_tpm_info)
{
	uint64_t interfaceReg = read64(CRB_REG(cur_loc, CRB_REG_INTF_ID));

	crb_tpm_info->vendor_id = (interfaceReg >> 48) & 0xFFFF;
	crb_tpm_info->device_id = (interfaceReg >> 32) & 0xFFFF;
	crb_tpm_info->revision = (interfaceReg >> 24) & 0xFF;
}

/*
 * crb_tpm_is_active
 *
 * Checks that CRB interface is available and active.
 *
 * The body was derived from crb_probe() which unlike this function can also
 * write to registers.
 */
bool crb_tpm_is_active(void)
{
	uint64_t tpmStatus = read64(CRB_REG(0, CRB_REG_INTF_ID));
	printk(BIOS_SPEW, "Interface ID Reg. %llx\n", tpmStatus);

	if ((tpmStatus & CRB_INTF_REG_CAP_CRB) == 0) {
		printk(BIOS_DEBUG, "TPM: CRB Interface is not supported.\n");
		return false;
	}

	if ((tpmStatus & (0xf)) != 1) {
		printk(BIOS_DEBUG, "TPM: CRB Interface is not active.\n");
		return false;
	}

	return true;
}

/*
 * crb_tpm_base_address
 *
 * Returns the CRB TPM base address.
 */
uintptr_t crb_tpm_base_address(void)
{
	return CONFIG_CRB_TPM_BASE_ADDRESS;
}
