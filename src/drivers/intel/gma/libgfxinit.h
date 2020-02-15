/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef DRIVERS_INTEL_GMA_LIBGFXINIT_H
#define DRIVERS_INTEL_GMA_LIBGFXINIT_H

enum {
	GMA_PORT_DISABLED,
	GMA_PORT_LVDS,
	GMA_PORT_EDP,
	GMA_PORT_DP1,
	GMA_PORT_DP2,
	GMA_PORT_DP3,
	GMA_PORT_HDMI1,	/* or DVI */
	GMA_PORT_HDMI2,	/* or DVI */
	GMA_PORT_HDMI3,	/* or DVI */
	GMA_PORT_ANALOG,
};

void gma_gfxinit(int *lightup_ok);
int gma_read_edid(unsigned char edid[], int port);

#endif
