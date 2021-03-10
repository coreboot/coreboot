/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_SMU_H
#define AMD_BLOCK_SMU_H

#include <types.h>
#include <soc/smu.h> /* SoC-dependent definitions for SMU access */

/* Arguments indexed locations are contiguous; the number is SoC-dependent */
#define REG_ADDR_MESG_ARG(x)	(REG_ADDR_MESG_ARGS_BASE + ((x) * sizeof(uint32_t)))

struct smu_payload {
	uint32_t msg[SMU_NUM_ARGS];
};

/*
 * Send a message and bi-directional payload to the SMU. The SMU's response, if any, is
 * returned via *arg. Returns CB_SUCCESS if success or CB_ERR on failure.
 */
enum cb_err send_smu_message(enum smu_message_id message_id, struct smu_payload *arg);

#endif /* AMD_BLOCK_SMU_H */
