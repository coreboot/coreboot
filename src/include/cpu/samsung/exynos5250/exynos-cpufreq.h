/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *      http://www.samsung.com
 *
 * EXYNOS - CPU frequency scaling support
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* Define various levels of ARM frequency */
enum cpufreq_level {
	CPU_FREQ_L200,		/* 200 MHz */
	CPU_FREQ_L300,		/* 300 MHz */
	CPU_FREQ_L400,		/* 400 MHz */
	CPU_FREQ_L500,		/* 500 MHz */
	CPU_FREQ_L600,		/* 600 MHz */
	CPU_FREQ_L700,		/* 700 MHz */
	CPU_FREQ_L800,		/* 800 MHz */
	CPU_FREQ_L900,		/* 900 MHz */
	CPU_FREQ_L1000,		/* 1000 MHz */
	CPU_FREQ_L1100,		/* 1100 MHz */
	CPU_FREQ_L1200,		/* 1200 MHz */
	CPU_FREQ_L1300,		/* 1300 MHz */
	CPU_FREQ_L1400,		/* 1400 MHz */
	CPU_FREQ_L1500,		/* 1500 MHz */
	CPU_FREQ_L1600,		/* 1600 MHz */
	CPU_FREQ_L1700,		/* 1700 MHz */
	CPU_FREQ_LCOUNT,
};

/*
 * Initialize ARM frequency scaling
 *
 * @param blob  FDT blob
 * @return	int value, 0 for success
 */
int exynos5250_cpufreq_init(const void *blob);

/*
 * Switch ARM frequency to new level
 *
 * @param new_freq_level	enum cpufreq_level, states new frequency
 * @return			int value, 0 for success
 */
int exynos5250_set_frequency(enum cpufreq_level new_freq_level);
