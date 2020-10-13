/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <elog.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <timestamp.h>
#include <soc/gpio.h>
#include <soc/me.h>
#include <soc/pei_data.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/spi.h>

/* Entry from cpu/intel/car/romstage.c. */
void mainboard_romstage_entry(void)
{
	struct romstage_params rp = { 0 };

	post_code(0x30);

	/* System Agent Early Initialization */
	systemagent_early_init();

	/* PCH Early Initialization */
	pch_early_init();

	/* Get power state */
	rp.power_state = fill_power_state();

	elog_boot_notify(rp.power_state->prev_sleep_state == ACPI_S3);

	/* Print useful platform information */
	report_platform_info();

	/* Set CPU frequency to maximum */
	set_max_freq();

	/* Initialize GPIOs */
	init_gpios(mainboard_gpio_config);

	/* Fill in mainboard pei_date. */
	mainboard_pre_raminit(&rp);

	post_code(0x32);

	timestamp_add_now(TS_BEFORE_INITRAM);

	rp.pei_data.boot_mode = rp.power_state->prev_sleep_state;

	/* Print ME state before MRC */
	intel_me_status();

	/* Save ME HSIO version */
	intel_me_hsio_version(&rp.power_state->hsio_version,
			      &rp.power_state->hsio_checksum);

	/* Initialize RAM */
	raminit(&rp.pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);

	romstage_handoff_init(rp.power_state->prev_sleep_state == ACPI_S3);

	mainboard_post_raminit(&rp);
}
