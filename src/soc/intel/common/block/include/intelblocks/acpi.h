/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
 * Copyright (C) 2017 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_BLOCK_ACPI_H
#define SOC_INTEL_COMMON_BLOCK_ACPI_H

#include <arch/acpi.h>
#include <device/device.h>
#include <intelblocks/cpulib.h>
#include <stdint.h>

/* Forward declare the power state struct here */
struct chipset_power_state;

/* Forward  declare the global nvs structure here */
struct global_nvs_t;

/* Return ACPI name for this device */
const char *soc_acpi_name(const struct device *dev);

/* Read the scis from soc specific register. Returns int scis value */
uint32_t soc_read_sci_irq_select(void);

/* Write the scis from soc specific register. */
void soc_write_sci_irq_select(uint32_t scis);

/*
 * Calls acpi_write_hpet which creates and fills HPET table and
 * adds it to the RSDT (and XSDT) structure.
 */
unsigned long southbridge_write_acpi_tables(device_t device,
					    unsigned long current,
					    struct acpi_rsdp *rsdp);

/*
 * Creates acpi gnvs and adds it to the DSDT table.
 * GNVS creation is chipset specific and is done in soc specific acpi.c file.
 */
void southbridge_inject_dsdt(device_t device);

/*
 * This function populates the gnvs structure in acpi table.
 * Defined as weak in common acpi as gnvs structure definition is
 * chipset specific.
 */
void acpi_create_gnvs(struct global_nvs_t *gnvs);

/*
 * get_cstate_map returns a table of processor specific acpi_cstate_t entries
 * and number of entries in the table
 */
acpi_cstate_t *soc_get_cstate_map(size_t *num_entries);

/*
 * Chipset specific quirks for the wake enable bits.
 * Returns wake events for the soc.
 */
uint32_t acpi_fill_soc_wake(uint32_t generic_pm1_en,
			    const struct chipset_power_state *ps);

/* Chipset specific settings for filling up fadt table */
void soc_fill_fadt(acpi_fadt_t *fadt);

/* Return the polarity flag for SCI IRQ */
int soc_madt_sci_irq_polarity(int sci);

/* Generate P-state tables */
void generate_p_state_entries(int core, int cores_per_package);

/* Generate T-state tables */
void generate_t_state_entries(int core, int cores_per_package);

/*
 * soc specific power states generation. We need this to be defined by soc
 * as the state generations varies in chipsets e.g. APL generates T and P
 * states while SKL generates  * P state only depending on a devicetree config
 */
void soc_power_states_generation(int core_id, int cores_per_package);

#endif				/* _SOC_INTEL_COMMON_BLOCK_ACPI_H_ */
