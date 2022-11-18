/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include "../common/fan_control.h"
#include "f81803a_hwm.h"

static const char msg_err_invalid[] = "Error: invalid";
static const char msg_err_wrong_order[] = "Error: wrong order,";
static const char msg_err_fan[] = "fan";
static const char msg_err_temp_source[] = "temperature source";
static const char msg_err_type[] = "type";
static const char msg_err_mode[] = "mode";
static const char msg_err_rate[] = "change rate";
static const char msg_err_frequency[] = "frequency";
static const char msg_err_temp_sensor[] = "temperature sensor";
static const char msg_err_bondary[] = "boundary";
static const char msg_err_section[] = "section";
static const char no_msg[] = "";

struct cross_ref {
	int selection;
	const char *message;
};
static struct cross_ref msg_table[] = {
	{HWM_STATUS_INVALID_FAN,		msg_err_fan},
	{HWM_STATUS_INVALID_TEMP_SOURCE,	msg_err_temp_source},
	{HWM_STATUS_INVALID_TYPE,		msg_err_type},
	{HWM_STATUS_INVALID_MODE,		msg_err_mode},
	{HWM_STATUS_INVALID_RATE,		msg_err_rate},
	{HWM_STATUS_INVALID_FREQUENCY,		msg_err_frequency},
	{HWM_STATUS_INVALID_TEMP_SENSOR,	msg_err_temp_sensor},
	{0, NULL},
};

static const char *get_msg(int err)
{
	int i = 0;
	while (msg_table[i].selection) {
		if (msg_table[i].selection == err)
			return msg_table[i].message;
		i++;
	}
	return no_msg;
}

static int message_invalid_1(int err, u8 fan)
{
	if (err == HWM_STATUS_INVALID_FAN)
		printk(BIOS_ERR, "%s %s %d!\n", msg_err_invalid, get_msg(err), fan);
	else
		printk(BIOS_ERR, "%s Fan %d %s!\n", msg_err_invalid, fan, get_msg(err));
	return err;
}

static int message_invalid_2(int err, u8 fan)
{
	switch (err) {
	case HWM_STATUS_INVALID_BOUNDARY_VALUE:
		printk(BIOS_ERR, "%s fan %d %s value!\n", msg_err_invalid, fan,
					msg_err_bondary);
		break;
	case HWM_STATUS_INVALID_SECTION_VALUE:
		printk(BIOS_ERR, "%s fan %d %s value!\n", msg_err_invalid, fan,
					msg_err_section);
		break;
	case HWM_STATUS_BOUNDARY_WRONG_ORDER:
		printk(BIOS_ERR, "%s fan %d %s!\n", msg_err_wrong_order, fan, msg_err_bondary);
		break;
	case HWM_STATUS_SECTIONS_WRONG_ORDER:
		printk(BIOS_ERR, "%s fan %d %s!\n", msg_err_wrong_order, fan, msg_err_section);
		break;
	default:
		break;
	}
	return err;
}

static void write_hwm_reg(u16 address, u8 index, u8 value)
{
	u16 index_add, data_add;
	index_add = address | 0x0001;	/* force odd address */
	data_add = index_add + 1;
	outb(index, index_add);
	outb(value, data_add);
}

static u8 read_hwm_reg(u16 address, u8 index)
{
	u16 index_add, data_add;
	index_add = address | 0x0001;	/* force odd address */
	data_add = index_add + 1;
	outb(index, index_add);
	return inb(data_add);
}

static void hwm_reg_modify(u16 address, u8 index, u8 shift, u8 mask,
								u8 value)
{
	u8 use_mask = mask << shift;
	u8 use_value = (value & mask) << shift;
	u8 temp = read_hwm_reg(address, index);

	temp &= ~use_mask;
	temp |= use_value;
	write_hwm_reg(address, index, temp);
}

/*
 * Registers 0x94,0x95, 0x96 and 0x9b have 2 versions (banks) selected through
 * bit 7 of register 0x9f.
 */
static inline void select_hwm_bank(u16 address, u8 value)
{
	hwm_reg_modify(address, FAN_FAULT_TIME_REG, FAN_FUNC_PROG_SEL_SHIFT,
				FAN_BIT_MASK, value);
}

/*
 * Boundaries and sections must be presented in the same order as in the HWM
 * registers, that is, from highest value to lowest. This procedure checks for
 * the correct order.
 */
static int check_value_seq(u8 *values, u8 count)
{
	u8 last_value = CPU_DAMAGE_TEMP;
	u8 current_value, i;
	for (i = 0; i < count; i++) {
		current_value = values[i];
		if (current_value > CPU_DAMAGE_TEMP)
			return STATUS_INVALID_VALUE;
		if (current_value >= last_value)
			return STATUS_INVALID_ORDER;
		last_value = current_value;
	}
	return HWM_STATUS_SUCCESS;
}

int set_sensor_type(u16 base_address, external_sensor sensor,
						temp_sensor_type type)
{
	u8 sensor_status = read_hwm_reg(base_address, TP_DIODE_STATUS);

	printk(BIOS_DEBUG, "%s\n", __func__);
	switch (sensor) {
	case EXTERNAL_SENSOR1:
		if (sensor_status & TP_EXTERNAL_SENSOR1_OPEN) {
			printk(BIOS_WARNING, "Sensor 1 disconnected!\n");
			return HWM_STATUS_WARNING_SENSOR_DISCONNECTED;
		}
		hwm_reg_modify(base_address, TP_SENSOR_TYPE,
			TP_SENSOR1_TYPE_SHIFT, TP_SENSOR_TYPE_MASK, type);
		break;
	case EXTERNAL_SENSOR2:
		if (sensor_status & TP_EXTERNAL_SENSOR2_OPEN) {
			printk(BIOS_WARNING, "Sensor 2 disconnected!\n");
			return HWM_STATUS_WARNING_SENSOR_DISCONNECTED;
		}
		hwm_reg_modify(base_address, TP_SENSOR_TYPE,
			TP_SENSOR2_TYPE_SHIFT, TP_SENSOR_TYPE_MASK, type);
		break;
	case IGNORE_SENSOR:
		break;
	default:
		return message_invalid_1(HWM_STATUS_INVALID_TEMP_SENSOR, 0);
	}
	return HWM_STATUS_SUCCESS;
}

int set_fan_temperature_source(u16 base_address, u8 fan,
						fan_temp_source source)
{
	u8 index, high_value, low_value;

	printk(BIOS_DEBUG, "%s\n", __func__);
	if ((fan < FIRST_FAN) || (fan > LAST_FAN))
		return message_invalid_1(HWM_STATUS_INVALID_FAN, fan);
	index = FAN_ADJUST(fan, FAN_TMP_MAPPING);
	high_value = (source >> 2) & FAN_BIT_MASK;
	low_value = source & FAN_TEMP_SEL_LOW_MASK;
	hwm_reg_modify(base_address, index, FAN_TEMP_SEL_HIGH_SHIFT,
				FAN_BIT_MASK, high_value);
	hwm_reg_modify(base_address, index, FAN_TEMP_SEL_LOW_SHIFT,
				FAN_TEMP_SEL_LOW_MASK, low_value);
	/*
	 * Fan 1 has a weight mechanism for adjusting for next fan speed. Basically the idea is
	 * to react more aggressively (normally CPU fan) based on how high another temperature
	 * (system, thermistor near the CPU, anything) is. This would be highly platform
	 * dependent, and by setting the weight temperature same as the control temperature.
	 * This code cancels the weight mechanism and make it work with any board. If a board
	 * wants to use the weight mechanism, OEM should implement it after calling the main
	 * HWM programming.
	 */
	if (fan == FIRST_FAN) {
		select_hwm_bank(base_address, 1);
		hwm_reg_modify(base_address, FAN_MODE_REG,
				FAN1_ADJ_SEL_SHIFT, FAN1_ADJ_SEL_MASK, source);
		select_hwm_bank(base_address, 0);
	}
	return HWM_STATUS_SUCCESS;
}

int set_fan_type_mode(u16 base_address, u8 fan, fan_type type, fan_mode mode)
{
	u8 shift;

	printk(BIOS_DEBUG, "%s\n", __func__);
	if ((fan < FIRST_FAN) || (fan > LAST_FAN))
		return message_invalid_1(HWM_STATUS_INVALID_FAN, fan);
	select_hwm_bank(base_address, 0);
	if (type < FAN_TYPE_RESERVED) {
		shift = FAN_TYPE_SHIFT(fan);
		hwm_reg_modify(base_address, FAN_TYPE_REG, shift,
						FAN_TYPE_MASK, type);
	}
	if (mode < FAN_MODE_DEFAULT) {
		shift = FAN_MODE_SHIFT(fan);
		hwm_reg_modify(base_address, FAN_MODE_REG, shift,
						FAN_MODE_MASK, mode);
	}
	return HWM_STATUS_SUCCESS;
}

int set_pwm_frequency(u16 base_address, u8 fan, fan_pwm_freq frequency)
{
	u8 shift, index, byte;

	printk(BIOS_DEBUG, "%s\n", __func__);
	if ((fan < FIRST_FAN) || (fan > LAST_FAN))
		return message_invalid_1(HWM_STATUS_INVALID_FAN, fan);
	byte = read_hwm_reg(base_address, FAN_TYPE_REG);
	shift = FAN_TYPE_SHIFT(fan);
	if (((byte >> shift) & FAN_TYPE_PWM_CHECK) == FAN_TYPE_PWM_CHECK) {
		printk(BIOS_WARNING, "Fan %d not programmed as PWM!\n", fan);
		return HWM_STATUS_WARNING_FAN_NOT_PWM;
	}
	select_hwm_bank(base_address, 1);
	shift = FAN_FREQ_SEL_ADD_SHIFT(fan);
	byte = (frequency >> 1) & FAN_BIT_MASK;
	hwm_reg_modify(base_address, FAN_MODE_REG, shift, FAN_BIT_MASK,
									byte);
	select_hwm_bank(base_address, 0);
	index = FAN_ADJUST(fan, FAN_TMP_MAPPING);
	byte = frequency & FAN_BIT_MASK;
	hwm_reg_modify(base_address, index, FAN_PWM_FREQ_SEL_SHIFT,
							FAN_BIT_MASK, byte);
	return HWM_STATUS_SUCCESS;
}

int set_sections(u16 base_address, u8 fan, u8 *boundaries, u8 *sections)
{
	int status, temp;
	u8 i, index, value;

	printk(BIOS_DEBUG, "%s\n", __func__);
	if ((fan < FIRST_FAN) || (fan > LAST_FAN))
		return message_invalid_1(HWM_STATUS_INVALID_FAN, fan);
	status = check_value_seq(boundaries,
				FINTEK_BOUNDARIES_SIZE);
	if (status != HWM_STATUS_SUCCESS) {
		if (status == STATUS_INVALID_VALUE)
			return message_invalid_2(HWM_STATUS_INVALID_BOUNDARY_VALUE, fan);
		return message_invalid_2(HWM_STATUS_BOUNDARY_WRONG_ORDER, fan);
	}
	status = check_value_seq(sections,
				FINTEK_SECTIONS_SIZE);
	if (status != HWM_STATUS_SUCCESS) {
		if (status == STATUS_INVALID_VALUE)
			return message_invalid_2(HWM_STATUS_INVALID_SECTION_VALUE, fan);
		return message_invalid_2(HWM_STATUS_SECTIONS_WRONG_ORDER, fan);
	}
	index = FAN_ADJUST(fan, FAN_BOUND_TEMP);
	for (i = 0; i < FINTEK_BOUNDARIES_SIZE; i++) {
		value = boundaries[i];
		write_hwm_reg(base_address, index, value);
		index++;
	}
	index = FAN_ADJUST(fan, FAN_SECTION_SPEED);
	for (i = 0; i < FINTEK_SECTIONS_SIZE; i++) {
		value = sections[i];
		if (value > 100)
			return message_invalid_2(HWM_STATUS_INVALID_SECTION_VALUE, fan);
		temp = (255 * value) / 100;
		value = (u8)(temp & 0x00ff);
		write_hwm_reg(base_address, index, value);
		index++;
	}
	return HWM_STATUS_SUCCESS;
}

int set_fan_speed_change_rate(u16 base_address, u8 fan, fan_rate_up rate_up,
						fan_rate_down rate_down)
{
	u8 shift, index;

	printk(BIOS_DEBUG, "%s\n", __func__);
	if ((fan < FIRST_FAN) || (fan > LAST_FAN))
		return message_invalid_1(HWM_STATUS_INVALID_FAN, fan);

	index = FAN_ADJUST(fan, FAN_TMP_MAPPING);
	shift = FAN_RATE_SHIFT(fan);

	if (rate_up == FAN_UP_RATE_JUMP) {
		hwm_reg_modify(base_address, index, FAN_JUMP_UP_SHIFT,
							FAN_BIT_MASK, 1);
	} else {
		hwm_reg_modify(base_address, index, FAN_JUMP_UP_SHIFT,
							FAN_BIT_MASK, 0);
		if (rate_up < FAN_UP_RATE_DEFAULT) {
			hwm_reg_modify(base_address,	FAN_UP_RATE_REG,
					shift, FAN_RATE_MASK, rate_up);
		}
	}

	if (rate_down == FAN_DOWN_RATE_JUMP) {
		hwm_reg_modify(base_address, index, FAN_JUMP_DOWN_SHIFT,
							FAN_BIT_MASK, 1);
	} else {
		hwm_reg_modify(base_address, index, FAN_JUMP_UP_SHIFT,
							FAN_BIT_MASK, 0);
		select_hwm_bank(base_address, 0);
		if (rate_down < FAN_DOWN_RATE_DEFAULT) {
			hwm_reg_modify(base_address,	FAN_DOWN_RATE_REG,
					shift, FAN_RATE_MASK, rate_down);
			hwm_reg_modify(base_address, FAN_DOWN_RATE_REG,
					FAN_DOWN_RATE_DIFF_FROM_UP_SHIFT,
					FAN_BIT_MASK, 0);
		}
		if (rate_down == FAN_DOWN_RATE_SAME_AS_UP) {
			hwm_reg_modify(base_address, FAN_DOWN_RATE_REG,
					FAN_DOWN_RATE_DIFF_FROM_UP_SHIFT,
					FAN_BIT_MASK, 1);
		}
	}
	return HWM_STATUS_SUCCESS;
}

int set_fan_follow(u16 base_address, u8 fan, fan_follow follow)
{
	u8 index;

	printk(BIOS_DEBUG, "%s\n", __func__);
	if ((fan < FIRST_FAN) || (fan > LAST_FAN))
		return message_invalid_1(HWM_STATUS_INVALID_FAN, fan);
	index = FAN_ADJUST(fan, FAN_TMP_MAPPING);
	hwm_reg_modify(base_address, index, FAN_INTERPOLATION_SHIFT,
				FAN_BIT_MASK, follow);
	return HWM_STATUS_SUCCESS;
}
