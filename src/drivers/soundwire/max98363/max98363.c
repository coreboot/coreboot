/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_soundwire.h>
#include <device/device.h>
#include <device/path.h>
#include <device/soundwire.h>
#include <mipi/ids.h>
#include <stdio.h>

#include "chip.h"

static struct soundwire_address max98363_address = {
	.version = SOUNDWIRE_VERSION_1_2,
	.manufacturer_id = MIPI_MFG_ID_MAXIM,
	.part_id = MIPI_DEV_ID_MAXIM_MAX98363,
	.class = MIPI_CLASS_NONE
};

static struct soundwire_slave max98363_slave = {
	.wake_up_unavailable = false,
	.test_mode_supported = false,
	.clock_stop_mode1_supported = true,
	.simplified_clockstopprepare_sm_supported = true,
	.clockstopprepare_hard_reset_behavior = false,
	.highPHY_capable = false,
	.paging_supported = false,
	.bank_delay_supported = false,
	.port15_read_behavior = false,
	.source_port_list = 0,
	.sink_port_list = SOUNDWIRE_PORT(1),
};

static struct soundwire_audio_mode max98363_audio_mode = {
	/* Bus frequency must be 2/4/8/16 divider of supported input frequencies. */
	.bus_frequency_configs_count = 19,
	.bus_frequency_configs = {
		9600 * KHz,  4800 * KHz, 2400 * KHz, 1200 * KHz,	/* 19.2 MHz */
		11289600,    5644800,    2822400,    1411200,		/* 22.5792 MHz */
		12000 * KHz, 6000 * KHz, 3000 * KHz, 1500 * KHz,	/* 24 MHz */
		12288 * KHz, 6144 * KHz, 3072 * KHz, 1536 * KHz,	/* 24.576 MHz */
			     8000 * KHz, 4000 * KHz, 2000 * KHz,	/* 32 MHz (no /2) */
	},
	/* Support 16 KHz to 96 KHz sampling frequency */
	.sampling_frequency_configs_count = 8,
	.sampling_frequency_configs = {
		16 * KHz,
		22.05 * KHz,
		24 * KHz,
		32 * KHz,
		44.1 * KHz,
		48 * KHz,
		88.2 * KHz,
		96 * KHz,
	},
	.prepare_channel_behavior = CHANNEL_PREPARE_ANY_FREQUENCY
};

static struct soundwire_dpn max98363_dp1 = {
	.port_wordlength_configs_count = 1,
	.port_wordlength_configs = { 32 },
	.data_port_type = FULL_DATA_PORT,
	.max_grouping_supported = BLOCK_GROUP_COUNT_1,
	.simplified_channelprepare_sm = false,
	.imp_def_dpn_interrupts_supported = 0,
	.min_channel_number = 1,
	.max_channel_number = 1,
	.modes_supported = MODE_ISOCHRONOUS | MODE_TX_CONTROLLED |
			   MODE_RX_CONTROLLED | MODE_FULL_ASYNCHRONOUS,
	.block_packing_mode = true,
	.port_audio_mode_count = 1,
	.port_audio_mode_list = { 0 }
};

static const struct soundwire_codec max98363_codec = {
	.slave = &max98363_slave,
	.audio_mode = { &max98363_audio_mode },
	.dpn = {
		{
			/* Data Input for Speaker Path */
			.port = 1,
			.sink = &max98363_dp1
		}
	}
};

static void soundwire_max98363_fill_ssdt(const struct device *dev)
{
	struct drivers_soundwire_max98363_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_dp *dsd;

	if (!scope)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));

	/* Set codec address IDs. */
	max98363_address.link_id = dev->path.generic.id;
	max98363_address.unique_id = dev->path.generic.subid;

	acpigen_write_ADR_soundwire_device(&max98363_address);
	acpigen_write_name_string("_DDN", config->desc ? : dev->chip_ops->name);
	acpigen_write_STA(acpi_device_status(dev));

	dsd = acpi_dp_new_table("_DSD");
	soundwire_gen_codec(dsd, &max98363_codec, NULL);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *soundwire_max98363_acpi_name(const struct device *dev)
{
	struct drivers_soundwire_max98363_config *config = dev->chip_info;
	if (config->acpi_name[0] != 0)
		return config->acpi_name;
	snprintf(config->acpi_name, sizeof(config->acpi_name), "SW%1X%1X",
		 dev->path.generic.id, dev->path.generic.subid);
	return config->acpi_name;
}

static struct device_operations soundwire_max98363_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= soundwire_max98363_acpi_name,
	.acpi_fill_ssdt		= soundwire_max98363_fill_ssdt,
};

static void soundwire_max98363_enable(struct device *dev)
{
	dev->ops = &soundwire_max98363_ops;
}

struct chip_operations drivers_soundwire_max98363_ops = {
	CHIP_NAME("Maxim MAX98363 SoundWire Codec")
	.enable_dev = soundwire_max98363_enable
};
