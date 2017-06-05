/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 * Copyright 2017 Intel Corp.
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

#ifndef _SOC_CANNONLAKE_GPIO_SOC_DEFS_H_
#define _SOC_CANNONLAKE_GPIO_SOC_DEFS_H_

/*
 * There are 9 GPIO groups. GPP_A -> GPP_H and GPD. GPD is the special case
 * where that group is not so generic. So most of the fixed numbers and macros
 * are based on the GPP groups. The GPIO groups are accessed through register
 * blocks called communities.
 */
#define GPP_A			0
#define GPP_B			1
#define GPP_G			2
#define GPP_D			3
#define GPP_F			4
#define GPP_H			5
#define GPP_C			6
#define GPP_E			7
#define GPD			8
#define GPIO_NUM_GROUPS		9
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
/* Group G */
#define GPP_G0			48
#define GPP_G1			49
#define GPP_G2			50
#define GPP_G3			51
#define GPP_G4			52
#define GPP_G5			53
#define GPP_G6			54
#define GPP_G7			55

#define NUM_GPIO_COM0_PADS	(GPP_G7 - GPP_A0 + 1)

/* Group D */
#define GPP_D0			56
#define GPP_D1			57
#define GPP_D2			58
#define GPP_D3			59
#define GPP_D4			60
#define GPP_D5			61
#define GPP_D6			62
#define GPP_D7			63
#define GPP_D8			64
#define GPP_D9			65
#define GPP_D10			66
#define GPP_D11			67
#define GPP_D12			68
#define GPP_D13			69
#define GPP_D14			70
#define GPP_D15			71
#define GPP_D16			72
#define GPP_D17			73
#define GPP_D18			74
#define GPP_D19			75
#define GPP_D20			76
#define GPP_D21			77
#define GPP_D22			78
#define GPP_D23			79
/* Group F */
#define GPP_F0			80
#define GPP_F1			81
#define GPP_F2			82
#define GPP_F3			83
#define GPP_F4			84
#define GPP_F5			85
#define GPP_F6			86
#define GPP_F7			87
#define GPP_F8			88
#define GPP_F9			89
#define GPP_F10			90
#define GPP_F11			91
#define GPP_F12			92
#define GPP_F13			93
#define GPP_F14			94
#define GPP_F15			95
#define GPP_F16			96
#define GPP_F17			97
#define GPP_F18			98
#define GPP_F19			99
#define GPP_F20			100
#define GPP_F21			101
#define GPP_F22			102
#define GPP_F23			103
/* Group H */
#define GPP_H0			104
#define GPP_H1			105
#define GPP_H2			106
#define GPP_H3			107
#define GPP_H4			108
#define GPP_H5			109
#define GPP_H6			110
#define GPP_H7			111
#define GPP_H8			112
#define GPP_H9			113
#define GPP_H10			114
#define GPP_H11			115
#define GPP_H12			116
#define GPP_H13			117
#define GPP_H14			118
#define GPP_H15			119
#define GPP_H16			120
#define GPP_H17			121
#define GPP_H18			122
#define GPP_H19			123
#define GPP_H20			124
#define GPP_H21			125
#define GPP_H22			126
#define GPP_H23			127

#define NUM_GPIO_COM1_PADS	(GPP_H23 - GPP_D0 + 1)

/* Group C */
#define GPP_C0			128
#define GPP_C1			129
#define GPP_C2			130
#define GPP_C3			131
#define GPP_C4			132
#define GPP_C5			133
#define GPP_C6			134
#define GPP_C7			135
#define GPP_C8			136
#define GPP_C9			137
#define GPP_C10			138
#define GPP_C11			139
#define GPP_C12			140
#define GPP_C13			141
#define GPP_C14			142
#define GPP_C15			143
#define GPP_C16			144
#define GPP_C17			145
#define GPP_C18			146
#define GPP_C19			147
#define GPP_C20			148
#define GPP_C21			149
#define GPP_C22			150
#define GPP_C23			151
/* Group E */
#define GPP_E0			152
#define GPP_E1			153
#define GPP_E2			154
#define GPP_E3			155
#define GPP_E4			156
#define GPP_E5			157
#define GPP_E6			158
#define GPP_E7			159
#define GPP_E8			160
#define GPP_E9			161
#define GPP_E10			162
#define GPP_E11			163
#define GPP_E12			164
#define GPP_E13			165
#define GPP_E14			166
#define GPP_E15			167
#define GPP_E16			168
#define GPP_E17			169
#define GPP_E18			170
#define GPP_E19			171
#define GPP_E20			172
#define GPP_E21			173
#define GPP_E22			174
#define GPP_E23			175

#define NUM_GPIO_COM3_PADS	(GPP_E23 - GPP_C0 + 1)

/* Group GPD  */
#define GPD0			176
#define GPD1			177
#define GPD2			178
#define GPD3			179
#define GPD4			180
#define GPD5			181
#define GPD6			182
#define GPD7			183
#define GPD8			184
#define GPD9			185
#define GPD10			186
#define GPD11			187

#define NUM_GPIO_COM2_PADS	(GPD11 - GPD0 + 1)

#endif

