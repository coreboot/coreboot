/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <stdint.h>
#include <string.h>

#include "ec.h"
#include "commands.h"

int wilco_ec_get_info(enum get_ec_info_cmd type, char *info)
{
	struct ec_response_get_ec_info rsp;

	if (!info)
		return -1;
	if (wilco_ec_sendrecv(KB_EC_INFO, type, &rsp, sizeof(rsp)) < 0)
		return -1;

	/* Copy returned string */
	strncpy(info, rsp.data, sizeof(rsp.data));
	return 0;
}

void wilco_ec_print_all_info(void)
{
	char info[EC_INFO_MAX_SIZE];

	if (!wilco_ec_get_info(GET_EC_LABEL, info))
		printk(BIOS_INFO, "EC Label      : %s\n", info);

	if (!wilco_ec_get_info(GET_EC_SVN_REV, info))
		printk(BIOS_INFO, "EC Revision   : %s\n", info);

	if (!wilco_ec_get_info(GET_EC_MODEL_NO, info))
		printk(BIOS_INFO, "EC Model Num  : %s\n", info);

	if (!wilco_ec_get_info(GET_EC_BUILD_DATE, info))
		printk(BIOS_INFO, "EC Build Date : %s\n", info);
}
