/* SPDX-License-Identifier: GPL-2.0-only */

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
void gma_gfxstop(void);
int gma_read_edid(unsigned char edid[], int port);

#endif
