/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_soundwire.h>
#include <device/device.h>
#include <device/mipi_ids.h>
#include <device/path.h>
#include <device/soundwire.h>
#include <stdio.h>

#include "chip.h"

static struct soundwire_address alc5682_address = {
	.version = SOUNDWIRE_VERSION_1_1,
	.manufacturer_id = MIPI_MFG_ID_REALTEK,
	.part_id = MIPI_DEV_ID_REALTEK_ALC5682,
	.class = MIPI_CLASS_NONE
};

static struct soundwire_slave alc5682_slave = {
	.wake_up_unavailable = false,
	.test_mode_supported = false,
	.clock_stop_mode1_supported = true,
	.simplified_clockstopprepare_sm_supported = true,
	.clockstopprepare_hard_reset_behavior = false,
	.highPHY_capable = false,
	.paging_supported = false,
	.bank_delay_supported = false,
	.port15_read_behavior = false,
	.source_port_list = SOUNDWIRE_PORT(1) | SOUNDWIRE_PORT(2) |
			    SOUNDWIRE_PORT(3) | SOUNDWIRE_PORT(4),
	.sink_port_list = SOUNDWIRE_PORT(1) | SOUNDWIRE_PORT(2) |
			  SOUNDWIRE_PORT(3) | SOUNDWIRE_PORT(4)
};

static struct soundwire_bra_mode alc5682_dp0_bra_mode = {
	.bus_frequency_configs_count = 7,
	.bus_frequency_configs = {
		1000 * KHz,	/* 1 MHz */
		2400 * KHz,	/* 2.4 MHz */
		3000 * KHz,	/* 3 MHz */
		4000 * KHz,	/* 4 MHz */
		4800 * KHz,	/* 4.8 MHz */
		9600 * KHz,	/* 9.6 MHz */
		12000 * KHz,	/* 12 MHz */
	},
	.max_data_per_frame = 470,
	.min_us_between_transactions = 0
};

static struct soundwire_dp0 alc5682_dp0 = {
	.port_max_wordlength = 64,
	.port_min_wordlength = 1,
	.bra_imp_def_response_supported = false,
	.simplified_channel_prepare_sm = true,
	.imp_def_dp0_interrupts_supported = 0,
	.imp_def_bpt_supported = true,
	.bra_mode_count = 1,
	.bra_mode_list = { 0 }
};

static struct soundwire_audio_mode alc5682_audio_mode = {
	.bus_frequency_configs_count = 7,
	.bus_frequency_configs = {
		1000 * KHz,	/* 1 MHz */
		2400 * KHz,	/* 2.4 MHz */
		3000 * KHz,	/* 3 MHz */
		4000 * KHz,	/* 4 MHz */
		4800 * KHz,	/* 4.8 MHz */
		9600 * KHz,	/* 9.6 MHz */
		12000 * KHz,	/* 12 MHz */
	},
	/* Support 8 KHz to 192 KHz sampling frequency */
	.max_sampling_frequency = 192 * KHz,
	.min_sampling_frequency = 8 * KHz,
	.prepare_channel_behavior = CHANNEL_PREPARE_ANY_FREQUENCY
};

static struct soundwire_dpn alc5682_dpn = {
	.port_wordlength_configs_count = 3,
	.port_wordlength_configs = { 16, 20, 24 },
	.data_port_type = FULL_DATA_PORT,
	.max_grouping_supported = BLOCK_GROUP_COUNT_1,
	.simplified_channelprepare_sm = false,
	.imp_def_dpn_interrupts_supported = 0,
	.min_channel_number = 1,
	.max_channel_number = 2,
	.modes_supported = MODE_ISOCHRONOUS | MODE_TX_CONTROLLED |
			   MODE_RX_CONTROLLED | MODE_FULL_ASYNCHRONOUS,
	.block_packing_mode = true,
	.port_audio_mode_count = 1,
	.port_audio_mode_list = { 0 }
};

static const struct soundwire_codec alc5682_codec = {
	.slave = &alc5682_slave,
	.dp0_bra_mode = { &alc5682_dp0_bra_mode },
	.dp0 = &alc5682_dp0,
	.audio_mode = { &alc5682_audio_mode },
	.dpn = {
		{
			.port = 1,
			.source = &alc5682_dpn,
			.sink = &alc5682_dpn,
		},
		{
			.port = 2,
			.source = &alc5682_dpn,
			.sink = &alc5682_dpn,
		},
		{
			.port = 3,
			.source = &alc5682_dpn,
			.sink = &alc5682_dpn,
		},
		{
			.port = 4,
			.source = &alc5682_dpn,
			.sink = &alc5682_dpn,
		}
	}
};

static void soundwire_alc5682_fill_ssdt(const struct device *dev)
{
	struct drivers_soundwire_alc5682_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_dp *dsd;

	if (!scope)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));

	/* Set codec address IDs. */
	alc5682_address.link_id = dev->path.generic.id;
	alc5682_address.unique_id = dev->path.generic.subid;

	acpigen_write_ADR_soundwire_device(&alc5682_address);
	acpigen_write_name_string("_DDN", config->desc ? : dev->chip_ops->name);
	acpigen_write_STA(acpi_device_status(dev));

	dsd = acpi_dp_new_table("_DSD");
	soundwire_gen_codec(dsd, &alc5682_codec, NULL);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *soundwire_alc5682_acpi_name(const struct device *dev)
{
	struct drivers_soundwire_alc5682_config *config = dev->chip_info;
	static char name[5];

	if (config->name)
		return config->name;
	snprintf(name, sizeof(name), "SW%1X%1X", dev->path.generic.id, dev->path.generic.subid);
	return name;
}

static struct device_operations soundwire_alc5682_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= soundwire_alc5682_acpi_name,
	.acpi_fill_ssdt		= soundwire_alc5682_fill_ssdt,
};

static void soundwire_alc5682_enable(struct device *dev)
{
	dev->ops = &soundwire_alc5682_ops;
}

struct chip_operations drivers_soundwire_alc5682_ops = {
	CHIP_NAME("Realtek ALC5682 SoundWire Codec")
	.enable_dev = soundwire_alc5682_enable
};
