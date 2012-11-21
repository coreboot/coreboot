/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *      http://www.samsung.com
 * Akshay Saraswat <Akshay.s@samsung.com>
 *
 * EXYNOS - Thermal Management Unit
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

#ifndef __ASM_ARCH_THERMAL_H
#define __ASM_ARCH_THERMAL_H

struct tmu_reg {
	unsigned triminfo;
	unsigned rsvd1;
	unsigned rsvd2;
	unsigned rsvd3;
	unsigned rsvd4;
	unsigned triminfo_control;
	unsigned rsvd5;
	unsigned rsvd6;
	unsigned tmu_control;
	unsigned rsvd7;
	unsigned tmu_status;
	unsigned sampling_internal;
	unsigned counter_value0;
	unsigned counter_value1;
	unsigned rsvd8;
	unsigned rsvd9;
	unsigned current_temp;
	unsigned rsvd10;
	unsigned rsvd11;
	unsigned rsvd12;
	unsigned threshold_temp_rise;
	unsigned threshold_temp_fall;
	unsigned rsvd13;
	unsigned rsvd14;
	unsigned past_temp3_0;
	unsigned past_temp7_4;
	unsigned past_temp11_8;
	unsigned past_temp15_12;
	unsigned inten;
	unsigned intstat;
	unsigned intclear;
	unsigned rsvd15;
	unsigned emul_con;
};

enum tmu_status_t {
	TMU_STATUS_INIT = 0,
	TMU_STATUS_NORMAL,
	TMU_STATUS_WARNING,
	TMU_STATUS_TRIPPED,
};

/*
 * Monitors status of the TMU device and exynos temperature
 *
 * @param temp	pointer to the current temperature value
 * @return	enum tmu_status_t value, code indicating event to execute
 *		and -1 on error
 */
enum tmu_status_t tmu_monitor(int *temp);

/*
 * Initialize TMU device
 *
 * @param blob  FDT blob
 * @return	int value, 0 for success
 */
int tmu_init(const void *blob);
#endif
