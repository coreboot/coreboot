/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/pi/hudson/hudson.h>

/* Mask BIST bit 31. One result of Silicon Observation
 * report_bist_failure(bist & 0x7FFFFFFF);
 */

static void romstage_main_template(void)
{
	u32 val;

	if (!cpu_init_detectedx && boot_cpu()) {
		post_code(0x30);

#if CONFIG(HUDSON_UART)
		configure_hudson_uart();
#endif
		post_code(0x31);
		console_init();
	}
}

void agesa_postcar(struct sysinfo *cb)
{
	/* After AMD_INIT_ENV -> move to ramstage ? */
	if (acpi_is_wakeup_s4()) {
		outb(0xEE, PM_INDEX);
		outb(0x8, PM_DATA);
	}
}
