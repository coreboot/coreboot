/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
int soc_fill_acpi_wake(uint32_t *pm1, uint32_t **gpe0);

#if IS_ENABLED(CONFIG_SOC_INTEL_COMMON_ACPI)

/*
 * cpu_get_bus_clock returns the bus clock frequency in KHz.
 * This is the value the clock ratio is multiplied with.
 */
uint32_t cpu_get_bus_clock(void);

/*
 * cpu_get_coord_type returns coordination type (SW_ANY or SW_ALL or HW_ALL)
 * which is used to populate _PSD object.
 */
int cpu_get_coord_type(void);

/*
 * cpu_config_tdp_levels returns the number of TDP levels supported
 * by this processor
 */
int cpu_config_tdp_levels(void);

/*
 * cpu_get_min_ratio returns the minimum frequency ratio that is supported
 * by this processor
 */
uint32_t cpu_get_min_ratio(void);

/*
 * cpu_get_max_ratio returns the nominal TDP ratio if available or the
 * maximum non turbo frequency ratio for this processor
 */
uint32_t cpu_get_max_ratio(void);

/*
 * cpu_get_power_max calculates CPU TDP in mW
 */
uint32_t cpu_get_power_max(void);

/*
 * cpu_get_max_turbo_ratio returns the maximum turbo ratio limit for the
 * processor
 */
uint32_t cpu_get_max_turbo_ratio(void);

/*
 * get_cstate_map returns a table of processor specific acpi_cstate_t entries
 * and number of entries in the table
 */
acpi_cstate_t *soc_get_cstate_map(int *num_entries);

/*
 * get_soc_tss_table returns a table of processor specific acpi_tstate_t entries
 * and number of entries in the table
 */
acpi_tstate_t *soc_get_tss_table(int *num_entries);

/*
 * soc_get_acpi_base_address returns the ACPI base address for the SOC
 */
uint16_t soc_get_acpi_base_address(void);

#endif /* CONFIG_SOC_INTEL_COMMON_ACPI */
#endif
