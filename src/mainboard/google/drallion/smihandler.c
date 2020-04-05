/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
