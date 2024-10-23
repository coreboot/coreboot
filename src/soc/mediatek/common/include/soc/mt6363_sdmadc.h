/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MT6363_SDMADC_H__
#define __MT6363_SDMADC_H__

#include <soc/spmi.h>
#include <types.h>

enum {
	PMIC_AUXADC_ADC_OUT_CH12_L_ADDR		= 0x10d2,
	MT6363_AUXADC_RQST1			= 0x1109,
	PMIC_SDMADC_EXT_THR_SRC_SEL_ADDR	= 0x11c4,
};

/*
 * enum auxadc_channels - enumeration of auxadc channels
 */
enum auxadc_channel {
	AUXADC_CHAN_VIN1,
	AUXADC_CHAN_VIN2,
	AUXADC_CHAN_VIN3,
	AUXADC_CHAN_VIN4,
	AUXADC_CHAN_VIN5,
	AUXADC_CHAN_VIN6,
	AUXADC_CHAN_VIN7,
	AUXADC_CHAN_MAX,
};

/*
 * enum sdmadc_pures - enumeration of sdmadc pull up resistor
 */
enum sdmadc_pures {
	SDMADC_100K,
	SDMADC_30K,
	SDMADC_400K,
	SDMADC_OPEN,
};

/**
 * enum auxadc_val_type - enumeration of auxadc value's type
 */
enum auxadc_val_type {
	AUXADC_VAL_PROCESSED,
	AUXADC_VAL_RAW,
};

/**
 * struct auxadc_chan_spec - specification of an auxadc channel
 * @channel:		What auxadc_channel is it.
 * @hw_info:		Hardware design level information
 * @hw_info.ref_volt	Reference voltage
 * @hw_info.ratio:	Resistor Ratio.
 *			If read operation is not provided, the processed
 *			value will be calculated as following:
 *			(raw value * ratio[0] * ref_volt) / ratio[1]
 * @hw_info.max_time:	Maximum wait time(us).
 * @hw_info.min_time:	Minimum wait time(us).
 * @hw_info.poll_time:	Wait time(us) between every polling during retry.
 * @hw_info.enable_reg	Register address of enable control.
 * @hw_info.enable_mask	Mask of enable control.
 * @hw_info.ready_reg	Register address of channel's ready status.
 * @hw_info.ready_mask	Mask of ready status.
 * @hw_info.value_reg	Register address of raw value.
 * @hw_info.res		Resolution of this channel (number of valid bits).
 */
struct auxadc_chan_spec {
	int channel;
	struct {
		u32 ref_volt;
		u32 ratio[2];
		u32 max_time;
		u32 min_time;
		u32 poll_time;
		u32 enable_reg;
		u32 enable_mask;
		u32 ready_reg;
		u32 ready_mask;
		u32 value_reg;
		u8 res;
	} hw_info;
	struct {
		u32 set_reg;
		u32 cmd;
	} sdmadc_hw_info;
};

int mt6363_sdmadc_read(enum auxadc_channel channel, int *val, enum sdmadc_pures pures,
		       enum auxadc_val_type type);

#endif /* __MT6363_SDMADC_H__ */
