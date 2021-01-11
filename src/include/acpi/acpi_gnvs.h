/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_GNVS_H__
#define __ACPI_GNVS_H__

#include <types.h>

void acpi_create_gnvs(void);
#if CONFIG(ACPI_SOC_NVS)
void *acpi_get_gnvs(void);
#else
static inline void *acpi_get_gnvs(void) { return NULL; }
#endif

void gnvs_assign_chromeos(void *gnvs_section);
void gnvs_set_ecfw_rw(void);

/*
 * These functions populate the gnvs structure in acpi table.
 * Defined as weak in common acpi as gnvs structure definition is
 * chipset specific.
 */
struct global_nvs;
void soc_fill_gnvs(struct global_nvs *gnvs);
void mainboard_fill_gnvs(struct global_nvs *gnvs);

#endif
