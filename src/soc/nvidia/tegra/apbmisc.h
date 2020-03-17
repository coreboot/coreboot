/*
 * This file is part of the coreboot project.
 *
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

#ifndef __SOC_NVIDIA_TEGRA_APBMISC_H__
#define __SOC_NVIDIA_TEGRA_APBMISC_H__

#include <stdint.h>

struct apbmisc {
	u32 reserved0[2];		/* ABP_MISC_PP_ offsets 00 and 04 */
	u32 pp_strapping_opt_a;		/* _STRAPPING_OPT_A_0, offset 08 */
	u32 reserved1[6];		/* ABP_MISC_PP_ offsets 0C-20 */
	u32 pp_config_ctl;		/* _CONFIG_CTL_0, offset 24 */
	u32 reserved2[6];		/* APB_MISC_PP_ offsets 28-3C */
	u32 pp_pinmux_global;		/* _PINMUX_GLOBAL_0, offset 40 */
};

#define PP_CONFIG_CTL_TBE		(1 << 7)
#define PP_CONFIG_CTL_JTAG		(1 << 6)

#define PP_PINMUX_CLAMP_INPUTS		(1 << 0)

enum {
	MISC_GP_HIDREV = 0x804
};

struct tegra_revision {
	int hid_fam;
	int chip_id;
	int major;
	int minor;
};

void enable_jtag(void);
void clamp_tristate_inputs(void);
void tegra_revision_info(struct tegra_revision *id);

enum {
	PP_STRAPPING_OPT_A_RAM_CODE_SHIFT = 4,
	PP_STRAPPING_OPT_A_RAM_CODE_MASK =
		0xF << PP_STRAPPING_OPT_A_RAM_CODE_SHIFT,
};

#endif	/* __SOC_NVIDIA_TEGRA_APBMISC_H__ */
