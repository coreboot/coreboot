/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Samsung Electronics
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

#ifndef CPU_SAMSUNG_EXYNOS5250_PWM_H
#define CPU_SAMSUNG_EXYNOS5250_PWM_H

#define PRESCALER_0		(8 - 1)		/* prescaler of timer 0, 1 */
#define PRESCALER_1		(16 - 1)	/* prescaler of timer 2, 3, 4 */

/* Divider MUX */
#define MUX_DIV_1		0		/* 1/1 period */
#define MUX_DIV_2		1		/* 1/2 period */
#define MUX_DIV_4		2		/* 1/4 period */
#define MUX_DIV_8		3		/* 1/8 period */
#define MUX_DIV_16		4		/* 1/16 period */

#define MUX_DIV_SHIFT(x)	(x * 4)

#define TCON_OFFSET(x)		((x + 1) * (!!x) << 2)

#define TCON_START(x)		(1 << TCON_OFFSET(x))
#define TCON_UPDATE(x)		(1 << (TCON_OFFSET(x) + 1))
#define TCON_INVERTER(x)	(1 << (TCON_OFFSET(x) + 2))
#define TCON_AUTO_RELOAD(x)	(1 << (TCON_OFFSET(x) + 3))
#define TCON4_AUTO_RELOAD	(1 << 22)

struct s5p_timer {
	unsigned int	tcfg0;
	unsigned int	tcfg1;
	unsigned int	tcon;
	unsigned int	tcntb0;
	unsigned int	tcmpb0;
	unsigned int	tcnto0;
	unsigned int	tcntb1;
	unsigned int	tcmpb1;
	unsigned int	tcnto1;
	unsigned int	tcntb2;
	unsigned int	tcmpb2;
	unsigned int	tcnto2;
	unsigned int	tcntb3;
	unsigned int	tcmpb3;
	unsigned int	tcnto3;
	unsigned int	tcntb4;
	unsigned int	tcnto4;
	unsigned int	tintcstat;
};

int pwm_config(int pwm_id, int duty_ns, int period_ns);
int pwm_check_enabled(int pwm_id);
void pwm_disable(int pwm_id);
int pwm_enable(int pwm_id);
int pwm_init(int pwm_id, int div, int invert);

#endif
