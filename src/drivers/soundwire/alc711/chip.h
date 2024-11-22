/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_SOUNDWIRE_ALC711_CHIP_H__
#define __DRIVERS_SOUNDWIRE_ALC711_CHIP_H__

#include <device/soundwire.h>
#include <mipi/ids.h>

struct drivers_soundwire_alc711_config {
	const char *name;
	const char *desc;
	struct soundwire_address alc711_address;
};

#endif /* __DRIVERS_SOUNDWIRE_ALC711_CHIP_H__ */
