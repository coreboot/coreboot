/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "ec.h"
#include <ec/compal/ene932/ec.h>

/* The keyboard matrix tells the EC how the keyboard is wired internally */
static void set_keyboard_matrix_us(void)
{
	ec_kbc_write_cmd(0x59);
	ec_kbc_write_ib(0xE5);
}

/* Tell EC to operate in APM mode. Events generate SMIs instead of SCIs */
static void enter_apm_mode(void)
{
	ec_kbc_write_cmd(0x59);
	ec_kbc_write_ib(0xE9);
}

void lenovo_g505s_ec_init(void)
{
	set_keyboard_matrix_us();

	/*
	 * The EC has a special "blinking Caps Lock LED" mode which it normally
	 * enters when it believes the OS is not responding. It occasionally
	 * disables battery charging when in this mode, although other
	 * functionality is unaffected. Although the EC starts in APM mode by
	 * default, it only leaves the "blinking Caps Lock LED" mode after
	 * receiving the following command.
	 */
	enter_apm_mode();
}
