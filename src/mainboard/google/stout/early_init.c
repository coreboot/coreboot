/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <bootmode.h>
#include <ec/quanta/it8518/ec.h>
#include "ec.h"
#include "onboard.h"

void mainboard_late_rcba_config(void)
{
	/*
	 *             GFX         INTA -> PIRQA (MSI)
	 * D20IP_XHCI  XHCI        INTA -> PIRQD (MSI)
	 * D26IP_E2P   EHCI #2     INTA -> PIRQF
	 * D27IP_ZIP   HDA         INTA -> PIRQA (MSI)
	 * D28IP_P2IP  WLAN        INTA -> PIRQD
	 * D28IP_P3IP  Card Reader INTB -> PIRQE
	 * D28IP_P6IP  LAN         INTC -> PIRQB
	 * D29IP_E1P   EHCI #1     INTA -> PIRQD
	 * D31IP_SIP   SATA        INTA -> PIRQB (MSI)
	 * D31IP_SMIP  SMBUS       INTB -> PIRQH
	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (NOINT << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
		(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D30IP) = (NOINT << D30IP_PIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (NOINT << D28IP_P1IP) | (INTA << D28IP_P2IP) |
		(INTB << D28IP_P3IP) | (NOINT << D28IP_P4IP) |
		(NOINT << D28IP_P5IP) | (INTC << D28IP_P6IP) |
		(NOINT << D28IP_P7IP) | (NOINT << D28IP_P8IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (NOINT << D25IP_LIP);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);
	RCBA32(D20IP) = (INTA << D20IP_XHCIIP);

	/* Device interrupt route registers */
	DIR_ROUTE(D31IR, PIRQB, PIRQH, PIRQA, PIRQC);
	DIR_ROUTE(D29IR, PIRQD, PIRQE, PIRQF, PIRQG);
	DIR_ROUTE(D28IR, PIRQD, PIRQE, PIRQB, PIRQC);
	DIR_ROUTE(D27IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D26IR, PIRQF, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D25IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D22IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D20IR, PIRQD, PIRQE, PIRQF, PIRQG);
}

	/*
	 * The Stout EC needs to be reset to RW mode. It is important that
	 * the RTC_PWR_STS is not set until ramstage EC init.
	 */
static void early_ec_init(void)
{
	u8 ec_status = ec_read(EC_STATUS_REG);
	int rec_mode = get_recovery_mode_switch();

	if (((ec_status & 0x3) == EC_IN_RO_MODE) ||
	    ((ec_status & 0x3) == EC_IN_RECOVERY_MODE)) {
		printk(BIOS_DEBUG, "EC Cold Boot Detected\n");
		if (!rec_mode) {
			/*
			 * Tell EC to exit RO mode
			 */
			printk(BIOS_DEBUG, "EC will exit RO mode and boot normally\n");
			ec_write_cmd(EC_CMD_EXIT_BOOT_BLOCK);
			die("wait for ec to reset");
		}
	} else {
		printk(BIOS_DEBUG, "EC Warm Boot Detected\n");
		ec_write_cmd(EC_CMD_WARM_RESET);
	}
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* TODO: Confirm if nortbridge_fill_pei_data() gets .system_type right (should be 0) */
}

void mainboard_early_init(int s3resume)
{
	/* Do ec reset as early as possible, but skip it on S3 resume */
	if (!s3resume) {
		early_ec_init();
	}
}
