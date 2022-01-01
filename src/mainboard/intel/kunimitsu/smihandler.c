/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <ec/google/chromeec/smm.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <intelblocks/smihandler.h>
#include "ec.h"
#include "gpio.h"

int mainboard_io_trap_handler(int smif)
{
	switch (smif) {
	case 0x99:
		printk(BIOS_DEBUG, "Sample\n");
		gnvs->smif = 0;
		break;
	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 0
	 * On failure, the IO Trap Handler returns a value != 0
	 *
	 * For now, we force the return value to 0 and log all traps to
	 * see what's going on.
	 */
	return 1;
}

void mainboard_smi_gpi_handler(const struct gpi_status *sts)
{
	if (gpi_status_get(sts, EC_SMI_GPI))
		chromeec_smi_process_events();
}

void mainboard_smi_sleep(u8 slp_typ)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS,
					MAINBOARD_EC_S5_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS,
					MAINBOARD_EC_SMI_EVENTS);
	return 0;
}
