/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>

// SMFI commands

// Indicates that EC is ready to accept commands
#define CMD_NONE 0
// Probe for Dasharo EC protocol
#define CMD_PROBE 1
// Read board string
#define CMD_BOARD 2
// Read version string
#define CMD_VERSION 3
// Write bytes to console
#define CMD_PRINT 4
// Access SPI chip
#define CMD_SPI 5
// Reset EC
#define CMD_RESET 6
// Get fan speeds
#define CMD_FAN_GET 7
// Set fan speeds
#define CMD_FAN_SET 8
// Get keyboard map index
#define CMD_KEYMAP_GET 9
// Set keyboard map index
#define CMD_KEYMAP_SET 10
// Get LED value by index
#define CMD_LED_GET_VALUE 11
// Set LED value by index
#define CMD_LED_SET_VALUE 12
// Get LED color by index
#define CMD_LED_GET_COLOR 13
// Set LED color by index
#define CMD_LED_SET_COLOR 14
// Get LED matrix mode and speed
#define CMD_LED_GET_MODE 15
// Set LED matrix mode and speed
#define CMD_LED_SET_MODE 16
// Get key matrix state
#define CMD_MATRIX_GET 17
// Save LED settings to ROM
#define CMD_LED_SAVE 18
// Enable/disable no input mode
#define CMD_SET_NO_INPUT 19
// Set fan curve
#define CMD_FAN_CURVE_SET 20
// Get security state
#define CMD_SECURITY_GET 21,
// Set security state
#define CMD_SECURITY_SET 22,
// Set camera enablement
#define CMD_CAMERA_ENABLEMENT_SET 23
// Set WiFi + Bluetooth card enablement
#define CMD_WIFI_BT_ENABLEMENT_SET 24
// Get a persistent option by index
#define CMD_OPTION_GET 25
// Set a persistent option by index
#define CMD_OPTION_SET 26

// Print command. Registers are unique for each command
#define CMD_PRINT_REG_FLAGS 2
#define CMD_PRINT_REG_LEN 3
#define CMD_PRINT_REG_DATA 4

// SPI command
// Read from SPI chip if set, write otherwise
#define CMD_SPI_FLAG_READ BIT(0)
// Disable SPI chip after executing command
#define CMD_SPI_FLAG_DISABLE BIT(1)
// Run firmware from scratch RAM if necessary
#define CMD_SPI_FLAG_SCRATCH BIT(2)
// Write to backup ROM instead
#define CMD_SPI_FLAG_BACKUP BIT(3)

#define CMD_LED_INDEX_ALL 0xFF

// Persistent option definitions
enum {
	OPT_POWER_ON_AC = 0,
	NUM_OPTIONS
};

uint8_t dasharo_ec_smfi_cmd(uint8_t cmd, uint8_t len, uint8_t *data);
uint8_t dasharo_ec_read_version(uint8_t *data);
uint8_t dasharo_ec_read_board(uint8_t *data);
