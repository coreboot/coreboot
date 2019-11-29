/*
 * This file is part of the coreboot project.
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
#include <arch/romstage.h>
#include <bootblock_common.h>
#include <cbmem.h>
#include <console/console.h>
#include <halt.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <smp/node.h>
#include <string.h>
#include <timestamp.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/state_machine.h>

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

static void romstage_main(void)
{
	struct postcar_frame pcf;
	struct sysinfo romstage_state;
	struct sysinfo *cb = &romstage_state;
	u8 initial_apic_id = (u8) (cpuid_ebx(1) >> 24);
	int cbmem_initted = 0;

	/* Enable PCI MMIO configuration. */
	amd_initmmio();

	fill_sysinfo(cb);

	if (initial_apic_id == 0) {

		timestamp_init(timestamp_get());
		timestamp_add_now(TS_START_ROMSTAGE);

		platform_once(cb);

		console_init();
	}

	printk(BIOS_DEBUG, "APIC %02d: CPU Family_Model = %08x\n",
		initial_apic_id, cpuid_eax(1));

	agesa_execute_state(cb, AMD_INIT_RESET);

	agesa_execute_state(cb, AMD_INIT_EARLY);

	timestamp_add_now(TS_BEFORE_INITRAM);

	if (!cb->s3resume)
		agesa_execute_state(cb, AMD_INIT_POST);
	else
		agesa_execute_state(cb, AMD_INIT_RESUME);

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
}

static void ap_romstage_main(void)
{
	struct sysinfo romstage_state;
	struct sysinfo *cb = &romstage_state;

	/* Enable PCI MMIO configuration. */
	amd_initmmio();

	fill_sysinfo(cb);

	agesa_execute_state(cb, AMD_INIT_RESET);

	agesa_execute_state(cb, AMD_INIT_EARLY);

	/* Not reached. */
	halt();
}

/* This wrapper enables easy transition away from ROMCC_BOOTBLOCK
 * keeping changes in cache_as_ram.S easy to manage.
 */
asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	romstage_main();
}

asmlinkage void ap_bootblock_c_entry(void)
{
	ap_romstage_main();
}
