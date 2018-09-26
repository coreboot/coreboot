/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Intel Corp.
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

#ifndef _SOC_CANNONLAKE_GPIO_SOC_DEFS_CNP_H_H_
#define _SOC_CANNONLAKE_GPIO_SOC_DEFS_CNP_H_H_

/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 */

#define GPP_A			0
#define GPP_B			1
#define GPP_C			2
#define GPP_D			3
#define GPP_G			4
#define GPP_K			5
#define GPP_H			6
#define GPP_E			7
#define GPP_F			8
#define GPP_I			9
#define GPP_J			0xA
#define GPD			0xC
#define GPIO_NUM_GROUPS		12
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

#define NUM_GPIO_COM0_PADS	(GPP_B23 - GPP_A0 + 1)

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

/* Group G */
#define GPP_G0			96
#define GPP_G1			97
#define GPP_G2			98
#define GPP_G3			99
#define GPP_G4			100
#define GPP_G5			101
#define GPP_G6			102
#define GPP_G7			103

#define NUM_GPIO_COM1_PADS	(GPP_G7 - GPP_C0 + 1)

/* Group K */
#define GPP_K0			104
#define GPP_K1			105
#define GPP_K2			106
#define GPP_K3			107
#define GPP_K4			108
#define GPP_K5			109
#define GPP_K6			110
#define GPP_K7			111
#define GPP_K8			112
#define GPP_K9			113
#define GPP_K10			114
#define GPP_K11			115
#define GPP_K12			116
#define GPP_K13			117
#define GPP_K14			118
#define GPP_K15			119
#define GPP_K16			120
#define GPP_K17			121
#define GPP_K18			122
#define GPP_K19			123
#define GPP_K20			124
#define GPP_K21			125
#define GPP_K22			126
#define GPP_K23			127

/* Group H */
#define GPP_H0			128
#define GPP_H1			129
#define GPP_H2			130
#define GPP_H3			131
#define GPP_H4			132
#define GPP_H5			133
#define GPP_H6			134
#define GPP_H7			135
#define GPP_H8			136
#define GPP_H9			137
#define GPP_H10			138
#define GPP_H11			139
#define GPP_H12			140
#define GPP_H13			141
#define GPP_H14			142
#define GPP_H15			143
#define GPP_H16			144
#define GPP_H17			145
#define GPP_H18			146
#define GPP_H19			147
#define GPP_H20			148
#define GPP_H21			149
#define GPP_H22			150
#define GPP_H23			151

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

/* Group F */
#define GPP_F0			165
#define GPP_F1			166
#define GPP_F2			167
#define GPP_F3			168
#define GPP_F4			169
#define GPP_F5			170
#define GPP_F6			171
#define GPP_F7			172
#define GPP_F8			173
#define GPP_F9			174
#define GPP_F10			175
#define GPP_F11			176
#define GPP_F12			177
#define GPP_F13			178
#define GPP_F14			179
#define GPP_F15			180
#define GPP_F16			181
#define GPP_F17			182
#define GPP_F18			183
#define GPP_F19			184
#define GPP_F20			185
#define GPP_F21			186
#define GPP_F22			187
#define GPP_F23			188

#define NUM_GPIO_COM3_PADS	(GPP_F23 - GPP_K0 + 1)

/* Group I */
#define GPP_I0			189
#define GPP_I1			190
#define GPP_I2			191
#define GPP_I3			192
#define GPP_I4			193
#define GPP_I5			194
#define GPP_I6			195
#define GPP_I7			196
#define GPP_I8			197
#define GPP_I9			198
#define GPP_I10			199
#define GPP_I11			200
#define GPP_I12			201
#define GPP_I13			202
#define GPP_I14			203

/* Group J */
#define GPP_J0			204
#define GPP_J1			205
#define GPP_J2			206
#define GPP_J3			207
#define GPP_J4			208
#define GPP_J5			209
#define GPP_J6			210
#define GPP_J7			211
#define GPP_J8			212
#define GPP_J9			213
#define GPP_J10			214
#define GPP_J11			215

#define NUM_GPIO_COM4_PADS	(GPP_J11 - GPP_I0 + 1)

/* Group GPD  */
#define GPD0			216
#define GPD1			217
#define GPD2			218
#define GPD3			219
#define GPD4			220
#define GPD5			221
#define GPD6			222
#define GPD7			223
#define GPD8			224
#define GPD9			225
#define GPD10			226
#define GPD11			227

#define NUM_GPIO_COM2_PADS	(GPD11 - GPD0 + 1)

#define TOTAL_PADS			(GPD11 + 1)
#endif
