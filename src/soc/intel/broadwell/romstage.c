/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/romstage.h>
#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <elog.h>
#include <romstage_handoff.h>
#include <soc/me.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include <stdint.h>

__weak void mainboard_post_raminit(const int s3resume)
{
}

/* Entry from cpu/intel/car/romstage.c. */
void mainboard_romstage_entry(void)
{
	post_code(0x30);

	/* System Agent Early Initialization */
	systemagent_early_init();

	/* PCH Early Initialization */
	pch_early_init();

	/* Get power state */
	struct chipset_power_state *const power_state = fill_power_state();

	int s3resume = power_state->prev_sleep_state == ACPI_S3;

	elog_boot_notify(s3resume);

	/* Print useful platform information */
	report_platform_info();

	/* Set CPU frequency to maximum */
	set_max_freq();

	/* Initialize GPIOs */
	setup_pch_lp_gpios(mainboard_lp_gpio_map);

	/* Print ME state before MRC */
	intel_me_status();

	/* Save ME HSIO version */
	intel_me_hsio_version(&power_state->hsio_version,
			      &power_state->hsio_checksum);

	perform_raminit(power_state);

	romstage_handoff_init(s3resume);

	mainboard_post_raminit(s3resume);
}
