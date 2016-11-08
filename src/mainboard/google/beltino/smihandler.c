/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright 2012 Google Inc.
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

#include <arch/acpi.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <cpu/x86/smm.h>
#include <northbridge/intel/haswell/haswell.h>
#include <southbridge/intel/lynxpoint/me.h>
#include <southbridge/intel/lynxpoint/nvs.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <elog.h>
#include <superio/ite/it8772f/it8772f.h>
#include "onboard.h"

static int mainboard_finalized = 0;

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APM_CNT_FINALIZE:
		if (mainboard_finalized) {
			printk(BIOS_DEBUG, "SMI#: Already finalized\n");
			return 0;
		}

		intel_pch_finalize_smm();
		intel_northbridge_haswell_finalize_smm();
		intel_cpu_haswell_finalize_smm();

		mainboard_finalized = 1;
		break;
	default:
		break;
	}
	return 0;
}

void mainboard_smi_sleep(u8 slp_typ)
{
	switch (slp_typ) {
	case ACPI_S3:
		set_power_led(LED_BLINK);
		break;
	case ACPI_S4:
	case ACPI_S5:
		set_power_led(LED_OFF);
		break;
	default:
		break;
	}
	return;
}
