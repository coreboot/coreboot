/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/smm.h>
#include <amdblocks/smu.h>
#include <soc/smu.h>
#include <types.h>

/* Tell the SMU to get the USB controllers ready for S3 */
void soc_smi_usb_s3_entry(void)
{
	struct smu_payload msg = { 0 };

	send_smu_message(SMC_MSG_USBS3ENTRY, &msg);
}
