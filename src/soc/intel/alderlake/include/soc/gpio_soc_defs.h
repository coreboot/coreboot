/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _SOC_ALDERLAKE_GPIO_SOC_DEFS_H_
#define _SOC_ALDERLAKE_GPIO_SOC_DEFS_H_

/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 *
 * Refer to Alder Lake PCH EDS Chapter 27, MISCCFG register offset 0x10
 * for each GPIO community to get GPIO group to GPE_DWx assignment.
 */
#define GPP_B			0x0
#define GPP_T			0x1
#define GPP_A			0x2
#define GPP_R			0x3
#define GPD			0x4
#define GPP_S			0x5
#define GPP_H			0x6
#define GPP_D			0x7
#define GPP_F			0xA
#define GPP_C			0xB
#define GPP_E			0xC

#define GPIO_MAX_NUM_PER_GROUP	26

#define COMM_0			0
#define COMM_1			1
#define COMM_2			2
#define COMM_3			3
#define COMM_4			4
#define COMM_5			5
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
#define GPP_ESPI_CLK_LOOPBK	66

#define GPIO_COM0_START		GPP_B0
#define GPIO_COM0_END		GPP_ESPI_CLK_LOOPBK
#define NUM_GPIO_COM0_PADS	(GPIO_COM0_END - GPIO_COM0_START + 1)

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

/* Reserved GPIOs */
#define GPP_CPU_RSVD_1		120
#define GPP_CPU_RSVD_2		121
#define GPP_CPU_RSVD_3		122
#define GPP_CPU_RSVD_4		123
#define GPP_CPU_RSVD_5		124
#define GPP_CPU_RSVD_6		125
#define GPP_CPU_RSVD_7		126
#define GPP_CPU_RSVD_8		127
#define GPP_CPU_RSVD_9		128
#define GPP_CPU_RSVD_10		129
#define GPP_CPU_RSVD_11		130
#define GPP_CPU_RSVD_12		131
#define GPP_CPU_RSVD_13		132
#define GPP_CPU_RSVD_14		133
#define GPP_CPU_RSVD_15		134
#define GPP_CPU_RSVD_16		135
#define GPP_CPU_RSVD_17		136
#define GPP_CPU_RSVD_18		137
#define GPP_CPU_RSVD_19		138
#define GPP_CPU_RSVD_20		139
#define GPP_CPU_RSVD_21		140
#define GPP_CPU_RSVD_22		141
#define GPP_CPU_RSVD_23		142
#define GPP_CPU_RSVD_24		143

/* Group VGPIO */
#define GPP_VGPIO_0		144
#define GPP_VGPIO_4		145
#define GPP_VGPIO_5		146
#define GPP_VGPIO_6		147
#define GPP_VGPIO_7		148
#define GPP_VGPIO_8		149
#define GPP_VGPIO_9		150
#define GPP_VGPIO_10		151
#define GPP_VGPIO_11		152
#define GPP_VGPIO_12		153
#define GPP_VGPIO_13		154
#define GPP_VGPIO_18		155
#define GPP_VGPIO_19		156
#define GPP_VGPIO_20		157
#define GPP_VGPIO_21		158
#define GPP_VGPIO_22		159
#define GPP_VGPIO_23		160
#define GPP_VGPIO_24		161
#define GPP_VGPIO_25		162
#define GPP_VGPIO_30		163
#define GPP_VGPIO_31		164
#define GPP_VGPIO_32		165
#define GPP_VGPIO_33		166
#define GPP_VGPIO_34		167
#define GPP_VGPIO_35		168
#define GPP_VGPIO_36		169
#define GPP_VGPIO_37		170

#define GPIO_COM1_START		GPP_S0
#define GPIO_COM1_END		GPP_VGPIO_37
#define NUM_GPIO_COM1_PADS	(GPIO_COM1_END - GPIO_COM1_START + 1)

/* Group GPD */
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
#define NUM_GPIO_COM2_PADS	(GPIO_COM2_END - GPIO_COM2_START + 1)

/* PCIE VGPIO group */
#define GPP_CPU_RSVD_25		188
#define GPP_CPU_RSVD_26		189
#define GPP_CPU_RSVD_27		190
#define GPP_CPU_RSVD_28		191
#define GPP_CPU_RSVD_29		192
#define GPP_CPU_RSVD_30		193
#define GPP_CPU_RSVD_31		194
#define GPP_CPU_RSVD_32		195
#define GPP_CPU_RSVD_33		196
#define GPP_CPU_RSVD_34		197
#define GPP_CPU_RSVD_35		198
#define GPP_CPU_RSVD_36		199
#define GPP_CPU_RSVD_37		200
#define GPP_CPU_RSVD_38		201
#define GPP_CPU_RSVD_39		202
#define GPP_vGPIO_PCIE_0	203
#define GPP_vGPIO_PCIE_1	204
#define GPP_vGPIO_PCIE_2	205
#define GPP_vGPIO_PCIE_3	206
#define GPP_vGPIO_PCIE_4	207
#define GPP_vGPIO_PCIE_5	208
#define GPP_vGPIO_PCIE_6	209
#define GPP_vGPIO_PCIE_7	210
#define GPP_vGPIO_PCIE_8	211
#define GPP_vGPIO_PCIE_9	212
#define GPP_vGPIO_PCIE_10	213
#define GPP_vGPIO_PCIE_11	214
#define GPP_vGPIO_PCIE_12	215
#define GPP_vGPIO_PCIE_13	216
#define GPP_vGPIO_PCIE_14	217
#define GPP_vGPIO_PCIE_15	218
#define GPP_vGPIO_PCIE_16	219
#define GPP_vGPIO_PCIE_17	220
#define GPP_vGPIO_PCIE_18	221
#define GPP_vGPIO_PCIE_19	222
#define GPP_vGPIO_PCIE_20	223
#define GPP_vGPIO_PCIE_21	224
#define GPP_vGPIO_PCIE_22	225
#define GPP_vGPIO_PCIE_23	226
#define GPP_vGPIO_PCIE_24	227
#define GPP_vGPIO_PCIE_25	228
#define GPP_vGPIO_PCIE_26	229
#define GPP_vGPIO_PCIE_27	230
#define GPP_vGPIO_PCIE_28	231
#define GPP_vGPIO_PCIE_29	232
#define GPP_vGPIO_PCIE_30	233
#define GPP_vGPIO_PCIE_31	234
#define GPP_vGPIO_PCIE_32	235
#define GPP_vGPIO_PCIE_33	236
#define GPP_vGPIO_PCIE_34	237
#define GPP_vGPIO_PCIE_35	238
#define GPP_vGPIO_PCIE_36	239
#define GPP_vGPIO_PCIE_37	240
#define GPP_vGPIO_PCIE_38	241
#define GPP_vGPIO_PCIE_39	242
#define GPP_vGPIO_PCIE_40	243
#define GPP_vGPIO_PCIE_41	244
#define GPP_vGPIO_PCIE_42	245
#define GPP_vGPIO_PCIE_43	246
#define GPP_vGPIO_PCIE_44	247
#define GPP_vGPIO_PCIE_45	248
#define GPP_vGPIO_PCIE_46	249
#define GPP_vGPIO_PCIE_47	250
#define GPP_vGPIO_PCIE_48	251
#define GPP_vGPIO_PCIE_49	252
#define GPP_vGPIO_PCIE_50	253
#define GPP_vGPIO_PCIE_51	254
#define GPP_vGPIO_PCIE_52	255
#define GPP_vGPIO_PCIE_53	256
#define GPP_vGPIO_PCIE_54	257
#define GPP_vGPIO_PCIE_55	258
#define GPP_vGPIO_PCIE_56	259
#define GPP_vGPIO_PCIE_57	260
#define GPP_vGPIO_PCIE_58	261
#define GPP_vGPIO_PCIE_59	262
#define GPP_vGPIO_PCIE_60	263
#define GPP_vGPIO_PCIE_61	264
#define GPP_vGPIO_PCIE_62	265
#define GPP_vGPIO_PCIE_63	266
#define GPP_vGPIO_PCIE_64	267
#define GPP_vGPIO_PCIE_65	268
#define GPP_vGPIO_PCIE_66	269
#define GPP_vGPIO_PCIE_67	270
#define GPP_vGPIO_PCIE_68	271
#define GPP_vGPIO_PCIE_69	272
#define GPP_vGPIO_PCIE_70	273
#define GPP_vGPIO_PCIE_71	274
#define GPP_vGPIO_PCIE_72	275
#define GPP_vGPIO_PCIE_73	276
#define GPP_vGPIO_PCIE_74	277
#define GPP_vGPIO_PCIE_75	278
#define GPP_vGPIO_PCIE_76	279
#define GPP_vGPIO_PCIE_77	280
#define GPP_vGPIO_PCIE_78	281
#define GPP_vGPIO_PCIE_79	282
#define GPP_CPU_RSVD_40		283
#define GPP_CPU_RSVD_41		284
#define GPP_CPU_RSVD_42		285
#define GPP_CPU_RSVD_43		286
#define GPP_CPU_RSVD_44		287
#define GPP_CPU_RSVD_45		288
#define GPP_CPU_RSVD_46		289
#define GPP_CPU_RSVD_47		290
#define GPP_vGPIO_PCIE_80	291
#define GPP_vGPIO_PCIE_81	292
#define GPP_vGPIO_PCIE_82	293
#define GPP_vGPIO_PCIE_83	294

#define GPIO_COM3_START		GPP_CPU_RSVD_25
#define GPIO_COM3_END		GPP_vGPIO_PCIE_83
#define NUM_GPIO_COM3_PADS      (GPIO_COM3_END - GPIO_COM3_START + 1)

/* Group C */
#define GPP_C0			295
#define GPP_C1			296
#define GPP_C2			297
#define GPP_C3			298
#define GPP_C4			299
#define GPP_C5			300
#define GPP_C6			301
#define GPP_C7			302
#define GPP_C8			303
#define GPP_C9			304
#define GPP_C10			305
#define GPP_C11			306
#define GPP_C12			307
#define GPP_C13			308
#define GPP_C14			309
#define GPP_C15			310
#define GPP_C16			311
#define GPP_C17			312
#define GPP_C18			313
#define GPP_C19			314
#define GPP_C20			315
#define GPP_C21			316
#define GPP_C22			317
#define GPP_C23			318

/* Group F */
#define GPP_F0			319
#define GPP_F1			320
#define GPP_F2			321
#define GPP_F3			322
#define GPP_F4			323
#define GPP_F5			324
#define GPP_F6			325
#define GPP_F7			326
#define GPP_F8			327
#define GPP_F9			328
#define GPP_F10			329
#define GPP_F11			330
#define GPP_F12			331
#define GPP_F13			332
#define GPP_F14			333
#define GPP_F15			334
#define GPP_F16			335
#define GPP_F17			336
#define GPP_F18			337
#define GPP_F19			338
#define GPP_F20			339
#define GPP_F21			340
#define GPP_F22			341
#define GPP_F23			342
#define GPP_F_CLK_LOOPBK	343

/* Group HVMOS */
#define GPP_L_BKLTEN		344
#define GPP_L_BKLTCTL		345
#define GPP_L_VDDEN		346
#define GPP_SYS_PWROK		347
#define GPP_SYS_RESETB		348
#define GPP_MLK_RSTB		349

/* Group E */
#define GPP_E0			350
#define GPP_E1			351
#define GPP_E2			352
#define GPP_E3			353
#define GPP_E4			354
#define GPP_E5			355
#define GPP_E6			356
#define GPP_E7			357
#define GPP_E8			358
#define GPP_E9			359
#define GPP_E10			360
#define GPP_E11			361
#define GPP_E12			362
#define GPP_E13			363
#define GPP_E14			364
#define GPP_E15			365
#define GPP_E16			366
#define GPP_E17			367
#define GPP_E18			368
#define GPP_E19			369
#define GPP_E20			370
#define GPP_E21			371
#define GPP_E22			372
#define GPP_E23			373
#define GPP_E_CLK_LOOPBK	374

#define GPIO_COM4_START		GPP_C0
#define GPIO_COM4_END		GPP_E_CLK_LOOPBK
#define NUM_GPIO_COM4_PADS	(GPIO_COM4_END - GPIO_COM4_START + 1)

/* Group R */
#define GPP_R0			375
#define GPP_R1			376
#define GPP_R2			377
#define GPP_R3			378
#define GPP_R4			379
#define GPP_R5			380
#define GPP_R6			381
#define GPP_R7			382

/* Group SPI0 */
#define GPP_SPI0_IO_2		383
#define GPP_SPI0_IO_3		384
#define GPP_SPI0_MOSI_IO_0	385
#define GPP_SPI0_MOSI_IO_1	386
#define GPP_SPI0_TPM_CSB	387
#define GPP_SPI0_FLASH_0_CSB	388
#define GPP_SPI0_FLASH_1_CSB	389
#define GPP_SPI0_CLK		390

#define GPIO_COM5_START		GPP_R0
#define GPIO_COM5_END		GPP_SPI0_CLK
#define NUM_GPIO_COM5_PADS	(GPIO_COM5_END - GPIO_COM5_START + 1)

#define TOTAL_GPIO_COMM		(COMM_5 + 1)
#define TOTAL_PADS		(GPIO_COM5_END + 1)

#endif
