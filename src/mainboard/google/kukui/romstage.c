/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#include <arch/exception.h>
#include <console/console.h>
#include <program_loading.h>
#include <timestamp.h>

void main(void)
{
	timestamp_add_now(TS_START_ROMSTAGE);

	/* Init UART baudrate when PLL on. */
	console_init();
	exception_init();

	run_ramstage();
}
