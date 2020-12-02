/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ACPI_H_
#define _SOC_ACPI_H_

#include <acpi/acpi.h>
#include <soc/nvs.h>

#define MEM_BLK_COUNT      0x140
typedef struct {
	uint8_t   buf[32];
} MEM_BLK;

/* P-state configuration */
#define PSS_MAX_ENTRIES			16
#define PSS_RATIO_STEP			1
#define PSS_LATENCY_TRANSITION		10
#define PSS_LATENCY_BUSMASTER		10

unsigned long northbridge_write_acpi_tables(const struct device *device,
	unsigned long current, struct acpi_rsdp *rsdp);
unsigned long southbridge_write_acpi_tables(const struct device *device,
	unsigned long current, struct acpi_rsdp *rsdp);
uint32_t soc_read_sci_irq_select(void);
int soc_madt_sci_irq_polarity(int sci);
void soc_power_states_generation(int core, int cores_per_package);
acpi_tstate_t *soc_get_tss_table(int *entries);
acpi_cstate_t *soc_get_cstate_map(size_t *entries);
int calculate_power(int tdp, int p1_ratio, int ratio);
void uncore_inject_dsdt(const struct device *device);
unsigned long xeonsp_acpi_create_madt_lapics(unsigned long current);

#endif /* _SOC_ACPI_H_ */
