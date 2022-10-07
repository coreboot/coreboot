/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <cpu/x86/save_state.h>
#include <cpu/x86/smm.h>
#include <types.h>

/* These are weakly linked such that platforms can link only the save state
   ops they actually require. */
const struct smm_save_state_ops *legacy_ops __weak = NULL;
const struct smm_save_state_ops *em64t100_ops __weak = NULL;
const struct smm_save_state_ops *em64t101_ops __weak = NULL;
const struct smm_save_state_ops *amd64_ops __weak = NULL;

static const struct smm_save_state_ops *save_state;

/* Returns -1 on failure, 0 on success */
static int init_save_state(void)
{
	const uint32_t revision = smm_revision();
	int i;
	static bool initialized = false;
	const struct smm_save_state_ops *save_state_ops[] = {
		legacy_ops,
		em64t100_ops,
		em64t101_ops,
		amd64_ops,
	};

	if (initialized)
		return 0;

	for (i = 0; i < ARRAY_SIZE(save_state_ops); i++) {
		const struct smm_save_state_ops *ops = save_state_ops[i];
		const uint32_t *rev;

		if (ops == NULL)
			continue;

		for (rev = ops->revision_table; *rev != SMM_REV_INVALID; rev++)
			if (*rev == revision) {
				save_state = ops;
				initialized = true;
				return 0;
			}
	}

	return -1;
}

int get_apmc_node(u8 cmd)
{
	if (init_save_state())
		return -1;

	return save_state->apmc_node(cmd);
}

int get_save_state_reg(const enum cpu_reg reg, const int node, void *out, const uint8_t length)
{
	if (init_save_state())
		return -1;

	if (node > CONFIG_MAX_CPUS)
		return -1;

	return save_state->get_reg(reg, node, out, length);
}

int set_save_state_reg(const enum cpu_reg reg, const int node, void *in, const uint8_t length)
{
	if (init_save_state())
		return -1;

	if (node > CONFIG_MAX_CPUS)
		return -1;

	return save_state->set_reg(reg, node, in, length);
}
