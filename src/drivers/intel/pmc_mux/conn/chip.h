/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRIVERS_INTEL_PMC_MUX_CONN_H__
#define __DRIVERS_INTEL_PMC_MUX_CONN_H__

#include <boot/coreboot_tables.h>

struct drivers_intel_pmc_mux_conn_config {
	/* A pointer to the SoC's USB-2 device */
	DEVTREE_CONST struct device *usb2_port;
	/* A pointer to the SoC's USB-3 device */
	DEVTREE_CONST struct device *usb3_port;
	/* Orientation of the sideband signals (SBU) */
	enum type_c_orientation sbu_orientation;
	/* Orientation of the High Speed lines */
	enum type_c_orientation hsl_orientation;
};

/*
 * Method verifies input "conn" device.
 * Returns 'true' if device passed is Intel PMC MUX Conn device else returns 'false'.
 * Method also outputs the usb2 and usb3 port numbers associated with the 'conn' device
 */
bool intel_pmc_mux_conn_get_ports(const struct device *conn, unsigned int *usb2_port,
				unsigned int *usb3_port);

#endif /* __DRIVERS_INTEL_PMC_MUX_CONN_H__ */
