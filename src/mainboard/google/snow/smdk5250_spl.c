/*
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <arch/gpio.h>
#include <cpu/samsung/exynos5-common/spl.h>
#include <cpu/samsung/exynos5250/gpio.h>

#define SIGNATURE	0xdeadbeef

/* Parameters of early board initialization in SPL */
static struct spl_machine_param machine_param = {
	.signature	= SIGNATURE,
	.version	= 1,
	.params		= "vmubfasirMw",
	.size		= sizeof(machine_param),

	.mem_iv_size	= 0x1f,
	.mem_type	= DDR_MODE_DDR3,

	/*
	 * Set uboot_size to 0x100000 bytes.
	 *
	 * This is an overly conservative value chosen to accommodate all
	 * possible U-Boot image.  You are advised to set this value to a
	 * smaller realistic size via scripts that modifies the .machine_param
	 * section of output U-Boot image.
	 */
	.uboot_size	= 0x100000,

	.boot_source	= BOOT_MODE_OM,
	.frequency_mhz	= 800,
	.arm_freq_mhz	= 1700,
	.serial_base	= 0x12c30000,
	.i2c_base	= 0x12c60000,
	.board_rev_gpios = GPIO_D00 | (GPIO_D01 << 16),
	.mem_manuf	= MEM_MANUF_SAMSUNG,
	.bad_wake_gpio	= GPIO_Y10,
};

struct spl_machine_param *spl_get_machine_params(void)
{
	if (machine_param.signature != SIGNATURE) {
		/* TODO: Call panic() here */
		while (1)
			;
	}

	return &machine_param;
}

#if 0
int board_get_revision(void)
{
	struct spl_machine_param *params = spl_get_machine_params();
	unsigned gpio[CONFIG_BOARD_REV_GPIO_COUNT];

	gpio[0] = params->board_rev_gpios & 0xffff;
	gpio[1] = params->board_rev_gpios >> 16;
	return gpio_decode_number(gpio, CONFIG_BOARD_REV_GPIO_COUNT);
}
#endif

int board_wakeup_permitted(void)
{
	struct spl_machine_param *param = spl_get_machine_params();
	const int gpio = param->bad_wake_gpio;
	int is_bad_wake;

	/* We're a bad wakeup if the gpio was defined and was high */
	is_bad_wake = ((gpio != -1) && gpio_get_value(gpio));

	return !is_bad_wake;
}
