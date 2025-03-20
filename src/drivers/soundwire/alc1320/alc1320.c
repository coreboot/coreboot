/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_soundwire.h>
#include <device/device.h>
#include <device/soundwire.h>
#include <mipi/ids.h>
#include <stdio.h>

#include "chip.h"

static struct soundwire_slave alc1320_slave = {
	.wake_up_unavailable = false,
	.test_mode_supported = false,
	.clock_stop_mode1_supported = true,
	.simplified_clockstopprepare_sm_supported = true,
	.clockstopprepare_hard_reset_behavior = false,
	.highPHY_capable = false,
	.paging_supported = false,
	.bank_delay_supported = false,
	.port15_read_behavior = false,
	.source_port_list = SOUNDWIRE_PORT(2) | SOUNDWIRE_PORT(4),
	.sink_port_list = SOUNDWIRE_PORT(1)
};

static struct soundwire_audio_mode alc1320_audio_mode = {
	/* Bus frequency must be 1/2/4/8 divider of supported input frequencies. */
	.bus_frequency_configs_count = 20,
	.bus_frequency_configs = {
		9600 * KHz,  4800 * KHz, 2400 * KHz, 1200 * KHz,	/* 19.2 MHz */
		11289600,    5644800,    2822400,    1411200,		/* 22.5792 MHz */
		12000 * KHz, 6000 * KHz, 3000 * KHz, 1500 * KHz,	/* 24 MHz */
		12288 * KHz, 6144 * KHz, 3072 * KHz, 1536 * KHz,	/* 24.576 MHz */
		16000 * KHz, 8000 * KHz, 4000 * KHz, 2000 * KHz,	/* 32 MHz */
	},
	/* Support 16 KHz to 192 KHz sampling frequency */
	.sampling_frequency_configs_count = 6,
	.sampling_frequency_configs = {
		16 * KHz,
		32 * KHz,
		44.1 * KHz,
		48 * KHz,
		96 * KHz,
		192 * KHz,
	},
	.prepare_channel_behavior = CHANNEL_PREPARE_ANY_FREQUENCY
};

static struct soundwire_dpn alc1320_dp1 = {
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

static const struct soundwire_codec alc1320_codec = {
	.slave = &alc1320_slave,
	.audio_mode = { &alc1320_audio_mode },
	.dpn = {
		{
			/* Data Input for Speaker Path */
			.port = 1,
			.sink = &alc1320_dp1
		},
		{
			/* Data out for I.V sensing */
			.port = 2,
			.source = &alc1320_dp1
		},
		{
			/* Data out for I.V sensing */
			.port = 4,
			.source = &alc1320_dp1
		}
	}

};

static void soundwire_alc1320_fill_ssdt(const struct device *dev)
{
	struct drivers_soundwire_alc1320_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_dp *dsd;

	if (!dev->enabled || !scope)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));

	/* Set codec address IDs. */
	config->alc1320_address.link_id = dev->path.generic.id;
	config->alc1320_address.unique_id = dev->path.generic.subid;
	config->alc1320_address.manufacturer_id = MIPI_MFG_ID_REALTEK;

	acpigen_write_ADR_soundwire_device(&config->alc1320_address);
	acpigen_write_name_string("_DDN", config->desc ? : dev->chip_ops->name);
	acpigen_write_STA(acpi_device_status(dev));

	dsd = acpi_dp_new_table("_DSD");
	soundwire_gen_codec(dsd, &alc1320_codec, NULL);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *soundwire_alc1320_acpi_name(const struct device *dev)
{
	struct drivers_soundwire_alc1320_config *config = dev->chip_info;
	static char name[5];

	if (config->name)
		return config->name;
	snprintf(name, sizeof(name), "SW%1X%1X", dev->path.generic.id, dev->path.generic.subid);
	return name;
}

static struct device_operations soundwire_alc1320_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= soundwire_alc1320_acpi_name,
	.acpi_fill_ssdt		= soundwire_alc1320_fill_ssdt,
};

static void soundwire_alc1320_enable(struct device *dev)
{
	dev->ops = &soundwire_alc1320_ops;
}

struct chip_operations drivers_soundwire_alc1320_ops = {
	.name = "Realtek ALC1320 SoundWire Codec",
	.enable_dev = soundwire_alc1320_enable
};
