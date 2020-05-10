/* SPDX-License-Identifier: GPL-2.0-only */

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
