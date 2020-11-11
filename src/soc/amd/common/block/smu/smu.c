/* SPDX-License-Identifier: GPL-2.0-only */

#include <timer.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <amdblocks/smu.h>
#include <soc/pci_devs.h>
#include <soc/smu.h>
#include <types.h>

static uint32_t smu_read32(uint32_t reg)
{
	pci_write_config32(SOC_GNB_DEV, SMU_INDEX_ADDR, reg);
	return pci_read_config32(SOC_GNB_DEV, SMU_DATA_ADDR);
}

static void smu_write32(uint32_t reg, uint32_t val)
{
	pci_write_config32(SOC_GNB_DEV, SMU_INDEX_ADDR, reg);
	pci_write_config32(SOC_GNB_DEV, SMU_DATA_ADDR, val);
}

#define SMU_MESG_RESP_TIMEOUT	0x00
#define SMU_MESG_RESP_OK	0x01

/* returns SMU_MESG_RESP_OK, SMU_MESG_RESP_TIMEOUT or a negative number */
static int32_t smu_poll_response(bool print_command_duration)
{
	struct stopwatch sw;
	const long timeout_ms = 10 * MSECS_PER_SEC;
	int32_t result;

	stopwatch_init_msecs_expire(&sw, timeout_ms);

	do {
		result = smu_read32(REG_ADDR_MESG_RESP);
		if (result) {
			if (print_command_duration)
				printk(BIOS_SPEW, "SMU command consumed %ld usecs\n",
					stopwatch_duration_usecs(&sw));
			return result;
		}
	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "Error: timeout sending SMU message\n");
	return SMU_MESG_RESP_TIMEOUT;
}

/*
 * Send a message and bi-directional payload to the SMU. SMU response, if any, is returned via
 * *arg.
 */
enum cb_err send_smu_message(enum smu_message_id message_id, struct smu_payload *arg)
{
	size_t i;

	/* wait until SMU can process a new request; don't care if an old request failed */
	if (smu_poll_response(false) == SMU_MESG_RESP_TIMEOUT)
		return CB_ERR;

	/* clear response register */
	smu_write32(REG_ADDR_MESG_RESP, 0);

	/* populate arguments */
	for (i = 0 ; i < SMU_NUM_ARGS ; i++)
		smu_write32(REG_ADDR_MESG_ARG(i), arg->msg[i]);

	/* send message to SMU */
	smu_write32(REG_ADDR_MESG_ID, message_id);

	/* wait until SMU has processed the message and check if it was successful */
	if (smu_poll_response(true) != SMU_MESG_RESP_OK)
		return CB_ERR;

	/* copy returned values */
	for (i = 0 ; i < SMU_NUM_ARGS ; i++)
		arg->msg[i] = smu_read32(REG_ADDR_MESG_ARG(i));

	return CB_SUCCESS;
}
