/* SPDX-License-Identifier: GPL-2.0-only */

/* Enable EC backed Keyboard Backlight in ACPI */
#define EC_ENABLE_KEYBOARD_BACKLIGHT

/* EC wake is GPIO27 which is a special DeepSX wake pin */
#define EC_ENABLE_WAKE_PIN	0x70
