/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _TEGRA_MIPI_PHY_H
#define _TEGRA_MIPI_PHY_H

/*
 * Macros for calculating the phy timings
 */
/* Period of one bit time in nano seconds */
#define DSI_TBIT_Factorized(Freq)    (((1000) * (1000))/(Freq))
#define DSI_TBIT(Freq)       (DSI_TBIT_Factorized(Freq)/(1000))

//#define NV_MAX(a,b) (((a) > (b)) ? (a) : (b))

/* Period of one byte time in nano seconds */
#define DSI_TBYTE(Freq)    ((DSI_TBIT_Factorized(Freq)) * (8))
#define DSI_PHY_TIMING_DIV(X, Freq) ((X*1000) / (DSI_TBYTE(Freq)))

/*
 * As per Mipi spec (minimum):
 * (3 + MAX(8 * DSI_TBIT, 60 + 4 * DSI_TBIT) / DSI_TBYTE)
 */
#define DSI_THSTRAIL_VAL(Freq) \
     (MAX(((8) * (DSI_TBIT(Freq))), ((60) + ((4) * (DSI_TBIT(Freq))))))

int mipi_dphy_set_timing(struct tegra_dsi *dsi);

#endif
