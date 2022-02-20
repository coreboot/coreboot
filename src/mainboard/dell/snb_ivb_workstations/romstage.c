/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/common/rcba.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/smsc/sch5545/sch5545.h>
#include <superio/smsc/sch5545/sch5545_emi.h>

#include <baseboard/sch5545_ec.h>

void mainboard_late_rcba_config(void)
{
	DIR_ROUTE(D31IR, PIRQA, PIRQD, PIRQC, PIRQA);
	DIR_ROUTE(D29IR, PIRQH, PIRQD, PIRQA, PIRQC);
	DIR_ROUTE(D28IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D27IR, PIRQG, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D26IR, PIRQA, PIRQF, PIRQC, PIRQD);
	DIR_ROUTE(D25IR, PIRQE, PIRQF, PIRQG, PIRQH);
	DIR_ROUTE(D22IR, PIRQA, PIRQD, PIRQC, PIRQB);
	DIR_ROUTE(D20IR, PIRQA, PIRQB, PIRQC, PIRQD);
}

void mainboard_early_init(int s3resume)
{
	uint16_t ec_fw_version;

	/*
	 * We do EC initialization in romstage, because it makes no sense to
	 * bloat the bootblock any more. Secondly, the EC expects to receive
	 * correct initialization sequence from the host in the time window of
	 * about 3-5 seconds since system reset. If it doesn't receive the
	 * initialization sequence, it enters an error path which results in
	 * fans spinned up to high speed. In this state EC doesn't respond to
	 * further messages sent over EMI. The issue appears after power
	 * failure, where EC loses its configuration. For this particular
	 * reasons we do the initialization in romstage instead of ramstage.
	 */
	sch5545_emi_init(0x2e);
	if (sch5545_emi_get_int_mask_high())
		printk(BIOS_SPEW, "EC interrupt mask MSB is not 0\n");

	sch5545_ec_hwm_early_init();

	if (!s3resume) {
		ec_fw_version = sch5545_get_ec_fw_version();
		printk(BIOS_DEBUG, "SCH5545 EC firmware version %04x\n", ec_fw_version);
		sch5545_update_ec_firmware(ec_fw_version);
	}
	printk(BIOS_DEBUG, "EC early init complete.\n");

	/* Disable SMIs and clear SMI status */
	outb(0, SCH5545_RUNTIME_REG_BASE + SCH5545_RR_SMI_EN);
	outb(SCH5545_SMI_GLOBAL_STS, SCH5545_RUNTIME_REG_BASE + SCH5545_RR_SMI_STS);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[1], 0x51, id_only);
	read_spd(&spd[2], 0x52, id_only);
	read_spd(&spd[3], 0x53, id_only);
}
