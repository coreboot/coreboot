/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_GNVS_H__
#define __ACPI_GNVS_H__

#include <types.h>

void *acpi_get_gnvs(void);
void *gnvs_get_or_create(void);
void acpi_inject_nvsa(void);

void gnvs_assign_chromeos(void);
void gnvs_set_ecfw_rw(void);

/* Platform code must implement these. */
struct global_nvs;
size_t gnvs_size_of_array(void);
uint32_t *gnvs_cbmc_ptr(struct global_nvs *gnvs);
void *gnvs_chromeos_ptr(struct global_nvs *gnvs);

/*
 * Creates acpi gnvs and adds it to the DSDT table.
 * GNVS creation is chipset specific and is done in soc specific acpi.c file.
 */
struct device;
void southbridge_inject_dsdt(const struct device *device);

/*
 * This function populates the gnvs structure in acpi table.
 * Defined as weak in common acpi as gnvs structure definition is
 * chipset specific.
 */

void acpi_create_gnvs(struct global_nvs *gnvs);

void soc_fill_gnvs(struct global_nvs *gnvs);
void mainboard_fill_gnvs(struct global_nvs *gnvs);

#endif
