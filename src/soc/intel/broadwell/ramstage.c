/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <romstage_handoff.h>
#include <stdlib.h>
#include <broadwell/nvs.h>
#include <broadwell/pm.h>
#include <broadwell/ramstage.h>
#include <chip.h>

/* Save bit index for first enabled event in PM1_STS for \_SB._SWS */
static void s3_save_acpi_wake_source(global_nvs_t *gnvs)
{
	struct chipset_power_state *ps = cbmem_find(CBMEM_ID_POWER_STATE);
	uint16_t pm1;

	if (!ps)
		return;

	pm1 = ps->pm1_sts & ps->pm1_en;

	/* Scan for first set bit in PM1 */
	for (gnvs->pm1i = 0; gnvs->pm1i < 16; gnvs->pm1i++) {
		if (pm1 & 1)
			break;
		pm1 >>= 1;
	}

	/* If unable to determine then return -1 */
	if (gnvs->pm1i >= 16)
		gnvs->pm1i = -1;

	printk(BIOS_DEBUG, "ACPI System Wake Source is PM1 Index %d\n",
	       gnvs->pm1i);
}

static inline void set_acpi_sleep_type(int val)
{
#if CONFIG_HAVE_ACPI_RESUME
	acpi_slp_type = val;
#endif
}

static void s3_resume_prepare(void)
{
        global_nvs_t *gnvs;
        struct romstage_handoff *romstage_handoff;

        gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(global_nvs_t));

        romstage_handoff = cbmem_find(CBMEM_ID_ROMSTAGE_INFO);
        if (romstage_handoff == NULL || romstage_handoff->s3_resume == 0) {
                if (gnvs != NULL) {
                        memset(gnvs, 0, sizeof(global_nvs_t));
                }
                set_acpi_sleep_type(0);
                return;
        }

        set_acpi_sleep_type(3);
        s3_save_acpi_wake_source(gnvs);
}

void broadwell_init_pre_device(void *chip_info)
{
	s3_resume_prepare();
	broadwell_run_reference_code();
}
