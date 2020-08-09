/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CPU_X86_SAVE_STATE_H__
#define __CPU_X86_SAVE_STATE_H__

#include <stdint.h>

enum cpu_reg {
	RAX,
	RBX,
	RCX,
	RDX
};

#define SMM_REV_INVALID 0xffffffff

struct smm_save_state_ops {
	const uint32_t *revision_table;
	/* Accessors for CPU registers in the SMM save state
	   Returns -1 on failure, 0 on success */
	int (*get_reg)(const enum cpu_reg reg, const int node, void *out, const uint8_t length);
	int (*set_reg)(const enum cpu_reg reg, const int node, void *in, const uint8_t length);
	/* Returns -1 on failure, the node on which the 'cmd' was send on success */
	int (*apmc_node)(u8 cmd);
};

/* Return -1 on failure, otherwise returns which CPU node issued an APMC IO write */
int get_apmc_node(u8 cmd);
/* Return -1 on failure, 0 on succes.
   Accessors for the SMM save state CPU registers RAX, RBX, RCX and RDX */
int get_save_state_reg(const enum cpu_reg reg, const int node, void *out, const uint8_t length);
int set_save_state_reg(const enum cpu_reg reg, const int node, void *in, const uint8_t length);

#endif /* __CPU_X86_SAVE_STATE_H__ */
