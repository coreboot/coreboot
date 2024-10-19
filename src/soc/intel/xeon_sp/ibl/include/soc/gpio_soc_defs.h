/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef IBL_GPIO_DEFS_H
#define IBL_GPIO_DEFS_H

#ifndef __ACPI__
#include <stddef.h>
#endif

/* GPIO Community 0 */
#define COMM_0			0
#define  GPPC_A			0x0
#define  GPPC_B			0x1
#define  GPPC_S			0x2
/* GPIO Community 1 */
#define COMM_1			1
#define  GPPC_C			0x3
#define  GPP_D			0x4
/* GPIO Community 3 */
#define COMM_3			2
#define  GPP_E			0x5
/* GPIO Community 4 */
#define COMM_4			3
#define  GPPC_H			0x6
#define  GPPC_J			0x7
/* GPIO Community 5 */
#define COMM_5			4
#define  GPP_I			0x8
#define  GPP_L			0x9
#define  GPP_M			0xA
#define  GPP_N			0xB
/* GPIO Community 2 */
#define COMM_2			5
#define  GPP_O			0xC

#define GPIO_MAX_NUM_PER_GROUP		24

/* Group A */
#define GPPC_A0			0
#define GPPC_A1			1
#define GPPC_A2			2
#define GPPC_A3			3
#define GPPC_A4			4
#define GPPC_A5			5
#define GPPC_A6			6
#define GPPC_A7			7
#define GPPC_A8			8
#define GPPC_A9			9
#define GPPC_A10		10
#define GPPC_A11		11
#define GPPC_A12		12
#define GPPC_A13		13
#define GPPC_A14		14
#define GPPC_A15		15
#define GPPC_A16		16
#define GPPC_A17		17
#define GPPC_A18		18
#define GPPC_A19		19
#define ESPI_CLK_LOOPBK		20

/* Group B */
#define GPPC_B0			21
#define GPPC_B1			22
#define GPPC_B2			23
#define GPPC_B3			24
#define GPPC_B4			25
#define GPPC_B5			26
#define GPPC_B6			27
#define GPPC_B7			28
#define GPPC_B8			29
#define GPPC_B9			30
#define GPPC_B10		31
#define GPPC_B11		32
#define GPPC_B12		33
#define GPPC_B13		34
#define GPPC_B14		35
#define GPPC_B15		36
#define GPPC_B16		37
#define GPPC_B17		38
#define GPPC_B18		39
#define GPPC_B19		40
#define GPPC_B20		41
#define GPPC_B21		42
#define GPPC_B22		43
#define GPPC_B23		44

/*
 * Order 45-52 belongs to fields which are not described in EDS but
 * can be found while using Cscript. Keep the order here to make GPIO
 * table can be set as expected.
 */

/* Group S */
#define GPPC_S0			53
#define GPPC_S1			54
#define GPPC_S2			55
#define GPPC_S3			56
#define GPPC_S4			57
#define GPPC_S5			58
#define GPPC_S6			59
#define GPPC_S7			60
#define GPPC_S8			61
#define GPPC_S9			62
#define GPPC_S10		63
#define GPPC_S11		64
#define SPI_CLK_LOOPBK		65

#define NUM_GPIO_COM0_PADS	(SPI_CLK_LOOPBK - GPPC_A0 + 1)

/* Community 1 */
/* Group C */
#define GPPC_C0			66
#define GPPC_C1			67
#define GPPC_C2			68
#define GPPC_C3			69
#define GPPC_C4			70
#define GPPC_C5			71
#define GPPC_C6			72
#define GPPC_C7			73
#define GPPC_C8			74
#define GPPC_C9			75
#define GPPC_C10		76
#define GPPC_C11		77
#define GPPC_C12		78
#define GPPC_C13		79
#define GPPC_C14		80
#define GPPC_C15		81
#define GPPC_C16		82
#define GPPC_C17		83
#define GPPC_C18		84
#define GPPC_C19		85
#define GPPC_C20		86
#define GPPC_C21		87

/* Group D */
#define GPP_D0			88
#define GPP_D1			89
#define GPP_D2			90
#define GPP_D3			91	// Not described in EDS
#define GPP_D4			92	// Not described in EDS
#define GPP_D5			93	// Not described in EDS
#define GPP_D6			94
#define GPP_D7			95
#define GPP_D8			96
#define GPP_D9			97
#define GPP_D10			98
#define GPP_D11			99
#define GPP_D12			100
#define GPP_D13			101
#define GPP_D14			102
#define GPP_D15			103
#define GPP_D16			104
#define GPP_D17			105
#define GPP_D18			106
#define GPP_D19			107
#define GPP_D20			108
#define GPP_D21			109
#define GPP_D22			110
#define GPP_D23			111

#define NUM_GPIO_COM1_PADS	(GPP_D23 - GPPC_C0 + 1)

/* Community 3 */
/* Group E */
#define GPP_E0			112
#define GPP_E1			113
#define GPP_E2			114
#define GPP_E3			115
#define GPP_E4			116
#define GPP_E5			117
#define GPP_E6			118
#define GPP_E7			119
#define GPP_E8			120
#define GPP_E9			121
#define GPP_E10			122
#define GPP_E11			123
#define GPP_E12			124
#define GPP_E13			125
#define GPP_E14			126
#define GPP_E15			127
#define GPP_E16			128
#define GPP_E17			129
#define GPP_E18			130
#define GPP_E19			131
#define GPP_E20			132	// Not described in EDS
#define GPP_E21			133	// Not described in EDS
#define GPP_E22			134	// Not described in EDS
#define GPP_E23			135	// Not described in EDS

#define NUM_GPIO_COM3_PADS	(GPP_E23 - GPP_E0 + 1)

/* Community 4 */
/* Group H */
#define GPPC_H0			136
#define GPPC_H1			137
#define GPPC_H2			138	// Not described in EDS
#define GPPC_H3			139	// Not described in EDS
#define GPPC_H4			140	// Not described in EDS
#define GPPC_H5			141	// Not described in EDS
#define GPPC_H6			142
#define GPPC_H7			143
#define GPPC_H8			144	// Not described in EDS
#define GPPC_H9			145	// Not described in EDS
#define GPPC_H10		146	// Not described in EDS
#define GPPC_H11		147	// Not described in EDS
#define GPPC_H12		148	// Not described in EDS
#define GPPC_H13		149	// Not described in EDS
#define GPPC_H14		150	// Not described in EDS
#define GPPC_H15		151
#define GPPC_H16		152
#define GPPC_H17		153
#define GPPC_H18		154
#define GPPC_H19		155

/* Group J */
#define GPP_J0			156
#define GPP_J1			157
#define GPP_J2			158
#define GPP_J3			159
#define GPP_J4			160
#define GPP_J5			161
#define GPP_J6			162
#define GPP_J7			163
#define GPP_J8			164
#define GPP_J9			165	// Not described in EDS
#define GPP_J10			166	// Not described in EDS
#define GPP_J11			167	// Not described in EDS
#define GPP_J12			168
#define GPP_J13			169
#define GPP_J14			170
#define GPP_J15			171
#define GPP_J16			172	// Not described in EDS
#define GPP_J17			173	// Not described in EDS

#define NUM_GPIO_COM4_PADS	(GPP_J17 - GPPC_H0 + 1)

/* Community 5 */
/* Group I */
#define GPP_I0			180	// Not described in EDS
#define GPP_I1			181	// Not described in EDS
#define GPP_I2			182	// Not described in EDS
#define GPP_I3			183	// Not described in EDS
#define GPP_I4			184	// Not described in EDS
#define GPP_I5			185	// Not described in EDS
#define GPP_I6			186	// Not described in EDS
#define GPP_I7			187	// Not described in EDS
#define GPP_I8			188	// Not described in EDS
#define GPP_I9			189	// Not described in EDS
#define GPP_I10			190	// Not described in EDS
#define GPP_I11			191	// Not described in EDS
#define GPP_I12			192
#define GPP_I13			193
#define GPP_I14			194
#define GPP_I15			195
#define GPP_I16			196
#define GPP_I17			197
#define GPP_I18			198	// Not described in EDS
#define GPP_I19			199	// Not described in EDS
#define GPP_I20			200	// Not described in EDS
#define GPP_I21			201
#define GPP_I22			202
#define GPP_I23			203

/* Group L */
#define GPP_L0			204
#define GPP_L1			205
#define GPP_L2			206
#define GPP_L3			207
#define GPP_L4			208
#define GPP_L5			209
#define GPP_L6			210
#define GPP_L7			211
#define GPP_L8			212
#define GPP_L9			213	// Not described in EDS
#define GPP_L10			214	// Not described in EDS
#define GPP_L11			215	// Not described in EDS
#define GPP_L12			216	// Not described in EDS
#define GPP_L13			217	// Not described in EDS
#define GPP_L14			218	// Not described in EDS
#define GPP_L15			219	// Not described in EDS
#define GPP_L16			220	// Not described in EDS
#define GPP_L17			221	// Not described in EDS

/* Group M */
#define GPP_M0			222
#define GPP_M1			223
#define GPP_M2			224
#define GPP_M3			225
#define GPP_M4			226
#define GPP_M5			227
#define GPP_M6			228
#define GPP_M7			229
#define GPP_M8			230
#define GPP_M9			231	// Not described in EDS
#define GPP_M10			232	// Not described in EDS
#define GPP_M11			233
#define GPP_M12			234
#define GPP_M13			235	// Not described in EDS
#define GPP_M14			236	// Not described in EDS
#define GPP_M15			237
#define GPP_M16			238
#define GPP_M17			239

/* Group N */
#define GPP_N0			240	// Not described in EDS
#define GPP_N1			241
#define GPP_N2			242	// Not described in EDS
#define GPP_N3			243	// Not described in EDS
#define GPP_N4			244
#define GPP_N5			245	// Not described in EDS
#define GPP_N6			246	// Not described in EDS
#define GPP_N7			247	// Not described in EDS
#define GPP_N8			248	// Not described in EDS
#define GPP_N9			249	// Not described in EDS
#define GPP_N10			250	// Not described in EDS
#define GPP_N11			251	// Not described in EDS
#define GPP_N12			252	// Not described in EDS
#define GPP_N13			253	// Not described in EDS
#define GPP_N14			254	// Not described in EDS
#define GPP_N15			255	// Not described in EDS
#define GPP_N16			256	// Not described in EDS
#define GPP_N17			257	// Not described in EDS

#define NUM_GPIO_COM5_PADS	(GPP_N17 - GPP_I0 + 1)

/* Community 2 */
/* Group O */
#define GPP_O0			258
#define GPP_O1			259
#define GPP_O2			260
#define GPP_O3			261
#define GPP_O4			262
#define GPP_O5			263
#define GPP_O6			264
#define GPP_O7			265
#define GPP_O8			266
#define GPP_O9			267
#define GPP_O10			268
#define GPP_O11			269
#define GPP_O12			270
#define GPP_O13			271
#define GPP_O14			272
#define GPP_O15			273
#define GPP_O16			274

#define NUM_GPIO_COM2_PADS	(GPP_O16 - GPP_O0 + 1)

#define TOTAL_PADS		(GPP_O16 + 1)

#define GPIO_NUM_PAD_CFG_REGS   4 /* DW0, DW1, DW2, DW3 */

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

/* Register defines */
#define GPIO_MISCCFG		0x10

#define HOSTSW_OWN_REG_0	0x130
#define PAD_CFG_BASE		0x700
#define GPI_INT_STS_0		0x200
#define GPI_INT_EN_0		0x210
#define GPI_SMI_STS_0		0x270
#define GPI_SMI_EN_0		0x290

#define PAD_CFG_LOCK_A		0x100
#define PAD_CFG_LOCK_B		0x108
#define PAD_CFG_LOCK_C		0x100
#define PAD_CFG_LOCK_D		0x108

#endif /* IBL_GPIO_DEFS_H */
