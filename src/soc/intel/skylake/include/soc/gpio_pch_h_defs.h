/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_GPIO_PCH_H_DEFS_H_
#define _SOC_GPIO_PCH_H_DEFS_H_

/*
 * There are 10 GPIO groups. GPP_A -> GPP_I and GPD. GPD is the special case
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
#define GPP_H			7
#define GPP_I			8
#define GPD			9
#define GPIO_MAX_NUM_PER_GROUP	24

#define GPIO_DWx_COUNT		2 /* DW0 and DW1 */
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
/* Group F */
#define GPP_F0			109
#define GPP_F1			110
#define GPP_F2			111
#define GPP_F3			112
#define GPP_F4			113
#define GPP_F5			114
#define GPP_F6			115
#define GPP_F7			116
#define GPP_F8			117
#define GPP_F9			118
#define GPP_F10			119
#define GPP_F11			120
#define GPP_F12			121
#define GPP_F13			122
#define GPP_F14			123
#define GPP_F15			124
#define GPP_F16			125
#define GPP_F17			126
#define GPP_F18			127
#define GPP_F19			128
#define GPP_F20			129
#define GPP_F21			130
#define GPP_F22			131
#define GPP_F23			132
/* Group G */
#define GPP_G0			133
#define GPP_G1			134
#define GPP_G2			135
#define GPP_G3			136
#define GPP_G4			137
#define GPP_G5			138
#define GPP_G6			139
#define GPP_G7			140
#define GPP_G8			141
#define GPP_G9			142
#define GPP_G10			143
#define GPP_G11			144
#define GPP_G12			145
#define GPP_G13			146
#define GPP_G14			147
#define GPP_G15			148
#define GPP_G16			149
#define GPP_G17			150
#define GPP_G18			151
#define GPP_G19			152
#define GPP_G20			153
#define GPP_G21			154
#define GPP_G22			155
#define GPP_G23			156
/* Group H */
#define GPP_H0			157
#define GPP_H1			158
#define GPP_H2			159
#define GPP_H3			160
#define GPP_H4			161
#define GPP_H5			162
#define GPP_H6			163
#define GPP_H7			164
#define GPP_H8			165
#define GPP_H9			166
#define GPP_H10			167
#define GPP_H11			168
#define GPP_H12			169
#define GPP_H13			170
#define GPP_H14			171
#define GPP_H15			172
#define GPP_H16			173
#define GPP_H17			174
#define GPP_H18			175
#define GPP_H19			176
#define GPP_H20			177
#define GPP_H21			178
#define GPP_H22			179
#define GPP_H23			180

#define NUM_GPIO_COM1_PADS	(GPP_H23 - GPP_C0 + 1)

/* Group I */
#define GPP_I0			181
#define GPP_I1			182
#define GPP_I2			183
#define GPP_I3			184
#define GPP_I4			185
#define GPP_I5			186
#define GPP_I6			187
#define GPP_I7			188
#define GPP_I8			189
#define GPP_I9			190
#define GPP_I10			191

#define NUM_GPIO_COM3_PADS	(GPP_I10 - GPP_I0 + 1)

/* Group GPD  */
#define GPD0			192
#define GPD1			193
#define GPD2			194
#define GPD3			195
#define GPD4			196
#define GPD5			197
#define GPD6			198
#define GPD7			199
#define GPD8			200
#define GPD9			201
#define GPD10			202
#define GPD11			203

#define NUM_GPIO_COM2_PADS	(GPD11 - GPD0 + 1)

#define TOTAL_PADS		(GPD11 + 1)

#endif /* _SOC_GPIO_PCH_H_DEFS_H_ */
