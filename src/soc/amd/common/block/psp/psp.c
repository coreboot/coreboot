/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootstate.h>
#include <console/console.h>
#include <amdblocks/psp.h>
#include "psp_def.h"

static const char *psp_status_nobase = "error: PSP_ADDR_MSR and PSP BAR3 not assigned";
static const char *psp_status_halted = "error: PSP in halted state";
static const char *psp_status_recovery = "error: PSP recovery required";
static const char *psp_status_errcmd = "error sending command";
static const char *psp_status_init_timeout = "error: PSP init timeout";
static const char *psp_status_cmd_timeout = "error: PSP command timeout";
static const char *psp_status_noerror = "";

static const char *status_to_string(int err)
{
	switch (err) {
	case -PSPSTS_NOBASE:
		return psp_status_nobase;
	case -PSPSTS_HALTED:
		return psp_status_halted;
	case -PSPSTS_RECOVERY:
		return psp_status_recovery;
	case -PSPSTS_SEND_ERROR:
		return psp_status_errcmd;
	case -PSPSTS_INIT_TIMEOUT:
		return psp_status_init_timeout;
	case -PSPSTS_CMD_TIMEOUT:
		return psp_status_cmd_timeout;
	default:
		return psp_status_noerror;
	}
}

static uint32_t rd_resp_sts(struct mbox_buffer_header *header)
{
	return read32(&header->status);
}

/*
 * Print meaningful status to the console.  Caller only passes a pointer to a
 * buffer header if it's expected to contain its own status.
 */
void psp_print_cmd_status(int cmd_status, struct mbox_buffer_header *header)
{
	if (header && rd_resp_sts(header))
		printk(BIOS_DEBUG, "buffer status=0x%x ", rd_resp_sts(header));

	if (cmd_status)
		printk(BIOS_WARNING, "%s\n", status_to_string(cmd_status));
	else
		printk(BIOS_DEBUG, "OK\n");
}

enum cb_err psp_get_ftpm_capabilities(uint32_t *capabilities)
{
	int cmd_status;
	struct mbox_cmd_capability_query_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
	};

	printk(BIOS_DEBUG, "PSP: Querying fTPM capabilities... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_PSP_FTPM_QUERY, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);

	if (cmd_status)
		return CB_ERR;

	*capabilities = read32(&buffer.capabilities);
	return CB_SUCCESS;
}

enum cb_err psp_get_hsti_state(uint32_t *state)
{
	int cmd_status;
	struct mbox_cmd_hsti_query_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
	};

	printk(BIOS_DEBUG, "PSP: Querying HSTI state... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_HSTI_QUERY, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);

	if (cmd_status)
		return CB_ERR;

	*state = read32(&buffer.state);
	return CB_SUCCESS;
}

/*
 * Returns true if ROM Armor is enforced, that is after PSP command
 * MBOX_BIOS_CMD_ARMOR_ENTER_SMM_MODE has been executed, false otherwise.
 *
 * When ROM Armor is enforced the result will be cached.
 */
#if ENV_RAMSTAGE || ENV_SMM
bool psp_get_hsti_state_rom_armor_enforced(void)
{
	uint32_t hsti_state;

	if (!CONFIG(SOC_AMD_COMMON_BLOCK_PSP_ROM_ARMOR3))
		return false;

	static bool enforced;
	if (enforced)
		return true; /* ROM Armor already enforced, no need to check again */

	if (psp_get_hsti_state(&hsti_state) != CB_SUCCESS) {
		printk(BIOS_EMERG, "PSP: Failed to get HSTI state\n");
		return false;
	}
	enforced = hsti_state & HSTI_STATE_ROM_ARMOR_ENFORCED;
	if (enforced)
		printk(BIOS_INFO, "PSP: ROM Armor enforced\n");

	return enforced;
}
#endif

static enum cb_err psp_notify_df_lock(void)
{
	struct mbox_default_buffer buffer = {
		.header.size = sizeof(buffer.header),
	};
	int cmd_status;

	printk(BIOS_DEBUG, "PSP: Notify Data Fabric lock before BOOT_DONE... ");
	cmd_status = send_psp_command(MBOX_BIOS_CMD_LOCK_DF_REG, &buffer);
	psp_print_cmd_status(cmd_status, &buffer.header);
	/* Keep the stack frame live while a timed-out PSP may still use it. */
	if (cmd_status == -PSPSTS_CMD_TIMEOUT)
		die("PSP: Data Fabric lock command timed out\n");
	if (cmd_status || rd_resp_sts(&buffer.header)) {
		printk(BIOS_WARNING, "PSP: Skipping BOOT_DONE after Data Fabric lock failure\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}

static enum cb_err psp_notify_smm_lock(void)
{
	struct mbox_default_buffer buffer = {
		.header.size = sizeof(buffer.header),
	};
	int cmd_status;

	printk(BIOS_DEBUG, "PSP: Notify SMM lock before BOOT_DONE... ");
	cmd_status = send_psp_command(MBOX_BIOS_CMD_SMM_LOCK, &buffer);
	psp_print_cmd_status(cmd_status, &buffer.header);
	/* Keep the stack frame live while a timed-out PSP may still use it. */
	if (cmd_status == -PSPSTS_CMD_TIMEOUT)
		die("PSP: SMM lock command timed out\n");
	if (cmd_status || rd_resp_sts(&buffer.header)) {
		printk(BIOS_WARNING, "PSP: Skipping BOOT_DONE after SMM lock failure\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}

/*
 * Notify the PSP that the system is completing the boot process.  Upon
 * receiving this command, the PSP will only honor commands where the buffer
 * is in SMM space.
 */
static void psp_notify_boot_done(void)
{
	int cmd_status;
	struct mbox_default_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		}
	};

	printk(BIOS_DEBUG, "PSP: Notify that POST is finishing... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_BOOT_DONE, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);
}

static void psp_notify_boot_complete(void *unused)
{
	/* Resource assignment and CPU SMM lockdown precede payload boot. */
	if (CONFIG(SOC_AMD_COMMON_BLOCK_PSP_DF_LOCK) &&
	    psp_notify_df_lock() != CB_SUCCESS)
		return;
	if (CONFIG(SOC_AMD_COMMON_BLOCK_PSP_SMM_LOCK) &&
	    psp_notify_smm_lock() != CB_SUCCESS)
		return;
	psp_notify_boot_done();
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, psp_notify_boot_complete, NULL);
