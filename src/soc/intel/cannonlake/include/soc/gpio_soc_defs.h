/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CANNONLAKE_GPIO_SOC_DEFS_H_
#define _SOC_CANNONLAKE_GPIO_SOC_DEFS_H_

/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 */
#define GPP_A			0x0
#define GPP_B			0x1
#define GPP_G			0x2
#define GROUP_SPI		0x3
#define GPP_D			0x5
#define GPP_F			0x6
#define GPP_H			0x7
#define GROUP_VGPIO0		0x8
#define GROUP_VGPIO1		0x9
#define GPD			0xA
#define GROUP_AZA		0xB
#define GROUP_CPU		0xC
#define GPP_C			0x4
#define GPP_E			0xD
#define GROUP_JTAG		0xE
#define GROUP_HVMOS		0xF

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
/* Group SPI */
#define SPI0_IO_2		59
#define SPI0_IO_3		60
#define SPI0_MOSI		61
#define SPI0_MISO		62
#define SPI0_CS2_B		63
#define SPI0_CS0_B		64
#define SPI0_CS1_B		65
#define SPI0_CLK		66
#define SPI0_CLK_LOOPBK		67

#define NUM_GPIO_COM0_PADS	(SPI0_CLK_LOOPBK - GPP_A0 + 1)

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
/* Group VGPIO 0 */
#define CNV_BTEN		141
#define CNV_GNEN		142
#define CNV_WFEN		143
#define CNV_WCEN		144
#define CNV_BT_HOST_WAKE_B	145
#define CNV_BT_IF_SELECT	146
#define vCNV_BT_UART_TXD	147
#define vCNV_BT_UART_RXD	148
#define vCNV_BT_UART_CTS_B	149
#define vCNV_BT_UART_RTS_B	150
#define vCNV_MFUART1_TXD	151
#define vCNV_MFUART1_RXD	152
#define vCNV_MFUART1_CTS_B	153
#define vCNV_MFUART1_RTS_B	154
#define vCNV_GNSS_UART_TXD	155
#define vCNV_GNSS_UART_RXD	156
#define vCNV_GNSS_UART_CTS_B	157
#define vCNV_GNSS_UART_RTS_B	158
#define vUART0_TXD		159
#define vUART0_RXD		160
#define vUART0_CTS_B		161
#define vUART0_RTS_B		162
#define vISH_UART0_TXD		163
#define vISH_UART0_RXD		164
#define vISH_UART0_CTS_B	165
#define vISH_UART0_RTS_B	166
#define vISH_UART1_TXD		167
#define vISH_UART1_RXD		168
#define vISH_UART1_CTS_B	169
#define vISH_UART1_RTS_B	170
#define vCNV_BT_I2S_BCLK	171
#define vCNV_BT_I2S_WS_SYNC	172
/* Group VGPIO 1 */
#define vCNV_BT_I2S_SDO		173
#define vCNV_BT_I2S_SDI		174
#define vSSP2_SCLK		175
#define vSSP2_SFRM		176
#define vSSP2_TXD		177
#define vSSP2_RXD		178
#define vCNV_GNSS_HOST_WAKE_B	179
#define vSD3_CD_B		180

#define NUM_GPIO_COM1_PADS	(vSD3_CD_B - GPP_D0 + 1)

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
/* Group Jtag */
#define PCH_TDO			229
#define PCH_JTAGX		230
#define PROC_PRDY_B		231
#define PROC_PREQ_B		232
#define CPU_TRST_B		233
#define PCH_TDI			234
#define PCH_TMS			235
#define PCH_TCK			236
#define ITP_PMODE		237
/* Group HVMOS */
#define EDP_BKLTEN		238
#define EDP_BKLTCTL		239
#define EDP_VDDEN		240
#define SYS_PWROK		241
#define SYS_RESET_B		242
#define CL_RST_B		243

#define NUM_GPIO_COM4_PADS	(CL_RST_B - GPP_C0 + 1)

/* Group GPD */
#define GPD0			244
#define GPD1			245
#define GPD2			246
#define GPD3			247
#define GPD4			248
#define GPD5			249
#define GPD6			250
#define GPD7			251
#define GPD8			252
#define GPD9			253
#define GPD10			254
#define GPD11			255
#define SLP_LAN_B		256
#define SLP_SUS_B		257
#define WAKE_B			258
#define DRAM_RESET_B		259

#define NUM_GPIO_COM2_PADS	(DRAM_RESET_B - GPD0 + 1)

/* Group AZA */
#define HDA_BCLK		260
#define HDA_RST_B		261
#define HDA_SYNC		262
#define HDA_SDO			263
#define HDA_SDI0		264
#define HDA_SDI1		265
#define I2S1_SFRM		266
#define I2S1_TXD		267
/* Group CPU */
#define HDACPU_SDI		268
#define HDACPU_SDO		269
#define HDACPU_SCLK		270
#define PM_SYNC			271
#define PECI_IO			272
#define CPUPWRGD		273
#define THRMTRIP_B		274
#define PLTRST_CPU_B		275
#define PM_DOWN			276
#define TRIGGER_IN		277
#define TRIGGER_OUT		278

#define NUM_GPIO_COM3_PADS	(TRIGGER_OUT - HDA_BCLK + 1)

#define TOTAL_PADS		279

#define SD_PWR_EN_PIN		GPP_A17

#define COMM_0		0
#define COMM_1		1
#define COMM_2		2
#define COMM_3		3
#define COMM_4		4
#define TOTAL_GPIO_COMM	5

#endif
