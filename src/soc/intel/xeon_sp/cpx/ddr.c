/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/ddr.h>

uint32_t get_ddr_voltage(uint8_t DdrVoltage)
{
	/* SPD Byte 11: Module Nominal Voltage, currently DDR4 only supports 1.2V.
	   Either Bit 0 or Bit 1 is set, return 1.2V */
	if (DdrVoltage & 0x3)
		return 1200;
	return 0;
}

uint16_t get_max_memory_speed(uint32_t commonTck)
{
	if (commonTck <= DDR_8400_TCK_MIN)
		return 8400;
	else if (commonTck <= DDR_6400_TCK_MIN)
		return 6400;
	else if (commonTck <= DDR_6000_TCK_MIN)
		return 6000;
	else if (commonTck <= DDR_5600_TCK_MIN)
		return 5600;
	else if (commonTck <= DDR_5200_TCK_MIN)
		return 5200;
	else if (commonTck <= DDR_4800_TCK_MIN)
		return 4800;
	else if (commonTck <= DDR_4400_TCK_MIN)
		return 4400;
	else if (commonTck <= DDR_4266_TCK_MIN)
		return 4266;
	else if (commonTck <= DDR_4200_TCK_MIN)
		return 4200;
	else if (commonTck <= DDR_4000_TCK_MIN)
		return 4000;
	else if (commonTck <= DDR_3800_TCK_MIN)
		return 3800;
	else if (commonTck <= DDR_3733_TCK_MIN)
		return 3733;
	else if (commonTck <= DDR_3600_TCK_MIN)
		return 3600;
	else if (commonTck <= DDR_3466_TCK_MIN)
		return 3466;
	else if (commonTck <= DDR_3400_TCK_MIN)
		return 3400;
	else if (commonTck <= DDR_3200_TCK_MIN)
		return 3200;
	else if (commonTck <= DDR_3000_TCK_MIN)
		return 3000;
	else if (commonTck <= DDR_2933_TCK_MIN)
		return 2933;
	else if (commonTck <= DDR_2800_TCK_MIN)
		return 2800;
	else if (commonTck <= DDR_2666_TCK_MIN)
		return 2666;
	else if (commonTck <= DDR_2600_TCK_MIN)
		return 2600;
	else if (commonTck <= DDR_2400_TCK_MIN)
		return 2400;
	else if (commonTck <= DDR_2200_TCK_MIN)
		return 2200;
	else if (commonTck <= DDR_2133_TCK_MIN)
		return 2133;
	else if (commonTck <= DDR_2000_TCK_MIN)
		return 2000;
	else if (commonTck <= DDR_1866_TCK_MIN)
		return 1866;
	else if (commonTck <= DDR_1800_TCK_MIN)
		return 1800;
	else if (commonTck <= DDR_1600_TCK_MIN)
		return 1600;
	else if (commonTck <= DDR_1400_TCK_MIN)
		return 1400;
	else if (commonTck <= DDR_1333_TCK_MIN)
		return 1333;
	else if (commonTck <= DDR_1200_TCK_MIN)
		return 1200;
	else if (commonTck <= DDR_1066_TCK_MIN)
		return 1066;
	else if (commonTck <= DDR_1000_TCK_MIN)
		return 1000;
	else
		return 800;
}

__weak bool mainboard_dimm_slot_exists(uint8_t socket, uint8_t channel, uint8_t slot)
{
	return false;
}
