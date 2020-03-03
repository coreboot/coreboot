/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_DISPLAY_RESOURCES_H_
#define _SOC_DISPLAY_RESOURCES_H_

#include <stdlib.h>

#define MDP_MAX_CLOCK_NAME 30

/* MDP External Clocks Entry */
struct mdp_external_clock_entry {
	enum mdss_clock clk_type;

	/* Primary Clock Source */
	uint32_t clk_source;

	/* Secondary Clock source */
	uint32_t clk_secondary_source;

	/* Clock pre-divider */
	uint32_t clk_div;

	/* Clock M value */
	uint32_t clk_pll_m;

	/* Clock N value */
	uint32_t clk_pll_n;

	/* Clock 2D value */
	uint32_t clk_pll_2d;

	/* Clock may need source divider */
	uint32_t source_div;
};

#endif
