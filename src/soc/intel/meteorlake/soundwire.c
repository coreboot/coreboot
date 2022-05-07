/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_soundwire.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/soundwire.h>
#include <drivers/intel/soundwire/soundwire.h>
#include <intelblocks/pmclib.h>
#include <soc/pmc.h>
#include <stddef.h>
#include <string.h>

static const struct soundwire_link link_xtal_38_4 = {
	.clock_stop_mode0_supported = 1,
	.clock_stop_mode1_supported = 1,
	.clock_frequencies_supported_count = 1,
	.clock_frequencies_supported = { 4800 * KHz },
	.default_frame_rate = 48 * KHz,
	.default_frame_row_size = 50,
	.default_frame_col_size = 4,
	.dynamic_frame_shape = 1,
	.command_error_threshold = 16,
};

static const struct soundwire_link link_xtal_24 = {
	.clock_stop_mode0_supported = 1,
	.clock_stop_mode1_supported = 1,
	.clock_frequencies_supported_count = 1,
	.clock_frequencies_supported = { 6 * MHz },
	.default_frame_rate = 48 * KHz,
	.default_frame_row_size = 125,
	.default_frame_col_size = 2,
	.dynamic_frame_shape = 1,
	.command_error_threshold = 16,
};

static struct intel_soundwire_controller intel_controller = {
	.acpi_address = 0x40000000,
	.sdw = {
		.master_list_count = 4
	}
};

int soc_fill_soundwire_controller(struct intel_soundwire_controller **controller)
{
	const struct soundwire_link *link;
	enum pch_pmc_xtal xtal = pmc_get_xtal_freq();
	size_t i;

	/* Select link config based on XTAL frequency and set IP clock. */
	switch (xtal) {
	case XTAL_24_MHZ:
		link = &link_xtal_24;
		intel_controller.ip_clock = 24 * MHz;
		break;
	case XTAL_38_4_MHZ:
		link = &link_xtal_38_4;
		intel_controller.ip_clock = 38400 * KHz;
		break;
	case XTAL_19_2_MHZ:
	default:
		printk(BIOS_ERR, "%s: XTAL not supported: 0x%x\n", __func__, xtal);
		return -1;
	}

	/* Fill link config in controller map based on selected XTAL. */
	for (i = 0; i < intel_controller.sdw.master_list_count; i++)
		memcpy(&intel_controller.sdw.master_list[i], link, sizeof(*link));

	*controller = &intel_controller;
	return 0;
}
