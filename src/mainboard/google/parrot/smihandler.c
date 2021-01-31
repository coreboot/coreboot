/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <southbridge/intel/common/pmutil.h>
#include <southbridge/intel/common/pmbase.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <ec/compal/ene932/ec.h>
#include "ec.h"

static u8 mainboard_smi_ec(void)
{
	u8 src;

	ec_kbc_write_cmd(0x56);
	src = ec_kbc_read_ob();
	printk(BIOS_DEBUG, "%s src: %x\n", __func__, src);

	switch (src) {
	case EC_BATTERY_CRITICAL:
		break;
	case EC_LID_CLOSE:
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

		/* Go to S5 */
		write_pmbase32(PM1_CNT, read_pmbase32(PM1_CNT) | (0xf << 10));
		break;
	}

	return src;
}

void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << EC_SMI_GPI)) {
		/* Process all pending events from EC */
		do {} while (mainboard_smi_ec() != EC_NO_EVENT);
	} else if (gpi_sts & (1 << EC_LID_GPI)) {
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

		/* Go to S5 */
		write_pmbase32(PM1_CNT, read_pmbase32(PM1_CNT) | (0xf << 10));
	}
}

void mainboard_smi_sleep(u8 slp_typ)
{
	/* Disable SCI and SMI events */

	/* Clear pending events that may trigger immediate wake */

	/* Enable wake events */

	/* Tell the EC to Disable USB power */
	if (gnvs->s3u0 == 0 && gnvs->s3u1 == 0) {
		ec_kbc_write_cmd(0x45);
		ec_kbc_write_ib(0xF2);
	}
}

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
		/* Clear all pending events */
		/* EC cmd:59 data:E8 */
		ec_kbc_write_cmd(0x59);
		ec_kbc_write_ib(0xE8);

		/* Set LID GPI to generate SCIs */
		gpi_route_interrupt(EC_LID_GPI, GPI_IS_SCI);
		break;
	case APM_CNT_ACPI_DISABLE:
		/* Clear all pending events */
		/* EC cmd:59 data:e9 */
		ec_kbc_write_cmd(0x59);
		ec_kbc_write_ib(0xE9);

		/* Set LID GPI to generate SMIs */
		gpi_route_interrupt(EC_LID_GPI, GPI_IS_SMI);
		break;
	}
	return 0;
}
