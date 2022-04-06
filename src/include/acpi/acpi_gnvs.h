/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_GNVS_H__
#define __ACPI_GNVS_H__

#include <types.h>

struct global_nvs;

size_t size_of_dnvs(void);

#if CONFIG(ACPI_SOC_NVS)
void *acpi_get_gnvs(void);
void *acpi_get_device_nvs(void);
int acpi_reset_gnvs_for_wake(struct global_nvs **gnvs);
#else
static inline void *acpi_get_gnvs(void) { return NULL; }
static inline int acpi_reset_gnvs_for_wake(struct global_nvs **gnvs) { return -1; }
#endif

/*
 * These functions populate the gnvs structure in acpi table.
 * Defined as weak in common acpi as gnvs structure definition is
 * chipset specific.
 */
void soc_fill_gnvs(struct global_nvs *gnvs);
void mainboard_fill_gnvs(struct global_nvs *gnvs);

#endif
