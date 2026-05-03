/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CPU_INTEL_MODEL_2065X_CHIP_H__
#define __CPU_INTEL_MODEL_2065X_CHIP_H__

/* Keep this in sync with acpi.c */
enum cpu_acpi_level {
	CPU_ACPI_DISABLED = 0,
	CPU_ACPI_C1,
	CPU_ACPI_C2,
	CPU_ACPI_C3,
	CPU_ACPI_C6,
	CPU_ACPI_C7,
};

struct cpu_intel_model_2065x_config {
	int tcc_offset;		/* TCC Activation Offset */
};

#endif /* __CPU_INTEL_MODEL_2065X_CHIP_H__ */
