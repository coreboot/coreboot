/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _SOC_TIGERLAKE_GPIO_SOC_DEFS_H_
#define _SOC_TIGERLAKE_GPIO_SOC_DEFS_H_
/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 */
#define GPP_B			0x0
#define GPP_T			0x1
#define GPP_A			0x2
#define GPP_R			0x3
#define GPD			0x4
#define GPP_S			0x5
#define GPP_H			0x6
#define GPP_D			0x7
#define GPP_U			0x8
#define GPP_F			0xA
#define GPP_C			0xB
#define GPP_E			0xC

#define GPIO_MAX_NUM_PER_GROUP	27

#define COMM_0			0
#define COMM_1			1
#define COMM_2			2
/* GPIO community 3 is not exposed to be used and hence is skipped. */
#define COMM_4			3
#define COMM_5			4
/*
 * GPIOs are ordered monotonically increasing to match ACPI/OS driver.
 */
/* Group B */
#define GPP_B0			0
#define GPP_B1			1
#define GPP_B2			2
#define GPP_B3			3
#define GPP_B4			4
#define GPP_B5			5
#define GPP_B6			6
#define GPP_B7			7
#define GPP_B8			8
#define GPP_B9			9
#define GPP_B10			10
#define GPP_B11			11
#define GPP_B12			12
#define GPP_B13			13
#define GPP_B14			14
#define GPP_B15			15
#define GPP_B16			16
#define GPP_B17			17
#define GPP_B18			18
#define GPP_B19			19
#define GPP_B20			20
#define GPP_B21			21
#define GPP_B22			22
#define GPP_B23			23
#define GPP_B24			24 /* GSPI0_CLK_LOOPBK */
#define GPP_B25			25 /* GSPI1_CLK_LOOPBK */

/* Group T */
#define GPP_T0			26
#define GPP_T1			27
#define GPP_T2			28
#define GPP_T3			29
#define GPP_T4			30
#define GPP_T5			31
#define GPP_T6			32
#define GPP_T7			33
#define GPP_T8			34
#define GPP_T9			35
#define GPP_T10			36
#define GPP_T11			37
#define GPP_T12			38
#define GPP_T13			39
#define GPP_T14			40
#define GPP_T15			41

/* Group A */
#define GPP_A0			42
#define GPP_A1			43
#define GPP_A2			44
#define GPP_A3			45
#define GPP_A4			46
#define GPP_A5			47
#define GPP_A6			48
#define GPP_A7			49
#define GPP_A8			50
#define GPP_A9			51
#define GPP_A10			52
#define GPP_A11			53
#define GPP_A12			54
#define GPP_A13			55
#define GPP_A14			56
#define GPP_A15			57
#define GPP_A16			58
#define GPP_A17			59
#define GPP_A18			60
#define GPP_A19			61
#define GPP_A20			62
#define GPP_A21			63
#define GPP_A22			64
#define GPP_A23			65
#define GPP_A24			66 /* ESPI_CLK_LOOPBK */

#define GPIO_COM0_START		GPP_B0
#define GPIO_COM0_END		GPP_A24
#define NUM_GPIO_COM0_PADS	(GPP_A24 - GPP_B0 + 1)

/* Group S */
#define GPP_S0			67
#define GPP_S1			68
#define GPP_S2			69
#define GPP_S3			70
#define GPP_S4			71
#define GPP_S5			72
#define GPP_S6			73
#define GPP_S7			74

/* Group H */
#define GPP_H0			75
#define GPP_H1			76
#define GPP_H2			77
#define GPP_H3			78
#define GPP_H4			79
#define GPP_H5			80
#define GPP_H6			81
#define GPP_H7			82
#define GPP_H8			83
#define GPP_H9			84
#define GPP_H10			85
#define GPP_H11			86
#define GPP_H12			87
#define GPP_H13			88
#define GPP_H14			89
#define GPP_H15			90
#define GPP_H16			91
#define GPP_H17			92
#define GPP_H18			93
#define GPP_H19			94
#define GPP_H20			95
#define GPP_H21			96
#define GPP_H22			97
#define GPP_H23			98

/* Group D */
#define GPP_D0			99
#define GPP_D1			100
#define GPP_D2			101
#define GPP_D3			102
#define GPP_D4			103
#define GPP_D5			104
#define GPP_D6			105
#define GPP_D7			106
#define GPP_D8			107
#define GPP_D9			108
#define GPP_D10			109
#define GPP_D11			110
#define GPP_D12			111
#define GPP_D13			112
#define GPP_D14			113
#define GPP_D15			114
#define GPP_D16			115
#define GPP_D17			116
#define GPP_D18			117
#define GPP_D19			118
#define GPP_GSPI2_CLK_LOOPBK	119

/* Group U */
#define GPP_U0			120
#define GPP_U1			121
#define GPP_U2			122
#define GPP_U3			123
#define GPP_U4			124
#define GPP_U5			125
#define GPP_U6			126
#define GPP_U7			127
#define GPP_U8			128
#define GPP_U9			129
#define GPP_U10			130
#define GPP_U11			131
#define GPP_U12			132
#define GPP_U13			133
#define GPP_U14			134
#define GPP_U15			135
#define GPP_U16			136
#define GPP_U17			137
#define GPP_U18			138
#define GPP_U19			139
#define GPP_GSPI3_CLK_LOOPBK	140
#define GPP_GSPI4_CLK_LOOPBK	141
#define GPP_GSPI5_CLK_LOOPBK	142
#define GPP_GSPI6_CLK_LOOPBK	143

/* Group VGPIO */
#define CNV_BTEN		144
#define CNV_BT_HOST_WAKEB	145
#define CNV_BT_IF_SELECT	146
#define vCNV_BT_UART_TXD	147
#define vCNV_BT_UART_RXD	148
#define vCNV_BT_UART_CTS_B	149
#define vCNV_BT_UART_RTS_B	150
#define vCNV_MFUART1_TXD	151
#define vCNV_MFUART1_RXD	152
#define vCNV_MFUART1_CTS_B	153
#define vCNV_MFUART1_RTS_B	154
#define vUART0_TXD		155
#define vUART0_RXD		156
#define vUART0_CTS_B		157
#define vUART0_RTS_B		158
#define vISH_UART0_TXD		159
#define vISH_UART0_RXD		160
#define vISH_UART0_CTS_B	161
#define vISH_UART0_RTS_B	162
#define vCNV_BT_I2S_BCLK	163
#define vCNV_BT_I2S_WS_SYNC	164
#define vCNV_BT_I2S_SDO		165
#define vCNV_BT_I2S_SDI		166
#define vI2S2_SCLK		167
#define vI2S2_SFRM		168
#define vI2S2_TXD		169
#define vI2S2_RXD		170

#define GPIO_COM1_START		GPP_S0
#define GPIO_COM1_END		vI2S2_RXD
#define NUM_GPIO_COM1_PADS	(vI2S2_RXD - GPP_S0 + 1)

/* Group GPD  */
#define GPD0			171
#define GPD1			172
#define GPD2			173
#define GPD3			174
#define GPD4			175
#define GPD5			176
#define GPD6			177
#define GPD7			178
#define GPD8			179
#define GPD9			180
#define GPD10			181
#define GPD11			182
#define GPD_INPUT3VSEL		183
#define GPD_SLP_LANB		184
#define GPD_SLP_SUSB		185
#define GPD_WAKEB		186
#define GPD_DRAM_RESETB		187

#define GPIO_COM2_START		GPD0
#define GPIO_COM2_END		GPD_DRAM_RESETB
#define NUM_GPIO_COM2_PADS	(GPD_DRAM_RESETB - GPD0 + 1)

/* Group C */
#define GPP_C0			188
#define GPP_C1			189
#define GPP_C2			190
#define GPP_C3			191
#define GPP_C4			192
#define GPP_C5			193
#define GPP_C6			194
#define GPP_C7			195
#define GPP_C8			196
#define GPP_C9			197
#define GPP_C10			198
#define GPP_C11			199
#define GPP_C12			200
#define GPP_C13			201
#define GPP_C14			202
#define GPP_C15			203
#define GPP_C16			204
#define GPP_C17			205
#define GPP_C18			206
#define GPP_C19			207
#define GPP_C20			208
#define GPP_C21			209
#define GPP_C22			210
#define GPP_C23			211

/* Group F */
#define GPP_F0			212
#define GPP_F1			213
#define GPP_F2			214
#define GPP_F3			215
#define GPP_F4			216
#define GPP_F5			217
#define GPP_F6			218
#define GPP_F7			219
#define GPP_F8			220
#define GPP_F9			221
#define GPP_F10			222
#define GPP_F11			223
#define GPP_F12			224
#define GPP_F13			225
#define GPP_F14			226
#define GPP_F15			227
#define GPP_F16			228
#define GPP_F17			229
#define GPP_F18			230
#define GPP_F19			231
#define GPP_F20			232
#define GPP_F21			233
#define GPP_F22			234
#define GPP_F23			235
#define GPP_F_CLK_LOOPBK        236

/* Group HVCMOS */
#define GPP_L_BKLTEN		237
#define GPP_L_BKLTCTL		238
#define GPP_L_VDDEN		239
#define GPP_SYS_PWROK		240
#define GPP_SYS_RESETB		241
#define GPP_MLK_RSTB		242

/* Group E */
#define GPP_E0			243
#define GPP_E1			244
#define GPP_E2			245
#define GPP_E3			246
#define GPP_E4			247
#define GPP_E5			248
#define GPP_E6			249
#define GPP_E7			250
#define GPP_E8			251
#define GPP_E9			252
#define GPP_E10			253
#define GPP_E11			254
#define GPP_E12			255
#define GPP_E13			256
#define GPP_E14			257
#define GPP_E15			258
#define GPP_E16			259
#define GPP_E17			260
#define GPP_E18			261
#define GPP_E19			262
#define GPP_E20			263
#define GPP_E21			264
#define GPP_E22			265
#define GPP_E23			266
#define GPP_E_CLK_LOOPBK	267

/* Group JTAG */
#define GPP_JTAG_TDO		268
#define GPP_JTAG_X		269
#define GPP_JTAG_PRDYB		270
#define GPP_JTAG_PREQB		271
#define GPP_CPU_TRSTB		272
#define GPP_JTAG_TDI		273
#define GPP_JTAG_TMS		274
#define GPP_JTAG_TCK		275
#define GPP_DBG_PMODE		276

#define GPIO_COM4_START		GPP_C0
#define GPIO_COM4_END		GPP_DBG_PMODE
#define NUM_GPIO_COM4_PADS	(GPP_DBG_PMODE - GPP_C0 + 1)

/* Group R */
#define GPP_R0			277
#define GPP_R1			278
#define GPP_R2			279
#define GPP_R3			280
#define GPP_R4			281
#define GPP_R5			282
#define GPP_R6			283
#define GPP_R7			284

/* Group SPI */
#define GPP_SPI_IO_2		285
#define GPP_SPI_IO_3		286
#define GPP_SPI_MOSI_IO_0	287
#define GPP_SPI_MOSI_IO_1	288
#define GPP_SPI_TPM_CSB		289
#define GPP_SPI_FLASH_0_CSB	290
#define GPP_SPI_FLASH_1_CSB	291
#define GPP_SPI_CLK		292
#define GPP_CLK_LOOPBK		293

#define GPIO_COM5_START		GPP_R0
#define GPIO_COM5_END		GPP_CLK_LOOPBK
#define NUM_GPIO_COM5_PADS	(GPP_CLK_LOOPBK - GPP_R0 + 1)

#define TOTAL_GPIO_COMM		(COMM_5 + 1)
#define TOTAL_PADS		294

#endif
