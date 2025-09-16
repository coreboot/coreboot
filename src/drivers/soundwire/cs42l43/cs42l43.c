/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_soundwire.h>
#include <device/device.h>
#include <device/soundwire.h>
#include <mipi/ids.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#include "chip.h"

static struct soundwire_address cs42l43_address = {
	.version = SOUNDWIRE_VERSION_1_2,
	.manufacturer_id = MIPI_MFG_ID_CIRRUS,
	.part_id = MIPI_DEV_ID_CIRRUS_CS42L43,
	.class = MIPI_CLASS_SDCA
};

static struct soundwire_slave cs42l43_slave = {
	.wake_up_unavailable = false,
	.test_mode_supported = false,
	.clock_stop_mode1_supported = false,
	.simplified_clockstopprepare_sm_supported = false,
	.paging_supported = true,
	.source_port_list = SOUNDWIRE_PORT(1) | SOUNDWIRE_PORT(2),
	.sink_port_list = SOUNDWIRE_PORT(5) | SOUNDWIRE_PORT(6)
};

static struct soundwire_dp0 cs42l43_dp0 = {
	.port_wordlength_configs_count = 3,
	.port_wordlength_configs = {8, 16, 24},
	.bra_flow_controlled = true,
	.bra_imp_def_response_supported = true,
	.bra_role_supported = true,
	.imp_def_dp0_interrupts_supported = 0,
	.imp_def_bpt_supported = false,
	.bra_mode_count = 1,
	.bra_mode_list = { 0 },
	.simplified_channel_prepare_sm = false
};

static struct soundwire_bra_mode cs42l43_dp0_bra_mode = {
	.max_data_per_frame = 470,
	.block_alignment = 4
};

static struct soundwire_dpn cs42l43_dp1 = {
	.data_port_type = FULL_DATA_PORT,
	.max_grouping_supported = BLOCK_GROUP_COUNT_1,
	.modes_supported = MODE_ISOCHRONOUS,
	.max_async_buffer = 0,
	.block_packing_mode = true,
	.port_encoding_type = ENCODE_TWOS_COMPLEMENT,
	.port_wordlength_configs_count = 3,
	.port_wordlength_configs = {8, 16, 24},
	.simplified_channelprepare_sm = false,
	.channel_number_list_count = 4,
	.channel_number_list = {0, 1, 2, 3},
	.channel_combination_list_count = 4,
	.channel_combination_list = {0x1, 0x3, 0x7, 0xF},
	.port_audio_mode_count = 0
};

static struct soundwire_dpn cs42l43_dpn = {
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
	.channel_combination_list_count = 3,
	.channel_combination_list = {0x3, 0x2, 0x1},
	.port_audio_mode_count = 0
};

static const struct soundwire_codec cs42l43_codec = {
	.slave = &cs42l43_slave,
	.dp0 = &cs42l43_dp0,
	.dp0_bra_mode = { &cs42l43_dp0_bra_mode },
	.dpn = {
		{
			/* UAJ_MIC */
			.port = 2,
			.source = &cs42l43_dpn
		},
		{
			/* UAJ_SPK */
			.port = 5,
			.sink = &cs42l43_dpn
		},
		{
			/* MIC */
			.port = 1,
			.sink = &cs42l43_dp1
		},
		{
			/* SPK */
			.port = 6,
			.source = &cs42l43_dpn
		}
	}
};

static void soundwire_cs42l43_fill_ssdt(const struct device *dev)
{
	struct drivers_soundwire_cs42l43_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_dp *dsd;

	if (!scope)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));

	/* Set codec address IDs. */
	cs42l43_address.link_id = dev->path.generic.id;
	cs42l43_address.unique_id = dev->path.generic.subid;

	acpigen_write_ADR_soundwire_device(&cs42l43_address);
	acpigen_write_name_string("_DDN", config->desc ? : dev->chip_ops->name);
	acpigen_write_name_string("_SUB", config->sub);
	acpigen_write_STA(acpi_device_status(dev));

	dsd = acpi_dp_new_table("_DSD");

	if (config->bias_low)
		acpi_dp_add_integer(dsd, "cirrus,bias-low", 1);

	if (config->bias_sense_microamp) {
		if (config->bias_sense_microamp == BIAS_SENSE_OFF)
			config->bias_sense_microamp = 0;
		acpi_dp_add_integer(dsd, "cirrus,bias-sense-microamp", config->bias_sense_microamp);
	}

	if (config->bias_ramp_ms)
		acpi_dp_add_integer(dsd, "cirrus,bias-ramp-ms", config->bias_ramp_ms);

	if (config->detect_us)
		acpi_dp_add_integer(dsd, "cirrus,detect-us", config->detect_us);

	if (config->button_automute)
		acpi_dp_add_integer(dsd, "cirrus,button-automute", 1);

	if (config->buttons_ohms_count > 0)
		acpi_dp_add_integer_array(dsd, "cirrus,buttons-ohms", config->buttons_ohms,
					  config->buttons_ohms_count);

	if (config->tip_debounce_ms)
		acpi_dp_add_integer(dsd, "cirrus,tip-debounce-ms", config->tip_debounce_ms);

	if (config->tip_invert)
		acpi_dp_add_integer(dsd, "cirrus,tip-invert", 1);
	if (config->tip_disable_pullup)
		acpi_dp_add_integer(dsd, "cirrus,tip-disable-pullup", 1);

	if (config->tip_fall_db_ms) {
		if (config->tip_fall_db_ms == DB_0_MS)
			config->tip_fall_db_ms = 0;
		acpi_dp_add_integer(dsd, "cirrus,tip-fall-db-ms", config->tip_fall_db_ms);
	}
	if (config->tip_rise_db_ms) {
		if (config->tip_rise_db_ms == DB_0_MS)
			config->tip_rise_db_ms = 0;
		acpi_dp_add_integer(dsd, "cirrus,tip-rise-db-ms", config->tip_rise_db_ms);
	}

	if (config->use_ring_sense)
		acpi_dp_add_integer(dsd, "cirrus,use-ring-sense", 1);
	if (config->ring_invert)
		acpi_dp_add_integer(dsd, "cirrus,ring-invert", 1);
	if (config->ring_disable_pullup)
		acpi_dp_add_integer(dsd, "cirrus,ring-disable-pullup", 1);

	if (config->ring_fall_db_ms) {
		if (config->ring_fall_db_ms == DB_0_MS)
			config->ring_fall_db_ms = 0;
		acpi_dp_add_integer(dsd, "cirrus,ring-fall-db-ms", config->ring_fall_db_ms);
	}
	if (config->ring_rise_db_ms) {
		if (config->ring_rise_db_ms == DB_0_MS)
			config->ring_rise_db_ms = 0;
		acpi_dp_add_integer(dsd, "cirrus,ring-rise-db-ms", config->ring_rise_db_ms);
	}

	soundwire_gen_codec(dsd, &cs42l43_codec, NULL);

	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *soundwire_cs42l43_acpi_name(const struct device *dev)
{
	struct drivers_soundwire_cs42l43_config *config = dev->chip_info;
	if (config->acpi_name[0] != 0)
		return config->acpi_name;
	snprintf(config->acpi_name, sizeof(config->acpi_name), "SW%1X%1X",
		 dev->path.generic.id, dev->path.generic.subid);
	return config->acpi_name;
}

static struct device_operations soundwire_cs42l43_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= soundwire_cs42l43_acpi_name,
	.acpi_fill_ssdt		= soundwire_cs42l43_fill_ssdt,
};

static void soundwire_cs42l43_enable(struct device *dev)
{
	dev->ops = &soundwire_cs42l43_ops;
}

struct chip_operations drivers_soundwire_cs42l43_ops = {
	.name = "Cirrus CS42L43 SoundWire Codec",
	.enable_dev = soundwire_cs42l43_enable
};
