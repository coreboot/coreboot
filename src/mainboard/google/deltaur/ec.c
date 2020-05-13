/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <ec/google/wilco/commands.h>
#include <variant/ec.h>

void mainboard_post(uint8_t value)
{
	wilco_ec_save_post_code(value);
}
