/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <variants.h>

uint8_t get_memory_config_straps(void)
{
	/*
	 * The hardware supports a number of different memory configurations
	 * which are selected using four ID bits ID3 (GPP_B05), ID2 (GPP_B06),
	 * ID1 (GPP_B07) and ID0 (GPP_B08).
	 *
	 * +------+-----+-----+-----+-----+
	 * |      | ID3 | ID2 | ID1 | ID0 |
	 * +------+-----+-----+-----+-----+
	 * | 32GB |  1  |  0  |  0  |  0  |
	 * +------+-----+-----+-----+-----+
	 * | 64GB |  1  |  1  |  0  |  1  |
	 * +------+-----+-----+-----+-----+
	 *
	 * We return the value of these bits so that the index into the SPD
	 * table can be .spd[] values can be configured correctly in the
	 * memory configuration structure.
	 */

	gpio_t spd_id[] = {
		GPP_B05,
		GPP_B06,
		GPP_B07,
		GPP_B08,
	};

	return (uint8_t)gpio_base2_value(spd_id, ARRAY_SIZE(spd_id));
}
