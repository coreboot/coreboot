/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/path.h>
#include <elog.h>
#include <rtc.h>
#include <security/vboot/vboot_common.h>
#include <stdlib.h>
#include <timer.h>

#include "ec.h"

#define INVALID_HCMD 0xFF

/*
 * Map UHEPI masks to non UHEPI commands in order to support old EC FW
 * which does not support UHEPI command.
 */
static const struct {
	uint8_t set_cmd;
	uint8_t clear_cmd;
	uint8_t get_cmd;
} event_map[] = {
	[EC_HOST_EVENT_MAIN] = {
		INVALID_HCMD, EC_CMD_HOST_EVENT_CLEAR,
		INVALID_HCMD,
	},
	[EC_HOST_EVENT_B] = {
		INVALID_HCMD, EC_CMD_HOST_EVENT_CLEAR_B,
		EC_CMD_HOST_EVENT_GET_B,
	},
	[EC_HOST_EVENT_SCI_MASK] = {
		EC_CMD_HOST_EVENT_SET_SCI_MASK, INVALID_HCMD,
		EC_CMD_HOST_EVENT_GET_SCI_MASK,
	},
	[EC_HOST_EVENT_SMI_MASK] = {
		EC_CMD_HOST_EVENT_SET_SMI_MASK, INVALID_HCMD,
		EC_CMD_HOST_EVENT_GET_SMI_MASK,
	},
	[EC_HOST_EVENT_ALWAYS_REPORT_MASK] = {
		INVALID_HCMD, INVALID_HCMD, INVALID_HCMD,
	},
	[EC_HOST_EVENT_ACTIVE_WAKE_MASK] = {
		EC_CMD_HOST_EVENT_SET_WAKE_MASK, INVALID_HCMD,
		EC_CMD_HOST_EVENT_GET_WAKE_MASK,
	},
	[EC_HOST_EVENT_LAZY_WAKE_MASK_S0IX] = {
		EC_CMD_HOST_EVENT_SET_WAKE_MASK, INVALID_HCMD,
		EC_CMD_HOST_EVENT_GET_WAKE_MASK,
	},
	[EC_HOST_EVENT_LAZY_WAKE_MASK_S3] = {
		EC_CMD_HOST_EVENT_SET_WAKE_MASK, INVALID_HCMD,
		EC_CMD_HOST_EVENT_GET_WAKE_MASK,
	},
	[EC_HOST_EVENT_LAZY_WAKE_MASK_S5] = {
		EC_CMD_HOST_EVENT_SET_WAKE_MASK, INVALID_HCMD,
		EC_CMD_HOST_EVENT_GET_WAKE_MASK,
	},
};

uint8_t google_chromeec_calc_checksum(const uint8_t *data, int size)
{
	int csum;

	for (csum = 0; size > 0; data++, size--)
		csum += *data;
	return (uint8_t)(csum & 0xff);
}

int google_chromeec_kbbacklight(int percent)
{
	struct ec_params_pwm_set_keyboard_backlight params = {
		.percent = percent % 101,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_PWM_SET_KEYBOARD_BACKLIGHT,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_data_out = NULL,
		.cmd_size_in = sizeof(params),
		.cmd_size_out = 0,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	return 0;
}

void google_chromeec_post(uint8_t postcode)
{
	/* backlight is a percent. postcode is a uint8_t.
	 * Convert the uint8_t to %.
	 */
	postcode = (postcode/4) + (postcode/8);
	google_chromeec_kbbacklight(postcode);
}

/*
 * Query the EC for specified mask indicating enabled events.
 * The EC maintains separate event masks for SMI, SCI and WAKE.
 */
static int google_chromeec_uhepi_cmd(uint8_t mask, uint8_t action,
					uint64_t *value)
{
	int ret;
	struct ec_params_host_event params = {
		.action = action,
		.mask_type = mask,
	};
	struct ec_response_host_event resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_HOST_EVENT,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	if (action != EC_HOST_EVENT_GET)
		params.value = *value;
	else
		*value = 0;

	ret = google_chromeec_command(&cmd);

	if (action != EC_HOST_EVENT_GET)
		return ret;
	if (ret == 0)
		*value = resp.value;
	return ret;
}

static int google_chromeec_handle_non_uhepi_cmd(uint8_t hcmd, uint8_t action,
						uint64_t *value)
{
	int ret = -1;
	struct ec_params_host_event_mask params = {};
	struct ec_response_host_event_mask resp = {};
	struct chromeec_command cmd = {
		.cmd_code = hcmd,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	if (hcmd == INVALID_HCMD)
		return ret;

	if (action != EC_HOST_EVENT_GET)
		params.mask = (uint32_t)*value;
	else
		*value = 0;

	ret = google_chromeec_command(&cmd);

	if (action != EC_HOST_EVENT_GET)
		return ret;
	if (ret == 0)
		*value = resp.mask;

	return ret;
}

bool google_chromeec_is_uhepi_supported(void)
{
#define UHEPI_SUPPORTED 1
#define UHEPI_NOT_SUPPORTED 2

	static int uhepi_support;

	if (!uhepi_support) {
		uhepi_support = google_chromeec_check_feature
			(EC_FEATURE_UNIFIED_WAKE_MASKS) > 0 ? UHEPI_SUPPORTED :
			UHEPI_NOT_SUPPORTED;
		printk(BIOS_DEBUG, "Chrome EC: UHEPI %s\n",
			uhepi_support == UHEPI_SUPPORTED ?
			"supported" : "not supported");
	}
	return uhepi_support == UHEPI_SUPPORTED;
}

static uint64_t google_chromeec_get_mask(uint8_t type)
{
	uint64_t value = 0;

	if (google_chromeec_is_uhepi_supported()) {
		google_chromeec_uhepi_cmd(type, EC_HOST_EVENT_GET, &value);
	} else {
		assert(type < ARRAY_SIZE(event_map));
		google_chromeec_handle_non_uhepi_cmd(
					event_map[type].get_cmd,
					EC_HOST_EVENT_GET, &value);
	}
	return value;
}

static int google_chromeec_clear_mask(uint8_t type, uint64_t mask)
{
	if (google_chromeec_is_uhepi_supported())
		return google_chromeec_uhepi_cmd(type,
					EC_HOST_EVENT_CLEAR, &mask);

	assert(type < ARRAY_SIZE(event_map));
	return google_chromeec_handle_non_uhepi_cmd(
						event_map[type].clear_cmd,
						EC_HOST_EVENT_CLEAR, &mask);
}

static int google_chromeec_set_mask(uint8_t type, uint64_t mask)
{
	if (google_chromeec_is_uhepi_supported())
		return google_chromeec_uhepi_cmd(type,
					EC_HOST_EVENT_SET, &mask);

	assert(type < ARRAY_SIZE(event_map));
	return google_chromeec_handle_non_uhepi_cmd(
						event_map[type].set_cmd,
						EC_HOST_EVENT_SET, &mask);
}

static int google_chromeec_set_s3_lazy_wake_mask(uint64_t mask)
{
	printk(BIOS_DEBUG, "Chrome EC: Set S3 LAZY WAKE mask to 0x%016llx\n",
				mask);
	return google_chromeec_set_mask
		(EC_HOST_EVENT_LAZY_WAKE_MASK_S3, mask);
}

static int google_chromeec_set_s5_lazy_wake_mask(uint64_t mask)
{
	printk(BIOS_DEBUG, "Chrome EC: Set S5 LAZY WAKE mask to 0x%016llx\n",
				mask);
	return google_chromeec_set_mask
		(EC_HOST_EVENT_LAZY_WAKE_MASK_S5, mask);
}

static int google_chromeec_set_s0ix_lazy_wake_mask(uint64_t mask)
{
	printk(BIOS_DEBUG, "Chrome EC: Set S0iX LAZY WAKE mask to 0x%016llx\n",
				mask);
	return google_chromeec_set_mask
		(EC_HOST_EVENT_LAZY_WAKE_MASK_S0IX, mask);
}
static void google_chromeec_set_lazy_wake_masks(uint64_t s5_mask,
					uint64_t s3_mask, uint64_t s0ix_mask)
{
	if (google_chromeec_set_s5_lazy_wake_mask(s5_mask))
		printk(BIOS_DEBUG, "Error: Set S5 LAZY WAKE mask failed\n");
	if (google_chromeec_set_s3_lazy_wake_mask(s3_mask))
		printk(BIOS_DEBUG, "Error: Set S3 LAZY WAKE mask failed\n");
	/*
	 * Make sure S0Ix is supported before trying to set up the EC's
	 * S0Ix lazy wake mask.
	 */
	if (s0ix_mask && google_chromeec_set_s0ix_lazy_wake_mask(s0ix_mask))
		printk(BIOS_DEBUG, "Error: Set S0iX LAZY WAKE mask failed\n");
}

uint64_t google_chromeec_get_events_b(void)
{
	return google_chromeec_get_mask(EC_HOST_EVENT_B);
}

int google_chromeec_clear_events_b(uint64_t mask)
{
	printk(BIOS_DEBUG,
		"Chrome EC: clear events_b mask to 0x%016llx\n", mask);
	return google_chromeec_clear_mask(EC_HOST_EVENT_B, mask);
}

int google_chromeec_get_mkbp_event(struct ec_response_get_next_event *event)
{
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_NEXT_EVENT,
		.cmd_version = 0,
		.cmd_data_in = NULL,
		.cmd_size_in = 0,
		.cmd_data_out = event,
		.cmd_size_out = sizeof(*event),
		.cmd_dev_index = 0,
	};

	return google_chromeec_command(&cmd);
}

/* Get the current device event mask */
uint64_t google_chromeec_get_device_enabled_events(void)
{
	struct ec_params_device_event params = {
		.param = EC_DEVICE_EVENT_PARAM_GET_ENABLED_EVENTS,
	};
	struct ec_response_device_event resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_DEVICE_EVENT,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) == 0)
		return resp.event_mask;

	return 0;
}

/* Set the current device event mask */
int google_chromeec_set_device_enabled_events(uint64_t mask)
{
	struct ec_params_device_event params = {
		.event_mask = (uint32_t)mask,
		.param = EC_DEVICE_EVENT_PARAM_SET_ENABLED_EVENTS,
	};
	struct ec_response_device_event resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_DEVICE_EVENT,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	return google_chromeec_command(&cmd);
}

/* Read and clear pending device events */
uint64_t google_chromeec_get_device_current_events(void)
{
	struct ec_params_device_event params = {
		.param = EC_DEVICE_EVENT_PARAM_GET_CURRENT_EVENTS,
	};
	struct ec_response_device_event resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_DEVICE_EVENT,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) == 0)
		return resp.event_mask;

	return 0;
}

static void google_chromeec_log_device_events(uint64_t mask)
{
	uint64_t events;
	int i;

	if (!CONFIG(ELOG) || !mask)
		return;

	if (google_chromeec_check_feature(EC_FEATURE_DEVICE_EVENT) != 1)
		return;

	events = google_chromeec_get_device_current_events() & mask;
	printk(BIOS_INFO, "EC Device Events: 0x%016llx\n", events);

	for (i = 0; i < sizeof(events) * 8; i++) {
		if (EC_DEVICE_EVENT_MASK(i) & events)
			elog_add_event_byte(ELOG_TYPE_EC_DEVICE_EVENT, i);
	}
}

void google_chromeec_log_events(uint64_t mask)
{
	uint64_t events;
	int i;

	if (!CONFIG(ELOG))
		return;

	events = google_chromeec_get_events_b() & mask;

	/*
	 * This loop starts at 1 because the EC_HOST_EVENT_MASK macro subtracts
	 * 1 from its argument before applying the left-shift operator. This
	 * prevents a left-shift of -1 happening, and covers the entire 64-bit
	 * range of the event mask.
	 */
	for (i = 1; i <= sizeof(events) * 8; i++) {
		if (EC_HOST_EVENT_MASK(i) & events)
			elog_add_event_byte(ELOG_TYPE_EC_EVENT, i);
	}

	google_chromeec_clear_events_b(events);
}

void google_chromeec_events_init(const struct google_chromeec_event_info *info,
					bool is_s3_wakeup)
{
	if (is_s3_wakeup) {
		google_chromeec_log_events(info->log_events |
						info->s3_wake_events);

		/* Log and clear device events that may wake the system. */
		google_chromeec_log_device_events(info->s3_device_events);

		/* Disable SMI and wake events. */
		google_chromeec_set_smi_mask(0);

		/* Restore SCI event mask. */
		google_chromeec_set_sci_mask(info->sci_events);

	} else {
		google_chromeec_set_smi_mask(info->smi_events);

		google_chromeec_log_events(info->log_events |
						info->s5_wake_events);

		if (google_chromeec_is_uhepi_supported())
			google_chromeec_set_lazy_wake_masks
					(info->s5_wake_events,
					info->s3_wake_events,
					info->s0ix_wake_events);
	}

	/* Clear wake event mask. */
	google_chromeec_set_wake_mask(0);
}

int google_chromeec_check_feature(int feature)
{
	struct ec_response_get_features resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_FEATURES,
		.cmd_version = 0,
		.cmd_size_in = 0,
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	if (feature >= 8 * sizeof(resp.flags))
		return -1;

	return resp.flags[feature / 32] & EC_FEATURE_MASK_0(feature);
}

int google_chromeec_get_cmd_versions(int command, uint32_t *pmask)
{
	struct ec_params_get_cmd_versions_v1 params = {
		.cmd = command,
	};
	struct ec_response_get_cmd_versions resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_CMD_VERSIONS,
		.cmd_version = 1,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_size_out = sizeof(resp),
		.cmd_data_out = &resp,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	*pmask = resp.version_mask;
	return 0;
}

int google_chromeec_get_vboot_hash(uint32_t offset,
				struct ec_response_vboot_hash *resp)
{
	struct ec_params_vboot_hash params = {
		.cmd = EC_VBOOT_HASH_GET,
		.offset = offset,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_VBOOT_HASH,
		.cmd_version = 0,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_size_out = sizeof(*resp),
		.cmd_data_out = resp,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	return 0;
}

int google_chromeec_start_vboot_hash(enum ec_vboot_hash_type hash_type,
				uint32_t hash_offset,
				struct ec_response_vboot_hash *resp)
{
	struct ec_params_vboot_hash params = {
		.cmd = EC_VBOOT_HASH_START,
		.hash_type = hash_type,
		.nonce_size = 0,
		.offset = hash_offset,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_VBOOT_HASH,
		.cmd_version = 0,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_size_out = sizeof(*resp),
		.cmd_data_out = resp,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	return 0;
}

int google_chromeec_flash_protect(uint32_t mask, uint32_t flags,
	struct ec_response_flash_protect *resp)
{
	struct ec_params_flash_protect params = {
		.mask = mask,
		.flags = flags,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_FLASH_PROTECT,
		.cmd_version = EC_VER_FLASH_PROTECT,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_size_out = sizeof(*resp),
		.cmd_data_out = resp,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	return 0;
}

int google_chromeec_flash_region_info(enum ec_flash_region region,
				uint32_t *offset, uint32_t *size)
{
	struct ec_params_flash_region_info params = {
		.region = region,
	};
	struct ec_response_flash_region_info resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_FLASH_REGION_INFO,
		.cmd_version = EC_VER_FLASH_REGION_INFO,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_size_out = sizeof(resp),
		.cmd_data_out = &resp,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	if (offset)
		*offset = resp.offset;
	if (size)
		*size = resp.size;

	return 0;
}

int google_chromeec_flash_erase(uint32_t offset, uint32_t size)
{
	struct ec_params_flash_erase params = {
		.offset = offset,
		.size = size,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_FLASH_ERASE,
		.cmd_version = 0,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_size_out = 0,
		.cmd_data_out = NULL,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	return 0;
}

int google_chromeec_flash_info(struct ec_response_flash_info *info)
{
	struct chromeec_command cmd;

	cmd.cmd_code = EC_CMD_FLASH_INFO;
	cmd.cmd_version = 0;
	cmd.cmd_size_in = 0;
	cmd.cmd_data_in = NULL;
	cmd.cmd_size_out = sizeof(*info);
	cmd.cmd_data_out = info;
	cmd.cmd_dev_index = 0;

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	return 0;
}

/*
 * Write a block into EC flash.  Expects params_data to be a buffer where
 * the first N bytes are a struct ec_params_flash_write, and the rest of it
 * is the data to write to flash.
*/
int google_chromeec_flash_write_block(const uint8_t *params_data,
				uint32_t bufsize)
{
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_FLASH_WRITE,
		.cmd_version = EC_VER_FLASH_WRITE,
		.cmd_size_out = 0,
		.cmd_data_out = NULL,
		.cmd_size_in = bufsize,
		.cmd_data_in = params_data,
		.cmd_dev_index = 0,
	};

	assert(params_data);

	return google_chromeec_command(&cmd);
}

/*
 * EFS verification of flash.
 */
int google_chromeec_efs_verify(enum ec_flash_region region)
{
	struct ec_params_efs_verify params = {
		.region = region,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_EFS_VERIFY,
		.cmd_version = 0,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_size_out = 0,
		.cmd_data_out = NULL,
		.cmd_dev_index = 0,
	};
	int rv;

	/* It's okay if the EC doesn't support EFS */
	rv = google_chromeec_command(&cmd);
	if (rv != 0 && (cmd.cmd_code == EC_RES_INVALID_COMMAND))
		return 0;
	else if (rv != 0)
		return -1;

	return 0;
}

int google_chromeec_battery_cutoff(uint8_t flags)
{
	struct ec_params_battery_cutoff params = {
		.flags = flags,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_BATTERY_CUT_OFF,
		.cmd_version = 1,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_data_out = NULL,
		.cmd_size_out = 0,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	return 0;
}

int google_chromeec_read_limit_power_request(int *limit_power)
{
	struct ec_params_charge_state params = {
		.cmd = CHARGE_STATE_CMD_GET_PARAM,
		.get_param.param = CS_PARAM_LIMIT_POWER,
	};
	struct ec_response_charge_state resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_CHARGE_STATE,
		.cmd_version = 0,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_size_out = sizeof(resp),
		.cmd_data_out = &resp,
		.cmd_dev_index = 0,
	};
	int rv;

	rv = google_chromeec_command(&cmd);

	if (rv != 0 && (cmd.cmd_code == EC_RES_INVALID_COMMAND ||
				cmd.cmd_code == EC_RES_INVALID_PARAM)) {
		printk(BIOS_INFO, "PARAM_LIMIT_POWER not supported by EC.\n");
		*limit_power = 0;
		return 0;
	} else if (rv != 0) {
		return -1;
	}

	*limit_power = resp.get_param.value;

	return 0;
}

int google_chromeec_get_protocol_info(
	struct ec_response_get_protocol_info *resp)
{
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_PROTOCOL_INFO,
		.cmd_version = 0,
		.cmd_size_in = 0,
		.cmd_data_in = NULL,
		.cmd_data_out = resp,
		.cmd_size_out = sizeof(*resp),
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd))
		return -1;

	return 0;
}

int google_chromeec_set_sku_id(uint32_t skuid)
{
	struct ec_sku_id_info params = {
		.sku_id = skuid
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_SET_SKU_ID,
		.cmd_version = 0,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_data_out = NULL,
		.cmd_size_out = 0,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	return 0;
}

#if CONFIG(EC_GOOGLE_CHROMEEC_RTC)
int rtc_get(struct rtc_time *time)
{
	struct ec_response_rtc resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_RTC_GET_VALUE,
		.cmd_version = 0,
		.cmd_size_in = 0,
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return -1;

	return rtc_to_tm(resp.time, time);
}
#endif

int google_chromeec_reboot(int dev_idx, enum ec_reboot_cmd type, uint8_t flags)
{
	struct ec_params_reboot_ec params = {
		.cmd = type,
		.flags = flags,
	};
	struct ec_response_get_version resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_REBOOT_EC,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_data_out = &resp,
		.cmd_size_in = sizeof(params),
		.cmd_size_out = 0, /* ignore response, if any */
		.cmd_dev_index = dev_idx,
	};

	return google_chromeec_command(&cmd);
}

static int cbi_get_uint32(uint32_t *id, uint32_t tag)
{
	struct ec_params_get_cbi params = {
		.tag = tag,
	};
	uint32_t r = 0;
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_CROS_BOARD_INFO,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_data_out = &r,
		.cmd_size_in = sizeof(params),
		.cmd_size_out = sizeof(r),
		.cmd_dev_index = 0,
	};
	int rv;

	rv = google_chromeec_command(&cmd);
	if (rv != 0)
		return rv;

	*id = r;
	return 0;
}

int google_chromeec_cbi_get_sku_id(uint32_t *id)
{
	return cbi_get_uint32(id, CBI_TAG_SKU_ID);
}

int google_chromeec_cbi_get_fw_config(uint64_t *fw_config)
{
	uint32_t config;

	if (cbi_get_uint32(&config, CBI_TAG_FW_CONFIG))
		return -1;

	*fw_config = (uint64_t)config;
	/*
	 * If SSFC is configured to be part of FW_CONFIG, add it at the most significant
	 * 32 bits.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC_INCLUDE_SSFC_IN_FW_CONFIG)) {
		uint32_t ssfc;

		if (!google_chromeec_cbi_get_ssfc(&ssfc))
			*fw_config |= (uint64_t)ssfc << 32;
	}
	return 0;
}

int google_chromeec_cbi_get_oem_id(uint32_t *id)
{
	return cbi_get_uint32(id, CBI_TAG_OEM_ID);
}

int google_chromeec_cbi_get_board_version(uint32_t *version)
{
	return cbi_get_uint32(version, CBI_TAG_BOARD_VERSION);
}

int google_chromeec_cbi_get_ssfc(uint32_t *ssfc)
{
	return cbi_get_uint32(ssfc, CBI_TAG_SSFC);
}

static int cbi_get_string(char *buf, size_t bufsize, uint32_t tag)
{
	struct ec_params_get_cbi params = {
		.tag = tag,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_CROS_BOARD_INFO,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_data_out = buf,
		.cmd_size_in = sizeof(params),
		.cmd_size_out = bufsize,
	};
	int rv;

	rv = google_chromeec_command(&cmd);
	if (rv != 0)
		return rv;

	/* Ensure NUL termination. */
	buf[bufsize - 1] = '\0';

	return 0;
}

int google_chromeec_cbi_get_dram_part_num(char *buf, size_t bufsize)
{
	return cbi_get_string(buf, bufsize, CBI_TAG_DRAM_PART_NUM);
}

int google_chromeec_cbi_get_oem_name(char *buf, size_t bufsize)
{
	return cbi_get_string(buf, bufsize, CBI_TAG_OEM_NAME);
}

int google_chromeec_get_board_version(uint32_t *version)
{
	struct ec_response_board_version resp;
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_BOARD_VERSION,
		.cmd_version = 0,
		.cmd_size_in = 0,
		.cmd_size_out = sizeof(resp),
		.cmd_data_out = &resp,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd))
		return -1;

	*version = resp.board_version;
	return 0;
}

uint32_t google_chromeec_get_sku_id(void)
{
	struct ec_sku_id_info resp;
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_SKU_ID,
		.cmd_version = 0,
		.cmd_size_in = 0,
		.cmd_size_out = sizeof(resp),
		.cmd_data_out = &resp,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) != 0)
		return 0;

	return resp.sku_id;
}

static uint16_t google_chromeec_get_uptime_info(
	struct ec_response_uptime_info *resp)
{
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_UPTIME_INFO,
		.cmd_version = 0,
		.cmd_data_in = NULL,
		.cmd_size_in = 0,
		.cmd_data_out = resp,
		.cmd_size_out = sizeof(*resp),
		.cmd_dev_index = 0,
	};

	google_chromeec_command(&cmd);
	return cmd.cmd_code;
}

bool google_chromeec_get_ap_watchdog_flag(void)
{
	int i;
	struct ec_response_uptime_info resp;

	if (google_chromeec_get_uptime_info(&resp))
		return false;

	if (resp.ec_reset_flags & EC_RESET_FLAG_AP_WATCHDOG)
		return true;

	/* Find the last valid entry */
	for (i = ARRAY_SIZE(resp.recent_ap_reset) - 1; i >= 0; i--) {
		if (resp.recent_ap_reset[i].reset_time_ms == 0)
			continue;
		return (resp.recent_ap_reset[i].reset_cause ==
			CHIPSET_RESET_AP_WATCHDOG);
	}

	return false;
}

int google_chromeec_i2c_xfer(uint8_t chip, uint8_t addr, int alen,
			     uint8_t *buffer, int len, int is_read)
{
	union {
		struct ec_params_i2c_passthru p;
		uint8_t outbuf[EC_HOST_PARAM_SIZE];
	} params;
	union {
		struct ec_response_i2c_passthru r;
		uint8_t inbuf[EC_HOST_PARAM_SIZE];
	} response;
	struct ec_params_i2c_passthru *p = &params.p;
	struct ec_response_i2c_passthru *r = &response.r;
	struct ec_params_i2c_passthru_msg *msg = p->msg;
	struct chromeec_command cmd;
	uint8_t *pdata;
	int read_len, write_len;
	int size;
	int rv;

	p->port = 0;

	if (alen != 1) {
		printk(BIOS_ERR, "Unsupported address length %d\n", alen);
		return -1;
	}
	if (is_read) {
		read_len = len;
		write_len = alen;
		p->num_msgs = 2;
	} else {
		read_len = 0;
		write_len = alen + len;
		p->num_msgs = 1;
	}

	size = sizeof(*p) + p->num_msgs * sizeof(*msg);
	if (size + write_len > sizeof(params)) {
		printk(BIOS_ERR, "Params too large for buffer\n");
		return -1;
	}
	if (sizeof(*r) + read_len > sizeof(response)) {
		printk(BIOS_ERR, "Read length too big for buffer\n");
		return -1;
	}

	/* Create a message to write the register address and optional data */
	pdata = (uint8_t *)p + size;
	msg->addr_flags = chip;
	msg->len = write_len;
	pdata[0] = addr;
	if (!is_read)
		memcpy(pdata + 1, buffer, len);
	msg++;

	if (read_len) {
		msg->addr_flags = chip | EC_I2C_FLAG_READ;
		msg->len = read_len;
	}

	cmd.cmd_code = EC_CMD_I2C_PASSTHRU;
	cmd.cmd_version = 0;
	cmd.cmd_data_in = p;
	cmd.cmd_size_in = size + write_len;
	cmd.cmd_data_out = r;
	cmd.cmd_size_out = sizeof(*r) + read_len;
	cmd.cmd_dev_index = 0;
	rv = google_chromeec_command(&cmd);
	if (rv != 0)
		return rv;

	/* Parse response */
	if (r->i2c_status & EC_I2C_STATUS_ERROR) {
		printk(BIOS_ERR, "Transfer failed with status=0x%x\n",
		       r->i2c_status);
		return -1;
	}

	if (cmd.cmd_size_out < sizeof(*r) + read_len) {
		printk(BIOS_ERR, "Truncated read response\n");
		return -1;
	}

	if (read_len)
		memcpy(buffer, r->data, read_len);

	return 0;
}

int google_chromeec_set_sci_mask(uint64_t mask)
{
	printk(BIOS_DEBUG, "Chrome EC: Set SCI mask to 0x%016llx\n", mask);
	return google_chromeec_set_mask(EC_HOST_EVENT_SCI_MASK, mask);
}

int google_chromeec_set_smi_mask(uint64_t mask)
{
	printk(BIOS_DEBUG, "Chrome EC: Set SMI mask to 0x%016llx\n", mask);
	return google_chromeec_set_mask(EC_HOST_EVENT_SMI_MASK, mask);
}

int google_chromeec_set_wake_mask(uint64_t mask)
{
	printk(BIOS_DEBUG, "Chrome EC: Set WAKE mask to 0x%016llx\n", mask);
	return google_chromeec_set_mask
			(EC_HOST_EVENT_ACTIVE_WAKE_MASK, mask);
}

uint64_t google_chromeec_get_wake_mask(void)
{
	return google_chromeec_get_mask(EC_HOST_EVENT_ACTIVE_WAKE_MASK);
}

int google_chromeec_set_usb_charge_mode(uint8_t port_id, enum usb_charge_mode mode)
{
	struct ec_params_usb_charge_set_mode params = {
		.usb_port_id = port_id,
		.mode = mode,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_USB_CHARGE_SET_MODE,
		.cmd_version = 0,
		.cmd_size_in = sizeof(params),
		.cmd_data_in = &params,
		.cmd_size_out = 0,
		.cmd_data_out = NULL,
		.cmd_dev_index = 0,
	};

	return google_chromeec_command(&cmd);
}

/* Get charger voltage and current.  Also returns type of charger */
int google_chromeec_get_usb_pd_power_info(enum usb_chg_type *type,
					  uint16_t *current_max, uint16_t *voltage_max)
{
	struct ec_params_usb_pd_power_info params = {
		.port = PD_POWER_CHARGING_PORT,
	};
	struct ec_response_usb_pd_power_info resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_USB_PD_POWER_INFO,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};
	struct usb_chg_measures m;
	int rv;

	rv = google_chromeec_command(&cmd);
	if (rv != 0)
		return rv;

	/* values are given in milliAmps and milliVolts */
	*type = resp.type;
	m = resp.meas;
	*voltage_max = m.voltage_max;
	*current_max = m.current_max;
	return 0;
}

int google_chromeec_override_dedicated_charger_limit(uint16_t current_lim,
						     uint16_t voltage_lim)
{
	struct ec_params_dedicated_charger_limit params = {
		.current_lim = current_lim,
		.voltage_lim = voltage_lim,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_OVERRIDE_DEDICATED_CHARGER_LIMIT,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = NULL,
		.cmd_size_out = 0,
		.cmd_dev_index = 0,
	};

	return google_chromeec_command(&cmd);
}

int google_chromeec_set_usb_pd_role(uint8_t port, enum usb_pd_control_role role)
{
	struct ec_params_usb_pd_control params = {
		.port = port,
		.role = role,
		.mux = USB_PD_CTRL_MUX_NO_CHANGE,
		.swap = USB_PD_CTRL_SWAP_NONE,
	};
	struct ec_response_usb_pd_control resp;
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_USB_PD_CONTROL,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	return google_chromeec_command(&cmd);
}

int google_chromeec_hello(void)
{
	struct ec_params_hello params = {
		.in_data = 0x10203040,
	};
	struct ec_response_hello resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_HELLO,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_data_out = &resp,
		.cmd_size_in = sizeof(params),
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	int rv = google_chromeec_command(&cmd);
	if (rv)
		return -1;

	if (resp.out_data != (params.in_data + 0x01020304))
		return -1;

	return 0;
}

/*
 * Convert a reset cause ID to human-readable string, providing total coverage
 * of the 'cause' space.  The returned string points to static storage and must
 * not be free()ed.
 */
static const char *reset_cause_to_str(uint16_t cause)
{
	/* See also ChromiumOS EC include/chipset.h for details. */
	static const char * const reset_causes[] = {
		"(reset unknown)",
		"reset: board custom",
		"reset: ap hang detected",
		"reset: console command",
		"reset: keyboard sysreset",
		"reset: keyboard warm reboot",
		"reset: debug warm reboot",
		"reset: at AP's request",
		"reset: during EC initialization",
		"reset: AP watchdog",
	};

	static const size_t shutdown_cause_begin = 1 << 15;
	static const char * const shutdown_causes[] = {
		"shutdown: power failure",
		"shutdown: during EC initialization",
		"shutdown: board custom",
		"shutdown: battery voltage startup inhibit",
		"shutdown: power wait asserted",
		"shutdown: critical battery",
		"shutdown: by console command",
		"shutdown: entering G3",
		"shutdown: thermal",
		"shutdown: power button",
	};

	if (cause < ARRAY_SIZE(reset_causes))
		return reset_causes[cause];

	if (cause < shutdown_cause_begin)
		return "(reset unknown)";

	if (cause < shutdown_cause_begin + ARRAY_SIZE(shutdown_causes))
		return shutdown_causes[cause - shutdown_cause_begin];

	return "(shutdown unknown)";
}

/*
 * Copy the EC's information about resets of the AP and its own uptime for
 * debugging purposes.
 */
static void google_chromeec_log_uptimeinfo(void)
{
	/* See also ec_commands.h EC_RESET_FLAG_* for details. */
	static const char * const reset_flag_strings[] = {
		"other",
		"reset-pin",
		"brownout",
		"power-on",
		"watchdog",
		"soft",
		"hibernate",
		"rtc-alarm",
		"wake-pin",
		"low-battery",
		"sysjump",
		"hard",
		"ap-off",
		"preserved",
		"usb-resume",
		"rdd",
		"rbox",
		"security",
		"ap-watchdog",
	};
	struct ec_response_uptime_info cmd_resp;
	int i, flag, flag_count;

	if (google_chromeec_get_uptime_info(&cmd_resp)) {
		/*
		 * Deliberately say nothing for EC's that don't support this
		 * command
		 */
		return;
	}

	printk(BIOS_DEBUG, "Google Chrome EC uptime: %d.%03d seconds\n",
		cmd_resp.time_since_ec_boot_ms / MSECS_PER_SEC,
		cmd_resp.time_since_ec_boot_ms % MSECS_PER_SEC);

	printk(BIOS_DEBUG, "Google Chrome AP resets since EC boot: %d\n",
		cmd_resp.ap_resets_since_ec_boot);

	printk(BIOS_DEBUG, "Google Chrome most recent AP reset causes:\n");
	for (i = 0; i != ARRAY_SIZE(cmd_resp.recent_ap_reset); ++i) {
		if (cmd_resp.recent_ap_reset[i].reset_time_ms == 0)
			continue;

		printk(BIOS_DEBUG, "\t%d.%03d: %d %s\n",
			cmd_resp.recent_ap_reset[i].reset_time_ms /
				MSECS_PER_SEC,
			cmd_resp.recent_ap_reset[i].reset_time_ms %
				MSECS_PER_SEC,
			cmd_resp.recent_ap_reset[i].reset_cause,
			reset_cause_to_str(
				cmd_resp.recent_ap_reset[i].reset_cause));
	}

	printk(BIOS_DEBUG, "Google Chrome EC reset flags at last EC boot: ");
	flag_count = 0;
	for (flag = 0; flag != ARRAY_SIZE(reset_flag_strings); ++flag) {
		if ((cmd_resp.ec_reset_flags & (1 << flag)) != 0) {
			if (flag_count)
				printk(BIOS_DEBUG, " | ");
			printk(BIOS_DEBUG, "%s", reset_flag_strings[flag]);
			flag_count++;
		}
	}
	printk(BIOS_DEBUG, "\n");
}

/* Cache and retrieve the EC image type (ro or rw) */
enum ec_image google_chromeec_get_current_image(void)
{
	static enum ec_image ec_image_type = EC_IMAGE_UNKNOWN;

	if (ec_image_type != EC_IMAGE_UNKNOWN)
		return ec_image_type;

	struct ec_response_get_version resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_VERSION,
		.cmd_version = 0,
		.cmd_data_out = &resp,
		.cmd_size_in = 0,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	google_chromeec_command(&cmd);

	if (cmd.cmd_code) {
		printk(BIOS_DEBUG,
			"Google Chrome EC: version command failed!\n");
	} else {
		printk(BIOS_DEBUG, "Google Chrome EC: version:\n");
		printk(BIOS_DEBUG, "	ro: %s\n", resp.version_string_ro);
		printk(BIOS_DEBUG, "	rw: %s\n", resp.version_string_rw);
		printk(BIOS_DEBUG, "  running image: %d\n",
			resp.current_image);
		ec_image_type = resp.current_image;
	}

	/* Will still be UNKNOWN if command failed */
	return ec_image_type;
}

int google_chromeec_get_num_pd_ports(unsigned int *num_ports)
{
	struct ec_response_usb_pd_ports resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_USB_PD_PORTS,
		.cmd_version = 0,
		.cmd_data_out = &resp,
		.cmd_size_in = 0,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};
	int rv;

	rv = google_chromeec_command(&cmd);
	if (rv)
		return rv;

	*num_ports = resp.num_ports;
	return 0;
}

int google_chromeec_get_pd_port_caps(int port,
				struct usb_pd_port_caps *port_caps)
{
	struct ec_params_get_pd_port_caps params = {
		.port = port,
	};
	struct ec_response_get_pd_port_caps resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_PD_PORT_CAPS,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};
	int rv;

	rv = google_chromeec_command(&cmd);
	if (rv)
		return rv;

	port_caps->power_role_cap = resp.pd_power_role_cap;
	port_caps->try_power_role_cap = resp.pd_try_power_role_cap;
	port_caps->data_role_cap = resp.pd_data_role_cap;
	port_caps->port_location = resp.pd_port_location;

	return 0;
}

void google_chromeec_init(void)
{
	google_chromeec_log_uptimeinfo();
}

int google_ec_running_ro(void)
{
	return (google_chromeec_get_current_image() == EC_IMAGE_RO);
}

/* Returns data role and type of device connected */
static int google_chromeec_usb_pd_get_info(int port, bool *ufp, bool *dbg_acc,
				    bool *active_cable, uint8_t *dp_mode)
{
	struct ec_params_usb_pd_control pd_control = {
		.port = port,
		.role = USB_PD_CTRL_ROLE_NO_CHANGE,
		.mux = USB_PD_CTRL_ROLE_NO_CHANGE,
		.swap = USB_PD_CTRL_SWAP_NONE,
	};
	struct ec_response_usb_pd_control_v2 resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_USB_PD_CONTROL,
		.cmd_version = 2,
		.cmd_data_in = &pd_control,
		.cmd_size_in = sizeof(pd_control),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) < 0)
		return -1;

	*ufp = !(resp.role & PD_CTRL_RESP_ROLE_DATA);
	*dbg_acc = (resp.cc_state == PD_CC_DFP_DEBUG_ACC);
	*active_cable = !!(resp.control_flags & USB_PD_CTRL_ACTIVE_CABLE);
	*dp_mode = resp.dp_mode;

	return 0;
}

int google_chromeec_typec_control_enter_dp_mode(int port)
{
	if (!google_chromeec_check_feature(EC_FEATURE_TYPEC_REQUIRE_AP_MODE_ENTRY))
		return 0;

	struct ec_params_typec_control typec_control = {
		.port = port,
		.command = TYPEC_CONTROL_COMMAND_ENTER_MODE,
		.mode_to_enter = TYPEC_MODE_DP,
	};

	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_TYPEC_CONTROL,
		.cmd_version = 0,
		.cmd_data_in = &typec_control,
		.cmd_size_in = sizeof(typec_control),
		.cmd_data_out = NULL,
		.cmd_size_out = 0,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd) < 0)
		return -1;

	return 0;
}

/**
 * Check for the current mux state in EC. Flags representing the mux state found
 * in ec_commands.h
 */
int google_chromeec_usb_get_pd_mux_info(int port, uint8_t *flags)
{
	struct ec_params_usb_pd_mux_info req_mux = {
		.port = port,
	};
	struct ec_response_usb_pd_mux_info resp_mux = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_USB_PD_MUX_INFO,
		.cmd_version = 0,
		.cmd_data_in = &req_mux,
		.cmd_size_in = sizeof(req_mux),
		.cmd_data_out = &resp_mux,
		.cmd_size_out = sizeof(resp_mux),
		.cmd_dev_index = 0,
	};

	if (port < 0)
		return -1;

	if (google_chromeec_command(&cmd) < 0)
		return -1;

	*flags = resp_mux.flags;
	return 0;
}

/*
 * Obtain any USB-C mux data needed for the specified port
 * in: physical port number of the type-c port
 * out: struct usbc_mux_info mux_info stores USB-C mux data
 * Return: 0 on success, -1 on error
*/
int google_chromeec_get_usbc_mux_info(int port, struct usbc_mux_info *mux_info)
{
	uint8_t mux_flags;
	uint8_t dp_pin_mode;
	bool ufp, dbg_acc, active_cable;

	if (google_chromeec_usb_get_pd_mux_info(port, &mux_flags) < 0) {
		printk(BIOS_ERR, "Port C%d: get_pd_mux_info failed\n", port);
		return -1;
	}

	if (google_chromeec_usb_pd_get_info(port, &ufp, &dbg_acc,
					    &active_cable, &dp_pin_mode) < 0) {
		printk(BIOS_ERR, "Port C%d: pd_control failed\n", port);
		return -1;
	}

	mux_info->usb = !!(mux_flags & USB_PD_MUX_USB_ENABLED);
	mux_info->dp = !!(mux_flags & USB_PD_MUX_DP_ENABLED);
	mux_info->polarity = !!(mux_flags & USB_PD_MUX_POLARITY_INVERTED);
	mux_info->hpd_irq = !!(mux_flags & USB_PD_MUX_HPD_IRQ);
	mux_info->hpd_lvl = !!(mux_flags & USB_PD_MUX_HPD_LVL);
	mux_info->ufp = !!ufp;
	mux_info->dbg_acc = !!dbg_acc;
	mux_info->cable = !!active_cable;
	mux_info->dp_pin_mode = dp_pin_mode;

	return 0;
}

/**
 * Check if EC/TCPM is in an alternate mode or not.
 *
 * @param svid SVID of the alternate mode to check
 * @return	0: Not in the mode. -1: Error.
 *		>=1: bitmask of the ports that are in the mode.
 */
static int google_chromeec_pd_get_amode(uint16_t svid)
{
	struct ec_response_usb_pd_ports resp;
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_USB_PD_PORTS,
		.cmd_version = 0,
		.cmd_data_in = NULL,
		.cmd_size_in = 0,
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};
	int i;
	int ret = 0;

	if (google_chromeec_command(&cmd) < 0)
		return -1;

	for (i = 0; i < resp.num_ports; i++) {
		struct ec_params_usb_pd_get_mode_request params;
		struct ec_params_usb_pd_get_mode_response resp2;
		int svid_idx = 0;

		do {
			/* Reset cmd in each iteration in case
			   google_chromeec_command changes it. */
			params.port = i;
			params.svid_idx = svid_idx;
			cmd.cmd_code = EC_CMD_USB_PD_GET_AMODE;
			cmd.cmd_version = 0;
			cmd.cmd_data_in = &params;
			cmd.cmd_size_in = sizeof(params);
			cmd.cmd_data_out = &resp2;
			cmd.cmd_size_out = sizeof(resp2);
			cmd.cmd_dev_index = 0;

			if (google_chromeec_command(&cmd) < 0)
				return -1;
			if (resp2.svid == svid)
				ret |= BIT(i);
			svid_idx++;
		} while (resp2.svid);
	}

	return ret;
}

#define USB_SID_DISPLAYPORT 0xff01

/**
 * Wait for DisplayPort to be ready
 *
 * @param timeout_ms Wait aborts after <timeout_ms> ms.
 * @return	-1: Error. 0: Timeout.
 *		>=1: Bitmask of the ports that DP device is connected
 */
int google_chromeec_wait_for_displayport(long timeout_ms)
{
	struct stopwatch sw;
	int ret = 0;

	printk(BIOS_INFO, "Waiting for DisplayPort\n");
	stopwatch_init_msecs_expire(&sw, timeout_ms);
	while (1) {
		ret = google_chromeec_pd_get_amode(USB_SID_DISPLAYPORT);
		if (ret > 0)
			break;

		if (ret < 0) {
			printk(BIOS_ERR, "Can't get alternate mode!\n");
			return ret;
		}

		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING,
			       "DisplayPort not ready after %ldms. Abort.\n",
			       timeout_ms);
			return 0;
		}
		mdelay(200);
	}
	printk(BIOS_INFO, "DisplayPort ready after %lld ms\n",
	       stopwatch_duration_msecs(&sw));

	return ret;
}

int google_chromeec_wait_for_dp_hpd(int port, long timeout_ms)
{
	uint8_t mux_flags;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, timeout_ms);
	do {
		google_chromeec_usb_get_pd_mux_info(port, &mux_flags);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING, "HPD not ready after %ldms. Abort.\n", timeout_ms);
			return -1;
		}
		mdelay(100);
	} while (!(mux_flags & USB_PD_MUX_HPD_LVL) || !(mux_flags & USB_PD_MUX_DP_ENABLED));
	printk(BIOS_INFO, "HPD ready after %lld ms\n", stopwatch_duration_msecs(&sw));

	return 0;
}

int google_chromeec_get_keybd_config(struct ec_response_keybd_config *keybd)
{
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_GET_KEYBD_CONFIG,
		.cmd_version = 0,
		.cmd_data_in = NULL,
		.cmd_size_in = 0,
		.cmd_data_out = keybd,
		.cmd_size_out = sizeof(*keybd),
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd))
		return -1;

	return 0;
}

int google_chromeec_ap_reset(void)
{
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_AP_RESET,
		.cmd_version = 0,
		.cmd_data_in = NULL,
		.cmd_size_in = 0,
		.cmd_data_out = NULL,
		.cmd_size_out = 0,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd))
		return -1;

	return 0;
}

int google_chromeec_regulator_enable(uint32_t index, uint8_t enable)
{
	struct ec_params_regulator_enable params = {
		.index = index,
		.enable = enable,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_REGULATOR_ENABLE,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = NULL,
		.cmd_size_out = 0,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd))
		return -1;

	return 0;
}

int google_chromeec_regulator_is_enabled(uint32_t index, uint8_t *enabled)
{
	struct ec_params_regulator_is_enabled params = {
		.index = index,
	};
	struct ec_response_regulator_is_enabled resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_REGULATOR_IS_ENABLED,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd))
		return -1;

	*enabled = resp.enabled;

	return 0;
}

int google_chromeec_regulator_set_voltage(uint32_t index, uint32_t min_mv,
					  uint32_t max_mv)
{
	struct ec_params_regulator_set_voltage params = {
		.index = index,
		.min_mv = min_mv,
		.max_mv = max_mv,
	};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_REGULATOR_SET_VOLTAGE,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = NULL,
		.cmd_size_out = 0,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd))
		return -1;

	return 0;
}

int google_chromeec_regulator_get_voltage(uint32_t index, uint32_t *voltage_mv)
{
	struct ec_params_regulator_get_voltage params = {
		.index = index,
	};
	struct ec_response_regulator_get_voltage resp = {};
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_REGULATOR_GET_VOLTAGE,
		.cmd_version = 0,
		.cmd_data_in = &params,
		.cmd_size_in = sizeof(params),
		.cmd_data_out = &resp,
		.cmd_size_out = sizeof(resp),
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd))
		return -1;

	*voltage_mv = resp.voltage_mv;
	return 0;
}
