/* SPDX-License-Identifier: BSD-3-Clause */




#ifndef __CROS_EC_EC_CMD_API_H
#define __CROS_EC_EC_CMD_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This file consists of 3 sections corresponding to the different
 * methods used to determine the host command API function signature:
 *
 * 1. This section consists of functions that do not follow a simple
 *    pattern and need to be specified explicitly.
 *
 * 2. This section consists of functions that can be generated with the
 *    help of template macros.
 *
 * Note:
 *
 * A CROS_EC_COMMAND_INFO macro must be defined before including this
 * file. This is the data type holding the context info identifying the
 * EC performing the command.
 *
 * A CROS_EC_COMMAND macro must be defined before including this file
 * with the following signature:
 *
 * int CROS_EC_COMMAND(CROS_EC_COMMAND_INFO *h,
 *		       int command, int version,
 *		       const void *outdata, int outsize,
 *		       void *indata, int insize)
 *
 * This is the function implementing host command messaging with the EC.
 */

/*
 * Section 1: Functions that need to be implemented explicitly because
 * they do not adhere to simple naming that would permit
 * auto-generation.
 *
 * Please keep this list sorted by function name.
 */

static inline int ec_cmd_battery_config(CROS_EC_COMMAND_INFO *h, uint8_t *r)
{
	return CROS_EC_COMMAND(h, EC_CMD_BATTERY_CONFIG, 0, NULL, 0, r,
			       BATT_CONF_MAX_SIZE);
}

static inline int ec_cmd_get_sku_id(CROS_EC_COMMAND_INFO *h,
				    struct ec_sku_id_info *r)
{
	return CROS_EC_COMMAND(h, EC_CMD_GET_SKU_ID, 0, NULL, 0, r, sizeof(*r));
}

static inline int
ec_cmd_mkbp_info_get_next_data(CROS_EC_COMMAND_INFO *h,
			       const struct ec_params_mkbp_info *p,
			       union ec_response_get_next_data *r)
{
	return CROS_EC_COMMAND(h, EC_CMD_MKBP_INFO, 0, p, sizeof(*p), r,
			       sizeof(*r));
}

static inline int ec_cmd_set_sku_id(CROS_EC_COMMAND_INFO *h,
				    const struct ec_sku_id_info *p)
{
	return CROS_EC_COMMAND(h, EC_CMD_SET_SKU_ID, 0, p, sizeof(*p), NULL, 0);
}

static inline int ec_cmd_thermal_get_threshold_v1(
	CROS_EC_COMMAND_INFO *h,
	const struct ec_params_thermal_get_threshold_v1 *p,
	struct ec_thermal_config *r)
{
	return CROS_EC_COMMAND(h, EC_CMD_THERMAL_GET_THRESHOLD, 1, p,
			       sizeof(*p), r, sizeof(*r));
}

static inline int
ec_cmd_usb_pd_dev_info(CROS_EC_COMMAND_INFO *h,
		       const struct ec_params_usb_pd_info_request *p,
		       struct ec_params_usb_pd_rw_hash_entry *r)
{
	return CROS_EC_COMMAND(h, EC_CMD_USB_PD_DEV_INFO, 0, p, sizeof(*p), r,
			       sizeof(*r));
}

static inline int
ec_cmd_usb_pd_discovery(CROS_EC_COMMAND_INFO *h,
			const struct ec_params_usb_pd_info_request *p,
			struct ec_params_usb_pd_discovery_entry *r)
{
	return CROS_EC_COMMAND(h, EC_CMD_USB_PD_DISCOVERY, 0, p, sizeof(*p), r,
			       sizeof(*r));
}

static inline int
ec_cmd_usb_pd_set_amode(CROS_EC_COMMAND_INFO *h,
			const struct ec_params_usb_pd_set_mode_request *p)
{
	return CROS_EC_COMMAND(h, EC_CMD_USB_PD_SET_AMODE, 0, p, sizeof(*p),
			       NULL, 0);
}

static inline int ec_cmd_fp_frame(CROS_EC_COMMAND_INFO *h,
				  const struct ec_params_fp_frame *p,
				  uint8_t *r)
{
	return CROS_EC_COMMAND(h, EC_CMD_FP_FRAME, 0, p, sizeof(*p), r,
			       p->size);
}

static inline int ec_cmd_fp_template(CROS_EC_COMMAND_INFO *h,
				     const struct ec_params_fp_template *p,
				     int size)
{
	return CROS_EC_COMMAND(h, EC_CMD_FP_TEMPLATE, 0, p, size, NULL, 0);
}
/*
 * Section 2: EC interface functions that can be generated with the help
 * of template macros.
 *
 * _cmd    host command name
 * _v      host command version number
 * _fn     host command function name (will be prefixed with ec_cmd_)
 * _in     host command input (param) data type (will be prefixed with
 *         ec_params_)
 * _out    host command output (response) data type (will be prefixed with
 *         ec_response_)
 *
 * Template macros follow a naming scheme based on the arguments they
 * require:
 *
 * C0      host command version 0, does not need to be specified (most
 *         common/default case)
 * CV      host command with specified version
 *         one of C0, CV is required
 * F       function name generated (the lower case version of the _cmd arg)
 *         (required)
 * PF      parameter type derived from function name (_fn arg)
 * P       parameter type specified explicitly (i.e. can not be derived from
 *         _fn arg)
 *         PF, P are mutually exclusive and optional
 * RF      response type derived from function name (_fn arg)
 * R       response type specified explicitly (i.e. can not be derived from
 *         _fn arg)
 *         RF, R are mutually exclusive and optional
 */

/*
 * Template for EC interface functions that take a param and returns a
 * response.
 */
#define _CROS_EC_CV_F_P_R(_cmd, _v, _fn, _in, _out)                       \
	static inline int ec_cmd_##_fn(CROS_EC_COMMAND_INFO *h,           \
				       const struct ec_params_##_in *p,   \
				       struct ec_response_##_out *r)      \
	{                                                                 \
		return CROS_EC_COMMAND(h, (_cmd), (_v), p, sizeof(*p), r, \
				       sizeof(*r));                       \
	}

/*
 * Template for EC interface functions that take a param but do not
 * return a response.
 */
#define _CROS_EC_CV_F_P(_cmd, _v, _fn, _in)                                  \
	static inline int ec_cmd_##_fn(CROS_EC_COMMAND_INFO *h,              \
				       const struct ec_params_##_in *p)      \
	{                                                                    \
		return CROS_EC_COMMAND(h, (_cmd), (_v), p, sizeof(*p), NULL, \
				       0);                                   \
	}

/*
 * Template for EC interface functions that do not take a param but do
 * return a response.
 */
#define _CROS_EC_CV_F_R(_cmd, _v, _fn, _out)                         \
	static inline int ec_cmd_##_fn(CROS_EC_COMMAND_INFO *h,      \
				       struct ec_response_##_out *r) \
	{                                                            \
		return CROS_EC_COMMAND(h, (_cmd), (_v), NULL, 0, r,  \
				       sizeof(*r));                  \
	}

/*
 * Template for EC interface functions that do not take a param and do
 * not return a response.
 */
#define _CROS_EC_CV_F(_cmd, _v, _fn)                                       \
	static inline int ec_cmd_##_fn(CROS_EC_COMMAND_INFO *h)            \
	{                                                                  \
		return CROS_EC_COMMAND(h, (_cmd), (_v), NULL, 0, NULL, 0); \
	}

/*
 * Shorthand for host command version 0 where param and response name is
 * derived from the function name.
 */
#define _CROS_EC_C0_F_PF_RF(_cmd, _fn) _CROS_EC_CV_F_P_R(_cmd, 0, _fn, _fn, _fn)

/*
 * Shorthand for host command version 1 where param and response name is
 * derived from the function name.
 */
#define _CROS_EC_C1_F_PF_RF(_cmd, _fn) \
	_CROS_EC_CV_F_P_R(_cmd, 1, _fn##_v1, _fn##_v1, _fn##_v1)

/*
 * Shorthand for host command version 0 where param name is
 * derived from the function name and there is no response.
 */
#define _CROS_EC_C0_F_PF(_cmd, _fn) _CROS_EC_CV_F_P(_cmd, 0, _fn, _fn)

/*
 * Shorthand for host command version 1 where param name is
 * derived from the function name and there is no response.
 */
#define _CROS_EC_C1_F_PF(_cmd, _fn) _CROS_EC_CV_F_P(_cmd, 1, _fn##_v1, _fn##_v1)

/*
 * Shorthand for host command version 0 where response name is derived
 * from the function name and there is no param.
 */
#define _CROS_EC_C0_F_RF(_cmd, _fn) _CROS_EC_CV_F_R(_cmd, 0, _fn, _fn)

/*
 * Shorthand for host command version 1 where response name is derived
 * from the function name and there is no param.
 */
#define _CROS_EC_C1_F_RF(_cmd, _fn) _CROS_EC_CV_F_R(_cmd, 1, _fn##_v1, _fn##_v1)

/*
 * Shorthand for host command version 3 where response name is derived
 * from the function name and there is no param.
 */
#define _CROS_EC_C3_F_RF(_cmd, _fn) _CROS_EC_CV_F_R(_cmd, 3, _fn##_v3, _fn##_v3)

/*
 * Shorthand for host command version 0 where response and there are no
 * params or response.
 */
#define _CROS_EC_C0_F(_cmd, _fn) _CROS_EC_CV_F(_cmd, 0, _fn)

/*
 * Please keep this list sorted by host command. Sort with:
 *
 * clang-format '--style={BasedOnStyle: InheritParentConfig,
 * ColumnLimit: 888 }' include/ec_cmd_api.h | grep '^_CROS' |
 * LC_COLLATE=C sort -t '(' -k 2,2 | clang-format
 */

_CROS_EC_C0_F_PF_RF(EC_CMD_ADC_READ, adc_read);
_CROS_EC_CV_F_P(EC_CMD_ADD_ENTROPY, 0, add_entropy, rollback_add_entropy);
_CROS_EC_C0_F_PF(EC_CMD_AP_FW_STATE, ap_fw_state);
_CROS_EC_C0_F(EC_CMD_AP_RESET, ap_reset);
_CROS_EC_CV_F_P(EC_CMD_BATTERY_CUT_OFF, 1, battery_cut_off_v1, battery_cutoff);
_CROS_EC_C0_F(EC_CMD_BATTERY_CUT_OFF, battery_cut_off);
_CROS_EC_CV_F_P_R(EC_CMD_BATTERY_GET_DYNAMIC, 0, battery_get_dynamic,
		  battery_dynamic_info, battery_dynamic_info);
_CROS_EC_CV_F_P_R(EC_CMD_BATTERY_GET_STATIC, 0, battery_get_static,
		  battery_static_info, battery_static_info);
_CROS_EC_CV_F_P_R(EC_CMD_BATTERY_GET_STATIC, 1, battery_get_static_v1,
		  battery_static_info, battery_static_info_v1);
_CROS_EC_CV_F_P_R(EC_CMD_BATTERY_GET_STATIC, 2, battery_get_static_v2,
		  battery_static_info, battery_static_info_v2);
_CROS_EC_C0_F_PF_RF(EC_CMD_BATTERY_VENDOR_PARAM, battery_vendor_param);
_CROS_EC_C0_F_PF(EC_CMD_BUTTON, button);
_CROS_EC_C0_F_PF_RF(EC_CMD_CEC_GET, cec_get);
_CROS_EC_C0_F_PF(EC_CMD_CEC_SET, cec_set);
_CROS_EC_C1_F_PF(EC_CMD_CEC_WRITE_MSG, cec_write);
_CROS_EC_C0_F_PF_RF(EC_CMD_CEC_READ_MSG, cec_read);
_CROS_EC_C0_F_RF(EC_CMD_CEC_PORT_COUNT, cec_port_count);
_CROS_EC_C0_F_PF_RF(EC_CMD_CHARGESPLASH, chargesplash);
_CROS_EC_CV_F_P_R(EC_CMD_CHARGE_CONTROL, 2, charge_control_v2, charge_control,
		  charge_control);
_CROS_EC_CV_F_P(EC_CMD_CHARGE_CURRENT_LIMIT, 0, charge_current_limit,
		current_limit);
_CROS_EC_C0_F_RF(EC_CMD_CHARGE_PORT_COUNT, charge_port_count);
_CROS_EC_C0_F_PF_RF(EC_CMD_CHARGE_STATE, charge_state);
_CROS_EC_C0_F_PF(EC_CMD_CONFIG_POWER_BUTTON, config_power_button);
_CROS_EC_C0_F(EC_CMD_CONSOLE_SNAPSHOT, console_snapshot);
_CROS_EC_C0_F_PF_RF(EC_CMD_DEVICE_EVENT, device_event);
_CROS_EC_C0_F_RF(EC_CMD_DISPLAY_SOC, display_soc);
_CROS_EC_C0_F_PF(EC_CMD_EFS_VERIFY, efs_verify);
_CROS_EC_C1_F_PF(EC_CMD_EXTERNAL_POWER_LIMIT, external_power_limit);
_CROS_EC_C0_F_PF(EC_CMD_FLASH_ERASE, flash_erase);
_CROS_EC_CV_F_R(EC_CMD_FLASH_INFO, 1, flash_info_v1, flash_info_1);
_CROS_EC_C0_F_RF(EC_CMD_FLASH_INFO, flash_info);
_CROS_EC_CV_F_P_R(EC_CMD_FLASH_PROTECT, 1, flash_protect_v1, flash_protect,
		  flash_protect);
_CROS_EC_C0_F_PF_RF(EC_CMD_FLASH_PROTECT, flash_protect);
_CROS_EC_CV_F_P_R(EC_CMD_FLASH_REGION_INFO, 1, flash_region_info_v1,
		  flash_region_info, flash_region_info);
_CROS_EC_C0_F_RF(EC_CMD_FLASH_SPI_INFO, flash_spi_info);
_CROS_EC_C0_F_PF(EC_CMD_FORCE_LID_OPEN, force_lid_open);
_CROS_EC_C0_F_PF(EC_CMD_FP_SEED, fp_seed);
_CROS_EC_C0_F_PF_RF(EC_CMD_FP_MODE, fp_mode);
_CROS_EC_C0_F_PF_RF(EC_CMD_FP_READ_MATCH_SECRET, fp_read_match_secret);
_CROS_EC_C0_F_RF(EC_CMD_FP_ENC_STATUS, fp_encryption_status);
_CROS_EC_C0_F_RF(EC_CMD_FP_STATS, fp_stats);
_CROS_EC_C1_F_PF(EC_CMD_FP_CONTEXT, fp_context);
_CROS_EC_CV_F_R(EC_CMD_FP_INFO, 1, fp_info, fp_info);
_CROS_EC_CV_F_R(EC_CMD_GET_BOARD_VERSION, 0, get_board_version, board_version);
_CROS_EC_C0_F_RF(EC_CMD_GET_BOOT_TIME, get_boot_time);
_CROS_EC_C0_F_RF(EC_CMD_GET_CHIP_INFO, get_chip_info);
_CROS_EC_CV_F_P_R(EC_CMD_GET_CMD_VERSIONS, 1, get_cmd_versions_v1,
		  get_cmd_versions_v1, get_cmd_versions);
_CROS_EC_C0_F_PF_RF(EC_CMD_GET_CMD_VERSIONS, get_cmd_versions);
_CROS_EC_C0_F_RF(EC_CMD_GET_COMMS_STATUS, get_comms_status);
_CROS_EC_C0_F_RF(EC_CMD_GET_FEATURES, get_features);
_CROS_EC_CV_F_R(EC_CMD_GET_KEYBD_CONFIG, 0, get_keybd_config, keybd_config);
_CROS_EC_C0_F_RF(EC_CMD_GET_NEXT_EVENT, get_next_event);
_CROS_EC_C1_F_RF(EC_CMD_GET_NEXT_EVENT, get_next_event);
_CROS_EC_C3_F_RF(EC_CMD_GET_NEXT_EVENT, get_next_event);
_CROS_EC_CV_F_R(EC_CMD_GET_NEXT_EVENT, 2, get_next_event_v2, get_next_event_v1);
_CROS_EC_C0_F_PF_RF(EC_CMD_GET_PD_PORT_CAPS, get_pd_port_caps);
_CROS_EC_C0_F_RF(EC_CMD_GET_PROTOCOL_INFO, get_protocol_info);
_CROS_EC_CV_F_R(EC_CMD_GET_UPTIME_INFO, 0, get_uptime_info, uptime_info);
_CROS_EC_C0_F_RF(EC_CMD_GET_VERSION, get_version);
_CROS_EC_C1_F_RF(EC_CMD_GET_VERSION, get_version);
_CROS_EC_C0_F_PF_RF(EC_CMD_GPIO_GET, gpio_get);
_CROS_EC_C1_F_PF_RF(EC_CMD_GPIO_GET, gpio_get);
_CROS_EC_C0_F_PF(EC_CMD_GPIO_SET, gpio_set);
_CROS_EC_CV_F_P_R(EC_CMD_GSV_PAUSE_IN_S5, 0, gsv_pause_in_s5, get_set_value,
		  get_set_value);
_CROS_EC_C0_F_PF_RF(EC_CMD_HANG_DETECT, hang_detect);
_CROS_EC_C0_F_PF_RF(EC_CMD_HELLO, hello);
_CROS_EC_C0_F_PF_RF(EC_CMD_HIBERNATION_DELAY, hibernation_delay);
_CROS_EC_C0_F_PF_RF(EC_CMD_HOST_EVENT, host_event);
_CROS_EC_CV_F_P(EC_CMD_HOST_EVENT_CLEAR, 0, host_event_clear, host_event_mask);
_CROS_EC_CV_F_P(EC_CMD_HOST_EVENT_CLEAR_B, 0, host_event_clear_b,
		host_event_mask);
_CROS_EC_CV_F_R(EC_CMD_HOST_EVENT_GET_B, 0, host_event_get_b, host_event_mask);
_CROS_EC_CV_F_R(EC_CMD_HOST_EVENT_GET_SCI_MASK, 0, host_event_get_sci_mask,
		host_event_mask);
_CROS_EC_CV_F_R(EC_CMD_HOST_EVENT_GET_SMI_MASK, 0, host_event_get_smi_mask,
		host_event_mask);
_CROS_EC_CV_F_R(EC_CMD_HOST_EVENT_GET_WAKE_MASK, 0, host_event_get_wake_mask,
		host_event_mask);
_CROS_EC_CV_F_P(EC_CMD_HOST_EVENT_SET_SCI_MASK, 0, host_event_set_sci_mask,
		host_event_mask);
_CROS_EC_CV_F_P(EC_CMD_HOST_EVENT_SET_SMI_MASK, 0, host_event_set_smi_mask,
		host_event_mask);
_CROS_EC_CV_F_P(EC_CMD_HOST_EVENT_SET_WAKE_MASK, 0, host_event_set_wake_mask,
		host_event_mask);
_CROS_EC_C0_F_PF(EC_CMD_HOST_SLEEP_EVENT, host_sleep_event);
_CROS_EC_C1_F_PF_RF(EC_CMD_HOST_SLEEP_EVENT, host_sleep_event);
_CROS_EC_C0_F_PF_RF(EC_CMD_I2C_CONTROL, i2c_control);
_CROS_EC_C0_F_PF_RF(EC_CMD_I2C_PASSTHRU_PROTECT, i2c_passthru_protect);
_CROS_EC_C0_F_RF(EC_CMD_KEYBOARD_FACTORY_TEST, keyboard_factory_test);
_CROS_EC_CV_F_P_R(EC_CMD_LED_CONTROL, 1, led_control_v1, led_control,
		  led_control);
_CROS_EC_C0_F_PF_RF(EC_CMD_LOCATE_CHIP, locate_chip);
_CROS_EC_C0_F_RF(EC_CMD_MKBP_GET_CONFIG, mkbp_get_config);
_CROS_EC_C0_F_PF_RF(EC_CMD_MKBP_INFO, mkbp_info);
_CROS_EC_C0_F_PF(EC_CMD_MKBP_SET_CONFIG, mkbp_set_config);
_CROS_EC_C0_F_PF(EC_CMD_MKBP_SIMULATE_KEY, mkbp_simulate_key);
_CROS_EC_CV_F_P_R(EC_CMD_MKBP_WAKE_MASK, 0, mkbp_wake_mask,
		  mkbp_event_wake_mask, mkbp_event_wake_mask);
_CROS_EC_CV_F_P_R(EC_CMD_MOTION_SENSE_CMD, 1, motion_sense_cmd_v1, motion_sense,
		  motion_sense);
_CROS_EC_CV_F_P_R(EC_CMD_MOTION_SENSE_CMD, 2, motion_sense_cmd_v2, motion_sense,
		  motion_sense);
_CROS_EC_CV_F_P_R(EC_CMD_MOTION_SENSE_CMD, 4, motion_sense_cmd_v4, motion_sense,
		  motion_sense);
_CROS_EC_CV_F_P(EC_CMD_OVERRIDE_DEDICATED_CHARGER_LIMIT, 0,
		override_dedicated_charger_limit, dedicated_charger_limit);
_CROS_EC_C0_F_RF(EC_CMD_PCHG_COUNT, pchg_count);
_CROS_EC_C0_F_PF_RF(EC_CMD_PDC_TRACE_MSG_ENABLE, pdc_trace_msg_enable);
_CROS_EC_C0_F_RF(EC_CMD_PDC_TRACE_MSG_GET_ENTRIES, pdc_trace_msg_get_entries);
_CROS_EC_CV_F_P(EC_CMD_PD_CHARGE_PORT_OVERRIDE, 0, pd_charge_port_override,
		charge_port_override);
_CROS_EC_CV_F_P_R(EC_CMD_PD_CHIP_INFO, 3, pd_chip_info_v3, pd_chip_info,
		  pd_chip_info_v3);
_CROS_EC_CV_F_P_R(EC_CMD_PD_CHIP_INFO, 2, pd_chip_info_v2, pd_chip_info,
		  pd_chip_info_v2);
_CROS_EC_CV_F_P_R(EC_CMD_PD_CHIP_INFO, 1, pd_chip_info_v1, pd_chip_info,
		  pd_chip_info_v1);
_CROS_EC_C0_F_PF_RF(EC_CMD_PD_CHIP_INFO, pd_chip_info);
_CROS_EC_C0_F_PF(EC_CMD_PD_CONTROL, pd_control);
_CROS_EC_CV_F_R(EC_CMD_PD_HOST_EVENT_STATUS, 0, pd_host_event_status,
		host_event_status);
_CROS_EC_C0_F_PF(EC_CMD_PD_WRITE_LOG_ENTRY, pd_write_log_entry);
_CROS_EC_C0_F_RF(EC_CMD_PORT80_LAST_BOOT, port80_last_boot);
_CROS_EC_C1_F_RF(EC_CMD_POWER_INFO, power_info);
_CROS_EC_CV_F_P_R(EC_CMD_PSE, 0, pse, pse, pse_status);
_CROS_EC_C0_F_RF(EC_CMD_PSTORE_INFO, pstore_info);
_CROS_EC_C0_F_PF(EC_CMD_PSTORE_WRITE, pstore_write);
_CROS_EC_C0_F_PF_RF(EC_CMD_PWM_GET_DUTY, pwm_get_duty);
_CROS_EC_C0_F_RF(EC_CMD_PWM_GET_KEYBOARD_BACKLIGHT, pwm_get_keyboard_backlight);
_CROS_EC_C0_F_PF(EC_CMD_PWM_SET_DUTY, pwm_set_duty);
_CROS_EC_C0_F_PF(EC_CMD_PWM_SET_FAN_DUTY, pwm_set_fan_duty_v0);
_CROS_EC_C0_F_PF(EC_CMD_PWM_SET_KEYBOARD_BACKLIGHT, pwm_set_keyboard_backlight);
_CROS_EC_C0_F_PF_RF(EC_CMD_RAND_NUM, rand_num);
_CROS_EC_C0_F(EC_CMD_REBOOT, reboot);
_CROS_EC_C0_F(EC_CMD_REBOOT_AP_ON_G3, reboot_ap_on_g3);
_CROS_EC_C1_F_PF(EC_CMD_REBOOT_AP_ON_G3, reboot_ap_on_g3);
_CROS_EC_C0_F_PF(EC_CMD_REBOOT_EC, reboot_ec);
_CROS_EC_C0_F_PF(EC_CMD_REGULATOR_ENABLE, regulator_enable);
_CROS_EC_C0_F_PF_RF(EC_CMD_REGULATOR_GET_VOLTAGE, regulator_get_voltage);
_CROS_EC_C0_F_PF_RF(EC_CMD_REGULATOR_IS_ENABLED, regulator_is_enabled);
_CROS_EC_C0_F_PF(EC_CMD_REGULATOR_SET_VOLTAGE, regulator_set_voltage);
_CROS_EC_C0_F_PF_RF(EC_CMD_RGBKBD, rgbkbd);
_CROS_EC_C0_F_RF(EC_CMD_ROLLBACK_INFO, rollback_info);
_CROS_EC_CV_F_R(EC_CMD_RTC_GET_ALARM, 0, rtc_get_alarm, rtc);
_CROS_EC_CV_F_R(EC_CMD_RTC_GET_VALUE, 0, rtc_get_value, rtc);
_CROS_EC_CV_F_P(EC_CMD_RTC_SET_ALARM, 0, rtc_set_alarm, rtc);
_CROS_EC_CV_F_P(EC_CMD_RTC_SET_VALUE, 0, rtc_set_value, rtc);
_CROS_EC_C0_F_PF(EC_CMD_RWSIG_ACTION, rwsig_action);
_CROS_EC_C0_F_RF(EC_CMD_RWSIG_CHECK_STATUS, rwsig_check_status);
_CROS_EC_C0_F_RF(EC_CMD_RWSIG_INFO, rwsig_info);
_CROS_EC_C0_F_PF(EC_CMD_SET_ALARM_SLP_S0_DBG, set_alarm_slp_s0_dbg);
_CROS_EC_C0_F_PF(EC_CMD_SET_BASE_STATE, set_base_state);
_CROS_EC_C0_F_PF(EC_CMD_SET_TABLET_MODE, set_tablet_mode);
_CROS_EC_C0_F_PF_RF(EC_CMD_SMART_DISCHARGE, smart_discharge);
_CROS_EC_CV_F_P(EC_CMD_SWITCH_ENABLE_BKLIGHT, 0, switch_enable_bklight,
		switch_enable_backlight);
_CROS_EC_CV_F_P(EC_CMD_SWITCH_ENABLE_WIRELESS, 0, switch_enable_wireless,
		switch_enable_wireless_v0);
_CROS_EC_C1_F_PF_RF(EC_CMD_SWITCH_ENABLE_WIRELESS, switch_enable_wireless);
_CROS_EC_C0_F_RF(EC_CMD_SYSINFO, sysinfo);
_CROS_EC_C0_F_PF_RF(EC_CMD_TEMP_SENSOR_GET_INFO, temp_sensor_get_info);
_CROS_EC_C0_F_PF_RF(EC_CMD_TEST_PROTOCOL, test_protocol);
_CROS_EC_C0_F(EC_CMD_THERMAL_AUTO_FAN_CTRL, thermal_auto_fan_ctrl);
_CROS_EC_C0_F_PF_RF(EC_CMD_THERMAL_GET_THRESHOLD, thermal_get_threshold);
_CROS_EC_C0_F_PF(EC_CMD_THERMAL_SET_THRESHOLD, thermal_set_threshold);
_CROS_EC_C1_F_PF(EC_CMD_THERMAL_SET_THRESHOLD, thermal_set_threshold);
_CROS_EC_C0_F_PF_RF(EC_CMD_TMP006_GET_RAW, tmp006_get_raw);
_CROS_EC_C0_F_PF(EC_CMD_TYPEC_CONTROL, typec_control);
_CROS_EC_C0_F_PF_RF(EC_CMD_TYPEC_STATUS, typec_status);
_CROS_EC_C0_F_PF_RF(EC_CMD_TYPEC_VDM_RESPONSE, typec_vdm_response);
_CROS_EC_C0_F_PF(EC_CMD_USB_CHARGE_SET_MODE, usb_charge_set_mode);
_CROS_EC_C0_F_PF(EC_CMD_USB_MUX, usb_mux);
_CROS_EC_CV_F_P_R(EC_CMD_USB_PD_CONTROL, 2, usb_pd_control_v2, usb_pd_control,
		  usb_pd_control_v2);
_CROS_EC_C0_F_PF_RF(EC_CMD_USB_PD_CONTROL, usb_pd_control);
_CROS_EC_C0_F_PF(EC_CMD_USB_PD_DPS_CONTROL, usb_pd_dps_control);
_CROS_EC_C0_F_PF(EC_CMD_USB_PD_MUX_ACK, usb_pd_mux_ack);
_CROS_EC_C0_F_PF_RF(EC_CMD_USB_PD_MUX_INFO, usb_pd_mux_info);
_CROS_EC_C0_F_RF(EC_CMD_USB_PD_PORTS, usb_pd_ports);
_CROS_EC_C0_F_PF_RF(EC_CMD_USB_PD_POWER_INFO, usb_pd_power_info);
_CROS_EC_C0_F_PF(EC_CMD_USB_PD_RW_HASH_ENTRY, usb_pd_rw_hash_entry);
_CROS_EC_C0_F_PF_RF(EC_CMD_VBOOT_HASH, vboot_hash);
_CROS_EC_C0_F_PF_RF(EC_CMD_VSTORE_READ, vstore_read);
_CROS_EC_C0_F_PF(EC_CMD_VSTORE_WRITE, vstore_write);
_CROS_EC_C0_F_PF(EC_CMD_UCSI_PPM_SET, ucsi_ppm_set);
_CROS_EC_C0_F_PF(EC_CMD_UCSI_PPM_GET, ucsi_ppm_get);

#ifdef __cplusplus
}
#endif

#endif /* __CROS_EC_EC_CMD_API_H */
