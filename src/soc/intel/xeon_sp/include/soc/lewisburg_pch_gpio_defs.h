/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef LEWISBURG_GPIO_DEFS_H
#define LEWISBURG_GPIO_DEFS_H

#ifndef __ACPI__
#include <stddef.h>
#endif

/* GPIO Community 0 */
#define COMM_0			0
#define  GPP_A			0x0
#define  GPP_B			0x1
#define  GPP_F			0x2
/* GPIO Community 1 */
#define COMM_1			1
#define  GPP_C			0x3
#define  GPP_D			0x4
#define  GPP_E			0x5
/* GPIO Community 3 */
#define COMM_3			2
#define  GPP_I			0x6
/* GPIO Community 4 */
#define COMM_4			3
#define  GPP_J			0x7
#define  GPP_K			0x8
/* GPIO Community 5 */
#define COMM_5			4
#define  GPP_G			0x9
#define  GPP_H			0xA
#define  GPP_L			0xB
/* GPIO Community 2 */
#define COMM_2			5
#define  GPD			0xC

#define GPIO_MAX_NUM_PER_GROUP		24

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

/* Group F */
#define GPP_F0			48
#define GPP_F1			49
#define GPP_F2			50
#define GPP_F3			51
#define GPP_F4			52
#define GPP_F5			53
#define GPP_F6			54
#define GPP_F7			55
#define GPP_F8			56
#define GPP_F9			57
#define GPP_F10			58
#define GPP_F11			59
#define GPP_F12			60
#define GPP_F13			61
#define GPP_F14			62
#define GPP_F15			63
#define GPP_F16			64
#define GPP_F17			65
#define GPP_F18			66
#define GPP_F19			67
#define GPP_F20			68
#define GPP_F21			69
#define GPP_F22			70
#define GPP_F23			71

#define NUM_GPIO_COM0_PADS	(GPP_F23 - GPP_A0 + 1)

/* Community 1 */
/* Group C */
#define GPP_C0			72
#define GPP_C1			73
#define GPP_C2			74
#define GPP_C3			75
#define GPP_C4			76
#define GPP_C5			77
#define GPP_C6			78
#define GPP_C7			79
#define GPP_C8			80
#define GPP_C9			81
#define GPP_C10			82
#define GPP_C11			83
#define GPP_C12			84
#define GPP_C13			85
#define GPP_C14			86
#define GPP_C15			87
#define GPP_C16			88
#define GPP_C17			89
#define GPP_C18			90
#define GPP_C19			91
#define GPP_C20			92
#define GPP_C21			93
#define GPP_C22			94
#define GPP_C23			95

/* Group D */
#define GPP_D0			96
#define GPP_D1			97
#define GPP_D2			98
#define GPP_D3			99
#define GPP_D4			100
#define GPP_D5			101
#define GPP_D6			102
#define GPP_D7			103
#define GPP_D8			104
#define GPP_D9			105
#define GPP_D10			106
#define GPP_D11			107
#define GPP_D12			108
#define GPP_D13			109
#define GPP_D14			110
#define GPP_D15			111
#define GPP_D16			112
#define GPP_D17			113
#define GPP_D18			114
#define GPP_D19			115
#define GPP_D20			116
#define GPP_D21			117
#define GPP_D22			118
#define GPP_D23			119

/* Group E */
#define GPP_E0			120
#define GPP_E1			121
#define GPP_E2			122
#define GPP_E3			123
#define GPP_E4			124
#define GPP_E5			125
#define GPP_E6			126
#define GPP_E7			127
#define GPP_E8			128
#define GPP_E9			129
#define GPP_E10			130
#define GPP_E11			131
#define GPP_E12			132

#define NUM_GPIO_COM1_PADS	(GPP_E12 - GPP_C0 + 1)

/* Community 3 */
/* Group I */
#define GPP_I0			133
#define GPP_I1			134
#define GPP_I2			135
#define GPP_I3			136
#define GPP_I4			137
#define GPP_I5			138
#define GPP_I6			139
#define GPP_I7			140
#define GPP_I8			141
#define GPP_I9			142
#define GPP_I10			143

#define NUM_GPIO_COM3_PADS	(GPP_I10 - GPP_I0 + 1)

/* Community 4 */
/* Group J */
#define GPP_J0			144
#define GPP_J1			145
#define GPP_J2			146
#define GPP_J3			147
#define GPP_J4			148
#define GPP_J5			149
#define GPP_J6			150
#define GPP_J7			151
#define GPP_J8			152
#define GPP_J9			153
#define GPP_J10			154
#define GPP_J11			155
#define GPP_J12			156
#define GPP_J13			157
#define GPP_J14			158
#define GPP_J15			159
#define GPP_J16			160
#define GPP_J17			161
#define GPP_J18			162
#define GPP_J19			163
#define GPP_J20			164
#define GPP_J21			165
#define GPP_J22			166
#define GPP_J23			167

/* Group K */
#define GPP_K0			168
#define GPP_K1			169
#define GPP_K2			170
#define GPP_K3			171
#define GPP_K4			172
#define GPP_K5			173
#define GPP_K6			174
#define GPP_K7			175
#define GPP_K8			176
#define GPP_K9			177
#define GPP_K10			178

#define NUM_GPIO_COM4_PADS	(GPP_K10 - GPP_J0 + 1)

/* Community 5 */
/* Group G */
#define GPP_G0			179
#define GPP_G1			180
#define GPP_G2			181
#define GPP_G3			182
#define GPP_G4			183
#define GPP_G5			184
#define GPP_G6			185
#define GPP_G7			186
#define GPP_G8			187
#define GPP_G9			188
#define GPP_G10			189
#define GPP_G11			190
#define GPP_G12			191
#define GPP_G13			192
#define GPP_G14			193
#define GPP_G15			194
#define GPP_G16			195
#define GPP_G17			196
#define GPP_G18			197
#define GPP_G19			198
#define GPP_G20			199
#define GPP_G21			200
#define GPP_G22			201
#define GPP_G23			202

/* Group H */
#define GPP_H0			203
#define GPP_H1			204
#define GPP_H2			205
#define GPP_H3			206
#define GPP_H4			207
#define GPP_H5			208
#define GPP_H6			209
#define GPP_H7			210
#define GPP_H8			211
#define GPP_H9			212
#define GPP_H10			213
#define GPP_H11			214
#define GPP_H12			215
#define GPP_H13			216
#define GPP_H14			217
#define GPP_H15			218
#define GPP_H16			219
#define GPP_H17			220
#define GPP_H18			221
#define GPP_H19			222
#define GPP_H20			223
#define GPP_H21			224
#define GPP_H22			225
#define GPP_H23			226

/* Group L */
#define GPP_L0			227
#define GPP_L1			228
#define GPP_L2			229
#define GPP_L3			230
#define GPP_L4			231
#define GPP_L5			232
#define GPP_L6			233
#define GPP_L7			234
#define GPP_L8			235
#define GPP_L9			236
#define GPP_L10			237
#define GPP_L11			238
#define GPP_L12			239
#define GPP_L13			240
#define GPP_L14			241
#define GPP_L15			242
#define GPP_L16			243
#define GPP_L17			244
#define GPP_L18			245
#define GPP_L19			246

#define NUM_GPIO_COM5_PADS	(GPP_L19 - GPP_G0 + 1)

/* Community 2 */
/* Group GPD  */
#define GPD0			247
#define GPD1			248
#define GPD2			249
#define GPD3			250
#define GPD4			251
#define GPD5			252
#define GPD6			253
#define GPD7			254
#define GPD8			255
#define GPD9			256
#define GPD10			257
#define GPD11			258

#define NUM_GPIO_COM2_PADS	(GPD11 - GPD0 + 1)

#define TOTAL_PADS		(GPD11 + 1)

#define GPIO_NUM_PAD_CFG_REGS   2 /* DW0, DW1 */

#define NUM_GPIO_COMx_GPI_REGS(n)	\
		(ALIGN_UP((n), GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define NUM_GPIO_COM0_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM0_PADS)
#define NUM_GPIO_COM1_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM1_PADS)
#define NUM_GPIO_COM2_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM2_PADS)
#define NUM_GPIO_COM3_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM3_PADS)
#define NUM_GPIO_COM4_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM4_PADS)
#define NUM_GPIO_COM5_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM5_PADS)

#define NUM_GPI_STATUS_REGS			\
		((NUM_GPIO_COM0_GPI_REGS) +	\
		(NUM_GPIO_COM1_GPI_REGS)  +	\
		(NUM_GPIO_COM3_GPI_REGS)  +	\
		(NUM_GPIO_COM4_GPI_REGS)  +	\
		(NUM_GPIO_COM5_GPI_REGS)  +	\
		(NUM_GPIO_COM2_GPI_REGS))

/*
 * IOxAPIC IRQs for the GPIOs (This was taken from an intelltool dump)
 */

/* Community 0 */
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

/* Group F */
#define GPP_F0_IRQ		0x55
#define GPP_F1_IRQ		0x56
#define GPP_F2_IRQ		0x57
#define GPP_F3_IRQ		0x58
#define GPP_F4_IRQ		0x59
#define GPP_F5_IRQ		0x5a
#define GPP_F6_IRQ		0x5b
#define GPP_F7_IRQ		0x5c
#define GPP_F8_IRQ		0x5d
#define GPP_F9_IRQ		0x5e
#define GPP_F10_IRQ		0x5f
#define GPP_F11_IRQ		0x60
#define GPP_F12_IRQ		0x61
#define GPP_F13_IRQ		0x62
#define GPP_F14_IRQ		0x63
#define GPP_F15_IRQ		0x64
#define GPP_F16_IRQ		0x65
#define GPP_F17_IRQ		0x66
#define GPP_F18_IRQ		0x67
#define GPP_F19_IRQ		0x68
#define GPP_F20_IRQ		0x69
#define GPP_F21_IRQ		0x6a
#define GPP_F22_IRQ		0x6b
#define GPP_F23_IRQ		0x6c

/* Community 1 */
/* Group C */
#define GPP_C0_IRQ		0x18
#define GPP_C1_IRQ		0x19
#define GPP_C2_IRQ		0x1a
#define GPP_C3_IRQ		0x1b
#define GPP_C4_IRQ		0x1c
#define GPP_C5_IRQ		0x1d
#define GPP_C6_IRQ		0x1e
#define GPP_C7_IRQ		0x1f
#define GPP_C8_IRQ		0x20
#define GPP_C9_IRQ		0x21
#define GPP_C10_IRQ		0x22
#define GPP_C11_IRQ		0x23
#define GPP_C12_IRQ		0x24
#define GPP_C13_IRQ		0x25
#define GPP_C14_IRQ		0x26
#define GPP_C15_IRQ		0x27
#define GPP_C16_IRQ		0x28
#define GPP_C17_IRQ		0x29
#define GPP_C18_IRQ		0x2a
#define GPP_C19_IRQ		0x2b
#define GPP_C20_IRQ		0x2c
#define GPP_C21_IRQ		0x2d
#define GPP_C22_IRQ		0x2e
#define GPP_C23_IRQ		0x2f

/* Group D */
#define GPP_D0_IRQ		0x30
#define GPP_D1_IRQ		0x31
#define GPP_D2_IRQ		0x32
#define GPP_D3_IRQ		0x33
#define GPP_D4_IRQ		0x34
#define GPP_D5_IRQ		0x35
#define GPP_D6_IRQ		0x36
#define GPP_D7_IRQ		0x37
#define GPP_D8_IRQ		0x38
#define GPP_D9_IRQ		0x39
#define GPP_D10_IRQ		0x3a
#define GPP_D11_IRQ		0x3b
#define GPP_D12_IRQ		0x3c
#define GPP_D13_IRQ		0x3d
#define GPP_D14_IRQ		0x3e
#define GPP_D15_IRQ		0x3f
#define GPP_D16_IRQ		0x40
#define GPP_D17_IRQ		0x41
#define GPP_D18_IRQ		0x42
#define GPP_D19_IRQ		0x43
#define GPP_D20_IRQ		0x44
#define GPP_D21_IRQ		0x45
#define GPP_D22_IRQ		0x46
#define GPP_D23_IRQ		0x47

/* Group E */
#define GPP_E0_IRQ		0x48
#define GPP_E1_IRQ		0x49
#define GPP_E2_IRQ		0x4a
#define GPP_E3_IRQ		0x4b
#define GPP_E4_IRQ		0x4c
#define GPP_E5_IRQ		0x4d
#define GPP_E6_IRQ		0x4e
#define GPP_E7_IRQ		0x4f
#define GPP_E8_IRQ		0x50
#define GPP_E9_IRQ		0x51
#define GPP_E10_IRQ		0x52
#define GPP_E11_IRQ		0x53
#define GPP_E12_IRQ		0x54

/* Community 3 */
/* Group I */
#define GPP_I0_IRQ		0x18
#define GPP_I1_IRQ		0x19
#define GPP_I2_IRQ		0x1a
#define GPP_I3_IRQ		0x1b
#define GPP_I4_IRQ		0x1c
#define GPP_I5_IRQ		0x1d
#define GPP_I6_IRQ		0x1e
#define GPP_I7_IRQ		0x1f
#define GPP_I8_IRQ		0x20
#define GPP_I9_IRQ		0x21
#define GPP_I10_IRQ		0x22

/* Community 4 */
/* Group J */
#define GPP_J0_IRQ		0x18
#define GPP_J1_IRQ		0x19
#define GPP_J2_IRQ		0x1a
#define GPP_J3_IRQ		0x1b
#define GPP_J4_IRQ		0x1c
#define GPP_J5_IRQ		0x1d
#define GPP_J6_IRQ		0x1e
#define GPP_J7_IRQ		0x1f
#define GPP_J8_IRQ		0x20
#define GPP_J9_IRQ		0x21
#define GPP_J10_IRQ		0x22
#define GPP_J11_IRQ		0x23
#define GPP_J12_IRQ		0x24
#define GPP_J13_IRQ		0x25
#define GPP_J14_IRQ		0x26
#define GPP_J15_IRQ		0x27
#define GPP_J16_IRQ		0x28
#define GPP_J17_IRQ		0x29
#define GPP_J18_IRQ		0x2a
#define GPP_J19_IRQ		0x2b
#define GPP_J20_IRQ		0x2c
#define GPP_J21_IRQ		0x2d
#define GPP_J22_IRQ		0x2e
#define GPP_J23_IRQ		0x2f

/* Group K */
#define GPP_K0_IRQ		0x30
#define GPP_K1_IRQ		0x31
#define GPP_K2_IRQ		0x32
#define GPP_K3_IRQ		0x33
#define GPP_K4_IRQ		0x34
#define GPP_K5_IRQ		0x35
#define GPP_K6_IRQ		0x36
#define GPP_K7_IRQ		0x37
#define GPP_K8_IRQ		0x38
#define GPP_K9_IRQ		0x39
#define GPP_K10_IRQ		0x3a

/* Community 5 */
/* Group G */
#define GPP_G0_IRQ		0x6d
#define GPP_G1_IRQ		0x6e
#define GPP_G2_IRQ		0x6f
#define GPP_G3_IRQ		0x70
#define GPP_G4_IRQ		0x71
#define GPP_G5_IRQ		0x72
#define GPP_G6_IRQ		0x73
#define GPP_G7_IRQ		0x74
#define GPP_G8_IRQ		0x75
#define GPP_G9_IRQ		0x76
#define GPP_G10_IRQ		0x77
#define GPP_G11_IRQ		0x2c
#define GPP_G12_IRQ		0x2d
#define GPP_G13_IRQ		0x2e
#define GPP_G14_IRQ		0x2f
#define GPP_G15_IRQ		0x30
#define GPP_G16_IRQ		0x31
#define GPP_G17_IRQ		0x32
#define GPP_G18_IRQ		0x33
#define GPP_G19_IRQ		0x34
#define GPP_G20_IRQ		0x35
#define GPP_G21_IRQ		0x36
#define GPP_G22_IRQ		0x37
#define GPP_G23_IRQ		0x38

/* Group H */
#define GPP_H0_IRQ		0x39
#define GPP_H1_IRQ		0x3a
#define GPP_H2_IRQ		0x3b
#define GPP_H3_IRQ		0x3c
#define GPP_H4_IRQ		0x3d
#define GPP_H5_IRQ		0x3e
#define GPP_H6_IRQ		0x3f
#define GPP_H7_IRQ		0x40
#define GPP_H8_IRQ		0x41
#define GPP_H9_IRQ		0x42
#define GPP_H10_IRQ		0x43
#define GPP_H11_IRQ		0x44
#define GPP_H12_IRQ		0x45
#define GPP_H13_IRQ		0x46
#define GPP_H14_IRQ		0x47
#define GPP_H15_IRQ		0x48
#define GPP_H16_IRQ		0x49
#define GPP_H17_IRQ		0x4a
#define GPP_H18_IRQ		0x4b
#define GPP_H19_IRQ		0x4c
#define GPP_H20_IRQ		0x4d
#define GPP_H21_IRQ		0x4e
#define GPP_H22_IRQ		0x4f
#define GPP_H23_IRQ		0x50

/* Group L */
#define GPP_L2_IRQ		0x18
#define GPP_L3_IRQ		0x19
#define GPP_L4_IRQ		0x1a
#define GPP_L5_IRQ		0x1b
#define GPP_L6_IRQ		0x1c
#define GPP_L7_IRQ		0x1d
#define GPP_L8_IRQ		0x1e
#define GPP_L9_IRQ		0x1f
#define GPP_L10_IRQ		0x20
#define GPP_L11_IRQ		0x21
#define GPP_L12_IRQ		0x22
#define GPP_L13_IRQ		0x23
#define GPP_L14_IRQ		0x24
#define GPP_L15_IRQ		0x25
#define GPP_L16_IRQ		0x26
#define GPP_L17_IRQ		0x27
#define GPP_L18_IRQ		0x28
#define GPP_L19_IRQ		0x29

/* Community 2 */
/* Group GPD */
#define GPD0_IRQ		0x18
#define GPD1_IRQ		0x19
#define GPD2_IRQ		0x1a
#define GPD3_IRQ		0x1b
#define GPD4_IRQ		0x1c
#define GPD5_IRQ		0x1d
#define GPD6_IRQ		0x1e
#define GPD7_IRQ		0x1f
#define GPD8_IRQ		0x20
#define GPD9_IRQ		0x21
#define GPD10_IRQ		0x22
#define GPD11_IRQ		0x23

/* Register defines */
#define GPIO_MISCCFG		0x10
#define  GPIO_DRIVER_IRQ_ROUTE_MASK	8
#define  GPIO_DRIVER_IRQ_ROUTE_IRQ14	0
#define  GPIO_DRIVER_IRQ_ROUTE_IRQ15	8

#define HOSTSW_OWN_REG_0	0x80
#define PAD_CFG_BASE		0x400
#define GPI_INT_STS_0		0x100
#define GPI_INT_EN_0		0x110
#define GPI_SMI_STS_0		0x140
#define GPI_SMI_EN_0		0x150
#define GPI_NMI_STS_0		0x160
#define GPI_NMI_EN_0		0x170

#endif /* LEWISBURG_GPIO_DEFS_H */
