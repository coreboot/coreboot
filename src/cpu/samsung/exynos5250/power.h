/*
 * (C) Copyright 2012 Samsung Electronics
 * Register map for Exynos5 PMU
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
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

#ifndef __EXYNOS5_POWER_H__
#define __EXYNOS5_POWER_H__

/*
 * Power control
 */
#define S5PC100_OTHERS			0xE0108200
#define S5PC100_RST_STAT		0xE0108300
#define S5PC100_SLEEP_WAKEUP		(1 << 3)
#define S5PC100_WAKEUP_STAT		0xE0108304
#define S5PC100_INFORM0			0xE0108400

#define S5PC110_RST_STAT		0xE010A000
#define S5PC110_SLEEP_WAKEUP		(1 << 3)
#define S5PC110_WAKEUP_STAT		0xE010C200
#define S5PC110_OTHERS			0xE010E000
#define S5PC110_USB_PHY_CON		0xE010E80C
#define S5PC110_INFORM0			0xE010F000

/* Enable HW thermal trip with PS_HOLD_CONTROL register ENABLE_HW_TRIP bit */
void power_enable_hw_thermal_trip(void);

#define MIPI_PHY1_CONTROL_ENABLE		(1 << 0)
#define MIPI_PHY1_CONTROL_M_RESETN		(1 << 2)

#define POWER_USB_HOST_PHY_CTRL_EN		(1 << 0)
#define POWER_PS_HOLD_CONTROL_DATA_HIGH		(1 << 8)
#define POWER_ENABLE_HW_TRIP			(1UL << 31)

#define DPTX_PHY_ENABLE		(1 << 0)

/* PMU_DEBUG bits [12:8] = 0x1000 selects XXTI clock source */
#define PMU_DEBUG_XXTI                          0x1000
/* Mask bit[12:8] for xxti clock selection */
#define PMU_DEBUG_CLKOUT_SEL_MASK               0x1f00

/* Power Management Unit register map */
struct exynos5_power {
	/* Add registers as and when required */
	uint8_t		reserved1[0x0400];
	uint32_t	sw_reset;		/* 0x0400 */
	uint8_t		reserved2[0x0304];
	uint32_t	usb_host_phy_ctrl;	/* 0x0708 */
	uint8_t		reserved3[0x8];
	uint32_t	mipi_phy1_control;	/* 0x0714 */
	uint8_t		reserved4[0x8];
	uint32_t	dptx_phy_control;	/* 0x0720 */
	uint8_t		reserved5[0xdc];
	uint32_t	inform0;		/* 0x0800 */
	uint32_t	inform1;		/* 0x0804 */
	uint8_t		reserved6[0x1f8];
	uint32_t	pmu_debug;		/* 0x0A00*/
	uint8_t         reserved7[0x2908];
	uint32_t	ps_hold_ctrl;		/* 0x330c */
} __attribute__ ((__packed__));

/**
 * Perform a software reset.
 */
void power_reset(void);

/**
 * Power off the system; it should never return.
 */
void power_shutdown(void);

/* Enable DPTX PHY */
void power_enable_dp_phy(void);

void power_enable_usb_phy(void);
void power_disable_usb_phy(void);

/* Initialize the pmic voltages to power up the system */
int power_init(void);

/* Read the reset status. */
uint32_t power_read_reset_status(void);

/* Read the resume function and call it. */
void power_exit_wakeup(void);

/* pmu debug is used for xclkout, enable xclkout with source as XXTI */
void power_enable_xclkout(void);

#endif
