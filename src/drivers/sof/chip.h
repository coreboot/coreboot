/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_AUDIO_SOF_H__
#define __DRIVERS_AUDIO_SOF_H__

#include <acpi/acpi_device.h>
#include <stdint.h>

/* Speaker topology */
enum _spkr_tplg {
	max98373 = 1,
	max98360a,
	max98357a,
	max98357a_tdm,
	max98390,
	rt1011,
	rt1015,
};

/* Jack topology */
enum _jack_tplg {
	cs42l42 = 1,
	da7219,
	nau8825,
	rt5682,
};

/* Mic topology */
enum _mic_tplg {
	_1ch = 1,
	_2ch_pdm0,
	_2ch_pdm1,
	_4ch,
};

struct drivers_sof_config {
	enum _spkr_tplg spkr_tplg;
	enum _jack_tplg jack_tplg;
	enum _mic_tplg mic_tplg;
};

#endif /* __DRIVERS_AUDIO_SOF_H__ */
