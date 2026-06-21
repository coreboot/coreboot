/* SPDX-License-Identifier: BSD-3-Clause */

/* Host communication command constants for
 * Framework Computer fork of Chrome EC
 *
 * See EC at zephyr/program/framework/include/board_host_command.h
 */

#ifndef __CROS_EC_FWK_EC_COMMANDS_H
#define __CROS_EC_FWK_EC_COMMANDS_H

#include <ec/google/chromeec/ec_commands.h>

#ifndef __ACPI__

/*****************************************************************************/
/*
 * Configure the behavior of the flash notify
 */
#define EC_CMD_FLASH_NOTIFIED		0x3E01

enum ec_flash_notified_flags {
	/* Enable/Disable power button pulses for x86 devices */
	FLASH_ACCESS_SPI	= 0,
	FLASH_FIRMWARE_START	= 1,
	FLASH_FIRMWARE_DONE	= 2,
	FLASH_ACCESS_SPI_DONE	= 3,
	FLASH_FLAG_PD		= BIT(4),
};

struct ec_params_flash_notified {
	/* See enum ec_flash_notified_flags */
	uint8_t flags;
} __ec_align1;

/*****************************************************************************/
/*
 * Factory will need change Fnkey and power button
 * key scancode to test keyboard.
 */
#define EC_CMD_FACTORY_MODE	0x3E02
#define RESET_FOR_SHIP 0x5A

struct ec_params_factory_notified {
	/* factory mode enable */
	uint8_t flags;
} __ec_align1;

/*****************************************************************************/
/*
 * Configure the behavior of the charge limit control.
 */
#define EC_CMD_CHARGE_LIMIT_CONTROL	0x3E03
#define EC_CHARGE_LIMIT_RESTORE		0x7F

enum ec_chg_limit_control_modes {
	/* Disable all setting, charge control by charge_manage */
	CHG_LIMIT_DISABLE	= BIT(0),
	/* Set maximum and minimum percentage */
	CHG_LIMIT_SET_LIMIT	= BIT(1),
	/* Host read current setting */
	CHG_LIMIT_GET_LIMIT	= BIT(3),
	/* Enable override mode, allow charge to full this time */
	CHG_LIMIT_OVERRIDE	= BIT(7),
};

struct ec_params_ec_chg_limit_control {
	/* See enum ec_chg_limit_control_modes */
	uint8_t modes;
	uint8_t max_percentage;
	uint8_t min_percentage;
} __ec_align1;

struct ec_response_chg_limit_control {
	uint8_t max_percentage;
	uint8_t min_percentage;
} __ec_align1;

/*****************************************************************************/
/*
 * Get actual fan RPM, not target
 */

#define EC_CMD_PWM_GET_FAN_ACTUAL_RPM	0x3E04

struct ec_params_ec_pwm_get_actual_fan_rpm {
	/* The index of the fan */
	uint8_t index;
} __ec_align1;

struct ec_response_pwm_get_actual_fan_rpm {
	uint16_t rpm;
} __ec_align2;


/*****************************************************************************/
/*
 * This command only be used for the cold-boot stress test via the ODM customized tool.
 *
 * When running the cold-boot stress test, the tool sends the HC:EC_CMD_SET_AP_REBOOT_DELAY
 * to notify the EC to keep the PCH power "delay" seconds.
 * It also sends the HC:EC_CMD_REBOOT_AP_ON_G3 to set the flag to reboot AP on G3.
 */
#define EC_CMD_SET_AP_REBOOT_DELAY	0x3E05

extern int ap_boot_delay;
extern int stress_test_enable;

struct ec_response_ap_reboot_delay {
	/* range: 1 second ~ 180 seconds */
	uint8_t delay;
} __ec_align1;

/*****************************************************************************/
/*
 * This command uses to control the ME enable status.
 */
#define EC_CMD_ME_CONTROL		0x3E06
enum ec_mecontrol_modes {
	ME_LOCK		= BIT(0),
	ME_UNLOCK	= BIT(1),
};

struct ec_params_me_control {
	uint8_t me_mode;
} __ec_align1;



/*****************************************************************************/
/*
 * This command uses to notify the EC that the system is in non-ACPI mode.
 */
#define EC_CMD_NON_ACPI_NOTIFY		0x3E07

/*****************************************************************************/
/*
 * This command uses to control the PS2 emulation.
 */
#define EC_CMD_DISABLE_PS2_EMULATION	0x3E08

struct ec_params_ps2_emulation_control {
	uint8_t disable;
} __ec_align1;

/*****************************************************************************/
/*
 * This command uses for BIOS check Chassis.
 */
#define EC_CMD_CHASSIS_INTRUSION	0x3E09
#define EC_PARAM_CHASSIS_INTRUSION_MAGIC 0xCE
#define EC_PARAM_CHASSIS_BBRAM_MAGIC 0xEC

struct ec_params_chassis_intrusion_control {
	uint8_t clear_magic;
	uint8_t clear_chassis_status;
} __ec_align1;

struct ec_response_chassis_intrusion_control {
	uint8_t chassis_ever_opened;	/* bios used */
	uint8_t coin_batt_ever_remove;	/* factory used */
	uint8_t total_open_count;	/* reserved */
	uint8_t vtr_open_count;		/* reserved */
} __ec_align1;

/*****************************************************************************/
/*
 * This command uses to control the BBR status.
 */
#define EC_CMD_BB_RETIMER_CONTROL	0x3E0A

enum bb_retimer_control_mode {
	/* entry bb retimer firmware update mode */
	BB_ENTRY_FW_UPDATE_MODE = BIT(0),
	/* exit bb retimer firmware update mode */
	BB_EXIT_FW_UPDATE_MODE = BIT(1),
	/* enable compliance mode */
	BB_ENABLE_COMPLIANCE_MODE = BIT(2),
	/* disable compliance mode */
	BB_DISABLE_COMPLIANCE_MODE = BIT(3),
	/* Check fw update mode */
	BB_CHECK_STATUS	= BIT(7),
};

struct ec_params_bb_retimer_control_mode {
	uint8_t controller;
	/* See enum bb_retimer_control_mode */
	uint8_t modes;
} __ec_align1;

struct ec_response_bb_retimer_control_mode {
	uint8_t status;
} __ec_align1;

/*****************************************************************************/
/*
 * This command uses for BIOS boot check.
 */
#define EC_CMD_DIAGNOSIS		0x3E0B

enum ec_params_diagnosis_code {
	/* type: DDR */
	CODE_DDR_TRAINING_START	= 1,
	CODE_DDR_TRAINING_FINISH = 2,
	CODE_DDR_FAIL = 3,
	CODE_NO_EDP = 4,
	CODE_PORT80_COMPLETE = 0xFF,
};

struct ec_params_diagnosis {
	/* See enum ec_params_diagnosis_code */
	uint8_t diagnosis_code;
} __ec_align1;

/*****************************************************************************/
/*
 * This command uses to Swap Control Fn key for the system BIOS menu option.
 */
#define EC_CMD_UPDATE_KEYBOARD_MATRIX	0x3E0C
struct keyboard_matrix_map {
	uint8_t row;
	uint8_t col;
	uint16_t scanset;
} __ec_align1;
struct ec_params_update_keyboard_matrix {
	uint32_t num_items;
	uint32_t write;
	struct keyboard_matrix_map scan_update[32];
} __ec_align1;

/*****************************************************************************/
/*
 * This command uses to check the vpro status.
 */
#define EC_CMD_VPRO_CONTROL		0x3E0D

enum ec_vpro_control_modes {
	VPRO_OFF = 0,
	VPRO_ON = 1,
};

struct ec_params_vpro_control {
	uint8_t vpro_mode;
} __ec_align1;

/*****************************************************************************/
/*
 * This command uses to change the fingerprint LED level.
 */
#define EC_CMD_FP_LED_LEVEL_CONTROL	0x3E0E

/*
 * Discrete fingerprint LED brightness levels.
 *
 * Used as the set_led_level value of the HC v0. The EC maps each level to a
 * percentage internally (see FP_LED_* in the EC's led.h), so that host
 * software does not need to know the actual percentages.
 */
enum fp_led_brightness_level {
	FP_LED_BRIGHTNESS_HIGH = 0,
	FP_LED_BRIGHTNESS_MEDIUM = 1,
	FP_LED_BRIGHTNESS_LOW = 2,
	FP_LED_BRIGHTNESS_ULTRA_LOW = 3,
	/* Not allowed to set; only ever returned when getting the level */
	FP_LED_BRIGHTNESS_CUSTOM = 0xfe,
	FP_LED_BRIGHTNESS_AUTO = 0xff,
};

struct ec_params_fp_led_control_v0 {
	uint8_t set_led_level;
	uint8_t get_led_level;
} __ec_align1;

struct ec_response_fp_led_level_v0 {
	uint8_t percentage;
} __ec_align1;

struct ec_params_fp_led_control_v1 {
	uint8_t set_percentage;
	uint8_t get_led_level;
} __ec_align1;

struct ec_response_fp_led_level_v1 {
	uint8_t percentage;
	uint8_t level;
} __ec_align1;

/*****************************************************************************/
/*
 * This command return the chassis status
 */
#define EC_CMD_CHASSIS_OPEN_CHECK	0x3E0F

struct ec_response_chassis_open_check {
	uint8_t status;
} __ec_align1;

/*****************************************************************************/
/*
 * This command uses to notify the EC that the system is in ACPI mode.
 */
#define EC_CMD_ACPI_NOTIFY		0x3E10

/*****************************************************************************/
/*
 * This command returns the PD chip version.
 */
#define EC_CMD_READ_PD_VERSION		0x3E11

struct ec_response_read_pd_version_v0 {
	uint8_t pd0_version[8];
	uint8_t pd1_version[8];
} __ec_align1;
struct ec_response_read_pd_version_v1 {
	uint8_t pd_chip_count;
	uint8_t pd_versions[0][8];
} __ec_align1;
struct ec_response_read_pd_version_v1_4x {
	uint8_t pd_chip_count;
	uint8_t pd_versions[4][8];
} __ec_align1;

/*****************************************************************************/
/*
 * This command uses to control the standalone mode enable/disable
 */
#define EC_CMD_STANDALONE_MODE 0x3E13

struct ec_params_standalone_mode {
	uint8_t enable;
} __ec_align1;

/*****************************************************************************/
/*
 * This command returns the current state of the camera and microphone privacy switches
 */
#define EC_CMD_PRIVACY_SWITCHES_CHECK_MODE 0x3E14

struct ec_response_privacy_switches_check {
	uint8_t microphone;
	uint8_t camera;
} __ec_align1;

/*****************************************************************************/
/*
 * This command returns how many times did chassis(sw3) pressed
 */
#define EC_CMD_CHASSIS_COUNTER 0x3E15

struct ec_response_chassis_counter {
	uint8_t press_counter;
} __ec_align1;

/*****************************************************************************/
/*
 * This command returns the input deck state and board id.
 */
#define EC_CMD_CHECK_DECK_STATE		0x3E16

struct ec_params_deck_state {
	uint8_t mode;
} __ec_align1;

struct ec_response_deck_state {
	uint8_t input_deck_board_id[8];
	uint8_t deck_state;
} __ec_align1;

/*****************************************************************************/
/*
 * This command returns the simple ec version.
 */
#define EC_CMD_GET_SIMPLE_VERSION	0x3E17

struct ec_response_get_custom_version {
	char simple_version[9];
} __ec_align4;

/*****************************************************************************/
/*
 * This command returns the active charge pd chip.
 */
#define EC_CMD_GET_ACTIVE_CHARGE_PD_CHIP	0x3E18

struct ec_response_get_active_charge_pd_chip {
	uint8_t pd_chip;
} __ec_align1;

/*****************************************************************************/
/*
 * Enable/disable UEFI App mode
 *
 * Enable disables the power button functionality and allows to read it via
 * EC_CMD_UEFI_APP_BTN_STATUS host command instead.
 * This makes it possible to use it as a software button in a UEFI app.
 */
#define EC_CMD_UEFI_APP_MODE	0x3E19

struct ec_params_uefi_app_mode {
	/* 0x01 to enable, 0x00 to disable UEFI App mode */
	uint8_t flags;
} __ec_align1;

/*****************************************************************************/
/*
 * Read power button status
 */
#define EC_CMD_UEFI_APP_BTN_STATUS	0x3E1A

struct ec_response_uefi_app_btn_status {
	/* 0x00 if not pressed, 0x01 if pressed */
	uint8_t status;
} __ec_align1;

/*****************************************************************************/
/*
 * Check state of the Expansion Bay
 */
#define EC_CMD_EXPANSION_BAY_STATUS	0x3E1B

enum ec_expansion_bay_states {
	/* Valid module present and switch closed */
	MODULE_ENABLED	= BIT(0),
	/* Board ID invalid */
	MODULE_FAULT	= BIT(1),
	/* Hatch switch open/closed status */
	HATCH_SWITCH_CLOSED	= BIT(2),
};

struct ec_response_expansion_bay_status {
	/* Check ec_expansion_bay_states */
	uint8_t state;
	uint8_t board_id_0;
	uint8_t board_id_1;
} __ec_align1;

/*****************************************************************************/
/*
 * Get hardware diagnostics
 */
#define EC_CMD_GET_HW_DIAG 0x3E1C

/* See enum diagnostics_device_idx */
struct ec_response_get_hw_diag {
	uint32_t hw_diagnostics;
	uint8_t bios_complete;
	uint8_t device_complete;
} __ec_align1;

/*****************************************************************************/
/*
 * This command returns the serial of the GPU module
 * set params idx to the serial offset you want to query.
 * if idx == 0, this will query the header serial number.
 */
#define EC_CMD_GET_GPU_SERIAL	0x3E1D

struct ec_params_gpu_serial {
	uint8_t idx;
} __ec_align1;
struct ec_response_get_gpu_serial {
	uint8_t idx;
	uint8_t valid;
	char serial[20];
} __ec_align1;

/*****************************************************************************/
/*
 * This command returns the PCIE configuration of the GPU module
 * it will also return the GPU vendor type
 */
#define EC_CMD_GET_GPU_PCIE	0x3E1E

enum gpu_pcie_cfg {
	PCIE_8X1 = 0,
	PCIE_4X1 = 1,
	PCIE_4X2 = 2,
} __packed;
BUILD_ASSERT(sizeof(enum gpu_pcie_cfg) == sizeof(uint8_t));


enum gpu_vendor {
	GPU_VENDOR_INITIALIZING = 0,
	GPU_FAN_ONLY = 1,
	GPU_AMD_R23M = 2,
	GPU_SSD = 3,
	GPU_PCIE_ACCESSORY = 4,
	GPU_NV_GN22 = 5,
} __packed;
BUILD_ASSERT(sizeof(enum gpu_vendor) == sizeof(uint8_t));

struct ec_response_get_gpu_config {
	/* See enum gpu_pcie_cfg */
	uint8_t gpu_pcie_config;
	/* See enum gpu_vendor */
	uint8_t gpu_vendor;
} __ec_align1;

/*****************************************************************************/
/*
 * This command programs the GPU serial
 * set magic = 0x0D for GPU structure
 * set magic = 0x55 for SSD structure
 * currently only idx = 0 is supported to program the header serial number.
 */
#define EC_CMD_PROGRAM_GPU_EEPROM	0x3E1F

struct ec_params_program_gpu_serial {
	uint8_t magic;
	uint8_t idx;
	char serial[20];
} __ec_align1;
struct ec_response_program_gpu_serial {
	uint8_t valid;
} __ec_align1;

/*****************************************************************************/
/*
 * Used the host command to control the fingerprint power
 */
#define EC_CMD_FP_CONTROL	0x3E20

struct ec_params_fingerprint_control {
	/* 0x01 to enable, 0x00 to disable fingerprint power */
	uint8_t enable;
} __ec_align1;

/*****************************************************************************/
/*
 * Get cutoff status
 */
#define EC_CMD_GET_CUTOFF_STATUS 0x3E21

struct ec_response_get_cutoff_status {
	uint8_t status;
} __ec_align1;

/*****************************************************************************/
/*
 * This command return the AP throttle status
 */
#define EC_CMD_GET_AP_THROTTLE_STATUS	0x3E22

struct ec_response_get_ap_throttle_status {
	uint8_t soft_ap_throttle;
	uint8_t hard_ap_throttle;
} __ec_align1;

/*****************************************************************************/
/*
 * Get the current state of the pd port
 */
#define EC_CMD_GET_PD_PORT_STATE	0x3E23

struct ec_params_get_pd_port_state {
	uint8_t port;
} __ec_align1;

struct ec_response_get_pd_port_state {
	uint8_t c_state;
	uint8_t pd_state;
	uint8_t power_role;
	uint8_t data_role;
	uint8_t vconn;
	uint8_t epr_active;
	uint8_t epr_support;
	uint8_t cc_polarity;
	uint16_t voltage;
	uint16_t current;
	uint8_t active_port;
	uint8_t pd_alt_mode_status;
} __ec_align1;

/*****************************************************************************/
/*
 * Battery extender control
 */
#define EC_CMD_BATTERY_EXTENDER	0x3E24

struct ec_params_battery_extender {
	uint8_t disable;
	uint8_t trigger_days;
	uint16_t reset_minutes;
	uint8_t cmd;
	uint8_t manual;
} __ec_align1;

struct ec_response_battery_extender {
	uint8_t current_stage;
	uint16_t trigger_days;
	uint16_t reset_minutes;
	uint8_t disable;
	uint64_t trigger_timedelta;
	uint64_t reset_timedelta;
} __ec_align1;

enum battery_extender_cmd {
	BATT_EXTENDER_WRITE_CMD,
	BATT_EXTENDER_READ_CMD,
};

/*****************************************************************************/
/*
 * Host command to enable/disable wake on lan function
 */
#define EC_CMD_WAKE_ON_LAN	0x3E25

struct ec_params_wake_on_lan_control {
	uint8_t enable;
} __ec_align1;


struct ec_response_wake_on_lan_control {
	uint8_t enable;
} __ec_align1;

/*****************************************************************************/
/*
 * Measure a Board ID to see what's currently populated
 *
 * Similar to EC_CMD_GET_CROS_BOARD_INFO. But that is only measured once,
 * not for hotpluggable components.
 * We use the board ID for hotpluggable components like the inputmodules,
 * touchpad, powerbutton board and audio board.
 * To detect whether these are plugged in and which version of the is plugged in,
 * the board ID needs to be measured at run-time.
 *
 * For the Framework 16 inputmodules there is a dedicated command, which abstracts the logic,
 * because there is also a mux involed: EC_CMD_CHECK_DECK_STATE
 *
 * For Framework 13 we have multiple versions of the touchpad and speakers (audio board)
 * that BIOS/OS need to distinguish through this host command.
 *
 * Compared to EC_CMD_ADC_READ this host command adds the board specific logic
 * to convert the voltage level into the board ID index.
 */
#define EC_CMD_READ_BOARDID	0x3E26

enum ec_board_id_type {
	/* Mainboard - any system */
	HC_BOARD_ID_MAINBOARD = 0,
	/* Power button board - Framework 12 */
	HC_BOARD_ID_POWERBUTTON_BOARD = 1,
	/* Touchpad board - Framework 12, 13 */
	HC_BOARD_ID_TOUCHPAD = 2,
	/* Audio board - Framework 12, 13 */
	HC_BOARD_ID_AUDIO_BOARD = 3,
	/* dGPU board - Framework 16 */
	HC_BOARD_ID_DGPU0 = 4,
	/* dGPU board - Framework 16 */
	HC_BOARD_ID_DGPU1 = 5,
};

struct ec_params_read_boardid {
  /* See enum ec_board_id_type */
	uint8_t board_id_type;
} __ec_align1;

struct ec_response_read_boardid {
	/* -1 if unknown (some issue reading it)
	 * Otherwise 0-15
	 */
	int8_t board_id;
} __ec_align1;

/*****************************************************************************
 *
 * Host command to set the fan configuration
 */
#define EC_CMD_FAN_CONFIGURATION	0x3E27

enum fan_hc_command_t {
	FAN_HC_CMD_GET = 0,
	FAN_HC_CMD_SET = 1,
};

struct fan_parameter_t {
	uint8_t max_duty;	 /* Set the fan maximum duty */
	uint8_t min_duty;	 /* Set the fan minimum duty */
	uint8_t max_temperature; /* Set the maximum temperature point for fan's maximum duty */
	uint8_t min_temperature; /* Set the minimum temperature point for fan's minimum duty */
	/* Control the fan to minimum duty if the temperature is less than minimum temperature */
	uint8_t fan_always_on;
	/* Monitor which sensor source to control the fan. Reference enum sensor_source_t */
	uint8_t	sensor_source;
	uint8_t target_duty;	 /* Current fan duty */
};

struct ec_params_fan_configuration {
	uint8_t fan_index;
	/* CMD_SET to update the parameter from host. Reference enum fan_hc_command_t */
	uint8_t command;
	struct fan_parameter_t fan_config;
} __ec_align1;

struct ec_response_fan_configuration {
	struct fan_parameter_t fan_config;
} __ec_align1;

/*****************************************************************************/
/*
 * Host command to enable/disable force on the PSU in standby mode
 */
#define EC_CMD_PSU_CONTROL	0x3E28

struct ec_params_psu_control {
	/* bool; force enable the PSU in standby mode */
	uint8_t force_enable_in_standby;
} __ec_align1;


struct ec_response_psu_control {
	/* bool; force enable the PSU in standby mode */
	uint8_t force_enable_in_standby;
} __ec_align1;

/*****************************************************************************/
/*
 * This command uses to control the Power on AC attach enable/disable
 */
#define EC_CMD_POWER_ON_AC_ATTACH 0x3E2A

struct ec_params_power_on_ac_attach {
	uint8_t enable;
} __ec_align1;

#define EC_CMD_LED_PWM_CONTROL 0x3E2B

struct ec_params_led_pwm_control {
	/* See enum ec_led_id */
	uint8_t led_id;
	/* See enum led_color */
	uint8_t led_color;
	uint8_t pwm_r;
	uint8_t pwm_g;
	uint8_t pwm_b;
} __ec_align1;

#endif /* !__ACPI__ */

#endif /* __CROS_EC_FWK_EC_COMMANDS_H */
