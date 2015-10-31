/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Electronics
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
#include <libpayload-config.h>
#include <libpayload.h>

void serial_putchar(unsigned int c)
{
}

int serial_havechar(void)
{
	return 0;
}

int serial_getchar(void)
{
	return 0;
}

static struct console_input_driver consin = {
	.havekey = &serial_havechar,
	.getchar = &serial_getchar
};

static struct console_output_driver consout = {
	.putchar = &serial_putchar
};

void serial_console_init(void)
{
	console_add_input_driver(&consin);
	console_add_output_driver(&consout);
}
