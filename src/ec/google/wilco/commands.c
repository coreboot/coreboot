/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <ec/acpi/ec.h>
#include <stdint.h>
#include <string.h>

#include "ec.h"
#include "commands.h"

int wilco_ec_get_info(enum get_ec_info_cmd type, char *info)
{
	struct ec_response_get_ec_info rsp;

	if (!info)
		return -1;
	if (wilco_ec_sendrecv(KB_EC_INFO, type, &rsp, sizeof(rsp)) < 0)
		return -1;

	/* Copy returned string */
	strncpy(info, rsp.data, sizeof(rsp.data));
	return 0;
}

void wilco_ec_print_all_info(void)
{
	char info[EC_INFO_MAX_SIZE];

	if (!wilco_ec_get_info(GET_EC_LABEL, info))
		printk(BIOS_INFO, "EC Label      : %s\n", info);

	if (!wilco_ec_get_info(GET_EC_SVN_REV, info))
		printk(BIOS_INFO, "EC Revision   : %s\n", info);

	if (!wilco_ec_get_info(GET_EC_MODEL_NO, info))
		printk(BIOS_INFO, "EC Model Num  : %s\n", info);

	if (!wilco_ec_get_info(GET_EC_BUILD_DATE, info))
		printk(BIOS_INFO, "EC Build Date : %s\n", info);
}

static int wilco_ec_get_power_smi(struct ec_pm_event_state *pm)
{
	struct ec_response_power_smi {
		uint8_t pm_event_1;
		uint8_t pm_state_1;
		uint8_t hotkey;
		uint8_t pm_state_2;
		uint8_t pm_state_3;
		uint8_t pm_state_4;
		uint8_t pm_state_5;
		uint8_t pm_event_2;
		uint8_t pm_state_6;
	} __packed rsp;

	if (!pm)
		return -1;
	if (wilco_ec_sendrecv_noargs(KB_POWER_SMI, &rsp, sizeof(rsp)) < 0)
		return -1;

	pm->event[0] = rsp.pm_event_1;
	pm->event[1] = rsp.pm_event_2;
	pm->state[0] = rsp.pm_state_1;
	pm->state[1] = rsp.pm_state_2;
	pm->state[2] = rsp.pm_state_3;
	pm->state[3] = rsp.pm_state_4;
	pm->state[4] = rsp.pm_state_5;
	pm->state[5] = rsp.pm_state_6;
	pm->hotkey = rsp.hotkey;

	return 0;
}

static int wilco_ec_get_power_status(struct ec_pm_event_state *pm)
{
	struct ec_response_power_status {
		uint8_t pm_state_1;
		uint8_t pm_state_2;
		uint8_t pm_state_3;
		uint8_t pm_state_4;
		uint8_t pm_state_5;
		uint8_t ac_type_lsb;
		uint8_t pm_state_6;
		uint8_t pm_event_2;
		uint8_t ac_type_msb;
	} __packed rsp;

	if (!pm)
		return -1;
	if (wilco_ec_sendrecv_noargs(KB_POWER_STATUS, &rsp, sizeof(rsp)) < 0)
		return -1;

	pm->hotkey = 0;
	pm->event[0] = 0;
	pm->event[1] = rsp.pm_event_2;
	pm->state[0] = rsp.pm_state_1;
	pm->state[1] = rsp.pm_state_2;
	pm->state[2] = rsp.pm_state_3;
	pm->state[3] = rsp.pm_state_4;
	pm->state[4] = rsp.pm_state_5;
	pm->state[5] = rsp.pm_state_6;
	pm->ac_type = rsp.ac_type_msb << 8 | rsp.ac_type_lsb;

	return 0;
}

int wilco_ec_get_pm(struct ec_pm_event_state *pm, bool clear)
{
	if (clear)
		return wilco_ec_get_power_smi(pm);
	else
		return wilco_ec_get_power_status(pm);
}

int wilco_ec_get_lid_state(void)
{
	struct ec_pm_event_state pm;

	if (wilco_ec_get_power_status(&pm) < 0)
		return -1;

	return !!(pm.state[0] & EC_PM1_LID_OPEN);
}

int wilco_ec_get_board_id(uint8_t *id)
{
	return wilco_ec_mailbox(WILCO_EC_MSG_RAW, KB_BOARD_ID,
				NULL, 0, id, sizeof(*id));
}

void wilco_ec_slp_en(void)
{
	/* EC does not respond to this command */
	if (wilco_ec_mailbox(WILCO_EC_MSG_NO_RESPONSE,
			     KB_SLP_EN, NULL, 0, NULL, 0) < 0)
		printk(BIOS_ERR, "%s: command failed\n", __func__);
}

void wilco_ec_power_off(enum ec_power_off_reason reason)
{
	/* EC does not respond to this command */
	if (wilco_ec_mailbox(WILCO_EC_MSG_NO_RESPONSE,
			     KB_POWER_OFF, &reason, 1, NULL, 0) < 0)
		printk(BIOS_ERR, "%s: command failed\n", __func__);
}

int wilco_ec_radio_control(enum ec_radio radio, uint8_t state)
{
	uint8_t radio_control[3] = { radio, RADIO_WRITE, state };

	return wilco_ec_mailbox(WILCO_EC_MSG_DEFAULT, KB_RADIO_CONTROL,
				radio_control, ARRAY_SIZE(radio_control),
				NULL, 0);
}

int wilco_ec_change_wake(uint8_t source, enum ec_wake_change change)
{
	uint8_t wake_source[3] = { change, source };

	return wilco_ec_mailbox(WILCO_EC_MSG_DEFAULT, KB_ACPI_WAKEUP_CHANGE,
				wake_source, ARRAY_SIZE(wake_source),
				NULL, 0);
}

int wilco_ec_signed_fw(void)
{
	ec_set_ports(CONFIG_EC_BASE_ACPI_COMMAND,
		     CONFIG_EC_BASE_ACPI_DATA);
	return !!ec_read(EC_RAM_SIGNED_FW);
}

struct err_code_entry {
	uint8_t post_code;
	enum ec_err_code ec_err;
};

/*
 * Any post codes not listed in the post_code_err_map[] use default.
 */
static const enum ec_err_code default_ec_err = DLED_ROM;
static const struct err_code_entry post_code_err_map[] = {
	{ .post_code = POST_RAM_FAILURE, .ec_err = DLED_MEMORY, },
	{ .post_code = POST_VIDEO_FAILURE, .ec_err = DLED_PANEL, },
};

/* Records the most recent post code during boot */
static uint8_t wilco_ec_saved_post_code;

void wilco_ec_save_post_code(uint8_t post_code)
{
	wilco_ec_saved_post_code = post_code;
}

/* Send error code to the EC based on last saved post code */
void die_notify(void)
{
	size_t i;
	enum ec_err_code err_code = default_ec_err;

	for (i = 0; i < ARRAY_SIZE(post_code_err_map); i++) {
		if (post_code_err_map[i].post_code ==
		    wilco_ec_saved_post_code) {
			err_code = post_code_err_map[i].ec_err;
			break;
		}
	}

	printk(BIOS_EMERG, "Fatal error: post_code 0x%02x, EC err 0x%02x\n",
	       wilco_ec_saved_post_code, err_code);

	wilco_ec_mailbox(WILCO_EC_MSG_DEFAULT, KB_ERR_CODE,
			 &err_code, 1, NULL, 0);
}

/*
 * EC CPU ID data struct
 * MBOX[2] = 0xFF
 * MBOX[3] = CPUID_Low
 * MBOX[4] = CPUID_Mid
 * MBOX[5] = CPUID_High
 * MBOX[6] = CPU_Core
 * MBOX[7] = GPU_Core
 * MBOX[8] = Reserved
 */
int wilco_ec_set_cpuid(uint32_t cpuid, uint8_t cpu_cores, uint8_t gpu_cores)
{
	uint8_t cpu_id[7] = {0}, i;

	cpu_id[0] = 0xff;
	for (i = 1; i < 4; i++) {
		cpu_id[i] = cpuid & 0xff;
		cpuid = cpuid >> 8;
	}
	cpu_id[4] = cpu_cores;
	cpu_id[5] = gpu_cores;

	return wilco_ec_mailbox(WILCO_EC_MSG_DEFAULT, KB_CPU_ID, cpu_id,
				ARRAY_SIZE(cpu_id), NULL, 0);
}
