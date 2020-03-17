/*
 * This file is part of the coreboot project.
 *
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

#include <cpu/x86/smm.h>
#include <ec/google/wilco/smm.h>
#include <intelblocks/smihandler.h>
#include <variant/ec.h>

void mainboard_smi_espi_handler(void)
{
	wilco_ec_smi_espi();
}

void mainboard_smi_sleep(u8 slp_typ)
{
	wilco_ec_smi_sleep(slp_typ);
}

int mainboard_smi_apmc(u8 apmc)
{
	wilco_ec_smi_apmc(apmc);
	return 0;
}
