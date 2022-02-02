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
}

/* Wait for Reg to be expected Value  */
static int crb_wait_for_reg32(const void *addr, uint32_t timeoutMs, uint32_t mask,
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
			return 0;

		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR,
			       "CRB_WAIT: Error - Returning Zero with RegValue: %08x, Mask: %08x, Expected: %08x\n",
			       regValue, mask, expectedValue);
			return -1;
		}
	}
}

/* CRB PROBE
 *
 * Checks if the CRB Interface is ready
 */
static int crb_probe(void)
{
	uint64_t tpmStatus = read64(CRB_REG(cur_loc, CRB_REG_INTF_ID));
	printk(BIOS_SPEW, "Interface ID Reg. %llx\n", tpmStatus);

	if ((tpmStatus & CRB_INTF_REG_CAP_CRB) == 0) {
		printk(BIOS_DEBUG, "TPM: CRB Interface is not supported.\n");
		return -1;
	}

	if ((tpmStatus & (0xf)) != 1) {
		printk(BIOS_DEBUG,
		       "TPM: CRB Interface is not active. System needs reboot in order to active TPM.\n");
		write32(CRB_REG(cur_loc, CRB_REG_INTF_ID), CRB_INTF_REG_INTF_SEL);
		return -1;
	}

	write32(CRB_REG(cur_loc, CRB_REG_INTF_ID), CRB_INTF_REG_INTF_SEL);
	write32(CRB_REG(cur_loc, CRB_REG_INTF_ID), CRB_INTF_REG_INTF_LOCK);

	return 0;
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

	int rc = crb_wait_for_reg32(CRB_REG(locality, CRB_REG_LOC_STATE), 750,
				    LOC_STATE_LOC_ASSIGN, LOC_STATE_LOC_ASSIGN);
	if (!rc && (locality == 0))
		return locality;

	if (rc)
		write8(CRB_REG(locality, CRB_REG_LOC_CTRL), LOC_CTRL_REQ_ACCESS);

	rc = crb_wait_for_reg32(CRB_REG(locality, CRB_REG_LOC_STATE), 750, LOC_STATE_LOC_ASSIGN,
				LOC_STATE_LOC_ASSIGN);
	if (rc) {
		printk(BIOS_ERR, "TPM: Error - No Locality has been assigned TPM-wise.\n");
		return 0;
	}

	rc = crb_wait_for_reg32(CRB_REG(locality, CRB_REG_LOC_STATE), 1500,
				LOC_STATE_REG_VALID_STS, LOC_STATE_REG_VALID_STS);
	if (rc) {
		printk(BIOS_ERR, "TPM: Error - LOC_STATE Register %u contains errors.\n",
		       locality);
		return 0;
	}

	return locality;
}

/* Switch Device into a Ready State */
static int crb_switch_to_ready(void)
{
	/* Transition into ready state */
	write8(CRB_REG(cur_loc, CRB_REG_REQUEST), 0x1);
	int rc = crb_wait_for_reg32(CRB_REG(cur_loc, CRB_REG_REQUEST), 200,
					CRB_REG_REQUEST_CMD_RDY, 0x0);
	if (rc) {
		printk(BIOS_ERR,
		       "TPM: Error - TPM did not transition into ready state in time.\n");
		return -1;
	}

	/* Check TPM_CRB_CTRL_STS[0] to be "0" - no unrecoverable error */
	rc = crb_wait_for_reg32(CRB_REG(cur_loc, CRB_REG_STATUS), 500, CRB_REG_STATUS_ERROR,
				0x0);
	if (rc) {
		printk(BIOS_ERR, "TPM: Fatal Error - Could not recover.\n");
		return -1;
	}

	return 0;
}

/*
 * tpm2_init
 *
 * Even though the TPM does not need an initialization we check
 * if the TPM responds and is in IDLE mode, which should be the
 * normal bring up mode.
 *
 */
int tpm2_init(void)
{
	if (crb_probe()) {
		printk(BIOS_ERR, "TPM: Probe failed.\n");
		return -1;
	}

	/* Read back control area structure */
	crb_readControlArea();

	/* Good to go. */
	printk(BIOS_SPEW, "TPM: CRB TPM initialized successfully\n");

	return 0;
}

/*
 * tpm2_process_command
 */
size_t tpm2_process_command(const void *tpm2_command, size_t command_size, void *tpm2_response,
			    size_t max_response)
{
	int rc;

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
		printk(BIOS_ERR, "TPM: Error - Cmd Bit not cleared.\n");
		return -1;
	}

	if (crb_switch_to_ready())
		return -1;

	// Write to Command Buffer
	memcpy(control_area.command_bfr, tpm2_command, command_size);

	// Write Start Bit
	write8(CRB_REG(cur_loc, CRB_REG_START), 0x1);

	// Poll for Response
	rc = crb_wait_for_reg32(CRB_REG(cur_loc, CRB_REG_START), 3500, CRB_REG_START_START, 0);
	if (rc) {
		printk(BIOS_DEBUG, "TPM: Command Timed out.\n");
		return -1;
	}

	// Check for errors
	rc = crb_wait_for_reg32(CRB_REG(cur_loc, CRB_REG_STATUS), 200, CRB_REG_STATUS_ERROR, 0);
	if (rc) {
		printk(BIOS_DEBUG, "TPM: Command errored.\n");
		return -1;
	}

	// Get Response Length
	uint32_t length = be32_to_cpu(read32(control_area.response_bfr + 2));

	/* Response has to have at least 6 bytes */
	if (length < 6)
		return 1;

	// Copy Response
	memcpy(tpm2_response, control_area.response_bfr, length);

	if (crb_switch_to_ready()) {
		printk(BIOS_DEBUG, "TPM: Can not transition into ready state again.\n");
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
void tpm2_get_info(struct tpm2_info *tpm2_info)
{
	uint64_t interfaceReg = read64(CRB_REG(cur_loc, CRB_REG_INTF_ID));

	tpm2_info->vendor_id = (interfaceReg >> 48) & 0xFFFF;
	tpm2_info->device_id = (interfaceReg >> 32) & 0xFFFF;
	tpm2_info->revision = (interfaceReg >> 24) & 0xFF;
}
