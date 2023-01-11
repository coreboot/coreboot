/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <gpio.h>
#include <mainboard/google/cyan/spd/spd_util.h>

/*
 *   0b0000 - 4GiB total - 2 x 2GiB Samsung K4E8E304EE-EGCF
 *   0b0001 - 2GiB total - 1 x 2GiB Samsung K4E8E304EE-EGCF
 *   0b0010 - 4GiB total - 2 x 2GiB Hynix H9CCNNN8GTMLAR-NUD
 *   0b0011 - 2GiB total - 1 x 2GiB Hynix H9CCNNN8GTMLAR-NUD
 *   0b0100 - 4GiB total - 2 x 2GiB Micron MT52L256M32D1PF-107
 *   0b0101 - 2GiB total - 1 x 2GiB Micron MT52L256M32D1PF-107
 *   0b0110 - 2GiB total - 1 x 2GiB Samsung K4E8E324EB-EGCF
 *   0b0111 - 4GiB total - 2 x 2GiB Samsung K4E8E324EB-EGCF
 *   0b1000 - 2GiB total - 1 x 2GiB Hynix H9CCNNN8GTALAR-NUD
 *   0b1001 - 4GiB total - 2 x 4GiB Hynix H9CCNNN8GTALAR-NUD
 */

static const uint32_t dual_channel_config =
	(1 << 0) | (1 << 2) | (1 << 4) | (1 << 7) | (1 << 9);

uint8_t get_ramid(void)
{
	gpio_t spd_gpios[] = {
		GP_SW_64,	/* I2C3_SDA, RAMID0 */
		GP_SE_02,   /* MF_PLT_CLK1, RAMID1 */
		GP_SW_67,	/* I2C3_SCL, RAMID2 */
		GP_SW_80,	/* SATA_GP3, RAMID3 */
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

int get_variant_spd_index(int ram_id, int *dual)
{
	/* Determine if single or dual channel memory system */
	*dual = (dual_channel_config & (1 << ram_id)) ? 1 : 0;

	/* Display the RAM type */
	switch (ram_id) {
	case 0:
		printk(BIOS_DEBUG, "4GiB Samsung K4E8E304EE-EGCF\n");
		break;
	case 1:
		printk(BIOS_DEBUG, "2GiB Samsung K4E8E304EE-EGCF\n");
		break;
	case 2:
		printk(BIOS_DEBUG, "4GiB Hynix H9CCNNN8GTMLAR-NUD\n");
		break;
	case 3:
		printk(BIOS_DEBUG, "2GiB Hynix H9CCNNN8GTMLAR-NUD\n");
		break;
	case 4:
		printk(BIOS_DEBUG, "4GiB Micron MT52L256M32D1PF-107\n");
		break;
	case 5:
		printk(BIOS_DEBUG, "2GiB Micron MT52L256M32D1PF-107\n");
		break;
	case 6:
		printk(BIOS_DEBUG, "2GiB Samsung K4E8E324EB-EGCF\n");
		break;
	case 7:
		printk(BIOS_DEBUG, "4GiB Samsung K4E8E324EB-EGCF\n");
		break;
	case 8:
		printk(BIOS_DEBUG, "2GiB Hynix H9CCNNN8GTALAR-NUD\n");
		break;
	case 9:
		printk(BIOS_DEBUG, "4GiB Hynix H9CCNNN8GTALAR-NUD\n");
		break;
	}

	/* 1:1 mapping between ram_id and spd_index for setzer */
	return ram_id;
}
