/*
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "ec.h"
#include <ec/compal/ene932/ec.h>

/* The keyboard matrix tells the EC how the keyboard is wired internally */
static void set_keyboard_matrix_us(void)
{
	ec_kbc_write_cmd(0x59);
	ec_kbc_write_ib(0xE5);
}

/* Tell EC to operate in ACPI mode, thus generating SCIs on events, not SMIs */
static void enter_acpi_mode(void)
{
	ec_kbc_write_cmd(0x59);
	ec_kbc_write_ib(0xE8);
}

void pavilion_m6_1035dx_ec_init(void)
{
	set_keyboard_matrix_us();
	/* This could also be done in an SMI, should we decide to use SMM */
	enter_acpi_mode();
}
