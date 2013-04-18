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

#include <common.h>
#include <errno.h>
#include <fdtdec.h>
#include <asm/arch/exynos-tmu.h>
#include <asm/arch/power.h>

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

/* Tmeperature threshold values for various thermal events */
struct temperature_params {
	/* minimum value in temperature code range */
	unsigned int min_val;
	/* maximum value in temperature code range */
	unsigned int max_val;
	/* temperature threshold to start warning */
	unsigned int start_warning;
	/* temperature threshold CPU tripping */
	unsigned int start_tripping;
	/* temperature threshold for HW tripping */
	unsigned int hardware_tripping;
};

/* Pre-defined values and thresholds for calibration of current temperature */
struct tmu_data {
	/* pre-defined temperature thresholds */
	struct temperature_params ts;
	/* pre-defined efuse range minimum value */
	unsigned int efuse_min_value;
	/* pre-defined efuse value for temperature calibration */
	unsigned int efuse_value;
	/* pre-defined efuse range maximum value */
	unsigned int efuse_max_value;
	/* current temperature sensing slope */
	unsigned int slope;
};

/* TMU device specific details and status */
struct tmu_info {
	/* base Address for the TMU */
	unsigned tmu_base;
	/* mux Address for the TMU */
	int tmu_mux;
	/* pre-defined values for calibration and thresholds */
	struct tmu_data data;
	/* value required for triminfo_25 calibration */
	unsigned int te1;
	/* value required for triminfo_85 calibration */
	unsigned int te2;
	/* TMU DC value for threshold calculation */
	int dc_value;
	/* enum value indicating status of the TMU */
	int tmu_state;
};

/* Global struct tmu_info variable to store init values */
static struct tmu_info gbl_info;

/*
 * After reading temperature code from register, compensating
 * its value and calculating celsius temperatue,
 * get current temperatue.
 *
 * @return	current temperature of the chip as sensed by TMU
 */
int get_cur_temp(struct tmu_info *info)
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
 * @param temp	pointer to the current temperature value
 * @return	enum tmu_status_t value, code indicating event to execute
 */
enum tmu_status_t tmu_monitor(int *temp)
{
	if (gbl_info.tmu_state == TMU_STATUS_INIT)
		return -1;

	int cur_temp;
	struct tmu_data *data = &gbl_info.data;

	/* Read current temperature of the SOC */
	cur_temp = get_cur_temp(&gbl_info);
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
		gbl_info.tmu_state = TMU_STATUS_INIT;
		debug("EXYNOS_TMU: Thermal reading failed\n");
		return -1;
	}
	return 0;
}

/*
 * Get TMU specific pre-defined values from FDT
 *
 * @param info	pointer to the tmu_info struct
 * @param blob  FDT blob
 * @return	int value, 0 for success
 */
int get_tmu_fdt_values(struct tmu_info *info, const void *blob)
{
	int node;
	int error = 0;

	/* Get the node from FDT for TMU */
	node = fdtdec_next_compatible(blob, 0,
				      COMPAT_SAMSUNG_EXYNOS_TMU);
	if (node < 0) {
		debug("EXYNOS_TMU: No node for tmu in device tree\n");
		return -1;
	}

	/*
	 * Get the pre-defined TMU specific values from FDT.
	 * All of these are expected to be correct otherwise
	 * miscalculation of register values in tmu_setup_parameters
	 * may result in misleading current temperature.
	 */
	info->tmu_base = fdtdec_get_addr(blob, node, "reg");
	if (info->tmu_base == FDT_ADDR_T_NONE) {
		debug("%s: Missing tmu-base\n", __func__);
		return -1;
	}
	info->tmu_mux = fdtdec_get_int(blob,
				node, "samsung,mux", -1);
	error |= info->tmu_mux;
	info->data.ts.min_val = fdtdec_get_int(blob,
				node, "samsung,min-temp", -1);
	error |= info->data.ts.min_val;
	info->data.ts.max_val = fdtdec_get_int(blob,
				node, "samsung,max-temp", -1);
	error |= info->data.ts.max_val;
	info->data.ts.start_warning = fdtdec_get_int(blob,
				node, "samsung,start-warning", -1);
	error |= info->data.ts.start_warning;
	info->data.ts.start_tripping = fdtdec_get_int(blob,
				node, "samsung,start-tripping", -1);
	error |= info->data.ts.start_tripping;
	info->data.ts.hardware_tripping = fdtdec_get_int(blob,
				node, "samsung,hw-tripping", -1);
	error |= info->data.ts.hardware_tripping;
	info->data.efuse_min_value = fdtdec_get_int(blob,
				node, "samsung,efuse-min-value", -1);
	error |= info->data.efuse_min_value;
	info->data.efuse_value = fdtdec_get_int(blob,
				node, "samsung,efuse-value", -1);
	error |= info->data.efuse_value;
	info->data.efuse_max_value = fdtdec_get_int(blob,
				node, "samsung,efuse-max-value", -1);
	error |= info->data.efuse_max_value;
	info->data.slope = fdtdec_get_int(blob,
				node, "samsung,slope", -1);
	error |= info->data.slope;
	info->dc_value = fdtdec_get_int(blob,
				node, "samsung,dc-value", -1);
	error |= info->dc_value;

	if (error == -1) {
		debug("fail to get tmu node properties\n");
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
void tmu_setup_parameters(struct tmu_info *info)
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
 * @param blob  FDT blob
 * @return	int value, 0 for success
 */
int tmu_init(const void *blob)
{
	gbl_info.tmu_state = TMU_STATUS_INIT;
	if (get_tmu_fdt_values(&gbl_info, blob) < 0)
		return -1;

	tmu_setup_parameters(&gbl_info);
	gbl_info.tmu_state = TMU_STATUS_NORMAL;

	return 0;
}
