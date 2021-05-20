/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRIVERS_INTEL_PMC_MUX_CONN_H__
#define __DRIVERS_INTEL_PMC_MUX_CONN_H__

enum typec_orientation {
	/* The orientation of the signal follows the orientation of the CC lines. */
	TYPEC_ORIENTATION_FOLLOW_CC = 0,
	/* The orientation of the signal is fixed to follow CC1 */
	TYPEC_ORIENTATION_NORMAL,
	/* The orientation of the signal is fixed to follow CC2 */
	TYPEC_ORIENTATION_REVERSE,
};

struct drivers_intel_pmc_mux_conn_config {
	/* 1-based port numbers (from SoC point of view) */
	int usb2_port_number;
	/* 1-based port numbers (from SoC point of view) */
	int usb3_port_number;
	/* Orientation of the sideband signals (SBU) */
	enum typec_orientation sbu_orientation;
	/* Orientation of the High Speed lines */
	enum typec_orientation hsl_orientation;
};

/*
 * Method verifies input "conn" device.
 * Returns 'true' if device passed is Intel PMC MUX Conn device else returns 'false'.
 * Method also outputs the usb2 and usb3 port numbers associated with the 'conn' device
 */
bool intel_pmc_mux_conn_get_ports(const struct device *conn, unsigned int *usb2_port,
				unsigned int *usb3_port);

#endif /* __DRIVERS_INTEL_PMC_MUX_CONN_H__ */
