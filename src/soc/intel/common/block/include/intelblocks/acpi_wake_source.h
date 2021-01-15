/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _INTEL_COMMON_ACPI_H_
#define _INTEL_COMMON_ACPI_H_

#include <stdint.h>

/*
 * SOC specific handler to provide the wake source data for ACPI _SWS.
 *
 * @pm1:  PM1_STS register with only enabled events set
 * @gpe0: GPE0_STS registers with only enabled events set
 *
 * return the number of registers in the gpe0 array or -1 if nothing
 * is provided by this function.
 */
int soc_fill_acpi_wake(const struct chipset_power_state *ps, uint32_t *pm1, uint32_t **gpe0);

#endif /* _INTEL_COMMON_ACPI_H_ */
