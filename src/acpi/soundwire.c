/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <acpi/acpi_soundwire.h>
#include <commonlib/helpers.h>
#include <device/soundwire.h>
#include <stdbool.h>
#include <stddef.h>

/* Specification-defined prefix for SoundWire properties. */
#define SDW_PFX "mipi-sdw-"

/* Generate SoundWire property for integer. */
#define SDW_INT(__key, __val) \
	acpi_dp_add_integer(dsd, SDW_PFX __key, __val)

/* Generate SoundWire property for integer array. */
#define SDW_INT_ARRAY(__key, __val) \
	acpi_dp_add_integer_array(dsd, SDW_PFX __key, __val, __val##_count)

/**
 * struct soundwire_name_map - Map ACPI name to SoundWire property name.
 * @acpi_name: ACPI compatible name string.
 * @sdw_name: MIPI SoundWire property name string.
 */
struct soundwire_name_map {
	const char *acpi_name;
	const char *sdw_name;
};

static const struct soundwire_name_map bra_mode_names[] = {
	{ "BRA0", SDW_PFX "port-bra-mode-0" },
	{ "BRA1", SDW_PFX "port-bra-mode-1" },
	{ "BRA2", SDW_PFX "port-bra-mode-2" },
	{ "BRA3", SDW_PFX "port-bra-mode-3" },
};

static const struct soundwire_name_map audio_mode_names[] = {
	{ "MOD0", SDW_PFX "port-audio-mode-0" },
	{ "MOD1", SDW_PFX "port-audio-mode-1" },
	{ "MOD2", SDW_PFX "port-audio-mode-2" },
	{ "MOD3", SDW_PFX "port-audio-mode-3" },
};

static const struct soundwire_name_map dpn_source_names[] = {
	{ "DP0",  SDW_PFX "dp-0-subproperties" },
	{ "SRC1", SDW_PFX "dp-1-source-subproperties" },
	{ "SRC2", SDW_PFX "dp-2-source-subproperties" },
	{ "SRC3", SDW_PFX "dp-3-source-subproperties" },
	{ "SRC4", SDW_PFX "dp-4-source-subproperties" },
	{ "SRC5", SDW_PFX "dp-5-source-subproperties" },
	{ "SRC6", SDW_PFX "dp-6-source-subproperties" },
	{ "SRC7", SDW_PFX "dp-7-source-subproperties" },
	{ "SRC8", SDW_PFX "dp-8-source-subproperties" },
	{ "SRC9", SDW_PFX "dp-9-source-subproperties" },
	{ "SRCA", SDW_PFX "dp-10-source-subproperties" },
	{ "SRCB", SDW_PFX "dp-11-source-subproperties" },
	{ "SRCC", SDW_PFX "dp-12-source-subproperties" },
	{ "SRCD", SDW_PFX "dp-13-source-subproperties" }
};

static const struct soundwire_name_map dpn_sink_names[] = {
	{ "DP0",  SDW_PFX "dp-0-subproperties" },
	{ "SNK1", SDW_PFX "dp-1-sink-subproperties" },
	{ "SNK2", SDW_PFX "dp-2-sink-subproperties" },
	{ "SNK3", SDW_PFX "dp-3-sink-subproperties" },
	{ "SNK4", SDW_PFX "dp-4-sink-subproperties" },
	{ "SNK5", SDW_PFX "dp-5-sink-subproperties" },
	{ "SNK6", SDW_PFX "dp-6-sink-subproperties" },
	{ "SNK7", SDW_PFX "dp-7-sink-subproperties" },
	{ "SNK8", SDW_PFX "dp-8-sink-subproperties" },
	{ "SNK9", SDW_PFX "dp-9-sink-subproperties" },
	{ "SNKA", SDW_PFX "dp-10-sink-subproperties" },
	{ "SNKB", SDW_PFX "dp-11-sink-subproperties" },
	{ "SNKC", SDW_PFX "dp-12-sink-subproperties" },
	{ "SNKD", SDW_PFX "dp-13-sink-subproperties" }
};

static const struct soundwire_name_map link_names[] = {
	{ "LNK0", SDW_PFX "link-0-subproperties" },
	{ "LNK1", SDW_PFX "link-1-subproperties" },
	{ "LNK2", SDW_PFX "link-2-subproperties" },
	{ "LNK3", SDW_PFX "link-3-subproperties" },
	{ "LNK4", SDW_PFX "link-4-subproperties" },
	{ "LNK5", SDW_PFX "link-5-subproperties" },
	{ "LNK6", SDW_PFX "link-6-subproperties" },
	{ "LNK7", SDW_PFX "link-7-subproperties" }
};

static const char * const multilane_names[] = {
	SDW_PFX "lane-1-mapping",
	SDW_PFX "lane-2-mapping",
	SDW_PFX "lane-3-mapping",
	SDW_PFX "lane-4-mapping",
	SDW_PFX "lane-5-mapping",
	SDW_PFX "lane-6-mapping",
	SDW_PFX "lane-7-mapping",
	SDW_PFX "lane-8-mapping"
};

static const char * const multilane_master_lane_names[] = {
	SDW_PFX "master-lane-1",
	SDW_PFX "master-lane-2",
	SDW_PFX "master-lane-3",
	SDW_PFX "master-lane-4",
	SDW_PFX "master-lane-5",
	SDW_PFX "master-lane-6",
	SDW_PFX "master-lane-7",
	SDW_PFX "master-lane-8"
};

static const char * const multilane_slave_link_names[] = {
	SDW_PFX "slave-link-A",
	SDW_PFX "slave-link-B",
	SDW_PFX "slave-link-C",
	SDW_PFX "slave-link-D",
	SDW_PFX "slave-link-E",
	SDW_PFX "slave-link-F",
	SDW_PFX "slave-link-G",
	SDW_PFX "slave-link-I"
};

static const char * const multilane_bus_holder_names[] = {
	SDW_PFX "lane-1-bus-holder",
	SDW_PFX "lane-2-bus-holder",
	SDW_PFX "lane-3-bus-holder",
	SDW_PFX "lane-4-bus-holder",
	SDW_PFX "lane-5-bus-holder",
	SDW_PFX "lane-6-bus-holder",
	SDW_PFX "lane-7-bus-holder",
	SDW_PFX "lane-8-bus-holder"
};

static void soundwire_gen_interface_revision(struct acpi_dp *dsd)
{
	acpi_dp_add_integer(dsd, SDW_PFX "sw-interface-revision", SOUNDWIRE_SW_VERSION_1_0);
}

static void soundwire_gen_slave(struct acpi_dp *dsd, const struct soundwire_slave *prop)
{
	soundwire_gen_interface_revision(dsd);
	SDW_INT("wake-up-unavailable", prop->wake_up_unavailable);
	SDW_INT("test-mode-supported", prop->test_mode_supported);
	SDW_INT("clock-stop-mode1-supported", prop->clock_stop_mode1_supported);

	/* Clock Stop Prepare Timeout only used without simplified Clock Stop Prepare. */
	SDW_INT("simplified-clockstopprepare-sm-supported",
		prop->simplified_clockstopprepare_sm_supported);
	if (!prop->simplified_clockstopprepare_sm_supported)
		SDW_INT("clockstopprepare-timeout", prop->clockstopprepare_timeout);

	SDW_INT("clockstopprepare-hard-reset-behavior",
		prop->clockstopprepare_hard_reset_behavior);
	SDW_INT("slave-channelprepare-timeout", prop->slave_channelprepare_timeout);
	SDW_INT("highPHY-capable", prop->highPHY_capable);
	SDW_INT("paging-supported", prop->paging_supported);
	SDW_INT("bank-delay-supported", prop->bank_delay_supported);
	SDW_INT("port15-read-behavior", prop->port15_read_behavior);
	SDW_INT("master-count", prop->master_count);
	SDW_INT("source-port-list", prop->source_port_list);
	SDW_INT("sink-port-list", prop->sink_port_list);
}

static void soundwire_gen_multilane(struct acpi_dp *dsd, const struct soundwire_multilane *prop)
{
	size_t i;

	soundwire_gen_interface_revision(dsd);

	/* Fill out multilane map based on master/slave links. */
	for (i = 0; i < prop->lane_mapping_count && i < SOUNDWIRE_MAX_LANE; i++) {
		const struct soundwire_multilane_map *map = &prop->lane_mapping[i];
		const char *name;

		/* Get the name of this connection */
		if (map->direction == MASTER_LANE)
			name = multilane_master_lane_names[map->connection.master_lane];
		else
			name = multilane_slave_link_names[map->connection.slave_link];

		acpi_dp_add_string(dsd, multilane_names[map->lane], name);
	}

	/* Add bus holder properties. */
	for (i = 0; i < prop->lane_bus_holder_count; i++)
		acpi_dp_add_integer(dsd, multilane_bus_holder_names[i],
				    prop->lane_bus_holder[i]);
}

static void soundwire_gen_link(struct acpi_dp *dsd, const struct soundwire_link *prop)
{
	SDW_INT("clock-stop-mode0-supported", prop->clock_stop_mode0_supported);
	SDW_INT("clock-stop-mode1-supported", prop->clock_stop_mode1_supported);
	if (prop->clock_frequencies_supported_count > 0 &&
	    prop->clock_frequencies_supported_count < SOUNDWIRE_MAX) {
		SDW_INT_ARRAY("clock-frequencies-supported",
			      prop->clock_frequencies_supported);
	}
	SDW_INT("default-frame-rate", prop->default_frame_rate);
	SDW_INT("default-frame-row-size", prop->default_frame_row_size);
	SDW_INT("default-frame-col-size", prop->default_frame_col_size);
	SDW_INT("dynamic-frame-shape", prop->dynamic_frame_shape);
	SDW_INT("command-error-threshold", prop->command_error_threshold);
}

static void soundwire_gen_bra_mode(struct acpi_dp *dsd, const struct soundwire_bra_mode *prop)
{
	/* Bus frequency configs used if min/max not supported. */
	if (prop->bus_frequency_configs_count > 0 &&
	    prop->bus_frequency_configs_count < SOUNDWIRE_MAX) {
		SDW_INT_ARRAY("bra-mode-bus-frequency-configs", prop->bus_frequency_configs);
	} else {
		SDW_INT("bra-mode-min-bus-frequency", prop->min_bus_frequency);
		SDW_INT("bra-mode-max-bus-frequency", prop->max_bus_frequency);
	}
	SDW_INT("bra-mode-max-data-per-frame", prop->max_data_per_frame);
	SDW_INT("bra-mode-min-us-between-transactions", prop->min_us_between_transactions);
}

static void soundwire_gen_audio_mode(struct acpi_dp *dsd,
				     const struct soundwire_audio_mode *prop)
{
	/* Bus frequency configs used if min/max not supported. */
	if (prop->bus_frequency_configs_count > 0 &&
	    prop->bus_frequency_configs_count < SOUNDWIRE_MAX) {
		SDW_INT_ARRAY("audio-mode-bus-frequency-configs", prop->bus_frequency_configs);
	} else {
		SDW_INT("audio-mode-min-bus-frequency", prop->min_bus_frequency);
		SDW_INT("audio-mode-max-bus-frequency", prop->max_bus_frequency);
	}

	/* Sampling frequency configs used if min/max not supported. */
	if (prop->sampling_frequency_configs_count > 0 &&
	    prop->sampling_frequency_configs_count < SOUNDWIRE_MAX) {
		SDW_INT_ARRAY("audio-mode-sampling-frequency-configs",
			      prop->sampling_frequency_configs);
	} else {
		SDW_INT("audio-mode-max-sampling-frequency", prop->max_sampling_frequency);
		SDW_INT("audio-mode-min-sampling-frequency", prop->min_sampling_frequency);
	}

	SDW_INT("audio-mode-prepare-channel-behavior", prop->prepare_channel_behavior);
	SDW_INT("audio-mode-glitchless-transitions", prop->glitchless_transitions);
}

static void soundwire_gen_dp0(struct acpi_dp *dsd, const struct soundwire_dp0 *prop)
{
	size_t i;

	/* Max wordlength configs used if min/max not supported. */
	if (prop->port_wordlength_configs_count > 0 &&
	    prop->port_wordlength_configs_count < SOUNDWIRE_MAX) {
		SDW_INT_ARRAY("port-wordlength-configs", prop->port_wordlength_configs);
	} else {
		SDW_INT("port-max-wordlength", prop->port_max_wordlength);
		SDW_INT("port-min-wordlength", prop->port_min_wordlength);
	}
	SDW_INT("bra-flow-controlled", prop->bra_flow_controlled);
	SDW_INT("bra-imp-def-response-supported", prop->bra_imp_def_response_supported);
	SDW_INT("bra-role-supported", prop->bra_role_supported);
	SDW_INT("simplified-channel-prepare-sm", prop->simplified_channel_prepare_sm);
	SDW_INT("imp-def-dp0-interrupts-supported", prop->imp_def_dp0_interrupts_supported);
	SDW_INT("imp-def-bpt-supported", prop->imp_def_bpt_supported);

	/* Add bulk register access mode property pointers. */
	for (i = 0; i < prop->bra_mode_count && i < SOUNDWIRE_MAX_MODE; i++) {
		struct acpi_dp *bra = acpi_dp_new_table(bra_mode_names[i].acpi_name);
		acpi_dp_add_child(dsd, bra_mode_names[i].sdw_name, bra);
	}
}

static void soundwire_gen_dpn(struct acpi_dp *dsd, const struct soundwire_dpn *prop)
{
	size_t i;

	SDW_INT("data-port-type", prop->data_port_type);
	SDW_INT("max-grouping-supported", prop->max_grouping_supported);
	SDW_INT("imp-def-dpn-interrupts-supported", prop->imp_def_dpn_interrupts_supported);
	SDW_INT("modes-supported", prop->modes_supported);
	SDW_INT("max-async-buffer", prop->max_async_buffer);
	SDW_INT("block-packing-mode", prop->block_packing_mode);
	SDW_INT("port-encoding-type", prop->port_encoding_type);

	/* Max wordlength configs used if min/max not supported. */
	if (prop->port_wordlength_configs_count > 0 &&
	    prop->port_wordlength_configs_count < SOUNDWIRE_MAX) {
		SDW_INT_ARRAY("port-wordlength-configs", prop->port_wordlength_configs);
	} else {
		SDW_INT("port-max-wordlength", prop->port_max_wordlength);
		SDW_INT("port-min-wordlength", prop->port_min_wordlength);
	}

	/* Channel Prepare Timeout only used without simplified Channel Prepare. */
	SDW_INT("simplified-channelprepare-sm", prop->simplified_channelprepare_sm);
	if (!prop->simplified_channelprepare_sm)
		SDW_INT("port-channelprepare-timeout", prop->port_channelprepare_timeout);

	/* Channel number list used if min/max not supported. */
	if (prop->channel_number_list_count > 0 &&
	    prop->channel_number_list_count < SOUNDWIRE_MAX) {
		SDW_INT_ARRAY("channel-number-list", prop->channel_number_list);
	} else {
		SDW_INT("min-channel-number", prop->min_channel_number);
		SDW_INT("max-channel-number", prop->max_channel_number);
	}
	if (prop->channel_combination_list_count > 0 &&
	    prop->channel_combination_list_count < SOUNDWIRE_MAX) {
		SDW_INT_ARRAY("channel-combination-list", prop->channel_combination_list);
	}

	/* Add reference to Audio Mode properties. */
	for (i = 0; i < prop->port_audio_mode_count && i < SOUNDWIRE_MAX_MODE; i++) {
		struct acpi_dp *am = acpi_dp_new_table(audio_mode_names[i].acpi_name);
		acpi_dp_add_child(dsd, audio_mode_names[i].sdw_name, am);
	}
}

void soundwire_gen_controller(struct acpi_dp *dsd, const struct soundwire_controller *prop,
			      soundwire_link_prop_cb link_prop_cb)
{
	size_t i;

	soundwire_gen_interface_revision(dsd);
	SDW_INT("master-count", prop->master_list_count);

	/* Generate properties for each master link on the controller. */
	for (i = 0; i < prop->master_list_count && i < SOUNDWIRE_MAX_LINK; i++) {
		struct acpi_dp *link = acpi_dp_new_table(link_names[i].acpi_name);
		soundwire_gen_link(link, &prop->master_list[i]);

		/* Callback for custom link properties from the controller. */
		if (link_prop_cb)
			link_prop_cb(link, i, prop);
		acpi_dp_add_child(dsd, link_names[i].sdw_name, link);
	}
}

void soundwire_gen_codec(struct acpi_dp *dsd, const struct soundwire_codec *codec,
			 soundwire_dp_prop_cb dp_prop_cb)
{
	const struct soundwire_dpn_entry *entry;
	const struct soundwire_name_map *name;
	size_t i;

	/* Generate slave properties for this codec. */
	soundwire_gen_slave(dsd, codec->slave);

	/* Generate properties for multilane config, if provided. */
	if (codec->multilane)
		soundwire_gen_multilane(dsd, codec->multilane);

	/* Generate properties for data port 0, if provided. */
	if (codec->dp0) {
		struct acpi_dp *dp0;

		/* First generate any Bulk Register Access mode properties. */
		for (i = 0; i < SOUNDWIRE_MAX_MODE; i++) {
			const struct soundwire_bra_mode *prop = codec->dp0_bra_mode[i];
			struct acpi_dp *bra;

			/* Stop processing at the first undefined BRA mode. */
			if (!prop)
				break;
			name = &bra_mode_names[i];
			bra = acpi_dp_new_table(name->acpi_name);
			soundwire_gen_bra_mode(bra, prop);
			acpi_dp_add_child(dsd, name->sdw_name, bra);
		}

		name = &dpn_source_names[0];
		dp0 = acpi_dp_new_table(name->acpi_name);
		soundwire_gen_dp0(dp0, codec->dp0);

		/* Callback for custom properties from the codec. */
		if (dp_prop_cb)
			dp_prop_cb(dp0, 0, codec);
		acpi_dp_add_child(dsd, name->sdw_name, dp0);
	}

	/*
	 * First generate audio modes for the data ports.  This results in unnecessary
	 * (but harmless) references to the audio modes at the codec level, but it allows
	 * the data ports to use these objects without duplication.
	 */
	for (i = 0; i < SOUNDWIRE_MAX_MODE; i++) {
		const struct soundwire_audio_mode *prop = codec->audio_mode[i];
		struct acpi_dp *am;

		/* Stop processing at the first undefined audio mode. */
		if (!prop)
			break;
		name = &audio_mode_names[i];
		am = acpi_dp_new_table(name->acpi_name);
		soundwire_gen_audio_mode(am, prop);
		acpi_dp_add_child(dsd, name->sdw_name, am);
	}

	/* Now generate properties for source/slave on each defined data port. */
	for (entry = codec->dpn; entry; entry++) {
		struct acpi_dp *dpn;

		/* Stop processing at the first invalid data port. */
		if (entry->port < SOUNDWIRE_MIN_DPN || entry->port > SOUNDWIRE_MAX_DPN)
			break;

		if (entry->source) {
			name = &dpn_source_names[entry->port];
			dpn = acpi_dp_new_table(name->acpi_name);
			soundwire_gen_dpn(dpn, entry->source);

			/* Callback for custom properties from the codec. */
			if (dp_prop_cb)
				dp_prop_cb(dpn, entry->port, codec);
			acpi_dp_add_child(dsd, name->sdw_name, dpn);
		}
		if (entry->sink) {
			name = &dpn_sink_names[entry->port];
			dpn = acpi_dp_new_table(name->acpi_name);
			soundwire_gen_dpn(dpn, entry->sink);

			/* Callback for custom properties from the codec. */
			if (dp_prop_cb)
				dp_prop_cb(dpn, entry->port, codec);
			acpi_dp_add_child(dsd, name->sdw_name, dpn);
		}
	}
}
