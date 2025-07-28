/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_soundwire.h>
#include <device/device.h>
#include <device/soundwire.h>
#include <mipi/ids.h>
#include <stdio.h>

#include "chip.h"

static struct soundwire_multilane alc711_multilane = {
};

static struct soundwire_slave alc711_slave = {
	.wake_up_unavailable = false,
	.test_mode_supported = false,
	.clock_stop_mode1_supported = true,
	.simplified_clockstopprepare_sm_supported = true,
	.clockstopprepare_hard_reset_behavior = false,
	.highPHY_capable = false,
	.paging_supported = false,
	.bank_delay_supported = false,
	.port15_read_behavior = false,
	.source_port_list = SOUNDWIRE_PORT(2),
	.sink_port_list = SOUNDWIRE_PORT(1),
};

static struct soundwire_audio_mode alc711_audio_mode = {
	/* Bus frequency must be 1/2/4/8 divider of supported input frequencies. */
	.bus_frequency_configs_count = 12,
	.bus_frequency_configs = {
		9600 * KHz,
		4800 * KHz,
		2400 * KHz,
		1200 * KHz,
		12000 * KHz,
		6000 * KHz,
		3000 * KHz,
		1500 * KHz,
		12288 * KHz,
		6144 * KHz,
		3072 * KHz,
		1536 * KHz
	},
	/* Support 16 KHz to 192 KHz sampling frequency */
	.sampling_frequency_configs_count = 9,
	.sampling_frequency_configs = {
		16 * KHz,
		22.05 * KHz,
		24 * KHz,
		32 * KHz,
		44.1 * KHz,
		48 * KHz,
		88.2 * KHz,
		96 * KHz,
		192 * KHz
	},
	.prepare_channel_behavior = CHANNEL_PREPARE_ANY_FREQUENCY
};

static struct soundwire_dpn alc711_dp = {
	.port_wordlength_configs_count = 1,
	.port_wordlength_configs = { 32 },
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

static const struct soundwire_codec alc711_codec = {
	.slave = &alc711_slave,
	.audio_mode = { &alc711_audio_mode },
	.dpn = {
		{
			/* Data Input for Speaker Path */
			.port = 1,
			.sink = &alc711_dp
		},
		{
			/* Data Output for DSP Path */
			.port = 2,
			.source = &alc711_dp
		}
	},
	.multilane = &alc711_multilane

};

static void soundwire_alc711_fill_ssdt(const struct device *dev)
{
	struct drivers_soundwire_alc711_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_dp *dsd;
	size_t lane_mapping_count, audio_mode_count;

	if (!scope)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));

	/* Set codec address IDs. */
	config->alc711_address.link_id = dev->path.generic.id;
	config->alc711_address.unique_id = dev->path.generic.subid;
	config->alc711_address.manufacturer_id = MIPI_MFG_ID_REALTEK;

	acpigen_write_ADR_soundwire_device(&config->alc711_address);
	acpigen_write_name_string("_DDN", config->desc ? : dev->chip_ops->name);
	acpigen_write_STA(acpi_device_status(dev));

	/* Overlay multilane config, if provided. */
	lane_mapping_count = MIN(ARRAY_SIZE(config->multilane.lane_mapping), SOUNDWIRE_MAX_LANE);
	if (lane_mapping_count > 0) {
		alc711_multilane.lane_mapping_count = lane_mapping_count;
		memcpy(&alc711_multilane.lane_mapping, &config->multilane.lane_mapping,
			sizeof(struct soundwire_multilane_map) * lane_mapping_count);
	}

	/* Overlay Audio Mode config, if provided. */
	audio_mode_count = MIN(ARRAY_SIZE(config->audio_mode), SOUNDWIRE_MAX_MODE);
	if (audio_mode_count > 0)
		/* Currently only 1 audio mode is supported. */
		memcpy(&alc711_audio_mode, &config->audio_mode, sizeof(struct soundwire_audio_mode));

	/* Override the default (true) if disable_clkstop_sm_support is set in the devicetree. */
	if (config->disable_clkstop_sm_support)
		alc711_slave.simplified_clockstopprepare_sm_supported = false;

	dsd = acpi_dp_new_table("_DSD");
	soundwire_gen_codec(dsd, &alc711_codec, NULL);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *soundwire_alc711_acpi_name(const struct device *dev)
{
	struct drivers_soundwire_alc711_config *config = dev->chip_info;
	static char name[5];

	if (config->name)
		return config->name;
	snprintf(name, sizeof(name), "SW%1X%1X", dev->path.generic.id, dev->path.generic.subid);
	return name;
}

static struct device_operations soundwire_alc711_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= soundwire_alc711_acpi_name,
	.acpi_fill_ssdt		= soundwire_alc711_fill_ssdt,
};

static void soundwire_alc711_enable(struct device *dev)
{
	dev->ops = &soundwire_alc711_ops;
}

struct chip_operations drivers_soundwire_alc711_ops = {
#if CONFIG(DRIVERS_SOUNDWIRE_ALC_BASE_7XX)
	.name = "Realtek ALC 7 Series SoundWire Codec",
#else
	.name = "Unknown",
#endif
	.enable_dev = soundwire_alc711_enable
};
