/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ELKHARTLAKE_GPIO_SOC_DEFS_H_
#define _SOC_ELKHARTLAKE_GPIO_SOC_DEFS_H_

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
#define GPP_V			0x5
#define GPP_H			0x6
#define GPP_D			0x7
#define GPP_U			0x8
#define VGPIO			0x9
#define GPP_F			0xA
#define GPP_C			0xB
#define GPP_E			0xC
#define GPP_G			0xD

#define GPIO_NUM_GROUPS		15
#define GPIO_MAX_NUM_PER_GROUP	32
#define SD_PWR_EN_PIN		GPP_H17

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
#define GPIO_RSVD_0		24
#define GPIO_RSVD_1		25

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

/* Group G */
#define GPP_G0			42
#define GPP_G1			43
#define GPP_G2			44
#define GPP_G3			45
#define GPP_G4			46
#define GPP_G5			47
#define GPP_G6			48
#define GPP_G7			49
#define GPP_G8			50
#define GPP_G9			51
#define GPP_G10			52
#define GPP_G11			53
#define GPP_G12			54
#define GPP_G13			55
#define GPP_G14			56
#define GPP_G15			57
#define GPP_G16			58
#define GPP_G17			59
#define GPP_G18			60
#define GPP_G19			61
#define GPP_G20			62
#define GPP_G21			63
#define GPP_G22			64
#define GPP_G23			65
#define GPIO_RSVD_2		66

#define GPIO_COM0_START		GPP_B0
#define GPIO_COM0_END		GPIO_RSVD_2
#define NUM_GPIO_COM0_PADS	(GPIO_COM0_END - GPIO_COM0_START + 1)

/* Group V */
#define GPP_V0			67
#define GPP_V1			68
#define GPP_V2			69
#define GPP_V3			70
#define GPP_V4			71
#define GPP_V5			72
#define GPP_V6			73
#define GPP_V7			74
#define GPP_V8			75
#define GPP_V9			76
#define GPP_V10			77
#define GPP_V11			78
#define GPP_V12			79
#define GPP_V13			80
#define GPP_V14			81
#define GPP_V15			82

/* Group H */
#define GPP_H0			83
#define GPP_H1			84
#define GPP_H2			85
#define GPP_H3			86
#define GPP_H4			87
#define GPP_H5			88
#define GPP_H6			89
#define GPP_H7			90
#define GPP_H8			91
#define GPP_H9			92
#define GPP_H10			93
#define GPP_H11			94
#define GPP_H12			95
#define GPP_H13			96
#define GPP_H14			97
#define GPP_H15			98
#define GPP_H16			99
#define GPP_H17			100
#define GPP_H18			101
#define GPP_H19			102
#define GPP_H20			103
#define GPP_H21			104
#define GPP_H22			105
#define GPP_H23			106

/* Group D */
#define GPP_D0			107
#define GPP_D1			108
#define GPP_D2			109
#define GPP_D3			110
#define GPP_D4			111
#define GPP_D5			112
#define GPP_D6			113
#define GPP_D7			114
#define GPP_D8			115
#define GPP_D9			116
#define GPP_D10			117
#define GPP_D11			118
#define GPP_D12			119
#define GPP_D13			120
#define GPP_D14			121
#define GPP_D15			122
#define GPP_D16			123
#define GPP_D17			124
#define GPP_D18			125
#define GPP_D19			126
#define GPIO_RSVD_3		127

/* Group U */
#define GPP_U0			128
#define GPP_U1			129
#define GPP_U2			130
#define GPP_U3			131
#define GPP_U4			132
#define GPP_U5			133
#define GPP_U6			134
#define GPP_U7			135
#define GPP_U8			136
#define GPP_U9			137
#define GPP_U10			138
#define GPP_U11			139
#define GPP_U12			140
#define GPP_U13			141
#define GPP_U14			142
#define GPP_U15			143
#define GPP_U16			144
#define GPP_U17			145
#define GPP_U18			146
#define GPP_U19			147
#define GPIO_RSVD_4		148
#define GPIO_RSVD_5		149
#define GPIO_RSVD_6		150
#define GPIO_RSVD_7		151

/* Group VGPIO */
#define VGPIO_0			152
#define VGPIO_4			153
#define VGPIO_5			154
#define VGPIO_6			155
#define VGPIO_7			156
#define VGPIO_8			157
#define VGPIO_9			158
#define VGPIO_10		159
#define VGPIO_11		160
#define VGPIO_12		161
#define VGPIO_13		162
#define VGPIO_18		163
#define VGPIO_19		164
#define VGPIO_20		165
#define VGPIO_21		166
#define VGPIO_22		167
#define VGPIO_23		168
#define VGPIO_24		169
#define VGPIO_25		170
#define VGPIO_30		171
#define VGPIO_31		172
#define VGPIO_32		173
#define VGPIO_33		174
#define VGPIO_34		175
#define VGPIO_35		176
#define VGPIO_36		177
#define VGPIO_37		178
#define VGPIO_39		179

#define GPIO_COM1_START		GPP_V0
#define GPIO_COM1_END		VGPIO_39
#define NUM_GPIO_COM1_PADS	(GPIO_COM1_END - GPIO_COM1_START + 1)

/* Group GPD  */
#define GPD0			180
#define GPD1			181
#define GPD2			182
#define GPD3			183
#define GPD4			184
#define GPD5			185
#define GPD6			186
#define GPD7			187
#define GPD8			188
#define GPD9			189
#define GPD10			190
#define GPD11			191
#define GPIO_RSVD_8		192
#define GPIO_RSVD_9		193
#define GPIO_RSVD_10		194
#define GPIO_RSVD_11		195
#define GPIO_RSVD_12		196

#define GPIO_COM2_START		GPD0
#define GPIO_COM2_END		GPIO_RSVD_12
#define NUM_GPIO_COM2_PADS	(GPIO_COM2_END - GPIO_COM2_START + 1)

/* Group S */
#define GPIO_RSVD_13		197
#define GPIO_RSVD_14		198
#define GPIO_RSVD_15		199
#define GPIO_RSVD_16		200
#define GPIO_RSVD_17		201
#define GPIO_RSVD_18		202
#define GPIO_RSVD_19		203
#define GPIO_RSVD_20		204
#define GPIO_RSVD_21		205
#define GPIO_RSVD_22		206
#define GPIO_RSVD_23		207
#define GPIO_RSVD_24		208
#define GPIO_RSVD_25		209
#define GPIO_RSVD_26		210
#define GPIO_RSVD_27		211
#define GPIO_RSVD_28		212
#define GPIO_RSVD_29		213
#define GPP_S0			214
#define GPP_S1			215

/* Group A */
#define GPP_A0			216
#define GPP_A1			217
#define GPP_A2			218
#define GPP_A3			219
#define GPP_A4			220
#define GPP_A5			221
#define GPP_A6			222
#define GPP_A7			223
#define GPP_A8			224
#define GPP_A9			225
#define GPP_A10			226
#define GPP_A11			227
#define GPP_A12			228
#define GPP_A13			229
#define GPP_A14			230
#define GPP_A15			231
#define GPP_A16			232
#define GPP_A17			233
#define GPP_A18			234
#define GPP_A19			235
#define GPP_A20			236
#define GPP_A21			237
#define GPP_A22			238
#define GPP_A23			239

/* Group VGPIO 3 */
#define VGPIO_USB_0		240
#define VGPIO_USB_1		241
#define VGPIO_USB_2		242
#define VGPIO_USB_3		243

#define GPIO_COM3_START		GPIO_RSVD_13
#define GPIO_COM3_END		VGPIO_USB_3
#define NUM_GPIO_COM3_PADS	(GPIO_COM3_END - GPIO_COM3_START + 1)

/* Group C */
#define GPP_C0			244
#define GPP_C1			245
#define GPP_C2			246
#define GPP_C3			247
#define GPP_C4			248
#define GPP_C5			249
#define GPP_C6			250
#define GPP_C7			251
#define GPP_C8			252
#define GPP_C9			253
#define GPP_C10			254
#define GPP_C11			255
#define GPP_C12			256
#define GPP_C13			257
#define GPP_C14			258
#define GPP_C15			259
#define GPP_C16			260
#define GPP_C17			261
#define GPP_C18			262
#define GPP_C19			263
#define GPP_C20			264
#define GPP_C21			265
#define GPP_C22			266
#define GPP_C23			267

/* Group F */
#define GPP_F0			268
#define GPP_F1			269
#define GPP_F2			270
#define GPP_F3			271
#define GPP_F4			272
#define GPP_F5			273
#define GPP_F6			274
#define GPP_F7			275
#define GPP_F8			276
#define GPP_F9			277
#define GPP_F10			278
#define GPP_F11			279
#define GPP_F12			280
#define GPP_F13			281
#define GPP_F14			282
#define GPP_F15			283
#define GPP_F16			284
#define GPP_F17			285
#define GPP_F18			286
#define GPP_F19			287
#define GPP_F20			288
#define GPP_F21			289
#define GPP_F22			290
#define GPP_F23			291
#define GPIO_RSVD_30		292
#define GPIO_RSVD_31		293
#define GPIO_RSVD_32		294
#define GPIO_RSVD_33		295
#define GPIO_RSVD_34		296
#define GPIO_RSVD_35		297
#define GPIO_RSVD_36		298

/* Group E */
#define GPP_E0			299
#define GPP_E1			300
#define GPP_E2			301
#define GPP_E3			302
#define GPP_E4			303
#define GPP_E5			304
#define GPP_E6			305
#define GPP_E7			306
#define GPP_E8			307
#define GPP_E9			308
#define GPP_E10			309
#define GPP_E11			310
#define GPP_E12			311
#define GPP_E13			312
#define GPP_E14			313
#define GPP_E15			314
#define GPP_E16			315
#define GPP_E17			316
#define GPP_E18			317
#define GPP_E19			318
#define GPP_E20			319
#define GPP_E21			320
#define GPP_E22			321
#define GPP_E23			322

#define GPIO_COM4_START		GPP_C0
#define GPIO_COM4_END		GPP_E23
#define NUM_GPIO_COM4_PADS	(GPIO_COM4_END - GPIO_COM4_START + 1)

/* Group R */
#define GPP_R0			323
#define GPP_R1			324
#define GPP_R2			325
#define GPP_R3			326
#define GPP_R4			327
#define GPP_R5			328
#define GPP_R6			329
#define GPP_R7			330

#define GPIO_COM5_START		GPP_R0
#define GPIO_COM5_END		GPP_R7
#define NUM_GPIO_COM5_PADS	(GPIO_COM5_END - GPIO_COM5_START + 1)

#define TOTAL_PADS		331

#define COMM_0			0
#define COMM_1			1
#define COMM_2			2
#define COMM_3			3
#define COMM_4			4
#define COMM_5			5
#define TOTAL_GPIO_COMM		6

#endif
