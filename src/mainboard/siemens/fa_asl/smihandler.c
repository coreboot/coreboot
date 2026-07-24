/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/x86/smm.h>
#include <intelblocks/smihandler.h>

void mainboard_smi_espi_handler(void)
{
	return;

}

void mainboard_smi_sleep(u8 slp_typ)
{
	return;
}

int mainboard_smi_apmc(u8 apmc)
{
	return 0;
}
