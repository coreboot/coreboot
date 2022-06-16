/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_GPIO_SOC_DEFS_H_
#define _SOC_GPIO_SOC_DEFS_H_

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

#define NUM_GPIO_COM1_PADS	(GPP_E23 - GPP_C0 + 1)

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

#define NUM_GPIO_COM3_PADS	(GPP_G7 - GPP_F0 + 1)

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

#define NUM_GPIO_COM2_PADS	(GPD11 - GPD0 + 1)

#define TOTAL_PADS		(GPD11 + 1)

#endif /* _SOC_GPIO_SOC_DEFS_H_ */
