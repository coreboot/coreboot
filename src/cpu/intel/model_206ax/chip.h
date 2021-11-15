/* SPDX-License-Identifier: GPL-2.0-only */

/* Keep this in sync with acpi.c */
enum cpu_acpi_level {
	CPU_ACPI_DISABLED = 0,
	CPU_ACPI_C1,
	CPU_ACPI_C2,
	CPU_ACPI_C3,
	CPU_ACPI_C6,
	CPU_ACPI_C7,
	CPU_ACPI_C7S,
};

struct cpu_intel_model_206ax_config {
	enum cpu_acpi_level acpi_c1;
	enum cpu_acpi_level acpi_c2;
	enum cpu_acpi_level acpi_c3;

	int tcc_offset;		/* TCC Activation Offset */
};
