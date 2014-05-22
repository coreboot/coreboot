/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
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

#include <cbfs.h>
#include <console/console.h>
#include <string.h>
#include <ec/google/chromeec/ec.h>
#include <broadwell/cpu.h>
//#include <broadwell/gpio.h>
#include <broadwell/pei_data.h>
#include <broadwell/pei_wrapper.h>
#include <broadwell/romstage.h>
#include <mainboard/google/samus/spd/spd.h>
#include <mainboard/google/samus/gpio.h>

void mainboard_romstage_entry(struct romstage_params *rp)
{
	struct pei_data pei_data;

	post_code(0x32);

	printk(BIOS_INFO, "MLB: board version %d\n",
	       google_chromeec_get_board_version());

	/* Ensure the EC is in the right mode for recovery */
	google_chromeec_early_init();

	/* Initialize GPIOs */
	init_gpios(mainboard_gpio_config);

	/* Fill out PEI DATA */
	memset(&pei_data, 0, sizeof(pei_data));
	mainboard_fill_pei_data(&pei_data);
	mainboard_fill_spd_data(&pei_data);
	rp->pei_data = &pei_data;

	/*
	 * http://crosbug.com/p/29117
	 * Limit Broadwell SKU to 1333MHz and disable channel 1
	 */
	if (cpu_family_model() == BROADWELL_FAMILY_ULT) {
		pei_data.max_ddr3_freq = 1333;
		pei_data.dimm_channel1_disabled = 3;
		memset(pei_data.spd_data[1][0], 0, SPD_LEN);
	}

	romstage_common(rp);

	/*
	 * Enable PP3300_AUTOBAHN_EN after initial GPIO setup
	 * to prevent possible brownout.
	 */
	set_gpio(SAMUS_GPIO_PP3300_AUTOBAHN_EN, GPIO_OUT_HIGH);
}
