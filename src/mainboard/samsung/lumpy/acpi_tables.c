/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

#include <types.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <arch/acpigen.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <ec/acpi/ec.h>
#if IS_ENABLED(CONFIG_CHROMEOS)
#include <vendorcode/google/chromeos/gnvs.h>
#endif
#include <southbridge/intel/bd82x6x/nvs.h>

#include "thermal.h"

static global_nvs_t *gnvs_;

static void acpi_update_thermal_table(global_nvs_t *gnvs)
{
	gnvs->f4of = FAN4_THRESHOLD_OFF;
	gnvs->f4on = FAN4_THRESHOLD_ON;

	gnvs->f3of = FAN3_THRESHOLD_OFF;
	gnvs->f3on = FAN3_THRESHOLD_ON;

	gnvs->f2of = FAN2_THRESHOLD_OFF;
	gnvs->f2on = FAN2_THRESHOLD_ON;

	gnvs->f1of = FAN1_THRESHOLD_OFF;
	gnvs->f1on = FAN1_THRESHOLD_ON;

	gnvs->f0of = FAN0_THRESHOLD_OFF;
	gnvs->f0on = FAN0_THRESHOLD_ON;

	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
	gnvs->tmax = MAX_TEMPERATURE;
	gnvs->flvl = 5;
}

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	gnvs_ = gnvs;

	/*
	 * Disable 3G in suspend by default.
	 * Provide option to enable for http://crosbug.com/p/7925
	 */
	gnvs->s33g = 0;

	/* Disable USB ports in S3 by default */
	gnvs->s3u0 = 0;
	gnvs->s3u1 = 0;

	/* Disable USB ports in S5 by default */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;


	acpi_update_thermal_table(gnvs);

	gnvs->chromeos.vbt2 = ec_read(0xcb) ? ACTIVE_ECFW_RW : ACTIVE_ECFW_RO;
}
