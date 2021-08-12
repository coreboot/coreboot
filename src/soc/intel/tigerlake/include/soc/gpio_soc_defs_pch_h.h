/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_TIGERLAKE_GPIO_SOC_DEFS_PCH_H_H_
#define _SOC_TIGERLAKE_GPIO_SOC_DEFS_PCH_H_H_

/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 * These values come from the FSP and match the PMC values for simplicity
 */
#define GPD			0x0
#define GPP_A			0x1
#define GPP_R			0x2
#define GPP_B			0x3
#define GPP_D			0x4
#define GPP_C			0x5
#define GPP_S			0x6
#define GPP_G			0x7
#define GPP_E			0x9
#define GPP_F			0xA
#define GPP_H			0xB
#define GPP_J			0xC
#define GPP_K			0xD
#define GPP_I			0xE

#define GPIO_MAX_NUM_PER_GROUP	26

#define COMM_0			0
#define COMM_1			1
#define COMM_2			2
#define COMM_3			3
#define COMM_4			4
#define COMM_5			5

/*
 * GPIOs are ordered monotonically increasing to match ACPI/OS driver.
 * See tglh_pins in linux/drivers/pinctrl/intel/pinctrl-tigerlake.c
 */
/* Group A */
#define GPP_SPI0_IO_2		0
#define GPP_SPI0_IO_3		1
#define GPP_SPI0_MOSI_IO_0	2
#define GPP_SPI0_MISO_IO_1	3
#define GPP_SPI0_TPM_CSB	4
#define GPP_SPI0_FLASH_0_CSB	5
#define GPP_SPI0_FLASH_1_CSB	6
#define GPP_SPI0_CLK		7
#define GPP_A0			8
#define GPP_A1			9
#define GPP_A2			10
#define GPP_A3			11
#define GPP_A4			12
#define GPP_A5			13
#define GPP_A6			14
#define GPP_A7			15
#define GPP_A8			16
#define GPP_A9			17
#define GPP_A10			18
#define GPP_A11			19
#define GPP_A12			20
#define GPP_A13			21
#define GPP_A14			22
#define GPP_SPI0_CLK_LOOPBK	23
#define GPP_ESPI_CLK_LOOPBK	24

/* Group R */
#define GPP_R0			25
#define GPP_R1			26
#define GPP_R2			27
#define GPP_R3			28
#define GPP_R4			29
#define GPP_R5			30
#define GPP_R6			31
#define GPP_R7			32
#define GPP_R8			33
#define GPP_R9			34
#define GPP_R10			35
#define GPP_R11			36
#define GPP_R12			37
#define GPP_R13			38
#define GPP_R14			39
#define GPP_R15			40
#define GPP_R16			41
#define GPP_R17			42
#define GPP_R18			43
#define GPP_R19			44

/* Group B */
#define GPP_B0			45
#define GPP_B1			46
#define GPP_B2			47
#define GPP_B3			48
#define GPP_B4			49
#define GPP_B5			50
#define GPP_B6			51
#define GPP_B7			52
#define GPP_B8			53
#define GPP_B9			54
#define GPP_B10			55
#define GPP_B11			56
#define GPP_B12			57
#define GPP_B13			58
#define GPP_B14			59
#define GPP_B15			60
#define GPP_B16			61
#define GPP_B17			62
#define GPP_B18			63
#define GPP_B19			64
#define GPP_B20			65
#define GPP_B21			66
#define GPP_B22			67
#define GPP_B23			68
#define GPP_GSPI0_CLK_LOOPBK	69
#define GPP_GSPI1_CLK_LOOPBK	70

/* Group vGPIO_0 */
#define ESPI_USB_OCB_0		71
#define ESPI_USB_OCB_1		72
#define ESPI_USB_OCB_2		73
#define ESPI_USB_OCB_3		74
#define USB_CPU_OCB_0		75
#define USB_CPU_OCB_1		76
#define USB_CPU_OCB_2		77
#define USB_CPU_OCB_3		78


#define GPIO_COM0_START		GPP_SPI0_IO_2
#define GPIO_COM0_END		USB_CPU_OCB_3
#define NUM_GPIO_COM0_PADS	(GPIO_COM0_START - GPIO_COM0_END + 1)

/* Group D */
#define GPP_D0			79
#define GPP_D1			80
#define GPP_D2			81
#define GPP_D3			82
#define GPP_D4			83
#define GPP_D5			84
#define GPP_D6			85
#define GPP_D7			86
#define GPP_D8			87
#define GPP_D9			88
#define GPP_D10			89
#define GPP_D11			90
#define GPP_D12			91
#define GPP_D13			92
#define GPP_D14			93
#define GPP_D15			94
#define GPP_D16			95
#define GPP_D17			96
#define GPP_D18			97
#define GPP_D19			98
#define GPP_D20			99
#define GPP_D21			100
#define GPP_D22			101
#define GPP_D23			102
#define GPP_SPI1_CLK_LOOPBK	103
#define GPP_GSPI3_CLK_LOOPBK	104

/* Group C */
#define GPP_C0			105
#define GPP_C1			106
#define GPP_C2			107
#define GPP_C3			108
#define GPP_C4			109
#define GPP_C5			110
#define GPP_C6			111
#define GPP_C7			112
#define GPP_C8			113
#define GPP_C9			114
#define GPP_C10			115
#define GPP_C11			116
#define GPP_C12			117
#define GPP_C13			118
#define GPP_C14			119
#define GPP_C15			120
#define GPP_C16			121
#define GPP_C17			122
#define GPP_C18			123
#define GPP_C19			124
#define GPP_C20			125
#define GPP_C21			126
#define GPP_C22			127
#define GPP_C23			128

/* Group S */
#define GPP_S0			129
#define GPP_S1			130
#define GPP_S2			131
#define GPP_S3			132
#define GPP_S4			133
#define GPP_S5			134
#define GPP_S6			135
#define GPP_S7			136

/* Group G */
#define GPP_G0			137
#define GPP_G1			138
#define GPP_G2			139
#define GPP_G3			140
#define GPP_G4			141
#define GPP_G5			142
#define GPP_G6			143
#define GPP_G7			144
#define GPP_G8			145
#define GPP_G9			146
#define GPP_G10			147
#define GPP_G11			148
#define GPP_G12			149
#define GPP_G13			150
#define GPP_G14			151
#define GPP_G15			152
#define GPP_GSPI2_CLK_LOOPBK	153

/* Group vGPIO */
#define CNV_BTEN		154
#define CNV_BT_HOST_WAKEB	155
#define CNV_BT_IF_SELECT	156
#define vCNV_BT_UART_TXD	157
#define vCNV_BT_UART_RXD	158
#define vCNV_BT_UART_CTS_B	159
#define vCNV_BT_UART_RTS_B	160
#define vCNV_MFUART1_TXD	161
#define vCNV_MFUART1_RXD	162
#define vCNV_MFUART1_CTS_B	163
#define vCNV_MFUART1_RTS_B	164
#define vUART0_TXD		165
#define vUART0_RXD		166
#define vUART0_CTS_B		167
#define vUART0_RTS_B		168
#define vISH_UART0_TXD		169
#define vISH_UART0_RXD		170
#define vISH_UART0_CTS_B	171
#define vISH_UART0_RTS_B	172
#define vCNV_BT_I2S_BCLK	173
#define vCNV_BT_I2S_WS_SYNC	174
#define vCNV_BT_I2S_SDO		175
#define vCNV_BT_I2S_SDI		176
#define vI2S2_SCLK		177
#define vI2S2_SFRM		178
#define vI2S2_TXD		179
#define vI2S2_RXD		180

#define GPIO_COM1_START		GPP_D0
#define GPIO_COM1_END		vI2S2_RXD
#define NUM_GPIO_COM1_PADS	(GPIO_COM1_END - GPIO_COM1_START + 1)

/* Group E */
#define GPP_E0			181
#define GPP_E1			182
#define GPP_E2			183
#define GPP_E3			184
#define GPP_E4			185
#define GPP_E5			186
#define GPP_E6			187
#define GPP_E7			188
#define GPP_E8			189
#define GPP_E9			190
#define GPP_E10			191
#define GPP_E11			192
#define GPP_E12			193

/* Group F */
#define GPP_F0			194
#define GPP_F1			195
#define GPP_F2			196
#define GPP_F3			197
#define GPP_F4			198
#define GPP_F5			199
#define GPP_F6			200
#define GPP_F7			201
#define GPP_F8			202
#define GPP_F9			203
#define GPP_F10			204
#define GPP_F11			205
#define GPP_F12			206
#define GPP_F13			207
#define GPP_F14			208
#define GPP_F15			209
#define GPP_F16			210
#define GPP_F17			211
#define GPP_F18			212
#define GPP_F19			213
#define GPP_F20			214
#define GPP_F21			215
#define GPP_F22			216
#define GPP_F23			217

#define GPIO_COM3_START		GPP_E0
#define GPIO_COM3_END		GPP_F23
#define NUM_GPIO_COM3_PADS	(GPIO_COM3_END - GPIO_COM3_START + 1)

/* Group H */
#define GPP_H0			218
#define GPP_H1			219
#define GPP_H2			220
#define GPP_H3			221
#define GPP_H4			222
#define GPP_H5			223
#define GPP_H6			224
#define GPP_H7			225
#define GPP_H8			226
#define GPP_H9			227
#define GPP_H10			228
#define GPP_H11			229
#define GPP_H12			230
#define GPP_H13			231
#define GPP_H14			232
#define GPP_H15			233
#define GPP_H16			234
#define GPP_H17			235
#define GPP_H18			236
#define GPP_H19			237
#define GPP_H20			238
#define GPP_H21			239
#define GPP_H22			240
#define GPP_H23			241

/* Group J */
#define GPP_J0			242
#define GPP_J1			243
#define GPP_J2			244
#define GPP_J3			245
#define GPP_J4			246
#define GPP_J5			247
#define GPP_J6			248
#define GPP_J7			249
#define GPP_J8			250
#define GPP_J9			251

/* Group K */
#define GPP_K0			252
#define GPP_K1			253
#define GPP_K2			254
#define GPP_K3			255
#define GPP_K4			256
#define GPP_K5			257
#define GPP_K6			258
#define GPP_K7			259
#define GPP_K8			260
#define GPP_K9			261
#define GPP_K10			262
#define GPP_K11			263
#define GPP_SYS_PWROK		264
#define GPP_SYS_RESETB		265
#define GPP_MLK_RSTB		266

#define GPIO_COM4_START		GPP_H0
#define GPIO_COM4_END		GPP_MLK_RSTB
#define NUM_GPIO_COM4_PADS	(GPIO_COM4_END - GPIO_COM4_START + 1)

/* Group I */
#define GPP_I0			267
#define GPP_I1			268
#define GPP_I2			269
#define GPP_I3			270
#define GPP_I4			271
#define GPP_I5			272
#define GPP_I6			273
#define GPP_I7			274
#define GPP_I8			275
#define GPP_I9			276
#define GPP_I10			277
#define GPP_I11			278
#define GPP_I12			279
#define GPP_I13			280
#define GPP_I14			281

/* Group JTAG */
#define GPP_JTAG_TDO		282
#define GPP_JTAG_X		283
#define GPP_JTAG_PRDYB		284
#define GPP_JTAG_PREQB		285
#define GPP_JTAG_TDI		286
#define GPP_JTAG_TMS		287
#define GPP_JTAG_TCK		288
#define GPP_JTAG_PMODE		289
#define GPP_JTAG_CPU_TRSTB	290

#define GPIO_COM5_START		GPP_I0
#define GPIO_COM5_END		GPP_JTAG_CPU_TRSTB
#define NUM_GPIO_COM5_PADS	(GPIO_COM5_END - GPIO_COM5_START + 1)

/* Group GPD */
#define GPD0			291
#define GPD1			292
#define GPD2			293
#define GPD3			294
#define GPD4			295
#define GPD5			296
#define GPD6			297
#define GPD7			298
#define GPD8			299
#define GPD9			300
#define GPD10			301
#define GPD11			302
#define GPD12			303

#define GPIO_COM2_START		GPD0
#define GPIO_COM2_END		GPD12
#define NUM_GPIO_COM2_PADS	(GPIO_COM2_END - GPIO_COM2_START + 1)

#define TOTAL_GPIO_COMM		(COMM_5 + 1)
#define TOTAL_PADS		304

#endif
