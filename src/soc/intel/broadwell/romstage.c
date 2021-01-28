/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/romstage.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <elog.h>
#include <romstage_handoff.h>
#include <soc/gpio.h>
#include <soc/me.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <stdint.h>
#include <timestamp.h>

__weak void mainboard_fill_spd_data(struct pei_data *pei_data)
{
}

__weak void mainboard_post_raminit(const int s3resume)
{
}

/* Entry from cpu/intel/car/romstage.c. */
void mainboard_romstage_entry(void)
{
	struct pei_data pei_data = { 0 };

	post_code(0x30);

	/* System Agent Early Initialization */
	systemagent_early_init();

	/* PCH Early Initialization */
	pch_early_init();

	/* Get power state */
	struct chipset_power_state *const power_state = fill_power_state();

	elog_boot_notify(power_state->prev_sleep_state == ACPI_S3);

	/* Print useful platform information */
	report_platform_info();

	/* Set CPU frequency to maximum */
	set_max_freq();

	/* Initialize GPIOs */
	setup_pch_lp_gpios(mainboard_gpio_map);

	mainboard_fill_pei_data(&pei_data);
	mainboard_fill_spd_data(&pei_data);

	post_code(0x32);

	timestamp_add_now(TS_BEFORE_INITRAM);

	pei_data.boot_mode = power_state->prev_sleep_state;

	/* Print ME state before MRC */
	intel_me_status();

	/* Save ME HSIO version */
	intel_me_hsio_version(&power_state->hsio_version,
			      &power_state->hsio_checksum);

	/* Initialize RAM */
	sdram_initialize(&pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);

	if (pei_data.boot_mode != ACPI_S3) {
		cbmem_initialize_empty();
	} else if (cbmem_initialize()) {
		printk(BIOS_DEBUG, "Failed to recover CBMEM in S3 resume.\n");
		/* Failed S3 resume, reset to come up cleanly */
		system_reset();
	}

	save_mrc_data(&pei_data);

	setup_sdram_meminfo(&pei_data);

	romstage_handoff_init(power_state->prev_sleep_state == ACPI_S3);

	mainboard_post_raminit(power_state->prev_sleep_state == ACPI_S3);
}
