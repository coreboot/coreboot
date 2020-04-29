/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_ACPI_SOUNDWIRE_H__
#define __ACPI_ACPI_SOUNDWIRE_H__

#include <acpi/acpi_device.h>
#include <device/soundwire.h>

/**
 * soundwire_dp_prop_cb() - Callback to add custom data port properties.
 * @dsd: ACPI Device Property handle for this data port.
 * @port_id: Data Port ID from 0-14.
 * @codec: Properties that were passed to soundwire_gen_codec().
 */
typedef void soundwire_dp_prop_cb(struct acpi_dp *dsd, unsigned int port_id,
				  const struct soundwire_codec *codec);

/**
 * soundwire_gen_codec() - Generate SoundWire properties for codec device.
 * @dsd: ACPI Device Property handle.
 * @prop: Properties for codec which includes all other properties.
 * @dp_prop_cb: Callback to allow custom codec properties.
 */
void soundwire_gen_codec(struct acpi_dp *dsd, const struct soundwire_codec *codec,
			 soundwire_dp_prop_cb dp_prop_cb);

/**
 * soundwire_link_prop_cb() - Callback to add custom link properties.
 * @dsd: ACPI Device Property handle for master link.
 * @link_id: Link number for this master.
 * @controller: Properties that were passed to soundwire_gen_controller().
 */
typedef void soundwire_link_prop_cb(struct acpi_dp *dsd, unsigned int link_id,
				    const struct soundwire_controller *controller);

/**
 * soundwire_gen_controller() - Generate SoundWire properties for master links.
 * @dsd: ACPI Device Property handle for controller.
 * @prop: Properties for controller which includes all other properties.
 * @link_prop_cb: Callback to allow custom link properties.
 */
void soundwire_gen_controller(struct acpi_dp *dsd, const struct soundwire_controller *prop,
			      soundwire_link_prop_cb link_prop_cb);

#endif /* __ACPI_ACPI_SOUNDWIRE_H__ */
