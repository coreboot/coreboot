/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <endian.h>
#include <string.h>
#include <soc/gpio.h>
#include <soc/pei_data.h>
#include <soc/romstage.h>
#include <ec/google/chromeec/ec.h>
#include <mainboard/google/auron/ec.h>
#include <mainboard/google/auron/variant.h>

/* Gandof board memory configuration GPIOs */
#define SPD_GPIO_BIT0		13
#define SPD_GPIO_BIT1		9
#define SPD_GPIO_BIT2		47

/* Copy SPD data for on-board memory */
void mainboard_fill_spd_data(struct pei_data *pei_data)
{
	int spd_bits[3] = {
		SPD_GPIO_BIT0,
		SPD_GPIO_BIT1,
		SPD_GPIO_BIT2
	};
	int spd_gpio[3];
	int spd_index;
	size_t spd_file_len;
	char *spd_file;

	spd_gpio[0] = get_gpio(SPD_GPIO_BIT0);
	spd_gpio[1] = get_gpio(SPD_GPIO_BIT1);
	spd_gpio[2] = get_gpio(SPD_GPIO_BIT2);

	spd_index = spd_gpio[2] << 2 | spd_gpio[1] << 1 | spd_gpio[0];

	printk(BIOS_DEBUG, "SPD: index %d (GPIO%d=%d GPIO%d=%d GPIO%d=%d)\n",
	       spd_index,
	       spd_bits[2], spd_gpio[2],
	       spd_bits[1], spd_gpio[1],
	       spd_bits[0], spd_gpio[0]);

	spd_file = cbfs_map("spd.bin", &spd_file_len);
	if (!spd_file)
		die("SPD data not found.");

	if (spd_file_len < ((spd_index + 1) * SPD_LEN)) {
		printk(BIOS_ERR, "SPD index override to 0 - old hardware?\n");
		spd_index = 0;
	}

	if (spd_file_len < SPD_LEN)
		die("Missing SPD data.");

	memcpy(pei_data->spd_data[0][0],
		spd_file + (spd_index * SPD_LEN), SPD_LEN);
	/* Index 0-2 are 4GB config with both CH0 and CH1.
	 * Index 4-6 are 2GB config with CH0 only. */
	if (spd_index > 3)
		pei_data->dimm_channel1_disabled = 3;
	else
		memcpy(pei_data->spd_data[1][0],
			spd_file + (spd_index * SPD_LEN), SPD_LEN);

	/* Make sure a valid SPD was found */
	if (pei_data->spd_data[0][0][0] == 0)
		die("Invalid SPD data.");

	mainboard_print_spd_info(pei_data->spd_data[0][0]);
}
