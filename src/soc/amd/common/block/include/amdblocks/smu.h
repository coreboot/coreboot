/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_SMU_H
#define AMD_BLOCK_SMU_H

#include <types.h>
#include <soc/smu.h> /* SoC-dependent definitions for SMU access */

/* Arguments indexed locations are contiguous; the number is SoC-dependent */
#define SMN_SMU_MESG_ARG(x)	(SMN_SMU_MESG_ARGS_BASE + ((x) * sizeof(uint32_t)))

#define SMU_MESG_RESP_TIMEOUT	0x00
#define SMU_MESG_RESP_OK	0x01

struct smu_payload {
	uint32_t msg[SMU_NUM_ARGS];
};

/*
 * Send a message and bi-directional payload to the SMU. The raw variant updates the payload
 * for any completed command and returns the firmware response or SMU_MESG_RESP_TIMEOUT.
 * The checked variant updates the payload only on the standard success response of 1;
 * on error it leaves the caller's payload unchanged.
 */
int32_t send_smu_message_raw(enum smu_message_id message_id, struct smu_payload *arg);
enum cb_err send_smu_message(enum smu_message_id message_id, struct smu_payload *arg);

#endif /* AMD_BLOCK_SMU_H */
