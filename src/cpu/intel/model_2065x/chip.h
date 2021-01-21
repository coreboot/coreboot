/* SPDX-License-Identifier: GPL-2.0-only */

/* Magic value used to locate this chip in the device tree */
#define SPEEDSTEP_APIC_MAGIC 0xACAC

struct cpu_intel_model_2065x_config {
	int tcc_offset;		/* TCC Activation Offset */
};
