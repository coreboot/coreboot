/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <device/path.h>
#include <string.h>

#include "chip.h"

static const char *get_spkr_tplg_str(unsigned int index)
{
	switch (index) {
		case 1: return "max98373";
		case 2: return "max98360a";
		case 3: return "max98357a";
		case 4: return "max98357a-tdm";
		case 5: return "max98390";
		case 6: return "rt1011";
		case 7: return "rt1015";
		default: return "default";
	}
}

static const char *get_jack_tplg_str(unsigned int index)
{
	switch (index) {
		case 1: return "cs42l42";
		case 2: return "da7219";
		case 3: return "nau8825";
		case 4: return "rt5682";
		default: return "default";
	}
}

static const char *get_mic_tplg_str(unsigned int index)
{
	switch (index) {
		case 1: return "1ch";
		case 2: return "2ch-pdm0";
		case 3: return "2ch-pdm1";
		case 4: return "4ch";
		default: return "default";
	}
}

static void sof_fill_ssdt_generator(const struct device *dev)
{
	struct drivers_sof_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	struct acpi_dp *dsd;

	if (!dev->enabled || !config || !scope)
		return;

	/* Device */
	acpigen_write_scope(scope);

	/* DSD */
	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_string(dsd, "speaker-tplg",
				get_spkr_tplg_str(config->spkr_tplg));
	acpi_dp_add_string(dsd, "hp-tplg",
				get_jack_tplg_str(config->jack_tplg));
	acpi_dp_add_string(dsd, "mic-tplg",
				get_mic_tplg_str(config->mic_tplg));
	acpi_dp_write(dsd);
	acpigen_pop_len(); /* Scope */
}


static struct device_operations sof_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_fill_ssdt		= sof_fill_ssdt_generator,
};

static void sof_enable(struct device *dev)
{
	dev->ops = &sof_ops;
}

struct chip_operations drivers_sof_ops = {
	CHIP_NAME("SOF")
	.enable_dev = sof_enable
};
