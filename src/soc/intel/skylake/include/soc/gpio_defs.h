/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_GPIO_DEFS_H_
#define _SOC_GPIO_DEFS_H_

/*
 * There are 8 GPIO groups. GPP_A -> GPP_G and GPD. GPD is the special case
 * where that group is not so generic. So most of the fixed numbers and macros
 * are based on the GPP groups. The GPIO groups are accessed through register
 * blocks called communities.
 */
#define GPP_A			0
#define GPP_B			1
#define GPP_C			2
#define GPP_D			3
#define GPP_E			4
#define GPP_F			5
#define GPP_G			6
#define GPD			7
#define GPIO_NUM_GROUPS		8
#define GPIO_MAX_NUM_PER_GROUP	24

/*
 * GPIOs are ordered monotonically increasing to match ACPI/OS driver.
 */

/* Group A */
#define GPP_A0			0
#define GPP_A1			1
#define GPP_A2			2
#define GPP_A3			3
#define GPP_A4			4
#define GPP_A5			5
#define GPP_A6			6
#define GPP_A7			7
#define GPP_A8			8
#define GPP_A9			9
#define GPP_A10			10
#define GPP_A11			11
#define GPP_A12			12
#define GPP_A13			13
#define GPP_A14			14
#define GPP_A15			15
#define GPP_A16			16
#define GPP_A17			17
#define GPP_A18			18
#define GPP_A19			19
#define GPP_A20			20
#define GPP_A21			21
#define GPP_A22			22
#define GPP_A23			23
/* Group B */
#define GPP_B0			24
#define GPP_B1			25
#define GPP_B2			26
#define GPP_B3			27
#define GPP_B4			28
#define GPP_B5			29
#define GPP_B6			30
#define GPP_B7			31
#define GPP_B8			32
#define GPP_B9			33
#define GPP_B10			34
#define GPP_B11			35
#define GPP_B12			36
#define GPP_B13			37
#define GPP_B14			38
#define GPP_B15			39
#define GPP_B16			40
#define GPP_B17			41
#define GPP_B18			42
#define GPP_B19			43
#define GPP_B20			44
#define GPP_B21			45
#define GPP_B22			46
#define GPP_B23			47
/* Group C */
#define GPP_C0			48
#define GPP_C1			49
#define GPP_C2			50
#define GPP_C3			51
#define GPP_C4			52
#define GPP_C5			53
#define GPP_C6			54
#define GPP_C7			55
#define GPP_C8			56
#define GPP_C9			57
#define GPP_C10			58
#define GPP_C11			59
#define GPP_C12			60
#define GPP_C13			61
#define GPP_C14			62
#define GPP_C15			63
#define GPP_C16			64
#define GPP_C17			65
#define GPP_C18			66
#define GPP_C19			67
#define GPP_C20			68
#define GPP_C21			69
#define GPP_C22			70
#define GPP_C23			71
/* Group D */
#define GPP_D0			72
#define GPP_D1			73
#define GPP_D2			74
#define GPP_D3			75
#define GPP_D4			76
#define GPP_D5			77
#define GPP_D6			78
#define GPP_D7			79
#define GPP_D8			80
#define GPP_D9			81
#define GPP_D10			82
#define GPP_D11			83
#define GPP_D12			84
#define GPP_D13			85
#define GPP_D14			86
#define GPP_D15			87
#define GPP_D16			88
#define GPP_D17			89
#define GPP_D18			90
#define GPP_D19			91
#define GPP_D20			92
#define GPP_D21			93
#define GPP_D22			94
#define GPP_D23			95
/* Group E */
#define GPP_E0			96
#define GPP_E1			97
#define GPP_E2			98
#define GPP_E3			99
#define GPP_E4			100
#define GPP_E5			101
#define GPP_E6			102
#define GPP_E7			103
#define GPP_E8			104
#define GPP_E9			105
#define GPP_E10			106
#define GPP_E11			107
#define GPP_E12			108
#define GPP_E13			109
#define GPP_E14			110
#define GPP_E15			111
#define GPP_E16			112
#define GPP_E17			113
#define GPP_E18			114
#define GPP_E19			115
#define GPP_E20			116
#define GPP_E21			117
#define GPP_E22			118
#define GPP_E23			119
/* Group F */
#define GPP_F0			120
#define GPP_F1			121
#define GPP_F2			122
#define GPP_F3			123
#define GPP_F4			124
#define GPP_F5			125
#define GPP_F6			126
#define GPP_F7			127
#define GPP_F8			128
#define GPP_F9			129
#define GPP_F10			130
#define GPP_F11			131
#define GPP_F12			132
#define GPP_F13			133
#define GPP_F14			134
#define GPP_F15			135
#define GPP_F16			136
#define GPP_F17			137
#define GPP_F18			138
#define GPP_F19			139
#define GPP_F20			140
#define GPP_F21			141
#define GPP_F22			142
#define GPP_F23			143
/* Group G */
#define GPP_G0			144
#define GPP_G1			145
#define GPP_G2			146
#define GPP_G3			147
#define GPP_G4			148
#define GPP_G5			149
#define GPP_G6			150
#define GPP_G7			151
/* Group GPD  */
#define GPD0			152
#define GPD1			153
#define GPD2			154
#define GPD3			155
#define GPD4			156
#define GPD5			157
#define GPD6			158
#define GPD7			159
#define GPD8			160
#define GPD9			161
#define GPD10			162
#define GPD11			163

/*
 * IOxAPIC IRQs for the GPIOs
 */

/* Group A */
#define GPP_A0_IRQ		0x18
#define GPP_A1_IRQ		0x19
#define GPP_A2_IRQ		0x1a
#define GPP_A3_IRQ		0x1b
#define GPP_A4_IRQ		0x1c
#define GPP_A5_IRQ		0x1d
#define GPP_A6_IRQ		0x1e
#define GPP_A7_IRQ		0x1f
#define GPP_A8_IRQ		0x20
#define GPP_A9_IRQ		0x21
#define GPP_A10_IRQ		0x22
#define GPP_A11_IRQ		0x23
#define GPP_A12_IRQ		0x24
#define GPP_A13_IRQ		0x25
#define GPP_A14_IRQ		0x26
#define GPP_A15_IRQ		0x27
#define GPP_A16_IRQ		0x28
#define GPP_A17_IRQ		0x29
#define GPP_A18_IRQ		0x2a
#define GPP_A19_IRQ		0x2b
#define GPP_A20_IRQ		0x2c
#define GPP_A21_IRQ		0x2d
#define GPP_A22_IRQ		0x2e
#define GPP_A23_IRQ		0x2f
/* Group B */
#define GPP_B0_IRQ		0x30
#define GPP_B1_IRQ		0x31
#define GPP_B2_IRQ		0x32
#define GPP_B3_IRQ		0x33
#define GPP_B4_IRQ		0x34
#define GPP_B5_IRQ		0x35
#define GPP_B6_IRQ		0x36
#define GPP_B7_IRQ		0x37
#define GPP_B8_IRQ		0x38
#define GPP_B9_IRQ		0x39
#define GPP_B10_IRQ		0x3a
#define GPP_B11_IRQ		0x3b
#define GPP_B12_IRQ		0x3c
#define GPP_B13_IRQ		0x3d
#define GPP_B14_IRQ		0x3e
#define GPP_B15_IRQ		0x3f
#define GPP_B16_IRQ		0x40
#define GPP_B17_IRQ		0x41
#define GPP_B18_IRQ		0x42
#define GPP_B19_IRQ		0x43
#define GPP_B20_IRQ		0x44
#define GPP_B21_IRQ		0x45
#define GPP_B22_IRQ		0x46
#define GPP_B23_IRQ		0x47
/* Group C */
#define GPP_C0_IRQ		0x48
#define GPP_C1_IRQ		0x49
#define GPP_C2_IRQ		0x4a
#define GPP_C3_IRQ		0x4b
#define GPP_C4_IRQ		0x4c
#define GPP_C5_IRQ		0x4d
#define GPP_C6_IRQ		0x4e
#define GPP_C7_IRQ		0x4f
#define GPP_C8_IRQ		0x50
#define GPP_C9_IRQ		0x51
#define GPP_C10_IRQ		0x52
#define GPP_C11_IRQ		0x53
#define GPP_C12_IRQ		0x54
#define GPP_C13_IRQ		0x55
#define GPP_C14_IRQ		0x56
#define GPP_C15_IRQ		0x57
#define GPP_C16_IRQ		0x58
#define GPP_C17_IRQ		0x59
#define GPP_C18_IRQ		0x5a
#define GPP_C19_IRQ		0x5b
#define GPP_C20_IRQ		0x5c
#define GPP_C21_IRQ		0x5d
#define GPP_C22_IRQ		0x5e
#define GPP_C23_IRQ		0x5f
/* Group D */
#define GPP_D0_IRQ		0x60
#define GPP_D1_IRQ		0x61
#define GPP_D2_IRQ		0x62
#define GPP_D3_IRQ		0x63
#define GPP_D4_IRQ		0x64
#define GPP_D5_IRQ		0x65
#define GPP_D6_IRQ		0x66
#define GPP_D7_IRQ		0x67
#define GPP_D8_IRQ		0x68
#define GPP_D9_IRQ		0x69
#define GPP_D10_IRQ		0x6a
#define GPP_D11_IRQ		0x6b
#define GPP_D12_IRQ		0x6c
#define GPP_D13_IRQ		0x6d
#define GPP_D14_IRQ		0x6e
#define GPP_D15_IRQ		0x6f
#define GPP_D16_IRQ		0x70
#define GPP_D17_IRQ		0x71
#define GPP_D18_IRQ		0x72
#define GPP_D19_IRQ		0x73
#define GPP_D20_IRQ		0x74
#define GPP_D21_IRQ		0x75
#define GPP_D22_IRQ		0x76
#define GPP_D23_IRQ		0x77
/* Group E */
#define GPP_E0_IRQ		0x18
#define GPP_E1_IRQ		0x19
#define GPP_E2_IRQ		0x1a
#define GPP_E3_IRQ		0x1b
#define GPP_E4_IRQ		0x1c
#define GPP_E5_IRQ		0x1d
#define GPP_E6_IRQ		0x1e
#define GPP_E7_IRQ		0x1f
#define GPP_E8_IRQ		0x20
#define GPP_E9_IRQ		0x21
#define GPP_E10_IRQ		0x22
#define GPP_E11_IRQ		0x23
#define GPP_E12_IRQ		0x24
#define GPP_E13_IRQ		0x25
#define GPP_E14_IRQ		0x26
#define GPP_E15_IRQ		0x27
#define GPP_E16_IRQ		0x28
#define GPP_E17_IRQ		0x29
#define GPP_E18_IRQ		0x2a
#define GPP_E19_IRQ		0x2b
#define GPP_E20_IRQ		0x2c
#define GPP_E21_IRQ		0x2d
#define GPP_E22_IRQ		0x2e
#define GPP_E23_IRQ		0x2f
/* Group F */
#define GPP_F0_IRQ		0x30
#define GPP_F1_IRQ		0x31
#define GPP_F2_IRQ		0x32
#define GPP_F3_IRQ		0x33
#define GPP_F4_IRQ		0x34
#define GPP_F5_IRQ		0x35
#define GPP_F6_IRQ		0x36
#define GPP_F7_IRQ		0x37
#define GPP_F8_IRQ		0x38
#define GPP_F9_IRQ		0x39
#define GPP_F10_IRQ		0x3a
#define GPP_F11_IRQ		0x3b
#define GPP_F12_IRQ		0x3c
#define GPP_F13_IRQ		0x3d
#define GPP_F14_IRQ		0x3e
#define GPP_F15_IRQ		0x3f
#define GPP_F16_IRQ		0x40
#define GPP_F17_IRQ		0x41
#define GPP_F18_IRQ		0x42
#define GPP_F19_IRQ		0x43
#define GPP_F20_IRQ		0x44
#define GPP_F21_IRQ		0x45
#define GPP_F22_IRQ		0x46
#define GPP_F23_IRQ		0x47
/* Group G */
#define GPP_G0_IRQ		0x48
#define GPP_G1_IRQ		0x49
#define GPP_G2_IRQ		0x4a
#define GPP_G3_IRQ		0x4b
#define GPP_G4_IRQ		0x4c
#define GPP_G5_IRQ		0x4d
#define GPP_G6_IRQ		0x4e
#define GPP_G7_IRQ		0x4f
/* Group GPD */
#define GPD0_IRQ		0x50
#define GPD1_IRQ		0x51
#define GPD2_IRQ		0x52
#define GPD3_IRQ		0x53
#define GPD4_IRQ		0x54
#define GPD5_IRQ		0x55
#define GPD6_IRQ		0x56
#define GPD7_IRQ		0x57
#define GPD8_IRQ		0x58
#define GPD9_IRQ		0x59
#define GPD10_IRQ		0x5a
#define GPD11_IRQ		0x5b

/* Register defines. */
#define MISCCFG_OFFSET		0x10
#define  GPIO_DRIVER_IRQ_ROUTE_MASK	8
#define  GPIO_DRIVER_IRQ_ROUTE_IRQ14	0
#define  GPIO_DRIVER_IRQ_ROUTE_IRQ15	8
#define  GPE_DW_SHIFT		8
#define  GPE_DW_MASK		0xfff00
#define PAD_OWN_REG_OFFSET	0x20
#define  PAD_OWN_PADS_PER	8
#define  PAD_OWN_WIDTH_PER	4
#define  PAD_OWN_MASK		0x03
#define  PAD_OWN_HOST		0x00
#define  PAD_OWN_ME		0x01
#define  PAD_OWN_ISH		0x02
#define HOSTSW_OWN_REG_OFFSET	0xd0
#define  HOSTSW_OWN_PADS_PER	24
#define  HOSTSW_OWN_ACPI	0
#define  HOSTSW_OWN_GPIO	1
#define PAD_CFG_DW_OFFSET	0x400
	/* PADRSTCFG - when to reset the pad config */
#define  PADRSTCFG_SHIFT	30
#define  PADRSTCFG_MASK		0x3
#define  PADRSTCFG_DSW_PWROK	0
#define  PADRSTCFG_DEEP		1
#define  PADRSTCFG_PLTRST	2
#define  PADRSTCFG_RSMRST	3
	/* RXPADSTSEL - raw signal or internal state */
#define  RXPADSTSEL_SHIFT	29
#define  RXPADSTSEL_MASK	0x1
#define  RXPADSTSEL_RAW		0
#define  RXPADSTSEL_INTERNAL	1
	/* RXRAW1 - drive 1 instead instead of pad value */
#define  RXRAW1_SHIFT		28
#define  RXRAW1_MASK		0x1
#define  RXRAW1_NO		0
#define  RXRAW1_YES		1
	/* RXEVCFG - Interrupt and wake types */
#define  RXEVCFG_SHIFT		25
#define  RXEVCFG_MASK		0x3
#define  RXEVCFG_LEVEL		0
#define  RXEVCFG_EDGE		1
#define  RXEVCFG_DRIVE0		2
	/* PREGFRXSEL - use filtering on Rx pad */
#define  PREGFRXSEL_SHIFT	24
#define  PREGFRXSEL_MASK	0x1
#define  PREGFRXSEL_NO		0
#define  PREGFRXSEL_YES		1
	/* RXINV - invert signal to SMI, SCI, NMI, or IRQ routing. */
#define  RXINV_SHIFT		23
#define  RXINV_MASK		0x1
#define  RXINV_NO		0
#define  RXINV_YES		1
	/* GPIROUTIOXAPIC - route to io-xapic or not */
#define  GPIROUTIOXAPIC_SHIFT	20
#define  GPIROUTIOXAPIC_MASK	0x1
#define  GPIROUTIOXAPIC_NO	0
#define  GPIROUTIOXAPIC_YES	1
	/* GPIROUTSCI - route to SCI */
#define  GPIROUTSCI_SHIFT	19
#define  GPIROUTSCI_MASK	0x1
#define  GPIROUTSCI_NO		0
#define  GPIROUTSCI_YES		1
	/* GPIROUTSMI - route to SMI */
#define  GPIROUTSMI_SHIFT	18
#define  GPIROUTSMI_MASK	0x1
#define  GPIROUTSMI_NO		0
#define  GPIROUTSMI_YES		1
	/* GPIROUTNMI - route to NMI */
#define  GPIROUTNMI_SHIFT	17
#define  GPIROUTNMI_MASK	0x1
#define  GPIROUTNMI_NO		0
#define  GPIROUTNMI_YES		1
	/* PMODE - mode of pad */
#define  PMODE_SHIFT		10
#define  PMODE_MASK		0x3
#define  PMODE_GPIO		0
#define  PMODE_NF1		1
#define  PMODE_NF2		2
#define  PMODE_NF3		3
	/* GPIORXDIS - Disable Rx */
#define  GPIORXDIS_SHIFT	9
#define  GPIORXDIS_MASK		0x1
#define  GPIORXDIS_NO		0
#define  GPIORXDIS_YES		1
	/* GPIOTXDIS - Disable Tx */
#define  GPIOTXDIS_SHIFT	8
#define  GPIOTXDIS_MASK		0x1
#define  GPIOTXDIS_NO		0
#define  GPIOTXDIS_YES		1
	/* GPIORXSTATE - Internal state after glitch filter */
#define  GPIORXSTATE_SHIFT	1
#define  GPIORXSTATE_MASK	0x1
	/* GPIOTXSTATE - Drive value onto pad */
#define  GPIOTXSTATE_SHIFT	0
#define  GPIOTXSTATE_MASK	0x1
#define PAD_CFG_DW_OFFSET	0x400
	/* TERM - termination control */
#define  PAD_TERM_SHIFT		10
#define  PAD_TERM_MASK		0xf
#define  PAD_TERM_NONE		0
#define  PAD_TERM_5K_PD		2
#define  PAD_TERM_20K_PD	4
#define  PAD_TERM_1K_PU		9
#define  PAD_TERM_2K_PU		11
#define  PAD_TERM_5K_PU		10
#define  PAD_TERM_20K_PU	12
#define  PAD_TERM_667_PU	13
#define  PAD_TERM_NATIVE	15

#define GPI_GPE_STS_OFFSET	0x140
#define GPI_GPE_EN_OFFSET	0x160
#define GPI_SMI_STS_OFFSET	0x180
#define GPI_SMI_EN_OFFSET	0x1a0

#endif /* _SOC_GPIO_DEFS_H_ */
