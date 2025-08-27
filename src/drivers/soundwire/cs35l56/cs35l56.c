/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_soundwire.h>
#include <device/device.h>
#include <device/soundwire.h>
#include <mipi/ids.h>
#include <stdio.h>

#include "chip.h"

static struct soundwire_address cs35l56_address = {
	.version = SOUNDWIRE_VERSION_1_2,
	.manufacturer_id = MIPI_MFG_ID_CIRRUS,
	.class = MIPI_CLASS_SDCA
};

static struct soundwire_slave cs35l56_slave = {
	.wake_up_unavailable = false,
	.test_mode_supported = false,
	.clock_stop_mode1_supported = false,
	.simplified_clockstopprepare_sm_supported = false,
	.paging_supported = true,
	.source_port_list = SOUNDWIRE_PORT(3) | SOUNDWIRE_PORT(4),
	.sink_port_list = SOUNDWIRE_PORT(1) | SOUNDWIRE_PORT(2),
};

static struct soundwire_dp0 cs35l56_dp0 = {
	.port_wordlength_configs_count = 3,
	.port_wordlength_configs = {8, 16, 24},
	.bra_flow_controlled = true,
	.bra_imp_def_response_supported = true,
	.bra_role_supported = true,
	.imp_def_dp0_interrupts_supported = 0,
	.imp_def_bpt_supported = false,
	.bra_mode_count = 1,
	.bra_mode_list = { 0 }
};

static struct soundwire_bra_mode cs35l56_dp0_bra_mode = {
	.max_data_per_frame = 470,
	.block_alignment = 4,
};

static struct soundwire_dpn cs35l56_dpn = {
	.data_port_type = FULL_DATA_PORT,
	.max_grouping_supported = BLOCK_GROUP_COUNT_1,
	.modes_supported = MODE_ISOCHRONOUS,
	.max_async_buffer = 0,
	.block_packing_mode = true,
	.port_encoding_type = ENCODE_TWOS_COMPLEMENT,
	.port_wordlength_configs_count = 3,
	.port_wordlength_configs = {8, 16, 24},
	.simplified_channelprepare_sm = false,
	.channel_number_list_count = 2,
	.channel_number_list = {0, 1},
	.channel_combination_list_count = 1,
	.channel_combination_list = {0x3},
	.port_audio_mode_count = 0,
};

static const struct soundwire_codec cs35l56_codec = {
	.slave = &cs35l56_slave,
	.dp0 = &cs35l56_dp0,
	.dp0_bra_mode = { &cs35l56_dp0_bra_mode },
	.dpn = {
		{
			/* Data Input for Speaker Path */
			.port = 1,
			.sink = &cs35l56_dpn
		},
		{
			/* Data Input for Speaker Path */
			.port = 2,
			.sink = &cs35l56_dpn
		},
		{
			/* Data Output for Feedback Path */
			.port = 3,
			.source = &cs35l56_dpn
		},
		{
			/* Data Output for Feedback Path */
			.port = 4,
			.source = &cs35l56_dpn
		}
	}
};

static void soundwire_cs35l56_fill_ssdt(const struct device *dev)
{
	struct drivers_soundwire_cs35l56_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_dp *dsd;

	if (!scope)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));

	/* Set codec address IDs. */
	cs35l56_address.link_id = dev->path.generic.id;
	cs35l56_address.unique_id = dev->path.generic.subid;
	cs35l56_address.part_id = config->part_id;

	acpigen_write_ADR_soundwire_device(&cs35l56_address);
	acpigen_write_name_string("_DDN", config->desc ? : dev->chip_ops->name);
	acpigen_write_name_string("_SUB", config->sub);
	acpigen_write_STA(acpi_device_status(dev));

	/* for spkid gpio */
	if (config->spkid_gpio.pin_count) {
		acpigen_write_name("_CRS");
		acpigen_write_resourcetemplate_header();
		acpi_device_write_gpio(&config->spkid_gpio);
		acpigen_write_resourcetemplate_footer();
	}

	dsd = acpi_dp_new_table("_DSD");
	soundwire_gen_codec(dsd, &cs35l56_codec, NULL);

	if (config->spkid_gpio.pin_count)
		acpi_dp_add_gpio(dsd, "spk-id-gpios", acpi_device_path(dev),
			 0,  /* Index = 0 */
			 0,  /* Pin = 0 (There is a single pin in the GPIO resource). */
			 config->spkid_gpio.active_low);

	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *soundwire_cs35l56_acpi_name(const struct device *dev)
{
	struct drivers_soundwire_cs35l56_config *config = dev->chip_info;
	if (config->acpi_name[0] != 0)
		return config->acpi_name;
	snprintf(config->acpi_name, sizeof(config->acpi_name), "SW%1X%1X",
		 dev->path.generic.id, dev->path.generic.subid);
	return config->acpi_name;
}

static struct device_operations soundwire_cs35l56_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= soundwire_cs35l56_acpi_name,
	.acpi_fill_ssdt		= soundwire_cs35l56_fill_ssdt,
};

static void soundwire_cs35l56_enable(struct device *dev)
{
	dev->ops = &soundwire_cs35l56_ops;
}

struct chip_operations drivers_soundwire_cs35l56_ops = {
	.name = "Cirrus CS35L56 SoundWire Amplifier",
	.enable_dev = soundwire_cs35l56_enable
};
