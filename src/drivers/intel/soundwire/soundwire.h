/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_INTEL_SOUNDWIRE_H__
#define __DRIVERS_INTEL_SOUNDWIRE_H__

#include <device/soundwire.h>
#include <stdint.h>

/**
 * enum intel_soundwire_quirk - Quirks for controller master links.
 * @INTEL_SOUNDWIRE_QUIRK_STATIC_CLOCK: Link clock is fixed.
 * @INTEL_SOUNDWIRE_QUIRK_BUS_DISABLE: This link should be disabled.
 */
enum intel_soundwire_quirk {
	INTEL_SOUNDWIRE_QUIRK_STATIC_CLOCK = BIT(0),
	INTEL_SOUNDWIRE_QUIRK_BUS_DISABLE = BIT(1),
};

/**
 * struct intel_soundwire_controller - SoundWire controller configuration for Intel SoC.
 * @dev: Device handle for this controller.
 * @acpi_address: ACPI address for this controller.  This is a custom address that is not
 *                compatible with either PCI or SoundWire.
 * @ip_clock: Frequency of the source clock connected to the controller.
 * @quirk_mask: Quirks that can be passed to the kernel drivers.
 * @sdw: SoundWire controller properties defined in MIPI SoundWire DisCo Specification.
 */
struct intel_soundwire_controller {
	const struct device *dev;
	uint64_t acpi_address;
	unsigned int ip_clock;
	unsigned int quirk_mask;
	struct soundwire_controller sdw;
};

/**
 * soc_fill_soundwire_controller() - Get SoundWire controller properties from the SoC.
 * @controller: Properties to be filled by the SoC.
 * Return zero for success, -1 if there was any error filling the properties.
 */
int soc_fill_soundwire_controller(struct intel_soundwire_controller **controller);

#endif /* __DRIVERS_INTEL_SOUNDWIRE_H__ */
