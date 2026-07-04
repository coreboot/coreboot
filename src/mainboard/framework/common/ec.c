/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <console/console.h>
#include <ec/ec.h>
#include <ec/google/chromeec/ec.h>
#include <mainboard/framework/common/board_host_command.h>
#include <mainboard/framework/common/ec.h>
#include <option.h>

/* Send a Framework EC host command (version 0) that does not expect a response */
static int framework_ec_send(uint16_t cmd_code, const void *data, size_t size)
{
	struct chromeec_command cmd = {
		.cmd_code	= cmd_code,
		.cmd_version	= 0,
		.cmd_data_in	= data,
		.cmd_size_in	= size,
		.cmd_data_out	= NULL,
		.cmd_size_out	= 0,
		.cmd_dev_index	= 0,
	};

	return google_chromeec_command(&cmd);
}

static void framework_set_ps2_emulation(void)
{
	const bool enable = get_uint_option(PS2_EMULATION_OPTION_NAME, 1);
	const struct ec_params_ps2_emulation_control params = {
		.disable = !enable,
	};

	if (framework_ec_send(EC_CMD_DISABLE_PS2_EMULATION, &params, sizeof(params)))
		printk(BIOS_ERR, "Failed to %s PS/2 emulation\n",
		       enable ? "enable" : "disable");
}

/*
 * Signal to the EC that BIOS has finished POST.
 */
#define EC_CMD_DIAGNOSIS	0x3E0B
#define EC_DIAGNOSIS_PORT80_COMPLETE	0xFF

static void framework_set_standalone_mode(void)
{
	const bool enable = get_uint_option(STANDALONE_MODE_OPTION_NAME, 0);
	const struct ec_params_standalone_mode params = {
		.enable = enable,
	};

	if (framework_ec_send(EC_CMD_STANDALONE_MODE, &params, sizeof(params)))
		printk(BIOS_ERR, "Failed to %s standalone mode\n",
		       enable ? "enable" : "disable");
}

static void framework_set_fp_led_level(void)
{
	const unsigned int level = get_uint_option(FP_LED_LEVEL_OPTION_NAME,
						   FP_LED_LEVEL_EC_DEFAULT);

	/* Leave the EC at whatever it has stored. */
	if (level == FP_LED_LEVEL_EC_DEFAULT)
		return;

	const struct ec_params_fp_led_control_v0 params = {
		.set_led_level = (uint8_t)level,
		.get_led_level = 0,
	};

	if (framework_ec_send(EC_CMD_FP_LED_LEVEL_CONTROL, &params, sizeof(params)))
		printk(BIOS_ERR, "Failed to set fingerprint LED level to %u\n", level);
}

static void framework_set_input_deck_mode(void)
{
	const struct ec_params_deck_state params = {
		.mode = get_uint_option(INPUT_DECK_MODE_OPTION_NAME, INPUT_DECK_MODE_AUTO),
	};
	struct ec_response_deck_state resp;
	struct chromeec_command cmd = {
		.cmd_code	= EC_CMD_CHECK_DECK_STATE,
		.cmd_version	= 0,
		.cmd_data_in	= &params,
		.cmd_size_in	= sizeof(params),
		.cmd_data_out	= &resp,
		.cmd_size_out	= sizeof(resp),
		.cmd_dev_index	= 0,
	};

	if (google_chromeec_command(&cmd))
		printk(BIOS_ERR, "Failed to set input deck mode\n");
}

static void framework_set_stylus_protocol(void)
{
	const unsigned int proto = get_uint_option(STYLUS_PROTOCOL_OPTION_NAME,
						   STYLUS_PROTOCOL_MPP);
	struct ec_params_gpio_set params = {
		.val = (proto == STYLUS_PROTOCOL_USI) ? 0 : 1,
	};

	strncpy(params.name, STYLUS_PROTOCOL_GPIO_NAME, sizeof(params.name));

	if (framework_ec_send(EC_CMD_GPIO_SET, &params, sizeof(params)))
		printk(BIOS_ERR, "Failed to set stylus protocol GPIO\n");
}

static void framework_set_battery_charge_limit(void)
{
	const unsigned int limit = get_uint_option(BATTERY_CHARGE_LIMIT_OPTION_NAME,
						   BATTERY_CHARGE_LIMIT_EC_DEFAULT);
	struct ec_params_ec_chg_limit_control params;

	/* Leave the EC at whatever it has stored, e.g. a limit set at runtime via the OS. */
	if (limit == BATTERY_CHARGE_LIMIT_EC_DEFAULT)
		return;

	if (limit >= 100) {
		/* No limit: let the charge manager charge the battery to full. */
		params = (struct ec_params_ec_chg_limit_control) {
			.modes = CHG_LIMIT_DISABLE,
		};
	} else {
		/* Hold the charge at the limit: stop and resume at the same level. */
		params = (struct ec_params_ec_chg_limit_control) {
			.modes		= CHG_LIMIT_SET_LIMIT,
			.max_percentage	= (uint8_t)limit,
			.min_percentage	= (uint8_t)limit,
		};
	}

	if (framework_ec_send(EC_CMD_CHARGE_LIMIT_CONTROL, &params, sizeof(params)))
		printk(BIOS_ERR, "Failed to set battery charge limit to %u%%\n", limit);
}

void mainboard_ec_init(void)
{
	static const struct google_chromeec_event_info info = {
		.log_events = MAINBOARD_EC_LOG_EVENTS,
		.sci_events = MAINBOARD_EC_SCI_EVENTS,
		.s3_wake_events = MAINBOARD_EC_S3_WAKE_EVENTS,
		.s5_wake_events = MAINBOARD_EC_S5_WAKE_EVENTS,
		.s0ix_wake_events = MAINBOARD_EC_S0IX_WAKE_EVENTS,
	};

	printk(BIOS_DEBUG, "mainboard: EC init\n");

	google_chromeec_events_init(&info, acpi_is_wakeup_s3());

	/* Apply CFR-configured EC settings on every boot */
	framework_set_ps2_emulation();
	framework_set_standalone_mode();
	framework_set_fp_led_level();
	framework_set_battery_charge_limit();
	if (CONFIG(FRAMEWORK_TOUCHSCREEN_STYLUS))
		framework_set_stylus_protocol();
	if (CONFIG(FRAMEWORK_INPUT_DECK))
		framework_set_input_deck_mode();
}

static void framework_ec_signal_bios_complete(void *unused)
{
	const uint8_t diagnosis_code = EC_DIAGNOSIS_PORT80_COMPLETE;
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_DIAGNOSIS,
		.cmd_version = 0,
		.cmd_data_in = &diagnosis_code,
		.cmd_size_in = sizeof(diagnosis_code),
		.cmd_data_out = NULL,
		.cmd_size_out = 0,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd))
		printk(BIOS_ERR, "mainboard: Failed to signal BIOS complete to EC\n");
	else
		printk(BIOS_DEBUG, "mainboard: Signalled BIOS complete to EC\n");
}
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT,
		      framework_ec_signal_bios_complete, NULL);
