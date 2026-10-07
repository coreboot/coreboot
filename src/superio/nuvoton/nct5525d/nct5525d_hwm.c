/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <console/console.h>
#include <superio/hwm5_conf.h>
#include <superio/nuvoton/common/hwm.h>

#if CONFIG(SOC_AMD_COMMON_BLOCK_SMN)
#include <amdblocks/smn.h>
#endif

#include "chip.h"
#include "nct5525d_hwm.h"

/* Bank 0 */
#define HWM_FAN_OUT_MODE		0xf3	/* [1] CPUFANOUT, [0] SYSFANOUT: 1 = DC */
#define HWM_AUXFANIN2_PULSES		0xf5	/* [7:6] */
#define HWM_FANIN_PULSES		0xf6	/* [3:2] CPUFANIN, [1:0] SYSFANIN */

/* Bank 1, offsets from the per-fan control block */
#define FAN_SOURCE			0x00	/* [7] stop duty enable, [4:0] source */
#define  FAN_STOP_DUTY_EN		(1 << 7)
#define  FAN_SOURCE_MASK		0x1f
#define FAN_TARGET_TEMP			0x01
#define FAN_MODE_TOLERANCE		0x03	/* [7:4] mode, [2:0] tolerance */
#define  FAN_MODE_SHIFT			4
#define  FAN_MODE_MASK			(0xf << FAN_MODE_SHIFT)
#define  FAN_MODE_MANUAL		0
#define  FAN_MODE_THERMAL_CRUISE	1
#define  FAN_MODE_SMART_FAN_IV		4
#define  FAN_TOLERANCE_MASK		0x07
#define FAN_STEP_UP_TIME		0x04
#define FAN_STEP_DOWN_TIME		0x05
#define FAN_STOP_DUTY			0x06
#define FAN_START_DUTY			0x07
#define FAN_STOP_TIME			0x08
#define FAN_OUTPUT_VALUE		0x09
#define FAN_CRIT_TEMP			0x0a
#define FAN_CRIT_TOLERANCE		0x0b
#define FAN_CRIT_DUTY_CTRL		0x0c	/* [4] enable critical duty */
#define  FAN_CRIT_DUTY_EN		(1 << 4)
#define FAN_CRIT_DUTY			0x0d

/* Bank 1, offsets from the per-fan SMART FAN IV curve block */
#define FAN_CURVE_TEMP(i)		(0x00 + (i))
#define FAN_CURVE_DUTY(i)		(0x04 + (i))

/* Bank 3 */
#define HWM_SENSOR_TYPE			0x18	/* [3] AUXTIN, [2] CPUTIN, [1] SYSTIN: 1 = diode */
#define HWM_CURRENT_MODE		0x19	/* [2] AUXTIN, [1] CPUTIN, [0] SYSTIN: 1 = current */

/* SMBus master, offsets from the LDN B CR62h base */
#define SMBM_TSI_AGENT			0x0d	/* [1] agent 1, [0] agent 0 */
#define  SMBM_TSI_AGENT0_EN		(1 << 0)
#define SMBM_TSI0_ADDR			0x16
#define SMBM_TSI1_ADDR			0x17
#define SMBM_CTL2			0x0f	/* [1:0] bank select */
#define  SMBM_CTL2_BANK_MASK		0x3

struct nct5525d_fan {
	const char *name;
	uint8_t ctrl;		/* Bank 1 control block */
	uint8_t curve;		/* Bank 1 SMART FAN IV curve block */
};

static const struct nct5525d_fan fans[] = {
	{ "SYSFAN", 0x10, 0x60 },
	{ "CPUFAN", 0x20, 0x70 },
};

static void hwm_update(u16 base, u8 reg, u8 mask, u8 value)
{
	u8 tmp = pnp_read_hwm5_index(base, reg);

	pnp_write_hwm5_index(base, reg, (tmp & ~mask) | (value & mask));
}

#define AMD_REPORTED_TEMP_CTRL		0x00059800	/* SMN THM_TCON_CUR_TMP */
#define  AMD_CUR_TEMP_RANGE_SEL		(1 << 19)
#define  AMD_CUR_TEMP_TJ_SEL		(3 << 16)
#define AMD_TEMP_EXT_RANGE_ADJ		49

/*
 * AMD processors can report temperatures in an extended range of -49 to
 * 206.875 degrees C instead of 0 to 255.875 degrees C. Values are then
 * reported as the temperature plus 49 degrees C. This applies to SB-TSI
 * (RangeSelect, config register bit 2) as well as to the temperature reported
 * through SMN (CUR_TEMP_RANGE_SEL, AMD SoC Register Reference #56255,
 * THM_TCON_CUR_TMP). Linux handles this in sbtsi_temp and k10temp.
 *
 * The fan control uses the raw SB-TSI value, so temperature thresholds for a
 * TSI source need the same offset. The SB-TSI config register can't be read
 * through the Super I/O, so use the range selected by the processor.
 */
static int tsi_temp_offset(void)
{
#if CONFIG(SOC_AMD_COMMON_BLOCK_SMN)
	const u32 val = smn_read32(AMD_REPORTED_TEMP_CTRL);

	if ((val & AMD_CUR_TEMP_RANGE_SEL) ||
	    (val & AMD_CUR_TEMP_TJ_SEL) == AMD_CUR_TEMP_TJ_SEL)
		return AMD_TEMP_EXT_RANGE_ADJ;
#endif
	return 0;
}

/* The fan control temperature registers are signed, up to 127 degrees C. */
#define FAN_TEMP_MAX			127

static u8 temp_to_reg(u8 temp, int offset)
{
	if (temp + offset > FAN_TEMP_MAX) {
		printk(BIOS_WARNING, "NCT5525D: fan temperature %d C capped to %d C\n",
		       temp, FAN_TEMP_MAX - offset);
		return FAN_TEMP_MAX;
	}

	return temp + offset;
}

static void configure_fan(u16 base, const struct nct5525d_fan *fan,
			  const struct nct5525d_fan_config *cfg)
{
	const u8 c = fan->ctrl;
	int offset = 0;
	size_t i;

	if (cfg->source == NCT5525D_TEMP_SMBUSMASTER0) {
		offset = tsi_temp_offset();
		if (offset)
			printk(BIOS_DEBUG, "NCT5525D: %s TSI temperatures offset by %d C\n",
			       fan->name, offset);
	}

	nuvoton_hwm_select_bank(base, 1);

	if (cfg->source != NCT5525D_TEMP_KEEP)
		hwm_update(base, c + FAN_SOURCE, FAN_SOURCE_MASK, cfg->source);
	hwm_update(base, c + FAN_SOURCE, FAN_STOP_DUTY_EN,
		   cfg->stop_duty_en ? FAN_STOP_DUTY_EN : 0);

	if (cfg->step_up_time)
		pnp_write_hwm5_index(base, c + FAN_STEP_UP_TIME, cfg->step_up_time);
	if (cfg->step_down_time)
		pnp_write_hwm5_index(base, c + FAN_STEP_DOWN_TIME, cfg->step_down_time);
	if (cfg->stop_duty)
		pnp_write_hwm5_index(base, c + FAN_STOP_DUTY, cfg->stop_duty);

	for (i = 0; i < NCT5525D_FAN_CURVE_POINTS; i++) {
		pnp_write_hwm5_index(base, fan->curve + FAN_CURVE_TEMP(i),
				     temp_to_reg(cfg->temp[i], offset));
		pnp_write_hwm5_index(base, fan->curve + FAN_CURVE_DUTY(i), cfg->curve_duty[i]);
	}

	if (cfg->crit_temp)
		pnp_write_hwm5_index(base, c + FAN_CRIT_TEMP,
				     temp_to_reg(cfg->crit_temp, offset));

	/* The mode is written last, once the other parameters are in place. */
	hwm_update(base, c + FAN_MODE_TOLERANCE, FAN_MODE_MASK,
		   FAN_MODE_SMART_FAN_IV << FAN_MODE_SHIFT);
}

/* The current mode bits are one below the diode select bits. */
static void configure_sensor_type(u16 base, u8 bit, enum nct5525d_sensor_type type)
{
	const u8 diode = 1 << bit;
	const u8 current = 1 << (bit - 1);

	if (type == NCT5525D_SENSOR_KEEP)
		return;

	hwm_update(base, HWM_SENSOR_TYPE, diode, diode);
	hwm_update(base, HWM_CURRENT_MODE, current,
		   type == NCT5525D_SENSOR_DIODE_CURRENT ? current : 0);
}

void nct5525d_hwm_init(u16 base, const struct superio_nuvoton_nct5525d_config *conf)
{
	const struct nct5525d_fan_config *cfg[] = { &conf->sysfan, &conf->cpufan };
	size_t i;

	nuvoton_hwm_select_bank(base, 3);
	configure_sensor_type(base, 1, conf->systin_type);
	configure_sensor_type(base, 2, conf->cputin_type);
	configure_sensor_type(base, 3, conf->auxtin_type);

	for (i = 0; i < ARRAY_SIZE(fans); i++) {
		if (cfg[i]->mode != NCT5525D_FAN_SMART_FAN_IV)
			continue;
		printk(BIOS_DEBUG, "NCT5525D: configuring %s\n", fans[i].name);
		configure_fan(base, &fans[i], cfg[i]);
	}

	nuvoton_hwm_select_bank(base, 0);
}

void nct5525d_tsi_init(u16 base, const struct superio_nuvoton_nct5525d_config *conf)
{
	/* The TSI registers are in bank 0 of the SMBus master */
	outb(inb(base + SMBM_CTL2) & ~SMBM_CTL2_BANK_MASK, base + SMBM_CTL2);

	if (conf->tsi_agent0_enable)
		outb(inb(base + SMBM_TSI_AGENT) | SMBM_TSI_AGENT0_EN, base + SMBM_TSI_AGENT);
}
