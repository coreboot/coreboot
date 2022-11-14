/* SPDX-License-Identifier: GPL-2.0-only */

#include "rx6110sa.h"

struct drivers_i2c_rx6110sa_config {
	unsigned int bus_speed;		/* Bus clock in Hz */
	/* The day (of the week) is indicated by 7 bits, bit 0 to bit 6. */
	unsigned char user_weekday;	/* User day of the week to set */
	unsigned char user_day;		/* User day to set */
	unsigned char user_month;	/* User month to set */
	unsigned char user_year;	/* User year to set */
	unsigned char set_user_date;	/* Use user date from device tree */
	unsigned char cof_selection;	/* Set up "clock out" frequency */
	unsigned char timer_clk;	/* Set up timer clock */
	unsigned char timer_irq_en;	/* Interrupt generation on timer */
	unsigned short timer_preset;	/* Preset value for the timer */
	unsigned char timer_mode;	/* Set the timer mode of operation */
	unsigned char timer_en;		/* Enable timer operation */
	unsigned char irq_output_pin;	/* 0: IRQ2 pin used, 1: IRQ1 pin used */
	unsigned char fout_output_pin;	/* 0: IRQ2, 1: IRQ1, 2: DO/FOUT */
	unsigned char enable_1hz_out;	/* If set enables 1 Hz output on IRQ1 */
	unsigned char pmon_sampling;	/* Select power monitor sampling time */
	/* The following two bits set the power monitor and backup mode. */
	unsigned char bks_on;
	unsigned char bks_off;
	unsigned char iocut_en;		/* Disable backup of I/O circuit. */
};
