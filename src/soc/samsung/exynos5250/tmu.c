/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
 * Copyright 2013 Google Inc.
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

/* EXYNOS - Thermal Management Unit */

#include <console/console.h>
#include <arch/io.h>
#include "power.h"
#include "tmu.h"

#define TRIMINFO_RELOAD		1
#define CORE_EN			1
#define THERM_TRIP_EN		(1 << 12)

#define INTEN_RISE0		1
#define INTEN_RISE1		(1 << 4)
#define INTEN_RISE2		(1 << 8)
#define INTEN_FALL0		(1 << 16)
#define INTEN_FALL1		(1 << 20)
#define INTEN_FALL2		(1 << 24)

#define TRIM_INFO_MASK		0xff

#define INTCLEAR_RISE0		1
#define INTCLEAR_RISE1		(1 << 4)
#define INTCLEAR_RISE2		(1 << 8)
#define INTCLEAR_FALL0		(1 << 16)
#define INTCLEAR_FALL1		(1 << 20)
#define INTCLEAR_FALL2		(1 << 24)
#define INTCLEARALL		(INTCLEAR_RISE0 | INTCLEAR_RISE1 | \
				 INTCLEAR_RISE2 | INTCLEAR_FALL0 | \
				 INTCLEAR_FALL1 | INTCLEAR_FALL2)

struct tmu_info exynos5250_tmu_info = {
	.tmu_base = 0x10060000,
	.tmu_mux = 6,
	.data = {
		.ts = {
			.min_val = 25,
			.max_val = 125,
			.start_warning = 95,
			.start_tripping = 105,
			.hardware_tripping = 110,
		},
		.efuse_min_value = 40,
		.efuse_value = 55,
		.efuse_max_value = 100,
		.slope = 0x10008802,
	},
	.dc_value = 25,
};

/*
 * After reading temperature code from register, compensating
 * its value and calculating celsius temperature,
 * get current temperature.
 *
 * @return	current temperature of the chip as sensed by TMU
 */
static int get_cur_temp(struct tmu_info *info)
{
	int cur_temp;
	struct tmu_reg *reg = (struct tmu_reg *)info->tmu_base;

	/* Temperature code range between min 25 and max 125 */
	cur_temp = readl(&reg->current_temp) & 0xff;

	/* Calibrate current temperature */
	if (cur_temp)
		cur_temp = cur_temp - info->te1 + info->dc_value;

	return cur_temp;
}

/*
 * Monitors status of the TMU device and exynos temperature
 *
 * @info	TMU info
 * @temp	pointer to the current temperature value
 * @return	enum tmu_status_t value, code indicating event to execute
 */
enum tmu_status_t tmu_monitor(struct tmu_info *info, int *temp)
{
	if (info->tmu_state == TMU_STATUS_INIT)
		return -1;

	int cur_temp;
	struct tmu_data *data = &info->data;

	/* Read current temperature of the SOC */
	cur_temp = get_cur_temp(info);
	*temp = cur_temp;

	/* Temperature code lies between min 25 and max 125 */
	if (cur_temp >= data->ts.start_tripping &&
			cur_temp <= data->ts.max_val)
		return TMU_STATUS_TRIPPED;
	else if (cur_temp >= data->ts.start_warning)
		return TMU_STATUS_WARNING;
	else if (cur_temp < data->ts.start_warning &&
			cur_temp >= data->ts.min_val)
		return TMU_STATUS_NORMAL;
	/* Temperature code does not lie between min 25 and max 125 */
	else {
		info->tmu_state = TMU_STATUS_INIT;
		printk(BIOS_DEBUG, "EXYNOS_TMU: Thermal reading failed\n");
		return -1;
	}
	return 0;
}

/*
 * Calibrate and calculate threshold values and
 * enable interrupt levels
 *
 * @param	info pointer to the tmu_info struct
 */
static void tmu_setup_parameters(struct tmu_info *info)
{
	unsigned int te_temp, con;
	unsigned int warning_code, trip_code, hwtrip_code;
	unsigned int cooling_temp;
	unsigned int rising_value;
	struct tmu_data *data = &info->data;
	struct tmu_reg *reg = (struct tmu_reg *)info->tmu_base;

	/* Must reload for using efuse value at EXYNOS */
	writel(TRIMINFO_RELOAD, &reg->triminfo_control);

	/* Get the compensation parameter */
	te_temp = readl(&reg->triminfo);
	info->te1 = te_temp & TRIM_INFO_MASK;
	info->te2 = ((te_temp >> 8) & TRIM_INFO_MASK);

	if ((data->efuse_min_value > info->te1) ||
			(info->te1 > data->efuse_max_value)
			||  (info->te2 != 0))
		info->te1 = data->efuse_value;

	/* Get RISING & FALLING Threshold value */
	warning_code = data->ts.start_warning
			+ info->te1 - info->dc_value;
	trip_code = data->ts.start_tripping
			+ info->te1 - info->dc_value;
	hwtrip_code = data->ts.hardware_tripping
			+ info->te1 - info->dc_value;

	cooling_temp = 0;

	rising_value = ((warning_code << 8) |
			(trip_code << 16) |
			(hwtrip_code << 24));

	/* Set interrupt level */
	writel(rising_value, &reg->threshold_temp_rise);
	writel(cooling_temp, &reg->threshold_temp_fall);

	/*
	 * Need to init all register settings after getting parameter info
	 * [28:23] vref [11:8] slope - Tuning parameter
	 *
	 * WARNING: this slope value writes into many bits in the tmu_control
	 * register, with the default FDT value of 268470274 (0x10008802)
	 * we are using this essentially sets the default register setting
	 * from the TRM for tmu_control.
	 * TODO(bhthompson): rewrite this code such that we are not performing
	 * a hard wipe of tmu_control and re verify functionality.
	 */
	writel(data->slope, &reg->tmu_control);

	writel(INTCLEARALL, &reg->intclear);
	/* TMU core enable */
	con = readl(&reg->tmu_control);
	con |= (info->tmu_mux << 20) | THERM_TRIP_EN | CORE_EN;

	writel(con, &reg->tmu_control);

	/* Enable HW thermal trip */
	power_enable_hw_thermal_trip();

	/* LEV1 LEV2 interrupt enable */
	writel(INTEN_RISE1 | INTEN_RISE2, &reg->inten);
}

/*
 * Initialize TMU device
 *
 * @return	int value, 0 for success
 */
int tmu_init(struct tmu_info *info)
{
	info->tmu_state = TMU_STATUS_INIT;

	tmu_setup_parameters(info);
	info->tmu_state = TMU_STATUS_NORMAL;

	return 0;
}
