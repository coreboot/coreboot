/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdbool.h>
#include <stdint.h>

struct cpu_vr_config {
	/*
	 * Minimum voltage for C6/C7 state:
	 * 0x67 = 1.6V (full swing)
	 *  ...
	 * 0x79 = 1.7V
	 *  ...
	 * 0x83 = 1.8V (no swing)
	 */
	uint8_t cpu_min_vid;

	/*
	 * Set slow VR ramp rate on C-state exit:
	 * 0 = Fast VR ramp rate / 2
	 * 1 = Fast VR ramp rate / 4
	 * 2 = Fast VR ramp rate / 8
	 * 3 = Fast VR ramp rate / 16
	 */
	uint8_t slow_ramp_rate_set;

	/* Enable slow VR ramp rate */
	bool slow_ramp_rate_enable;
};

struct cpu_intel_haswell_config {
	int tcc_offset;		/* TCC Activation Offset */

	struct cpu_vr_config vr_config;

	/* Enable S0iX support */
	bool s0ix_enable;
};
