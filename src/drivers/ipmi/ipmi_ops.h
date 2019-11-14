#ifndef __IPMI_OPS_H
#define __IPMI_OPS_H
/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Wiwynn Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <types.h>
#include "ipmi_kcs.h"
#define IPMI_BMC_RESET_WDG_TIMER 0x22
#define IPMI_BMC_SET_WDG_TIMER 0x24
#define IPMI_BMC_GET_WDG_TIMER 0x25
#define IPMI_BMC_GET_SYSTEM_GUID 0x37

/* BMC watchdog timeout action */
enum ipmi_bmc_timeout_action_type {
	TIMEOUT_NO_ACTION = 0x00,
	TIMEOUT_HARD_RESET = 0x01,
	TIMEOUT_POWER_DOWN = 0x02,
	TIMEOUT_POWER_CYCLE = 0x03,
};
/* BMC Watchdog timer */
struct ipmi_wdt_req {
	uint8_t timer_use;
	uint8_t timer_actions;
	uint8_t pretimeout_interval;
	uint8_t timer_use_expiration_flags_clr;
	uint16_t initial_countdown_val;
} __packed;

struct ipmi_wdt_rsp {
	struct ipmi_rsp resp;
	struct ipmi_wdt_req data;
	uint16_t present_countdown_val;
} __packed;

struct ipmi_get_system_guid_rsp {
	struct ipmi_rsp resp;
	uint8_t data[16];
} __packed;
/*
 * Initialize and start BMC FRB2 watchdog timer with the
 * provided timer countdown and action values.
 * Returns CB_SUCCESS on success and CB_ERR if an error occurred
 */
enum cb_err ipmi_init_and_start_bmc_wdt(const int port, uint16_t countdown,
				uint8_t action);
/* Returns CB_SUCCESS on success and CB_ERR if an error occurred */
enum cb_err ipmi_stop_bmc_wdt(const int port);

/* IPMI get BMC system GUID and store it to parameter uuid.
 * Returns CB_SUCCESS on success and CB_ERR if an error occurred */
enum cb_err ipmi_get_system_guid(const int port, uint8_t *uuid);
#endif
