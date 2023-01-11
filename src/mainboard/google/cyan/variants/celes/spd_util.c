/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <gpio.h>
#include <mainboard/google/cyan/spd/spd_util.h>

/*
 * Usage of RAMID straps
 *
 *  RAMID1 - Single/Dual channel configuration
 *   0 - Dual channel, 1 - Single channel
 *
 *  Combination of RAMID3, RAMID2, RAMID0 - Index of SPD table
 *   Index 0 - Samsung K4E8E304EE-EGCE 1600MHz 23nm
 *   Index 1 - Samsung K4E8E324EB-EGCF 1866MHz 20nm
 */

uint8_t get_ramid(void)
{
	gpio_t spd_gpios[] = {
		GP_SW_80,	/* SATA_GP3,RAMID0 */
		GP_SE_02,	/* MF_PLT_CLK1, RAMID2 */
		GP_SW_64,	/* I2C3_SDA RAMID3 */
		GP_SW_67,	/* I2C3_SCL,RAMID1 */
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

int get_variant_spd_index(int ram_id, int *dual)
{
	int spd_index = ram_id & 0x7;

	/* Determine if single or dual channel memory system */
	/* RAMID3 is deterministic for celes */
	*dual = ((ram_id >> 3) & 0x1) ? 0 : 1;

	/* Display the RAM type */
	printk(BIOS_DEBUG, *dual ? "4GiB " : "2GiB ");
	switch (spd_index) {
	case 0:
		printk(BIOS_DEBUG, "Samsung K4E8E304EE-EGCE\n");
		break;
	case 1:
		printk(BIOS_DEBUG, "Samsung K4E8E324EB-EGCF\n");
		break;
	}

	return spd_index;
}
