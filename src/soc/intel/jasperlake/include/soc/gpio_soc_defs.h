/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_JASPERLAKE_GPIO_SOC_DEFS_H_
#define _SOC_JASPERLAKE_GPIO_SOC_DEFS_H_

/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 */

#define GPP_A			0x0
#define GPP_B			0x1
#define GPP_G			0x2
#define GPP_C			0x3
#define GPP_R			0x4
#define GPP_D			0x5
#define GPP_S			0x6
#define GPP_H			0x7
#define GPP_VGPIO		0x8
#define GPP_F			0x9
#define GPP_GPD			0xA
#define GPP_E			0xD

#define GPIO_MAX_NUM_PER_GROUP	24
#define SD_PWR_EN_PIN		GPP_H1

/*
 * GPIOs are ordered monotonically increasing to match ACPI/OS driver.
 */

/* Group F */
#define GPP_F0		0
#define GPP_F1		1
#define GPP_F2		2
#define GPP_F3		3
#define GPP_F4		4
#define GPP_F5		5
#define GPP_F6		6
#define GPP_F7		7
#define GPP_F8		8
#define GPP_F9		9
#define GPP_F10		10
#define GPP_F11		11
#define GPP_F12		12
#define GPP_F13		13
#define GPP_F14		14
#define GPP_F15		15
#define GPP_F16		16
#define GPP_F17		17
#define GPP_F18		18
#define GPP_F19		19

/* Group B */
#define GPIO_SPI0_IO_2		20
#define GPIO_SPI0_IO_3		21
#define GPIO_SPI0_MOSI_IO_0	22
#define GPIO_SPI0_MOSI_IO_1	23
#define GPIO_SPI0_TPM_CSB	24
#define GPIO_SPI0_FLASH_0_CSB	25
#define GPIO_SPI0_FLASH_1_CSB	26
#define GPIO_SPI0_CLK		27
#define GPIO_SPI0_CLK_LOOPBK	28
#define GPP_B0			29
#define GPP_B1			30
#define GPP_B2			31
#define GPP_B3			32
#define GPP_B4			33
#define GPP_B5			34
#define GPP_B6			35
#define GPP_B7			36
#define GPP_B8			37
#define GPP_B9			38
#define GPP_B10			39
#define GPP_B11			40
#define GPP_B12			41
#define GPP_B13			42
#define GPP_B14			43
#define GPP_B15			44
#define GPP_B16			45
#define GPP_B17			46
#define GPP_B18			47
#define GPP_B19			48
#define GPP_B20			49
#define GPP_B21			50
#define GPP_B22			51
#define GPP_B23			52
#define GPIO_GSPI0_CLK_LOOPBK	53
#define GPIO_GSPI1_CLK_LOOPBK	54

/* Group A */
#define GPP_A0			55
#define GPP_A1			56
#define GPP_A2			57
#define GPP_A3			58
#define GPP_A4			59
#define GPP_A5			60
#define GPP_A6			61
#define GPP_A7			62
#define GPP_A8			63
#define GPP_A9			64
#define GPP_A10			65
#define GPP_A11			66
#define GPP_A12			67
#define GPP_A13			68
#define GPP_A14			69
#define GPP_A15			70
#define GPP_A16			71
#define GPP_A17			72
#define GPP_A18			73
#define GPP_A19			74
#define GPIO_ESPI_CLK_LOOPBK	75

/* Group S */
#define GPP_S0		76
#define GPP_S1		77
#define GPP_S2		78
#define GPP_S3		79
#define GPP_S4		80
#define GPP_S5		81
#define GPP_S6		82
#define GPP_S7		83

/* Group R */
#define GPP_R0		84
#define GPP_R1		85
#define GPP_R2		86
#define GPP_R3		87
#define GPP_R4		88
#define GPP_R5		89
#define GPP_R6		90
#define GPP_R7		91

#define GPIO_COM0_START		GPP_F0
#define GPIO_COM0_END		GPP_R7
#define NUM_GPIO_COM0_PADS	(GPIO_COM0_END - GPIO_COM0_START + 1)

/* Group H */
#define GPP_H0		92
#define GPP_H1		93
#define GPP_H2		94
#define GPP_H3		95
#define GPP_H4		96
#define GPP_H5		97
#define GPP_H6		98
#define GPP_H7		99
#define GPP_H8		100
#define GPP_H9		101
#define GPP_H10		102
#define GPP_H11		103
#define GPP_H12		104
#define GPP_H13		105
#define GPP_H14		106
#define GPP_H15		107
#define GPP_H16		108
#define GPP_H17		109
#define GPP_H18		110
#define GPP_H19		111
#define GPP_H20		112
#define GPP_H21		113
#define GPP_H22		114
#define GPP_H23		115

/* Group D */
#define GPP_D0			116
#define GPP_D1			117
#define GPP_D2			118
#define GPP_D3			119
#define GPP_D4			120
#define GPP_D5			121
#define GPP_D6			122
#define GPP_D7			123
#define GPP_D8			124
#define GPP_D9			125
#define GPP_D10			126
#define GPP_D11			127
#define GPP_D12			128
#define GPP_D13			129
#define GPP_D14			130
#define GPP_D15			131
#define GPP_D16			132
#define GPP_D17			133
#define GPP_D18			134
#define GPP_D19			135
#define GPP_D20			136
#define GPP_D21			137
#define GPP_D22			138
#define GPP_D23			139
#define GPIO_GSPI2_CLK_LOOPBK	140
#define GPIO_SPI1_CLK_LOOPBK	141

/* Group VGPIO */
#define VGPIO_0		142
#define VGPIO_3		143
#define VGPIO_4		144
#define VGPIO_5		145
#define VGPIO_6		146
#define VGPIO_7		147
#define VGPIO_8		148
#define VGPIO_9		149
#define VGPIO_10	150
#define VGPIO_11	151
#define VGPIO_12	152
#define VGPIO_13	153
#define VGPIO_18	154
#define VGPIO_19	155
#define VGPIO_20	156
#define VGPIO_21	157
#define VGPIO_22	158
#define VGPIO_23	159
#define VGPIO_24	160
#define VGPIO_25	161
#define VGPIO_30	162
#define VGPIO_31	163
#define VGPIO_32	164
#define VGPIO_33	165
#define VGPIO_34	166
#define VGPIO_35	167
#define VGPIO_36	168
#define VGPIO_37	169
#define VGPIO_39	170

/* Group C */
#define GPP_C0		171
#define GPP_C1		172
#define GPP_C2		173
#define GPP_C3		174
#define GPP_C4		175
#define GPP_C5		176
#define GPP_C6		177
#define GPP_C7		178
#define GPP_C8		179
#define GPP_C9		180
#define GPP_C10		181
#define GPP_C11		182
#define GPP_C12		183
#define GPP_C13		184
#define GPP_C14		185
#define GPP_C15		186
#define GPP_C16		187
#define GPP_C17		188
#define GPP_C18		189
#define GPP_C19		190
#define GPP_C20		191
#define GPP_C21		192
#define GPP_C22		193
#define GPP_C23		194

#define GPIO_COM1_START		GPP_H0
#define GPIO_COM1_END		GPP_C23
#define NUM_GPIO_COM1_PADS	(GPIO_COM1_END - GPIO_COM1_START + 1)

/* Group GPD */
#define GPD0			195
#define GPD1			196
#define GPD2			197
#define GPD3			198
#define GPD4			199
#define GPD5			200
#define GPD6			201
#define GPD7			202
#define GPD8			203
#define GPD9			204
#define GPD10			205
#define GPIO_INPUT3VSEL		206
#define GPIO_SLP_SUSB		207
#define GPIO_WAKEB		208
#define GPIO_DRAM_RESETB	209

#define GPIO_COM2_START		GPD0
#define GPIO_COM2_END		GPIO_DRAM_RESETB
#define NUM_GPIO_COM2_PADS	(GPIO_COM2_END - GPIO_COM2_START + 1)

/* Group E */
#define GPIO_L_BKLTEN	210
#define GPIO_L_BKLTCTL	211
#define GPIO_L_VDDEN	212
#define GPIO_SYS_PWROK	213
#define GPIO_SYS_RESETB	214
#define GPIO_MLK_RSTB	215
#define GPP_E0		216
#define GPP_E1		217
#define GPP_E2		218
#define GPP_E3		219
#define GPP_E4		220
#define GPP_E5		221
#define GPP_E6		222
#define GPP_E7		223
#define GPP_E8		224
#define GPP_E9		225
#define GPP_E10		226
#define GPP_E11		227
#define GPP_E12		228
#define GPP_E13		229
#define GPP_E14		230
#define GPP_E15		231
#define GPP_E16		232
#define GPP_E17		233
#define GPP_E18		234
#define GPP_E19		235
#define GPP_E20		236
#define GPP_E21		237
#define GPP_E22		238
#define GPP_E23		239

#define GPIO_COM4_START		GPIO_L_BKLTEN
#define GPIO_COM4_END		GPP_E23
#define NUM_GPIO_COM4_PADS	(GPIO_COM4_END - GPIO_COM4_START + 1)

/* Group G */
#define GPP_G0		240
#define GPP_G1		241
#define GPP_G2		242
#define GPP_G3		243
#define GPP_G4		244
#define GPP_G5		245
#define GPP_G6		246
#define GPP_G7		247

#define GPIO_COM5_START		GPP_G0
#define GPIO_COM5_END		GPP_G7
#define NUM_GPIO_COM5_PADS	(GPIO_COM5_END - GPIO_COM5_START + 1)

#define TOTAL_PADS	248

#define COMM_0		0
#define COMM_1		1
#define COMM_2		2
#define COMM_4		3
#define COMM_5		4
#define TOTAL_GPIO_COMM	5

#endif
