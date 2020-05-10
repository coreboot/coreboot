/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ec/quanta/ene_kb3940q/ec.h>
#include "ec.h"

void butterfly_ec_init(void)
{
	printk(BIOS_DEBUG, "Butterfly EC Init\n");

	/* Report EC info */
	/* EC version: 6 bytes */
	printk(BIOS_DEBUG,"  EC version: %c%c%c%c%c%c\n",
	       ec_mem_read(EC_FW_VER0), ec_mem_read(EC_FW_VER1),
	       ec_mem_read(EC_FW_VER2), ec_mem_read(EC_FW_VER3),
	       ec_mem_read(EC_FW_VER4), ec_mem_read(EC_FW_VER5));

	/* Disable wake on USB, LAN & RTC */
	/* Enable Wake from Keyboard */
	ec_mem_write(EC_EC_PSW, EC_PSW_IKB);

}
