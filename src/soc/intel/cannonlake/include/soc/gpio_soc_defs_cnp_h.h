/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CANNONLAKE_GPIO_SOC_DEFS_CNP_H_H_
#define _SOC_CANNONLAKE_GPIO_SOC_DEFS_CNP_H_H_

/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 */

#define GPP_A			0x0
#define GPP_B			0x1
#define GPP_C			0x2
#define GPP_D			0x3
#define GPP_G			0x4
#define GPD			0x5
#define GPP_E			0x6
#define GPP_F			0x7
#define GPP_H			0x8
#define GPP_K			0x9
#define GPP_I			0xA
#define GPP_J			0xB
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

#define NUM_GPIO_COM0_PADS	(GSPI1_CLK_LOOPBK - GPP_A0 + 1)

/* Group C */
#define GPP_C0			51
#define GPP_C1			52
#define GPP_C2			53
#define GPP_C3			54
#define GPP_C4			55
#define GPP_C5			56
#define GPP_C6			57
#define GPP_C7			58
#define GPP_C8			59
#define GPP_C9			60
#define GPP_C10			61
#define GPP_C11			62
#define GPP_C12			63
#define GPP_C13			64
#define GPP_C14			65
#define GPP_C15			66
#define GPP_C16			67
#define GPP_C17			68
#define GPP_C18			69
#define GPP_C19			70
#define GPP_C20			71
#define GPP_C21			72
#define GPP_C22			73
#define GPP_C23			74

/* Group D */
#define GPP_D0			75
#define GPP_D1			76
#define GPP_D2			77
#define GPP_D3			78
#define GPP_D4			79
#define GPP_D5			80
#define GPP_D6			81
#define GPP_D7			82
#define GPP_D8			83
#define GPP_D9			84
#define GPP_D10			85
#define GPP_D11			86
#define GPP_D12			87
#define GPP_D13			88
#define GPP_D14			89
#define GPP_D15			90
#define GPP_D16			91
#define GPP_D17			92
#define GPP_D18			93
#define GPP_D19			94
#define GPP_D20			95
#define GPP_D21			96
#define GPP_D22			97
#define GPP_D23			98

/* Group G */
#define GPP_G0			99
#define GPP_G1			100
#define GPP_G2			101
#define GPP_G3			102
#define GPP_G4			103
#define GPP_G5			104
#define GPP_G6			105
#define GPP_G7			106

/* AZA */
#define HDA_BCLK		107
#define HDA_RST_B		108
#define HDA_SYNC		109
#define HDA_SDO			110
#define HDA_SDI0		111
#define HDA_SDI1		112
#define I2S1_SFRM		113
#define I2S1_TXD		114

/* VGPIO_0 */
#define CNV_BTEN		115
#define CNV_GNEN		116
#define CNV_WFEN		117
#define CNV_WCEN		118
#define CNV_BT_HOST_WAKE_B	119
#define vCNV_GNSS_HOST_WAKE_B	120
#define vSD3_CD_B		121
#define CNV_BT_IF_SELECT	122
#define vCNV_BT_UART_TXD	123
#define vCNV_BT_UART_RXD	124
#define vCNV_BT_UART_CTS_B	125
#define vCNV_BT_UART_RTS_B	126
#define vCNV_MFUART1_TXD	127
#define vCNV_MFUART1_RXD	128
#define vCNV_MFUART1_CTS_B	129
#define vCNV_MFUART1_RTS_B	130
#define vCNV_GNSS_UART_TXD	131
#define vCNV_GNSS_UART_RXD	132
#define vCNV_GNSS_UART_CTS_B	133
#define vCNV_GNSS_UART_RTS_B	134
#define vUART0_TXD		135
#define vUART0_RXD		136
#define vUART0_CTS_B		137
#define vUART0_RTS_B		138
#define vISH_UART0_TXD		139
#define vISH_UART0_RXD		140
#define vISH_UART0_CTS_B	141
#define vISH_UART0_RTS_B	142
#define vISH_UART1_TXD		143
#define vISH_UART1_RXD		144
#define vISH_UART1_CTS_B	145
#define vISH_UART1_RTS_B	146
/* VGPIO_1 */
#define vCNV_BT_I2S_BCLK	147
#define vCNV_BT_I2S_WS_SYNC	148
#define vCNV_BT_I2S_SDO		149
#define vCNV_BT_I2S_SDI		150
#define vSSP2_SCLK		151
#define vSSP2_SFRM		152
#define vSSP2_TXD		153
#define vSSP2_RXD		154

#define NUM_GPIO_COM1_PADS	(vSSP2_RXD - GPP_C0 + 1)

/* Group K */
#define GPP_K0			155
#define GPP_K1			156
#define GPP_K2			157
#define GPP_K3			158
#define GPP_K4			159
#define GPP_K5			160
#define GPP_K6			161
#define GPP_K7			162
#define GPP_K8			163
#define GPP_K9			164
#define GPP_K10			165
#define GPP_K11			166
#define GPP_K12			167
#define GPP_K13			168
#define GPP_K14			169
#define GPP_K15			170
#define GPP_K16			171
#define GPP_K17			172
#define GPP_K18			173
#define GPP_K19			174
#define GPP_K20			175
#define GPP_K21			176
#define GPP_K22			177
#define GPP_K23			178

/* Group H */
#define GPP_H0			179
#define GPP_H1			180
#define GPP_H2			181
#define GPP_H3			182
#define GPP_H4			183
#define GPP_H5			184
#define GPP_H6			185
#define GPP_H7			186
#define GPP_H8			187
#define GPP_H9			188
#define GPP_H10			189
#define GPP_H11			190
#define GPP_H12			191
#define GPP_H13			192
#define GPP_H14			193
#define GPP_H15			194
#define GPP_H16			195
#define GPP_H17			196
#define GPP_H18			197
#define GPP_H19			198
#define GPP_H20			199
#define GPP_H21			200
#define GPP_H22			201
#define GPP_H23			202

/* Group E */
#define GPP_E0			203
#define GPP_E1			204
#define GPP_E2			205
#define GPP_E3			206
#define GPP_E4			207
#define GPP_E5			208
#define GPP_E6			209
#define GPP_E7			210
#define GPP_E8			211
#define GPP_E9			212
#define GPP_E10			213
#define GPP_E11			214
#define GPP_E12			215

/* Group F */
#define GPP_F0			216
#define GPP_F1			217
#define GPP_F2			218
#define GPP_F3			219
#define GPP_F4			220
#define GPP_F5			221
#define GPP_F6			222
#define GPP_F7			223
#define GPP_F8			224
#define GPP_F9			225
#define GPP_F10			226
#define GPP_F11			227
#define GPP_F12			228
#define GPP_F13			229
#define GPP_F14			230
#define GPP_F15			231
#define GPP_F16			232
#define GPP_F17			233
#define GPP_F18			234
#define GPP_F19			235
#define GPP_F20			236
#define GPP_F21			237
#define GPP_F22			238
#define GPP_F23			239

/* SPI */
#define SPI0_IO_2		240
#define SPI0_IO_3		241
#define SPI0_MOSI		242
#define SPI0_MISO		243
#define SPI0_CS2_B		244
#define SPI0_CS0_B		245
#define SPI0_CS1_B		246
#define SPI0_CLK		247
#define SPI0_CLK_LOOPBK		248

#define NUM_GPIO_COM3_PADS	(SPI0_CLK_LOOPBK - GPP_K0 + 1)

/* CPU */
#define HDACPU_SDI		249
#define HDACPU_SDO		250
#define HDACPU_SCLK		251
#define PM_SYNC			252
#define PECI_IO			253
#define CPUPWRGD		254
#define THRMTRIP_B		255
#define PLTRST_CPU_B		256
#define PM_DOWN			257
#define TRIGGER_IN		258
#define TRIGGER_OUT		259

/* JTAG */
#define PCH_TDO			260
#define PCH_JTAGX		261
#define PROC_PRDY_B		262
#define PROC_PREQ_B		263
#define CPU_TRST_B		264
#define PCH_TDI			265
#define PCH_TMS			266
#define PCH_TCK			267
#define ITP_PMODE		268

/* Group I */
#define GPP_I0			269
#define GPP_I1			270
#define GPP_I2			271
#define GPP_I3			272
#define GPP_I4			273
#define GPP_I5			274
#define GPP_I6			275
#define GPP_I7			276
#define GPP_I8			277
#define GPP_I9			278
#define GPP_I10			279
#define GPP_I11			280
#define GPP_I12			281
#define GPP_I13			282
#define GPP_I14			283
#define SYS_PWROK		284
#define SYS_RESET_B		285
#define CL_RST_B		286

/* Group J */
#define GPP_J0			287
#define GPP_J1			288
#define GPP_J2			289
#define GPP_J3			290
#define GPP_J4			291
#define GPP_J5			292
#define GPP_J6			293
#define GPP_J7			294
#define GPP_J8			295
#define GPP_J9			296
#define GPP_J10			297
#define GPP_J11			298

#define NUM_GPIO_COM4_PADS	(GPP_J11 - GPP_I0 + 1)

/* Group GPD */
#define GPD0			299
#define GPD1			300
#define GPD2			301
#define GPD3			302
#define GPD4			303
#define GPD5			304
#define GPD6			305
#define GPD7			306
#define GPD8			307
#define GPD9			308
#define GPD10			309
#define GPD11			310
#define SLP_LAN_B		311
#define SLP_SUS_B		312
#define WAKE_B			313
#define DRAM_RESET_B		314

#define NUM_GPIO_COM2_PADS	(DRAM_RESET_B - GPD0 + 1)

#define TOTAL_PADS		(DRAM_RESET_B + 1)

#define COMM_0		0
#define COMM_1		1
#define COMM_2		2
#define COMM_3		3
#define COMM_4		4
#define TOTAL_GPIO_COMM	5

#endif
