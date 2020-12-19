/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_GNVS_H__
#define __ACPI_GNVS_H__

#include <types.h>

void *acpi_get_gnvs(void);
void *gnvs_get_or_create(void);

void gnvs_assign_chromeos(void *gnvs_section);
void gnvs_set_ecfw_rw(void);

/* Platform code must implement these. */
struct global_nvs;
size_t gnvs_size_of_array(void);
uint32_t *gnvs_cbmc_ptr(struct global_nvs *gnvs);

/*
 * These functions populate the gnvs structure in acpi table.
 * Defined as weak in common acpi as gnvs structure definition is
 * chipset specific.
 */
void soc_fill_gnvs(struct global_nvs *gnvs);
void mainboard_fill_gnvs(struct global_nvs *gnvs);

#endif
