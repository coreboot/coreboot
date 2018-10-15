/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef EC_GOOGLE_WILCO_COMMANDS_H
#define EC_GOOGLE_WILCO_COMMANDS_H

#include <stdint.h>

enum {
	/* Retrieve information about the EC */
	KB_EC_INFO = 0x38,
	/* Set ACPI mode on or off */
	KB_ACPI = 0x3a,
	/* Manage the EC power button passthru to the host */
	KB_POWER_BUTTON_TO_HOST = 0x3e,
};

enum set_acpi_mode_cmd {
	ACPI_OFF = 0,
	ACPI_ON
};

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

#endif /* EC_GOOGLE_WILCO_COMMANDS_H */
