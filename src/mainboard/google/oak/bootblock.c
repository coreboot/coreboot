/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#include <arch/io.h>
#include <boardid.h>
#include <bootblock_common.h>
#include <delay.h>
#include <soc/gpio.h>
#include <soc/mt6391.h>
#include <soc/pericfg.h>
#include <soc/pinmux.h>

#include "gpio.h"

static void i2c_set_gpio_pinmux(void)
{
	gpio_set_mode(PAD_SDA1, PAD_SDA1_FUNC_SDA1);
	gpio_set_mode(PAD_SCL1, PAD_SCL1_FUNC_SCL1);
	gpio_set_mode(PAD_SDA4, PAD_SDA4_FUNC_SDA4);
	gpio_set_mode(PAD_SCL4, PAD_SCL4_FUNC_SCL4);
}

void bootblock_mainboard_early_init(void)
{
	/* Clear UART0 power down signal */
	clrbits_le32(&mt8173_pericfg->pdn0_set, PERICFG_UART0_PDN);
}

void bootblock_mainboard_init(void)
{
	/* adjust gpio params when external voltage is 1.8V */
	gpio_init(GPIO_EINT_1P8V);

	/* set i2c related gpio */
	i2c_set_gpio_pinmux();

	setup_chromeos_gpios();

	if (board_id() < 4)
		mt6391_enable_reset_when_ap_resets();
}
