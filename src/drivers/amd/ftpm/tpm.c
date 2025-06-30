/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * This is a driver for the AMD fTPM CRB Interface.
 *
 * The AMD fTPM uses the CRB interface, but doesn't implement all registers
 * defined in the TCG specification.
 *
 * The reduced CRB MMIO register space has:
 * - A START register to ring the doorbell
 * - An error STATUS register with only one bit
 * - DMA address and size register for the CRB
 * - No other status or control registers
 * - No way to read current locality (assumption is locality 0)
 * - No interface ID register
 * - No read only registers
 *
 * The TPM interface also assumes that the DRTM is always using locality 0.
 *
 * The fTPM needs to access the SPI flash and this is currently done using
 * the PSP SMI handler. Thus the fTPM will only operate after SMM has been
 * set up.
 *
 * The fTPM needs the PSP directory files type 0x04 and type 0x54. When
 * the regions are missing or corrupted the fTPM won't be operational.
 */

#include <timer.h>
#include <amdblocks/psp.h>
#include <amdblocks/reset.h>
#include <cbmem.h>
#include <console/console.h>
#include <commonlib/region.h>
#include <device/mmio.h>
#include <fmap.h>
#include <string.h>

#include "tpm.h"

/* FMAP regions used */
#define FMAP_NAME_PSP_NVRAM                 "PSP_NVRAM"
#define FMAP_NAME_PSP_RPMC_NVRAM            "PSP_RPMC_NVRAM"

/* Size of DMA buffer in DRAM */
#define FTPM_CRB_SIZE                  (4 * 4096)

/* Index of Count field in TPM response buffer */
#define TPM_RSP_SIZE_BYTE 2

static struct control_area {
	uint32_t start;
	uint32_t command_size;
	void *command_bfr;
	uint32_t response_size;
	void *response_bfr;
} control_area;

/* Read Control Area Structure back  */
static void crb_read_control_area(void)
{
	control_area.command_size = read32(CRB_REG(CRB_REG_CMD_SIZE));
	control_area.command_bfr =
		(void *)(uintptr_t)read64(CRB_REG(CRB_REG_CMD_ADDR));
	control_area.response_size = read32(CRB_REG(CRB_REG_RESP_SIZE));
	control_area.response_bfr =
		(void *)(uintptr_t)read64(CRB_REG(CRB_REG_RESP_ADDR));
}

/* Wait for reg to be expected value */
static tpm_result_t crb_wait_for_reg32(const void *addr,
				       uint32_t timeout_ms,
				       uint32_t mask,
				       uint32_t expected_value)
{
	uint32_t reg_value;
	struct stopwatch sw;

	// Set up a timer which breaks the loop after timeout
	stopwatch_init_msecs_expire(&sw, timeout_ms);

	while (1) {
		// Now check if the TPM is in IDLE mode
		reg_value = read32(addr);

		if ((reg_value & mask) == expected_value)
			return TPM_SUCCESS;

		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR,
			       "fTPM: Timed out with reg_value: %08x, mask: %08x, expected: %08x\n",
			       reg_value, mask, expected_value);
			return TPM_CB_TIMEOUT;
		}
	}
}

static int erase_region(const char *name)
{
	struct region_device store;

	if (fmap_locate_area_as_rdev_rw(name, &store)) {
		printk(BIOS_ERR, "fTPM: Unable to find FMAP region %s\n", name);
		return -1;
	}

	if (rdev_eraseat(&store, 0, region_device_sz(&store))
	    != region_device_sz(&store))
		return -1;

	printk(BIOS_NOTICE, "fTPM: Erased FMAP region %s\n", name);

	return 0;
}

/*
 * crb_tpm_init
 *
 * Even though the TPM does not need an initialization we check
 * if the TPM responds and is in IDLE mode, which should be the
 * normal bring up mode.
 */
tpm_result_t crb_tpm_init(void)
{
	bool psp_rpmc_nvram, psp_nvram, psp_dir;

	/*
	 * When recovery is required psp_ftpm_is_active() always returns false.
	 * Thus handle recovery before checking if fTPM is usable.
	 */
	psp_ftpm_needs_recovery(&psp_rpmc_nvram, &psp_nvram, &psp_dir);

	if (psp_rpmc_nvram) {
		if (erase_region(FMAP_NAME_PSP_RPMC_NVRAM))
			psp_rpmc_nvram = false;	/* Skip reset if erase failed */
	}

	if (psp_nvram) {
		if (erase_region(FMAP_NAME_PSP_NVRAM))
			psp_nvram = false;	/* Skip reset if erase failed */
	}

	if (psp_rpmc_nvram || psp_nvram) {
		printk(BIOS_DEBUG, "fTPM: Reset to recover fTPM...\n");
		cold_reset();
	}

	if (psp_dir) {
		printk(BIOS_ERR, "fTPM: fTPM driver corrupted. Need FW update.\n");
		return CB_ERR;
	}

	if (!psp_ftpm_is_active())
		return CB_ERR_NOT_IMPLEMENTED;

	if (ENV_HAS_CBMEM) {
		/* setting up fTPM communication buffers in cbmem */
		const struct cbmem_entry *entry;
		void *buf;
		uint32_t length = 2 * FTPM_CRB_SIZE;

		/* Only allocate once */
		entry = cbmem_entry_find(CBMEM_ID_TPM2_CRB);
		if (entry) {
			buf = cbmem_entry_start(entry);
			length = cbmem_entry_size(entry);
		} else {
			buf = cbmem_add(CBMEM_ID_TPM2_CRB, length);
			if (!buf) {
				printk(BIOS_ERR, "fTPM: CBMEM CRB buffer allocation failed\n");
				return CB_ERR;
			}
			printk(BIOS_DEBUG, "fTPM: CRB buffer created at %p\n", buf);
			memset(buf, 0, length);
		}

		/* Set command/response buffers in control area */
		write32(CRB_REG(CRB_REG_CMD_SIZE), FTPM_CRB_SIZE);
		write64(CRB_REG(CRB_REG_CMD_ADDR), (uintptr_t)buf);
		write32(CRB_REG(CRB_REG_RESP_SIZE), FTPM_CRB_SIZE);
		write64(CRB_REG(CRB_REG_RESP_ADDR), (uintptr_t)buf + FTPM_CRB_SIZE);

		/*
		 * The OS will read the CRB registers to find the communication
		 * buffers. No need to advertise it in ACPI.
		 */
	}

	/* Read back control area structure */
	crb_read_control_area();

	/* Opt out when CRB hasn't been set up yet */
	if (!control_area.command_size || !control_area.response_size)
		return CB_ERR_NOT_IMPLEMENTED;

	/* Good to go. */
	printk(BIOS_SPEW, "fTPM: CRB TPM initialized successfully\n");

	return TPM_SUCCESS;
}

/*
 * crb_tpm_process_command
 *
 * Transfers data to and from the fTPM.
 *
 */
size_t crb_tpm_process_command(const void *tpm2_command, size_t command_size,
			       void *tpm2_response, size_t max_response)
{
	uint32_t rsp_size;
	tpm_result_t rc;

	if (!control_area.response_size || !control_area.command_size) {
		printk(BIOS_ERR, "%s: Control area wasn't set up yet\n", __func__);
		return -1;
	}
	if (command_size > control_area.command_size) {
		printk(BIOS_ERR, "%s: Command size is too big: %zu > %u.\n",
		       __func__, command_size, control_area.command_size);
		return -1;
	}

	/* Check if CMD bit is cleared. */
	rc = crb_wait_for_reg32(CRB_REG(CRB_REG_START), 250, CRB_REG_START_START, 0x0);
	if (rc) {
		printk(BIOS_ERR, "%s: Cmd Bit didn't clear\n", __func__);
		return -1;
	}

	/* Write to Command Buffer */
	memcpy(control_area.command_bfr, tpm2_command, command_size);

	/* Clear Response buffer */
	memset(control_area.response_bfr, 0, control_area.response_size);

	/* Set Response size */
	write32(CRB_REG(CRB_REG_RESP_SIZE), control_area.response_size);

	/* Write Start Bit */
	write8(CRB_REG(CRB_REG_START), CRB_REG_START_START);

	/* Poll for Response */
	rc = crb_wait_for_reg32(CRB_REG(CRB_REG_START), 3500, CRB_REG_START_START, 0);
	if (rc) {
		printk(BIOS_DEBUG, "%s: Timeout waiting for CMD processing\n", __func__);
		return -1;
	}

	/* Check for errors */
	rc = read32(CRB_REG(CRB_REG_STATUS));
	if (rc & CRB_REG_STATUS_ERROR) {
		printk(BIOS_DEBUG, "fTPM Error (%#x): Command errored.\n", rc);
		return -1;
	}

	/* Get Response Length. Seems to be static on some platforms. */
	uint32_t length = read32(CRB_REG(CRB_REG_RESP_SIZE));
	assert(length <= control_area.response_size);

	/* Response has to have at least 6 bytes */
	if (length < 6) {
		printk(BIOS_DEBUG, "fTPM Error: Invalid response length: %u\n", length);
		return -1;
	}

	/* Get actual size from TPM packet */
	memcpy(&rsp_size, control_area.response_bfr + TPM_RSP_SIZE_BYTE, sizeof(rsp_size));
	rsp_size = be32_to_cpu(rsp_size);
	if ((size_t)rsp_size > max_response) {
		printk(BIOS_DEBUG, "fTPM Error: Response wouldn't fit into target buffer: %u > %zu\n",
		      rsp_size, max_response);
		return -1;
	}

	/* Copy Response */
	length = MIN(length, MIN(max_response, rsp_size));
	memcpy(tpm2_response, control_area.response_bfr, length);

	return length;
}

/*
 * crb_tpm_is_active
 *
 * Checks that CRB interface is available and active.
 */
bool crb_tpm_is_active(void)
{
	if (!psp_ftpm_is_active())
		return false;

	/* Read back control area structure */
	crb_read_control_area();

	/* Validate fields */
	if (!control_area.command_size || !control_area.response_size ||
	    !control_area.command_bfr || !control_area.response_bfr ||
	    (control_area.command_bfr == (void *)-1) ||
	    (control_area.response_bfr == (void *)-1))
		return false;

	return true;
}

uintptr_t crb_tpm_base_address(void)
{
	return psp_ftpm_base_address();
}
