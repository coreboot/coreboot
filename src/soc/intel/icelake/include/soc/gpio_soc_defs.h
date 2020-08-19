/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ICELAKE_GPIO_SOC_DEFS_H_
#define _SOC_ICELAKE_GPIO_SOC_DEFS_H_

/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 */
#define GPP_G			0x0
#define GPP_B			0x1
#define GPP_A			0x2
#define GPP_R			0x3
#define GPP_S			0x4
#define GPD			0x5
#define GPP_H			0x6
#define GPP_D			0x7
#define GPP_F			0x8
#define GPP_VGPIO		0x9
#define GPP_C			0xA
#define GPP_E			0xB

#define GPIO_NUM_GROUPS		11
#define GPIO_MAX_NUM_PER_GROUP	24

/*
 * GPIOs are ordered monotonically increasing to match ACPI/OS driver.
 */

/* Group G */
#define GPP_G0		0
#define GPP_G1		1
#define GPP_G2		2
#define GPP_G3		3
#define GPP_G4		4
#define GPP_G5		5
#define GPP_G6		6
#define GPP_G7		7

/* Group B */
#define GPP_B0		8
#define GPP_B1		9
#define GPP_B2		10
#define GPP_B3		11
#define GPP_B4		12
#define GPP_B5		13
#define GPP_B6		14
#define GPP_B7		15
#define GPP_B8		16
#define GPP_B9		17
#define GPP_B10		18
#define GPP_B11		19
#define GPP_B12		20
#define GPP_B13		21
#define GPP_B14		22
#define GPP_B15		23
#define GPP_B16		24
#define GPP_B17		25
#define GPP_B18		26
#define GPP_B19		27
#define GPP_B20		28
#define GPP_B21		29
#define GPP_B22		30
#define GPP_B23		31
#define GPIO_RSVD_0	32
#define GPIO_RSVD_1	33

/* Group A */
#define GPP_A0		34
#define GPP_A1		35
#define GPP_A2		36
#define GPP_A3		37
#define GPP_A4		38
#define GPP_A5		39
#define GPP_A6		40
#define GPP_A7		41
#define GPP_A8		42
#define GPP_A9		43
#define GPP_A10		44
#define GPP_A11		45
#define GPP_A12		46
#define GPP_A13		47
#define GPP_A14		48
#define GPP_A15		49
#define GPP_A16		50
#define GPP_A17		51
#define GPP_A18		52
#define GPP_A19		53
#define GPP_A20		54
#define GPP_A21		55
#define GPP_A22		56
#define GPP_A23		57

#define NUM_GPIO_COM0_PADS	(GPP_A23 - GPP_G0 + 1)

/* Group H */
#define GPP_H0		58
#define GPP_H1		59
#define GPP_H2		60
#define GPP_H3		61
#define GPP_H4		62
#define GPP_H5		63
#define GPP_H6		64
#define GPP_H7		65
#define GPP_H8		66
#define GPP_H9		67
#define GPP_H10		68
#define GPP_H11		69
#define GPP_H12		70
#define GPP_H13		71
#define GPP_H14		72
#define GPP_H15		73
#define GPP_H16		74
#define GPP_H17		75
#define GPP_H18		76
#define GPP_H19		77
#define GPP_H20		78
#define GPP_H21		79
#define GPP_H22		80
#define GPP_H23		81

/* Group D */
#define GPP_D0		82
#define GPP_D1		83
#define GPP_D2		84
#define GPP_D3		85
#define GPP_D4		86
#define GPP_D5		87
#define GPP_D6		88
#define GPP_D7		89
#define GPP_D8		90
#define GPP_D9		91
#define GPP_D10		92
#define GPP_D11		93
#define GPP_D12		94
#define GPP_D13		95
#define GPP_D14		96
#define GPP_D15		97
#define GPP_D16		98
#define GPP_D17		99
#define GPP_D18		100
#define GPP_D19		101
#define GPIO_RSVD_2	102

/* Group F */
#define GPP_F0		103
#define GPP_F1		104
#define GPP_F2		105
#define GPP_F3		106
#define GPP_F4		107
#define GPP_F5		108
#define GPP_F6		109
#define GPP_F7		110
#define GPP_F8		111
#define GPP_F9		112
#define GPP_F10		113
#define GPP_F11		114
#define GPP_F12		115
#define GPP_F13		116
#define GPP_F14		117
#define GPP_F15		118
#define GPP_F16		119
#define GPP_F17		120
#define GPP_F18		121
#define GPP_F19		122

#define NUM_GPIO_COM1_PADS	(GPP_F19 - GPP_H0 + 1)

/* Group GPD  */
#define GPD0		123
#define GPD1		124
#define GPD2		125
#define GPD3		126
#define GPD4		127
#define GPD5		128
#define GPD6		129
#define GPD7		130
#define GPD8		131
#define GPD9		132
#define GPD10		133
#define GPD11		134

#define NUM_GPIO_COM2_PADS	(GPD11 - GPD0 + 1)

/* Group C */
#define GPP_C0		135
#define GPP_C1		136
#define GPP_C2		137
#define GPP_C3		138
#define GPP_C4		139
#define GPP_C5		140
#define GPP_C6		141
#define GPP_C7		142
#define GPP_C8		143
#define GPP_C9		144
#define GPP_C10		145
#define GPP_C11		146
#define GPP_C12		147
#define GPP_C13		148
#define GPP_C14		149
#define GPP_C15		150
#define GPP_C16		151
#define GPP_C17		152
#define GPP_C18		153
#define GPP_C19		154
#define GPP_C20		155
#define GPP_C21		156
#define GPP_C22		157
#define GPP_C23		158
#define GPIO_RSVD_3	159
#define GPIO_RSVD_4	160
#define GPIO_RSVD_5	161
#define GPIO_RSVD_6	162
#define GPIO_RSVD_7	163
#define GPIO_RSVD_8	164

/* Group E */
#define GPP_E0		165
#define GPP_E1		166
#define GPP_E2		167
#define GPP_E3		168
#define GPP_E4		169
#define GPP_E5		170
#define GPP_E6		171
#define GPP_E7		172
#define GPP_E8		173
#define GPP_E9		174
#define GPP_E10		175
#define GPP_E11		176
#define GPP_E12		177
#define GPP_E13		178
#define GPP_E14		179
#define GPP_E15		180
#define GPP_E16		181
#define GPP_E17		182
#define GPP_E18		183
#define GPP_E19		184
#define GPP_E20		185
#define GPP_E21		186
#define GPP_E22		187
#define GPP_E23		188

#define NUM_GPIO_COM4_PADS	(GPP_E23 - GPP_C0 + 1)

/* Group R*/
#define GPP_R0		189
#define GPP_R1		190
#define GPP_R2		191
#define GPP_R3		192
#define GPP_R4		193
#define GPP_R5		194
#define GPP_R6		195
#define GPP_R7		196

/* Group S */
#define GPP_S0		197
#define GPP_S1		198
#define GPP_S2		199
#define GPP_S3		200
#define GPP_S4		201
#define GPP_S5		202
#define GPP_S6		203
#define GPP_S7		204

#define NUM_GPIO_COM5_PADS	(GPP_S7 - GPP_R0 + 1)

#define TOTAL_PADS	205

#define COMM_0		0
#define COMM_1		1
#define COMM_2		2
#define COMM_3		3
#define COMM_4		4
#define TOTAL_GPIO_COMM	5

#endif
