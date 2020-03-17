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

#ifndef __SOC_NVIDIA_TEGRA210_MTC_H__
#define __SOC_NVIDIA_TEGRA210_MTC_H__

#include <boot/coreboot_tables.h>

#if CONFIG(HAVE_MTC)

int tegra210_run_mtc(void);
void soc_add_mtc(struct lb_header *header);

#else

static inline int tegra210_run_mtc(void) { return 0; }
static inline void soc_add_mtc(struct lb_header *header) {}

#endif /* CONFIG_HAVE_MTC */

#endif /* __SOC_NVIDIA_TEGRA210_MTC_H__ */
