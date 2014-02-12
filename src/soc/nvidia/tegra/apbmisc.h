/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __SOC_NVIDIA_TEGRA_APBMISC_H__
#define __SOC_NVIDIA_TEGRA_APBMISC_H__

#include <stdint.h>

struct apbmisc {
	u32 reserved0[9];		/* ABP_MISC_PP_ offsets 00-20 */
	u32 pp_config_ctl;		/* _CONFIG_CTL_0, offset 24 */
};

#define PP_CONFIG_CTL_TBE		(1 << 7)
#define PP_CONFIG_CTL_JTAG		(1 << 6)

void enable_jtag(void);

#endif	/* __SOC_NVIDIA_TEGRA_APBMISC_H__ */
