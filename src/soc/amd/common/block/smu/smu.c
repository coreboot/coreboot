/* SPDX-License-Identifier: GPL-2.0-only */

#include <timer.h>
#include <console/console.h>
#include <amdblocks/smn.h>
#include <amdblocks/smu.h>
#include <soc/smu.h>
#include <thread.h>
#include <types.h>

/* returns SMU_MESG_RESP_OK, SMU_MESG_RESP_TIMEOUT or a negative number */
static int32_t smu_poll_response(bool print_command_duration)
{
	struct stopwatch sw;
	const long timeout_ms = 10 * MSECS_PER_SEC;
	int32_t result;

	stopwatch_init_msecs_expire(&sw, timeout_ms);

	while (1) {
		result = smn_read32(SMN_SMU_MESG_RESP);
		if (result)
			break;

		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "timeout sending SMU message\n");
			return SMU_MESG_RESP_TIMEOUT;
		}
		thread_yield();
	}

	if (print_command_duration)
		printk(BIOS_SPEW, "SMU command consumed %lld usecs\n",
		       stopwatch_duration_usecs(&sw));
	return result;
}

/*
 * Send a message and bi-directional payload to the SMU. SMU response, if any, is returned via
 * *arg.
 */
int32_t send_smu_message_raw(enum smu_message_id message_id, struct smu_payload *arg)
{
	int32_t response;
	size_t i;

	/* Wait until the SMU can process a new request; an old failed response is harmless. */
	if (smu_poll_response(false) == SMU_MESG_RESP_TIMEOUT)
		return SMU_MESG_RESP_TIMEOUT;

	/* Clear response register */
	smn_write32(SMN_SMU_MESG_RESP, 0);

	/* Populate arguments */
	for (i = 0; i < SMU_NUM_ARGS; i++)
		smn_write32(SMN_SMU_MESG_ARG(i), arg->msg[i]);

	/* Send message to SMU */
	smn_write32(SMN_SMU_MESG_ID, message_id);

	/* Wait until the SMU has processed the message */
	response = smu_poll_response(true);
	if (response == SMU_MESG_RESP_TIMEOUT)
		return SMU_MESG_RESP_TIMEOUT;

	/* Copy returned values, even when the response isn't SMU_MESG_RESP_OK */
	for (i = 0; i < SMU_NUM_ARGS; i++)
		arg->msg[i] = smn_read32(SMN_SMU_MESG_ARG(i));

	return response;
}

enum cb_err send_smu_message(enum smu_message_id message_id, struct smu_payload *arg)
{
	struct smu_payload response = *arg;

	if (send_smu_message_raw(message_id, &response) != SMU_MESG_RESP_OK)
		return CB_ERR;

	*arg = response;
	return CB_SUCCESS;
}
