#ifndef __IPMI_OPS_H
#define __IPMI_OPS_H
/*
 * This file is part of the coreboot project.
 *
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

struct ipmi_read_fru_data_req {
	uint8_t fru_device_id;
	uint16_t fru_offset;
	uint8_t count; /* count to read, 1-based. */
} __packed;

struct ipmi_read_fru_data_rsp {
	struct ipmi_rsp resp;
	uint8_t count; /* count returned, 1-based. */
	uint8_t data[CONFIG_IPMI_FRU_SINGLE_RW_SZ];
} __packed;

/* Platform Management FRU Information Storage Definition Spec. */
#define PRODUCT_MAN_TYPE_LEN_OFFSET 3
#define BOARD_MAN_TYPE_LEN_OFFSET 6

struct ipmi_fru_common_hdr {
	uint8_t format_version;
	uint8_t internal_use_area_offset;
	uint8_t chassis_area_offset;
	uint8_t board_area_offset;
	uint8_t product_area_offset;
	uint8_t multirecord_area_offset;
	uint8_t pad;
	uint8_t checksum;
} __packed;

/* The fru_xxx_info only declares the strings that may be added to SMBIOS. */
struct fru_product_info {
	char *manufacturer;
	char *product_name;
	char *product_partnumber;
	char *product_version;
	char *serial_number;
	char *asset_tag;
};

struct fru_board_info {
	char *manufacturer;
	char *product_name;
	char *serial_number;
	char *part_number;
};

struct fru_info_str {
	struct fru_product_info prod_info;
	struct fru_board_info board_info;
};

enum typecode {
	BINARY = 0,
	BCD_PLUS = 1,
	ASCII_6BIT = 2,
	ASCII_8BIT = 3,
};

enum fru_area {
	INTERNAL_USE_AREA = 0,
	CHASSIS_INFO_AREA = 1,
	BOARD_INFO_AREA = 2,
	PRODUCT_INFO_AREA = 3,
	MULTIRECORD_INFO_AREA = 4,
};
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

/* Read all FRU inventory areas string data into fru_info_str with
 * the same FRU device id. */
void read_fru_areas(const int port, uint8_t id, uint16_t offset,
			struct fru_info_str *fru_info_str);

/* Read a particular FRU inventory area into fru_info_str. */
void read_fru_one_area(const int port, uint8_t id, uint16_t offset,
		struct fru_info_str *fru_info_str, enum fru_area fru_area);
#endif
