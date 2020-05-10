/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_SIFIVE_HIFIVE_U_CLOCK_H__
#define __SOC_SIFIVE_HIFIVE_U_CLOCK_H__

void clock_init(void);
int clock_get_coreclk_khz(void);
int clock_get_tlclk_khz(void);

#endif /* __SOC_SIFIVE_HIFIFE_U_CLOCK_H__ */
