/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_GOOGLE_WILCO_COMMANDS_H
#define EC_GOOGLE_WILCO_COMMANDS_H

#include <types.h>

enum {
	/* Read and clear power state information */
	KB_POWER_SMI = 0x04,
	/* Read but do not clear power state information */
	KB_POWER_STATUS = 0x05,
	/* Inform the EC about the reason host is turning off */
	KB_POWER_OFF = 0x08,
	/* Control wireless radios */
	KB_RADIO_CONTROL = 0x2b,
	/* Save PS/2 data before S3 suspend */
	KB_SAVE = 0x2f,
	/* Restore PS/2 data after S3 resume */
	KB_RESTORE = 0x30,
	/* Manage the EC control of camera power */
	KB_CAMERA = 0x33,
	/* Retrieve information about the EC */
	KB_EC_INFO = 0x38,
	/* Set ACPI mode on or off */
	KB_ACPI = 0x3a,
	/* Board ID */
	KB_BOARD_ID = 0x3d,
	/* Change ACPI wake up source */
	KB_ACPI_WAKEUP_CHANGE = 0x4a,
	/* Manage the EC power button passthru to the host */
	KB_POWER_BUTTON_TO_HOST = 0x3e,
	/* Manage the EC control of speaker mute */
	KB_HW_MUTE_CONTROL = 0x60,
	/* Inform the EC that the host is about to enter S3 */
	KB_SLP_EN = 0x64,
	/* Inform the EC about BIOS boot progress */
	KB_BIOS_PROGRESS = 0xc2,
	/* Inform the EC that a fatal error occurred */
	KB_ERR_CODE = 0x7b,
	/* Set CPU ID */
	KB_CPU_ID = 0xbf,
};

enum ec_ram_addr {
	/* Indicate if EC uses signed firmware */
	EC_RAM_SIGNED_FW = 0x5c,
	/* Indicate support for S0ix */
	EC_RAM_S0IX_SUPPORT = 0xb8,
};

enum set_acpi_mode_cmd {
	ACPI_OFF = 0,
	ACPI_ON
};

enum bios_progress_code {
	BIOS_PROGRESS_BEFORE_MEMORY = 0x00,
	BIOS_PROGRESS_MEMORY_INIT = 0x01,
	BIOS_PROGRESS_VIDEO_INIT = 0x02,
	BIOS_PROGRESS_LOGO_DISPLAYED = 0x03,
	BIOS_PROGRESS_POST_COMPLETE = 0x04,
};

enum ec_audio_mute {
	AUDIO_MUTE = 0,		/* Mute speakers immediately */
	AUDIO_UNMUTE_125MS,	/* Unmute in 125ms */
};

enum ec_radio {
	RADIO_WIFI = 0,
	RADIO_WWAN,
	RADIO_BT,
};

enum ec_radio_action {
	RADIO_READ = 1,
	RADIO_WRITE,
	RADIO_TOGGLE,
};

enum ec_camera {
	CAMERA_ON = 0,
	CAMERA_OFF
};

enum ec_err_code {
	DLED_MEMORY = 0x03,
	DLED_PANEL = 0x10,
	DLED_ROM = 0x19,
};

/**
 * wilco_ec_radio_control() - Control wireless radios.
 * @ec_radio: Wireless radio type.
 * @state: Turn radio on or off.
 * Return: 0 if successful or negative error code on failure.
 */
int wilco_ec_radio_control(enum ec_radio radio, uint8_t state);

/*
 * EC Information
 */

enum get_ec_info_cmd {
	GET_EC_LABEL = 0,
	GET_EC_SVN_REV,
	GET_EC_MODEL_NO,
	GET_EC_BUILD_DATE
};

#define EC_INFO_MAX_SIZE 9
struct ec_response_get_ec_info {
	char data[EC_INFO_MAX_SIZE]; /* ASCII NUL terminated string */
};

/**
 * wilco_ec_get_info
 *
 * Read a specific information string from the EC and return it in
 * the caller-provided buffer of at least EC_INFO_MAX_SIZE bytes.
 *
 * @cmd:	Information to retrieve
 * @info:	Character array of EC_INFO_MAX_SIZE bytes
 *
 * Returns 0 if successful and resulting string is in 'info'
 * Returns -1 if the EC command fails
 */
int wilco_ec_get_info(enum get_ec_info_cmd cmd, char *info);

/**
 * wilco_ec_print_all_info
 *
 * Retrieve and print all the information strings from the EC:
 *
 *  GET_EC_LABEL
 *  GET_EC_SVN_REV
 *  GET_EC_MODEL_NO
 *  GET_EC_BUILD_DATE
 */
void wilco_ec_print_all_info(void);

/*
 * EC Power State
 */

enum ec_power_off_reason {
	EC_PWROFF_FLASH = 0x11,
	EC_PWROFF_AC_REMOVED = 0x12,
	EC_PWROFF_BAT_REMOVED = 0x13,
	EC_PWROFF_LOBAT = 0x15,
	EC_PWROFF_PWRB_IN_POST = 0x16,
	EC_PWROFF_FORCE_IMMEDIATE = 0x18,
	EC_PWROFF_WDT = 0x1b,
	EC_PWROFF_FORCE_THERMAL = 0x22,
	EC_PWROFF_ERR_CODE = 0x23,
	EC_PWROFF_PAID_PWRGD = 0x27,
	EC_PWROFF_PAID_CPU = 0x28,
	EC_PWROFF_PAID_GFX = 0x29,
	EC_PWROFF_PAID_CLK = 0x2a,
	EC_PWROFF_PAID_NOMEMORY = 0x2b,
	EC_PWROFF_PAID_MEMORY_ERR = 0x2c,
	EC_PWROFF_PAID_MEMORY_SPD = 0x2d,
	EC_SWOFF_ACPI = 0x31,
	EC_SWOFF_BOOT_PASSWORD = 0x33,
	EC_SWOFF_DISK_PASSWORD = 0x34,
	EC_SWOFF_POWER_CYCLE = 0x37,
	EC_SWOFF_HARD_RESET = 0x3b,
	EC_SWOFF_FSMI = 0x3f,
	EC_PWRLOG_THERMTRIP = 0x41,
	EC_PWRLOG_NO_S5 = 0x42,
	EC_PWROFF_4S_PWRB = 0x44,
	EC_PWROFF_ASF2_FORCEOFF = 0x45,
	EC_PWROFF_PWRB_THERMAL = 0x48,
	EC_PWROFF_AOAC_TIMER = 0x4b,
};

/**
 * wilco_ec_power_off
 *
 * Tell the EC why the host is about to power off.
*/
void wilco_ec_power_off(enum ec_power_off_reason reason);

/**
 * wilco_ec_slp_en
 *
 * Tell the EC that the host is entering a sleep state.
 */
void wilco_ec_slp_en(void);

enum ec_pm1_state {
	EC_PM1_AC_AVAIL = BIT(0),	/* AC available */
	EC_PM1_BAT_AVAIL = BIT(1),	/* Battery available */
	EC_PM1_LO_BAT1 = BIT(2),	/* Battery 1 low */
	EC_PM1_LO_BAT2 = BIT(3),	/* Battery 2 low */
	EC_PM1_LID_OPEN = BIT(4),	/* Lid is open */
	EC_PM1_LCD_POWER = BIT(5),	/* LCD is powered */
	EC_PM1_OVER_TEMP = BIT(6),	/* CPU is over temperature */
	EC_PM1_DOCKED = BIT(7),		/* System is docked */
};

enum ec_pm2_state {
	EC_PM2_SYS_MB_PCIE = BIT(0),	/* MB has PCIe */
	EC_PM2_SYS_MB_SATA = BIT(1),	/* MB has SATA */
	EC_PM2_PWRB_PRESSED = BIT(2),	/* Power button is pressed */
	EC_PM2_TURBO_MODE = BIT(3),	/* Turbo mode */
};

enum ec_pm3_state {
	EC_PM3_BAT1_PRES = BIT(2),	/* Battery 1 is present */
	EC_PM3_BAT2_PRES = BIT(3),	/* Battery 2 is present */
	EC_PM3_LOWER_PSTATE = BIT(6),	/* EC requests lower P-state */
	EC_PM3_CPU_THROTTLE = BIT(7),	/* EC requests CPU throttle */
};

enum ec_pm4_state {
	EC_PM4_BAT1_CHG = BIT(0),	/* Battery 1 is being charged */
	EC_PM4_BAT2_CHG = BIT(1),	/* Battery 2 is being charged */
	EC_PM4_BAT1_PWR = BIT(2),	/* Battery 1 is powering the system */
	EC_PM4_BAT2_PWR = BIT(3),	/* Battery 2 is powering the system */
	EC_PM4_PANEL_STATE = BIT(5),	/* Panel power state */
};

enum ec_pm5_state {
	EC_PM5_INT_HD_SATA = BIT(7),	/* Internal SATA HDD */
};

enum ec_pm6_state {
	EC_PM6_WLAN_SWITCH = BIT(0),	/* Wireless switch */
	EC_PM6_SYS_MB_MODEM = BIT(1),	/* MB has modem */
	EC_PM6_ETH_STATE = BIT(2),	/* Ethernet cable state */
	EC_PM6_AC_UPDATE = BIT(3),	/* Update AC information */
};

enum ec_pm1_event {
	EC_EV1_PWRB_PRESSED = BIT(0),	/* Power button was pressed */
	EC_EV1_HOTKEY_PRESSED = BIT(1),	/* Hotkey was pressed */
	EC_EV1_STATE_CHANGED = BIT(2),	/* PMx state changed */
};

enum ec_pm2_event {
	EC_EV2_ACPI_MONSWITCH = BIT(0),	/* Monitor switch status */
};

struct ec_pm_event_state {
	uint8_t event[2];		/* ec_pm{1,2}_event */
	uint8_t state[6];		/* ec_pm{1,2,3,4,5,6}_state */
	uint8_t hotkey;			/* Hotkey, if pressed */
	uint16_t ac_type;		/* AC adapter information */
};

/**
 * wilco_ec_get_pm
 *
 * Retrieve power and event information from the EC.
 *
 * @pm:		Power event state structure to fill out
 * @clear:	Clear EC event state after reading
 *
 * Returns 0 if EC command was successful
 * Returns -1 if EC command failed
 */
int wilco_ec_get_pm(struct ec_pm_event_state *pm, bool clear);

/**
 * wilco_ec_get_lid_state
 *
 * Retrieve the lid state without clearing it in the EC.
 *
 * Returns 1 if the lid is open, 0 if it is closed
 * Returns -1 if the EC command failed
 */
int wilco_ec_get_lid_state(void);

/**
 * wilco_ec_get_board_id
 *
 * Retrieve the board ID value from the EC.
 * @id:		Pointer to variable to store the ID read from the EC.
 *
 * Returns number of bytes transferred from the EC
 * Returns -1 if the EC command failed
 */
int wilco_ec_get_board_id(uint8_t *id);

enum ec_wake_change {
	WAKE_OFF = 0,
	WAKE_ON
};
/**
 * wilco_ec_change_wake_source
 *
 * Change acpi wake up source.
 * @source:		Wake up source that can be enabled/disabled.
 * @ec_wake_change:	On/off switch.
 *
 * Returns -1 if the EC command failed
 */
int wilco_ec_change_wake(uint8_t source, enum ec_wake_change change);

enum ec_acpi_wake_events {
	EC_ACPI_WAKE_PWRB = BIT(0),	/* Wake up by power button */
	EC_ACPI_WAKE_LID = BIT(1),	/* Wake up by lid switch */
	EC_ACPI_WAKE_RTC = BIT(5),	/* Wake up by RTC */
};

/**
 * wilco_ec_signed_fw
 *
 * Indicate if the EC uses signed firmware.
 *
 * Returns 1 if EC uses signed firmware, otherwise returns 0
 */
int wilco_ec_signed_fw(void);

/**
 * wilco_ec_save_post_code
 *
 * Save this post code as the most recent progress step.  If the boot fails
 * and calls die_notify() this post code will be used to send an error code
 * to the EC indicating the failure.
 *
 * @post_code: Post code to save
 */
void wilco_ec_save_post_code(uint8_t post_code);

/**
 * wilco_ec_set_cpuid
 *
 * Set CPU ID to EC.
 *
 * @cpuid:	read CPU ID from cpu_eax(1)
 * @cpu_cores:	cores of CPU
 * @gpu_cores:	cores of GPU
 *
 * Returns 0 if EC command was successful
 * Returns -1 if EC command failed
 */
int wilco_ec_set_cpuid(uint32_t cpuid, uint8_t cpu_cores, uint8_t gpu_cores);

#endif /* EC_GOOGLE_WILCO_COMMANDS_H */
