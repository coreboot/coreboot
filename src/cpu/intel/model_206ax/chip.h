/* SPDX-License-Identifier: GPL-2.0-only */

/* Magic value used to locate this chip in the device tree */
#define SPEEDSTEP_APIC_MAGIC 0xACAC

struct cpu_intel_model_206ax_config {
	int acpi_c1;		/* ACPI C1 */
	int acpi_c2;		/* ACPI C2 */
	int acpi_c3;		/* ACPI C3 */

	int tcc_offset;		/* TCC Activation Offset */
};
