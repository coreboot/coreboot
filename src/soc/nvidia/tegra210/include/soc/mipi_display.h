/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Defines for Mobile Industry Processor Interface (MIPI(R))
 * Display Working Group standards: DSI, DCS, DBI, DPI
 *
 * Author: Imre Deak <imre.deak@nokia.com>
 */
#ifndef __SOC_MIPI_DISPLAY_H__
#define __SOC_MIPI_DISPLAY_H__

#include <mipi/dsi.h>

/* MIPI DCS pixel formats */
#define MIPI_DCS_PIXEL_FMT_24BIT	7
#define MIPI_DCS_PIXEL_FMT_18BIT	6
#define MIPI_DCS_PIXEL_FMT_16BIT	5
#define MIPI_DCS_PIXEL_FMT_12BIT	3
#define MIPI_DCS_PIXEL_FMT_8BIT		2
#define MIPI_DCS_PIXEL_FMT_3BIT		1

#endif	/* __SOC_MIPI_DISPLAY_H__ */
