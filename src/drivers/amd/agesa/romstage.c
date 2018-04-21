/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Kyösti Mälkki
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
#include <arch/cpu.h>
#include <cbmem.h>
#include <cpu/amd/car.h>
#include <cpu/x86/bist.h>
#include <compiler.h>
#include <console/console.h>
#include <halt.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <smp/node.h>
#include <string.h>
#include <timestamp.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/state_machine.h>

#if IS_ENABLED(CONFIG_LATE_CBMEM_INIT)
#error "Only EARLY_CBMEM_INIT is supported."
#endif
#if !IS_ENABLED(CONFIG_POSTCAR_STAGE)
#error "Only POSTCAR_STAGE is supported."
#endif
#if HAS_LEGACY_WRAPPER
#error "LEGACY_WRAPPER code not supported"
#endif

void asmlinkage early_all_cores(void)
{
	amd_initmmio();
}

void __weak platform_once(struct sysinfo *cb)
{
	board_BeforeAgesa(cb);
}

static void fill_sysinfo(struct sysinfo *cb)
{
	memset(cb, 0, sizeof(*cb));
	cb->s3resume = acpi_is_wakeup_s3();

	agesa_set_interface(cb);
}

void * asmlinkage romstage_main(unsigned long bist)
{
	struct postcar_frame pcf;
	struct sysinfo romstage_state;
	struct sysinfo *cb = &romstage_state;
	u8 initial_apic_id = (u8) (cpuid_ebx(1) >> 24);
	int cbmem_initted = 0;

	fill_sysinfo(cb);

	if ((initial_apic_id == 0) && boot_cpu()) {

		timestamp_init(timestamp_get());
		timestamp_add_now(TS_START_ROMSTAGE);

		platform_once(cb);

		console_init();
	}

	printk(BIOS_DEBUG, "APIC %02d: CPU Family_Model = %08x\n",
		initial_apic_id, cpuid_eax(1));

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	agesa_execute_state(cb, AMD_INIT_RESET);

	agesa_execute_state(cb, AMD_INIT_EARLY);

	timestamp_add_now(TS_BEFORE_INITRAM);

	if (!cb->s3resume)
		agesa_execute_state(cb, AMD_INIT_POST);
	else
		agesa_execute_state(cb, AMD_INIT_RESUME);

	/* FIXME: Detect if TSC frequency changed during raminit? */
	timestamp_rescale_table(1, 4);
	timestamp_add_now(TS_AFTER_INITRAM);

	/* Work around AGESA setting all memory as WB on normal
	 * boot path.
	 */
	fixup_cbmem_to_UC(cb->s3resume);

	cbmem_initted = !cbmem_recovery(cb->s3resume);

	if (cb->s3resume && !cbmem_initted) {
		printk(BIOS_EMERG, "Unable to recover CBMEM\n");
		halt();
	}

	romstage_handoff_init(cb->s3resume);

	postcar_frame_init(&pcf, HIGH_ROMSTAGE_STACK_SIZE);
	recover_postcar_frame(&pcf, cb->s3resume);

	run_postcar_phase(&pcf);
	/* We do not return. */
	return NULL;
}
