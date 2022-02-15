/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/biosram.h>
#include <acpi/acpi.h>
#include <arch/cpu.h>
#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/lapic.h>
#include <halt.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <smp/node.h>
#include <string.h>
#include <timestamp.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/state_machine.h>

void __weak board_BeforeAgesa(struct sysinfo *cb) { }

static void fill_sysinfo(struct sysinfo *cb)
{
	memset(cb, 0, sizeof(*cb));
	cb->s3resume = acpi_is_wakeup_s3();

	agesa_set_interface(cb);
}

/* APs will enter directly here from bootblock, bypassing verstage
 * and potential fallback / normal bootflow detection.
 */
static void ap_romstage_main(void);

static void romstage_main(void)
{
	struct postcar_frame pcf;
	struct sysinfo romstage_state;
	struct sysinfo *cb = &romstage_state;
	int cbmem_initted = 0;

	fill_sysinfo(cb);

	timestamp_add_now(TS_ROMSTAGE_START);

	board_BeforeAgesa(cb);

	console_init();

	printk(BIOS_DEBUG, "APIC %02u: CPU Family_Model = %08x\n",
	       initial_lapicid(), cpuid_eax(1));

	set_ap_entry_ptr(ap_romstage_main);

	agesa_execute_state(cb, AMD_INIT_RESET);

	agesa_execute_state(cb, AMD_INIT_EARLY);

	timestamp_add_now(TS_INITRAM_START);

	if (!cb->s3resume)
		agesa_execute_state(cb, AMD_INIT_POST);
	else
		agesa_execute_state(cb, AMD_INIT_RESUME);

	timestamp_add_now(TS_INITRAM_END);

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

	fill_sysinfo(cb);

	agesa_execute_state(cb, AMD_INIT_RESET);

	agesa_execute_state(cb, AMD_INIT_EARLY);

	/* Not reached. */
	halt();
}

asmlinkage void car_stage_entry(void)
{
	romstage_main();
}

void *cbmem_top_chipset(void)
{
	/* Top of CBMEM is at highest usable DRAM address below 4GiB. */
	return (void *)restore_top_of_low_cacheable();
}
