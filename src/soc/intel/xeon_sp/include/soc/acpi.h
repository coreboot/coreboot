/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ACPI_H_
#define _SOC_ACPI_H_

#include <acpi/acpi.h>

#define MAX_SRAT_MEM_ENTRIES_PER_IMC	8
#define MAX_ACPI_MEMORY_AFFINITY_COUNT	256

/**
 Select C-state map set in config cstate_states
 **/
enum acpi_cstate_mode {
	CSTATES_ALL = 0,
	CSTATES_C1C6
};

unsigned long northbridge_write_acpi_tables(const struct device *device,
	unsigned long current, struct acpi_rsdp *rsdp);
unsigned long acpi_fill_cedt(unsigned long current);
unsigned long acpi_fill_hmat(unsigned long current);
unsigned long cxl_fill_srat(unsigned long current);

void iio_domain_set_acpi_name(struct device *dev, const char *prefix);

#define PCIE_NATIVE_HOTPLUG_CONTROL  0x01
#define SHPC_NATIVE_HOTPLUG_CONTROL  0x02
#define PCIE_PME_CONTROL             0x04
#define PCIE_AER_CONTROL             0x08
#define PCIE_CAP_STRUCTURE_CONTROL   0x10
#define PCIE_LTR_CONTROL             0x20
#define PCIE_DPC_COTROL              0x80

#define CXL_ERROR_REPORTING_CONTROL  0x01

void acpigen_write_OSC_pci_domain_fixed_caps(const struct device *domain,
				const uint32_t granted_pcie_features,
				const bool is_cxl_domain,
				const uint32_t granted_cxl_features);

#endif /* _SOC_ACPI_H_ */
