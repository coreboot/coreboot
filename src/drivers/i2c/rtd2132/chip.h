/* SPDX-License-Identifier: GPL-2.0-only */

struct drivers_i2c_rtd2132_config {
	/* Panel Power Sequencing. All units in ms. */
	u16 t1; /* Delay from panel Vcc enable to LVDS output enable. */
	u16 t2; /* Delay from LVDS output enable to PWM output enable. */
	u16 t3; /* Delay from PWM output enable to backlight output enable. */
	u16 t4; /* Delay from backlight output disable to PWM output disable. */
	u16 t5; /* Delay from PWM output disable to LVDS output disable. */
	u16 t6; /* Delay from LVDS output disable to panel Vcc disable. */
	u16 t7; /* Delay between tweo panel power on/off sequence. */

	/*
	 * LVDS swap.
	 *  0x00 - Normal
	 *  0x01 - Mirror
	 *  0x02 - P/N
	 *  0x03 - Mirror + P/N
	 *  0x04 - R/L
	 *
	 *  Dual Support or in bit 7:
	 *  0x80 - Dual Swap
	 */
	u8 lvds_swap;

	/*
	 * LVDS Spread Spectrum Clock
	 *  0x00 = DISABLED
	 *  0x05 = 0.5%
	 *  0x10 = 1.0%
	 *  0x15 = 1.5%
	 */
	u8 sscg_percent;
};
