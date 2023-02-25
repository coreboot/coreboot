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

static struct soundwire_address cs42l42_address = {
	.version = SOUNDWIRE_VERSION_1_0,
	.manufacturer_id = MIPI_MFG_ID_CIRRUS,
	.part_id = MIPI_DEV_ID_CIRRUS_CS42L42,
	.class = MIPI_CLASS_NONE
};

static struct soundwire_slave cs42l42_slave = {
	.wake_up_unavailable = false,
	.test_mode_supported = false,
	.clock_stop_mode1_supported = true,
	.simplified_clockstopprepare_sm_supported = false,
	.clockstopprepare_hard_reset_behavior = true,
	.highPHY_capable = false,
	.paging_supported = true,
	.bank_delay_supported = true,
	.port15_read_behavior = false,
	.source_port_list = SOUNDWIRE_PORT(1),
	.sink_port_list = SOUNDWIRE_PORT(2) | SOUNDWIRE_PORT(3)
};

static struct soundwire_bra_mode cs42l42_dp0_bra_mode = {
	.bus_frequency_configs_count = 6,
	.bus_frequency_configs = {
		11289600,	/* 11.2896 MHz */
		12000 * KHz,	/* 12 MHz */
		12288 * KHz,	/* 12.288 MHz */
		22579200,	/* 22.5792 MHz */
		24000 * KHz,	/* 24 MHz */
		24576 * KHz,	/* 24.576 MHz */
	},
	.max_data_per_frame = 4096, /* MaxRow*MaxCol = 256*16 = 4096 */
	.min_us_between_transactions = 0
};

static struct soundwire_dp0 cs42l42_dp0 = {
	.port_max_wordlength = 64,
	.port_min_wordlength = 1,
	.bra_imp_def_response_supported = false,
	.simplified_channel_prepare_sm = true,
	.imp_def_dp0_interrupts_supported = 0,
	.imp_def_bpt_supported = true,
	.bra_mode_count = 1,
	.bra_mode_list = { 0 }
};

static struct soundwire_audio_mode cs42l42_audio_mode = {
	.bus_frequency_configs_count = 6,
	.bus_frequency_configs = {
		11289600,	/* 11.2896 MHz */
		12000 * KHz,	/* 12 MHz */
		12288 * KHz,	/* 12.288 MHz */
		22579200,	/* 22.5792 MHz */
		24000 * KHz,	/* 24 MHz */
		24576 * KHz,	/* 24.576 MHz */
	},
	/* Support 8 KHz to 192 KHz sampling frequency */
	.max_sampling_frequency = 192 * KHz,
	.min_sampling_frequency = 8 * KHz,
	.prepare_channel_behavior = CHANNEL_PREPARE_ANY_FREQUENCY
};

static struct soundwire_dpn cs42l42_dp1 = {
	.port_max_wordlength = 64,
	.port_min_wordlength = 1,
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

static struct soundwire_dpn cs42l42_dpn = {
	.port_max_wordlength = 64,
	.port_min_wordlength = 1,
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

static const struct soundwire_codec cs42l42_codec = {
	.slave = &cs42l42_slave,
	.dp0_bra_mode = { &cs42l42_dp0_bra_mode },
	.dp0 = &cs42l42_dp0,
	.audio_mode = { &cs42l42_audio_mode },
	.dpn = {
		{
			.port = 1,
			.source = &cs42l42_dp1,
		},
		{
			.port = 2,
			.sink = &cs42l42_dpn,
		},
		{
			.port = 3,
			.sink = &cs42l42_dpn,
		}
	}
};

static void soundwire_cs42l42_fill_ssdt(const struct device *dev)
{
	struct drivers_soundwire_cs42l42_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	const char *path = acpi_device_path(dev);
	struct acpi_dp *dsd;
	int gpio_index = 0;

	if (!scope)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));

	/* Set codec address IDs. */
	cs42l42_address.link_id = dev->path.generic.id;
	cs42l42_address.unique_id = dev->path.generic.subid;

	acpigen_write_ADR_soundwire_device(&cs42l42_address);
	acpigen_write_name_string("_DDN", config->desc ? : dev->chip_ops->name);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();

	/* Use either Interrupt() or GpioInt() */
	if (config->irq_gpio.pin_count)
		acpi_device_write_gpio(&config->irq_gpio);
	else
		acpi_device_write_interrupt(&config->irq);

	/* for cs42l42 reset gpio */
	if (config->reset_gpio.pin_count)
		acpi_device_write_gpio(&config->reset_gpio);

	acpigen_write_resourcetemplate_footer();

	dsd = acpi_dp_new_table("_DSD");

	if (config->irq_gpio.pin_count)
		acpi_dp_add_gpio(dsd, "irq-gpios", path,
			 gpio_index++,  /* Index = 0 */
			 0,  /* Pin = 0 (There is a single pin in the GPIO resource). */
			 config->irq_gpio.active_low);
	if (config->reset_gpio.pin_count)
		acpi_dp_add_gpio(dsd, "reset-gpios", path,
			 gpio_index++,  /* Index = 0 or 1 (if irq gpio is written). */
			 0,  /* Pin = 0 (There is a single pin in the GPIO resource). */
			 config->reset_gpio.active_low);
	acpi_dp_add_integer(dsd, "cirrus,ts-inv", config->ts_inv ? 1 : 0);
	acpi_dp_add_integer(dsd, "cirrus,ts-dbnc-rise", config->ts_dbnc_rise);
	acpi_dp_add_integer(dsd, "cirrus,ts-dbnc-fall", config->ts_dbnc_fall);
	acpi_dp_add_integer(dsd, "cirrus,btn-det-init-dbnce", config->btn_det_init_dbnce);
	if (config->btn_det_init_dbnce > 200) {
		printk(BIOS_ERR, "%s: Incorrect btn_det_init_dbnce(%d). Using default of 100ms\n",
				__func__, config->btn_det_init_dbnce);
		config->btn_det_init_dbnce = 100;
	}
	acpi_dp_add_integer(dsd, "cirrus,btn-det-event-dbnce", config->btn_det_event_dbnce);
	if (config->btn_det_event_dbnce > 100) {
		printk(BIOS_ERR, "%s: Incorrect btn_det_event_dbnce(%d). Using default of 10ms\n",
				__func__, config->btn_det_event_dbnce);
		config->btn_det_event_dbnce = 10;
	}
	acpi_dp_add_integer_array(dsd, "cirrus,bias-lvls", config->bias_lvls, 4);
	acpi_dp_add_integer(dsd, "cirrus,hs-bias-ramp-rate", config->hs_bias_ramp_rate);
	if (config->hs_bias_sense_disable)
		acpi_dp_add_integer(dsd, "cirrus,hs-bias-sense-disable", 1);

	soundwire_gen_codec(dsd, &cs42l42_codec, NULL);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *soundwire_cs42l42_acpi_name(const struct device *dev)
{
	struct drivers_soundwire_cs42l42_config *config = dev->chip_info;
	if (config->acpi_name[0] != 0)
		return config->acpi_name;
	snprintf(config->acpi_name, sizeof(config->acpi_name), "SW%1X%1X",
		 dev->path.generic.id, dev->path.generic.subid);
	return config->acpi_name;
}

static struct device_operations soundwire_cs42l42_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= soundwire_cs42l42_acpi_name,
	.acpi_fill_ssdt		= soundwire_cs42l42_fill_ssdt,
};

static void soundwire_cs42l42_enable(struct device *dev)
{
	dev->ops = &soundwire_cs42l42_ops;
}

struct chip_operations drivers_soundwire_cs42l42_ops = {
	CHIP_NAME("Cirrus Logic CS42L42 SoundWire Codec")
	.enable_dev = soundwire_cs42l42_enable
};
