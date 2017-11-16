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
#define ESPI_CLK_LOOPBK		24
/* Group B */
#define GPP_B0			25
#define GPP_B1			26
#define GPP_B2			27
#define GPP_B3			28
#define GPP_B4			29
#define GPP_B5			30
#define GPP_B6			31
#define GPP_B7			32
#define GPP_B8			33
#define GPP_B9			34
#define GPP_B10			35
#define GPP_B11			36
#define GPP_B12			37
#define GPP_B13			38
#define GPP_B14			39
#define GPP_B15			40
#define GPP_B16			41
#define GPP_B17			42
#define GPP_B18			43
#define GPP_B19			44
#define GPP_B20			45
#define GPP_B21			46
#define GPP_B22			47
#define GPP_B23			48
#define GSPI0_CLK_LOOPBK	49
#define GSPI1_CLK_LOOPBK	50
/* Group G */
#define GPP_G0			51
#define GPP_G1			52
#define GPP_G2			53
#define GPP_G3			54
#define GPP_G4			55
#define GPP_G5			56
#define GPP_G6			57
#define GPP_G7			58

#define NUM_GPIO_COM0_PADS	(GPP_G7 - GPP_A0 + 1)

/* Group D */
#define GPP_D0			68
#define GPP_D1			69
#define GPP_D2			70
#define GPP_D3			71
#define GPP_D4			72
#define GPP_D5			73
#define GPP_D6			74
#define GPP_D7			75
#define GPP_D8			76
#define GPP_D9			77
#define GPP_D10			78
#define GPP_D11			79
#define GPP_D12			80
#define GPP_D13			81
#define GPP_D14			82
#define GPP_D15			83
#define GPP_D16			84
#define GPP_D17			85
#define GPP_D18			86
#define GPP_D19			87
#define GPP_D20			88
#define GPP_D21			89
#define GPP_D22			90
#define GPP_D23			91
#define GSPI2_CLK_LOOPBK	92
/* Group F */
#define GPP_F0			93
#define GPP_F1			94
#define GPP_F2			95
#define GPP_F3			96
#define GPP_F4			97
#define GPP_F5			98
#define GPP_F6			99
#define GPP_F7			100
#define GPP_F8			101
#define GPP_F9			102
#define GPP_F10			103
#define GPP_F11			104
#define GPP_F12			105
#define GPP_F13			106
#define GPP_F14			107
#define GPP_F15			108
#define GPP_F16			109
#define GPP_F17			110
#define GPP_F18			111
#define GPP_F19			112
#define GPP_F20			113
#define GPP_F21			114
#define GPP_F22			115
#define GPP_F23			116
/* Group H */
#define GPP_H0			117
#define GPP_H1			118
#define GPP_H2			119
#define GPP_H3			120
#define GPP_H4			121
#define GPP_H5			122
#define GPP_H6			123
#define GPP_H7			124
#define GPP_H8			125
#define GPP_H9			126
#define GPP_H10			127
#define GPP_H11			128
#define GPP_H12			129
#define GPP_H13			130
#define GPP_H14			131
#define GPP_H15			132
#define GPP_H16			133
#define GPP_H17			134
#define GPP_H18			135
#define GPP_H19			136
#define GPP_H20			137
#define GPP_H21			138
#define GPP_H22			139
#define GPP_H23			140

#define NUM_GPIO_COM1_PADS	(GPP_H23 - GPP_D0 + 1)

/* Group C */
#define GPP_C0			181
#define GPP_C1			182
#define GPP_C2			183
#define GPP_C3			184
#define GPP_C4			185
#define GPP_C5			186
#define GPP_C6			187
#define GPP_C7			188
#define GPP_C8			189
#define GPP_C9			190
#define GPP_C10			191
#define GPP_C11			192
#define GPP_C12			193
#define GPP_C13			194
#define GPP_C14			195
#define GPP_C15			196
#define GPP_C16			197
#define GPP_C17			198
#define GPP_C18			199
#define GPP_C19			200
#define GPP_C20			201
#define GPP_C21			202
#define GPP_C22			203
#define GPP_C23			204
/* Group E */
#define GPP_E0			205
#define GPP_E1			206
#define GPP_E2			207
#define GPP_E3			208
#define GPP_E4			209
#define GPP_E5			210
#define GPP_E6			211
#define GPP_E7			212
#define GPP_E8			213
#define GPP_E9			214
#define GPP_E10			215
#define GPP_E11			216
#define GPP_E12			217
#define GPP_E13			218
#define GPP_E14			219
#define GPP_E15			220
#define GPP_E16			221
#define GPP_E17			222
#define GPP_E18			223
#define GPP_E19			224
#define GPP_E20			225
#define GPP_E21			226
#define GPP_E22			227
#define GPP_E23			228

#define NUM_GPIO_COM3_PADS	(GPP_E23 - GPP_C0 + 1)

/* Group GPD  */
#define GPD0			229
#define GPD1			230
#define GPD2			231
#define GPD3			232
#define GPD4			233
#define GPD5			234
#define GPD6			235
#define GPD7			236
#define GPD8			237
#define GPD9			238
#define GPD10			239
#define GPD11			240

#define NUM_GPIO_COM2_PADS	(GPD11 - GPD0 + 1)

#define TOTAL_PADS		241
#endif
