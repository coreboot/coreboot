/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <amdblocks/smu.h>
#include <soc/smu.h>

/*
 * Request the SMU to put system into S3, S4, or S5. On entry, SlpTyp determines S-State and
 * SlpTypeEn gets set by the SMU. Function does not return if successful.
 */
void smu_sx_entry(void)
{
	struct smu_payload msg = { 0 }; /* Unused for SMC_MSG_S3ENTRY */

	printk(BIOS_DEBUG, "SMU: Put system into S3/S4/S5\n");
	send_smu_message(SMC_MSG_S3ENTRY, &msg);
}
