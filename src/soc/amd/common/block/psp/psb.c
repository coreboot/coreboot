/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
#include <amdblocks/smn.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/mmio.h>
#include <types.h>

#include "psp_def.h"

#define PSB_STATUS_OFFSET	0x10994

#define FUSE_PLATFORM_SECURE_BOOT_EN	BIT(24)

#define PSB_TEST_STATUS_MASK		0xff
#define PSB_FUSING_READY_MASK		BIT(8)

/* PSB Test Status and Error Codes (doc#56654) */
#define PSB_TEST_STATUS_PASS			0x00
#define PSB_TEST_STATUS_FUSE_READ_ERR		0x3e
#define PSB_TEST_STATUS_BIOS_KEY_BAD_USAGE	0x81
#define PSB_TEST_STATUS_BIOS_RTM_SIG_NOENT	0x82
#define PSB_TEST_STATUS_BIOS_RTM_COPY_ERR	0x83
#define PSB_TEST_STATUS_BIOS_RTM_BAD_SIG	0x84
#define PSB_TEST_STATUS_BIOS_KEY_BAD_SIG	0x85
#define PSB_TEST_STATUS_PLATFORM_BAD_ID		0x86
#define PSB_TEST_STATUS_BIOS_COPY_BIT_UNSET	0x87
#define PSB_TEST_STATUS_BIOS_CA_BAD_SIG		0x8a
#define PSB_TEST_STATUS_BIOS_CA_BAD_USAGE	0x8b
#define PSB_TEST_STATUS_BIOS_KEY_BAD_REVISION	0x8c

#define FUSE_STATUS_SUCCESS	0x00
#define FUSE_STATUS_NOT_ALLOWED	0x09
#define FUSE_STATUS_FUSING_ERR	0x0a
#define FUSE_STATUS_BOOT_DONE	0x0b

static const char *psb_test_status_to_string(u32 status)
{
	switch (status) {
	case PSB_TEST_STATUS_PASS:
		return "Psb Test Status PASS";
	case PSB_TEST_STATUS_FUSE_READ_ERR:
		return "Error reading fuse info";
	case PSB_TEST_STATUS_BIOS_KEY_BAD_USAGE:
		return "OEM BIOS signing key usage flag violation";
	case PSB_TEST_STATUS_BIOS_RTM_SIG_NOENT:
		return "BIOS RTM signature entry not found";
	case PSB_TEST_STATUS_BIOS_RTM_COPY_ERR:
		return "BIOS copy to DRAM failed";
	case PSB_TEST_STATUS_BIOS_RTM_BAD_SIG:
		return "BIOS RTM signature verification failed";
	case PSB_TEST_STATUS_BIOS_KEY_BAD_SIG:
		return "OEM BIOS signing key failed signature verification";
	case PSB_TEST_STATUS_PLATFORM_BAD_ID:
		return "Platform vendor id and/or model id binding violation";
	case PSB_TEST_STATUS_BIOS_COPY_BIT_UNSET:
		return "BIOS copy bit unset for reset image";
	case PSB_TEST_STATUS_BIOS_CA_BAD_SIG:
		return "OEM BIOS signing CA key failed signature verification";
	case PSB_TEST_STATUS_BIOS_CA_BAD_USAGE:
		return "OEM BIOS signing CA key usage flag violation";
	case PSB_TEST_STATUS_BIOS_KEY_BAD_REVISION:
		return "OEM BIOS signing key revision violation";
	default:
		return "Unknown failure";
	}
}

static const char *fuse_status_to_string(u32 status)
{
	switch (status) {
	case FUSE_STATUS_SUCCESS:
		return "PSB Fusing completed successfully";
	case FUSE_STATUS_NOT_ALLOWED:
		return "Fusing not allowed or already done";
	case FUSE_STATUS_FUSING_ERR:
		return "Fuse programming failed";
	case FUSE_STATUS_BOOT_DONE:
		return "Issued after BOOT DONE";
	default:
		return "Unknown failure";
	}
}

static uint32_t get_psb_status(void)
{
	return smn_read32(SMN_PSP_PUBLIC_BASE + PSB_STATUS_OFFSET);
}

/*
 * Request Platform Secure Boot enablement via the PSP if it is not already
 * enabled. Upon receiving this command, the PSP will program all PSB fuses
 * so long as the BIOS signing key token is valid.
 */
static enum cb_err psb_enable(void)
{
	u32 status;
	struct mbox_default_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		}
	};

	status = get_psb_status();
	printk(BIOS_INFO, "PSB: Status = %x\n", status);

	if (status & FUSE_PLATFORM_SECURE_BOOT_EN) {
		printk(BIOS_DEBUG, "PSB: Already enabled\n");
		return CB_SUCCESS;
	}

	status = soc_read_c2p38();
	printk(BIOS_INFO, "PSB: HSTI = %x\n", status);

	const u32 psb_test_status = status & PSB_TEST_STATUS_MASK;

	if (psb_test_status != PSB_TEST_STATUS_PASS) {
		printk(BIOS_ERR, "PSB: %s\n", psb_test_status_to_string(psb_test_status));
		return CB_ERR;
	}

	if (!(status & PSB_FUSING_READY_MASK)) {
		printk(BIOS_ERR, "PSB: Fusing not allowed\n");
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "PSB: Enable... ");

	const int cmd_status = send_psp_command(MBOX_BIOS_CMD_PSB_AUTO_FUSING, &buffer);

	psp_print_cmd_status(cmd_status, &buffer.header);

	if (cmd_status) {
		printk(BIOS_ERR, "PSB: Fusing request failed: %d\n", cmd_status);
		return CB_ERR;
	}

	const u32 fuse_status = read32(&buffer.header.status);
	if (fuse_status != FUSE_STATUS_SUCCESS) {
		printk(BIOS_ERR, "PSB: %s\n", fuse_status_to_string(fuse_status));
		return CB_ERR;
	}

	printk(BIOS_NOTICE, "PSB: Rebooting\n");
	cold_reset();
}

static void enable_secure_boot(void *unused)
{
	/*
	 * Enable secure boot before loading payload. Psb fusing is done late in
	 * boot process to avoid any fuse access conflicts with other components
	 * which happens during boot process.
	 */
	if (psb_enable() == CB_ERR)
		printk(BIOS_NOTICE, "Enabling PSB failed.\n");
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, enable_secure_boot, NULL);
