/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <pc80/tpm.h>
#include <reset.h>
#include "chromeos.h"

//#define EXTRA_LOGGING
#define UBOOT_DOES_TPM_STARTUP

#define TPM_LARGE_ENOUGH_COMMAND_SIZE 256	/* saves space in the firmware */

#define TPM_SUCCESS               ((u32)0x00000000)

#define TPM_E_IOERROR             ((u32)0x0000001f)
#define TPM_E_COMMUNICATION_ERROR ((u32)0x00005004)
#define TPM_E_NON_FATAL           ((u32)0x00000800)
#define TPM_E_INVALID_POSTINIT    ((u32)0x00000026)

#define TPM_E_NEEDS_SELFTEST     ((u32)(TPM_E_NON_FATAL + 1))
#define TPM_E_DOING_SELFTEST     ((u32)(TPM_E_NON_FATAL + 2))

#if CONFIG_NO_TPM_RESUME
static void init_vboot(int bootmode)
{
}
#else
static const struct {
	u8 buffer[12];
} tpm_resume_cmd = {
	{ 0x0, 0xc1, 0x0, 0x0, 0x0, 0xc, 0x0, 0x0, 0x0, 0x99, 0x0, 0x2 }
};

static const struct {
	u8 buffer[12];
} tpm_startup_cmd = {
	{0x0, 0xc1, 0x0, 0x0, 0x0, 0xc, 0x0, 0x0, 0x0, 0x99, 0x0, 0x1 }
};

static const struct {
	u8 buffer[10];
} tpm_continueselftest_cmd = {
	{ 0x0, 0xc1, 0x0, 0x0, 0x0, 0xa, 0x0, 0x0, 0x0, 0x53 }
};

static inline void FromTpmUint32(const u8 * buffer, u32 * x)
{
	*x = ((buffer[0] << 24) |
	      (buffer[1] << 16) | (buffer[2] << 8) | buffer[3]);
}

static inline int TpmCommandSize(const u8 * buffer)
{
	u32 size;
	FromTpmUint32(buffer + sizeof(u16), &size);
	return (int)size;
}

/* Gets the code field of a TPM command. */
static inline int TpmCommandCode(const u8 * buffer)
{
	u32 code;
	FromTpmUint32(buffer + sizeof(u16) + sizeof(u32), &code);
	return code;
}

/* Gets the return code field of a TPM result. */
static inline int TpmReturnCode(const u8 * buffer)
{
	return TpmCommandCode(buffer);
}

/* Like TlclSendReceive below, but do not retry if NEEDS_SELFTEST or
 * DOING_SELFTEST errors are returned.
 */
static u32 TlclSendReceiveNoRetry(const u8 * request,
				  u8 * response, int max_length)
{
	size_t response_length = max_length;
	u32 result;

#ifdef EXTRA_LOGGING
	printk(BIOS_DEBUG, "TPM: command: %x%x %x%x%x%x %x%x%x%x\n",
	       request[0], request[1],
	       request[2], request[3], request[4], request[5],
	       request[6], request[7], request[8], request[9]);
#endif

	result = TPM_SUCCESS;
	if (tis_sendrecv
	    (request, TpmCommandSize(request), response, &response_length))
		result = TPM_E_IOERROR;

	if (0 != result) {
		/* Communication with TPM failed, so response is garbage */
		printk(BIOS_DEBUG,
		       "TPM: command 0x%x send/receive failed: 0x%x\n",
		       TpmCommandCode(request), result);
		return TPM_E_COMMUNICATION_ERROR;
	}
	/* Otherwise, use the result code from the response */
	result = TpmReturnCode(response);

/* TODO: add paranoia about returned response_length vs. max_length
 * (and possibly expected length from the response header).  See
 * crosbug.com/17017 */

#ifdef EXTRA_LOGGING
	printk(BIOS_DEBUG, "TPM: response: %x%x %x%x%x%x %x%x%x%x\n",
	       response[0], response[1],
	       response[2], response[3], response[4], response[5],
	       response[6], response[7], response[8], response[9]);
#endif

	printk(BIOS_DEBUG, "TPM: command 0x%x returned 0x%x\n",
	       TpmCommandCode(request), result);

	return result;
}

static inline u32 TlclContinueSelfTest(void)
{
	u8 response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
	printk(BIOS_DEBUG, "TPM: Continue self test\n");
	/* Call the No Retry version of SendReceive to avoid recursion. */
	return TlclSendReceiveNoRetry(tpm_continueselftest_cmd.buffer,
				      response, sizeof(response));
}

/* Sends a TPM command and gets a response.  Returns 0 if success or the TPM
 * error code if error. In the firmware, waits for the self test to complete
 * if needed. In the host, reports the first error without retries. */
static u32 TlclSendReceive(const u8 * request, u8 * response, int max_length)
{
	u32 result = TlclSendReceiveNoRetry(request, response, max_length);
	/* When compiling for the firmware, hide command failures due to the self
	 * test not having run or completed. */
	/* If the command fails because the self test has not completed, try it
	 * again after attempting to ensure that the self test has completed. */
	if (result == TPM_E_NEEDS_SELFTEST || result == TPM_E_DOING_SELFTEST) {
		result = TlclContinueSelfTest();
		if (result != TPM_SUCCESS) {
			return result;
		}
#if defined(TPM_BLOCKING_CONTINUESELFTEST) || defined(VB_RECOVERY_MODE)
		/* Retry only once */
		result = TlclSendReceiveNoRetry(request, response, max_length);
#else
		/* This needs serious testing.  The TPM specification says:
		 * "iii. The caller MUST wait for the actions of
		 * TPM_ContinueSelfTest to complete before reissuing the
		 * command C1."  But, if ContinueSelfTest is non-blocking, how
		 * do we know that the actions have completed other than trying
		 * again? */
		do {
			result =
			    TlclSendReceiveNoRetry(request, response,
						   max_length);
		} while (result == TPM_E_DOING_SELFTEST);
#endif
	}

	return result;
}

static void init_vboot(int bootmode)
{
	u32 result;
	u8 response[TPM_LARGE_ENOUGH_COMMAND_SIZE];

#ifdef UBOOT_DOES_TPM_STARTUP
	/* Doing TPM startup when we're not coming in on the S3 resume path
	 * saves us roughly 20ms in boot time only. This does not seem to
	 * be worth an API change to vboot_reference-firmware right now, so
	 * let's keep the code around, but just bail out early:
	 */
	if (bootmode != 2)
		return;
#endif

	printk(BIOS_DEBUG, "Verified boot TPM initialization.\n");

	printk(BIOS_SPEW, "TPM: Init\n");
	if (tis_init())
		return;

	printk(BIOS_SPEW, "TPM: Open\n");
	if (tis_open())
		return;


	if (bootmode == 2) {
		/* S3 Resume */
		printk(BIOS_SPEW, "TPM: Resume\n");
		result = TlclSendReceive(tpm_resume_cmd.buffer,
					response, sizeof(response));
		if (result == TPM_E_INVALID_POSTINIT) {
			/* We're on a platform where the TPM maintains power
			 * in S3, so it's already initialized.
			 */
			printk(BIOS_DEBUG, "TPM: Already initialized.\n");
			return;
		}
	} else {
		printk(BIOS_SPEW, "TPM: Startup\n");
		result = TlclSendReceive(tpm_startup_cmd.buffer,
					response, sizeof(response));
	}

	if (result == TPM_SUCCESS) {
		printk(BIOS_SPEW, "TPM: OK.\n");
		return;
	}

#if !MOCK_TPM
	printk(BIOS_ERR, "TPM: Error code 0x%x. Hard reset!\n", result);
	hard_reset();
#endif
}
#endif

void init_chromeos(int bootmode)
{
	init_vboot(bootmode);
}
