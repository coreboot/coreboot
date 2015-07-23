/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef _SOC_GPIO_H_
#define _SOC_GPIO_H_

#include <stdint.h>

/*
 * GPP_Ax to GPP_Gx;
 * where x=24 [between GPIO Community A to F]
 * = 7 [only for GPIO Community G]
 */
#define MAX_GPIO_NUMBER		151	/* zero based */
#define GPIO_LIST_END		0xffffffff

/*
 * Skylake LP GPIO PIN to Pad Mapping
 */
#define GPIO_LP_GROUP_A			0x0
#define GPIO_LP_GROUP_B			0x1
#define GPIO_LP_GROUP_C			0x2
#define GPIO_LP_GROUP_D			0x3
#define GPIO_LP_GROUP_E			0x4
#define GPIO_LP_GROUP_F			0x5
#define GPIO_LP_GROUP_G			0x6

#define GPIO_LP_GROUP_GPP_A		0x0200
#define GPIO_LP_GROUP_GPP_B		0x0201
#define GPIO_LP_GROUP_GPP_C		0x0202
#define GPIO_LP_GROUP_GPP_D		0x0203
#define GPIO_LP_GROUP_GPP_E		0x0204
#define GPIO_LP_GROUP_GPP_F		0x0205
#define GPIO_LP_GROUP_GPP_G		0x0206

#define GPIO_GROUP_SHIFT		16
#define MAX_GPIO_PIN_PER_GROUP	24

/* GPIO TX STATE */
#define B_PCH_GPIO_TX_STATE	0x0001
#define N_PCH_GPIO_TX_STATE	0

/* Interrupt number */
#define B_PCH_GPIO_INTSEL 0x7F
#define N_PCH_GPIO_INTSEL 0

/* Structure for storing information about registers offset, community,
 * maximal pad number, smi status and smi enable for available groups
 */
typedef struct {
	u32 community;
	u32 padcfgoffset;
	u32 padpergroup;
	u32 smistsoffset;
	u32 smienoffset;
} GPIO_GROUP_INFO;

/*
 * GPIO Community 0 Registers are for GPP_A and GPP_B groups
 */
#define R_PCH_PCR_GPIO_GPP_A_PADCFG_OFFSET	0x400
#define R_PCH_PCR_GPIO_GPP_B_PADCFG_OFFSET	0x4C0
#define R_PCH_PCR_GPIO_GPP_A_SMI_STS	0x0180
#define R_PCH_PCR_GPIO_GPP_B_SMI_STS	0x0184
#define R_PCH_PCR_GPIO_GPP_A_SMI_EN	0x01A0
#define R_PCH_PCR_GPIO_GPP_B_SMI_EN	0x01A4

/*
 * GPIO Community 1 Registers are for GPP_C, GPP_D, GPP_E groups
 */
#define R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET	0x400
#define R_PCH_PCR_GPIO_GPP_D_PADCFG_OFFSET	0x4C0
#define R_PCH_PCR_GPIO_GPP_E_PADCFG_OFFSET	0x580
#define R_PCH_PCR_GPIO_GPP_C_SMI_STS	0x0180
#define R_PCH_PCR_GPIO_GPP_D_SMI_STS	0x0184
#define R_PCH_PCR_GPIO_GPP_E_SMI_STS	0x0188
#define R_PCH_PCR_GPIO_GPP_C_SMI_EN	0x01A0
#define R_PCH_PCR_GPIO_GPP_D_SMI_EN	0x01A4
#define R_PCH_PCR_GPIO_GPP_E_SMI_EN	0x01A8

/*
 * GPIO Community 3 Registers are for GPP_F and GPP_G groups
 */
#define R_PCH_PCR_GPIO_GPP_F_PADCFG_OFFSET	0x400
#define R_PCH_PCR_GPIO_GPP_G_PADCFG_OFFSET	0x4C0

/*
 * GPIO Community 2 Registers are for GPP_DSW
 */
#define R_PCH_PCR_GPIO_GPD_PADCFG_OFFSET	0x400

#define READ	0
#define WRITE	1

/* If in GPIO_GROUP_INFO structure certain register doesn't exist
 * it will have value equal to NO_REGISTER_PROPERTY
 */
#define NO_REGISTER_PROPERTY (~0u)

#define V_PCH_GPIO_GPP_A_PAD_MAX	24
#define V_PCH_GPIO_GPP_B_PAD_MAX	24
#define V_PCH_GPIO_GPP_C_PAD_MAX	24
#define V_PCH_GPIO_GPP_D_PAD_MAX	24
#define V_PCH_GPIO_GPP_E_PAD_MAX	24
#define V_PCH_GPIO_GPP_F_PAD_MAX	24
#define V_PCH_GPIO_GPP_G_PAD_MAX	8
#define V_PCH_GPIO_GPD_PAD_MAX		12

/* SOC has 8 GPIO communities GPP A~G, GPD */
#define GPIO_COMMUNITY_MAX		8

#define GPIO_GET_GROUP_INDEX(group)	(group & 0xFF)
#define GPIO_GET_GROUP_INDEX_FROM_PAD(pad)	(\
				GPIO_GET_GROUP_INDEX((pad >> 16)))
#define GPIO_GET_PAD_NUMBER(pad)	(pad & 0xFFFF)

/* Number of pins used by SerialIo controllers */
#define PCH_SERIAL_IO_PINS_PER_UART_CONTROLLER			4
#define PCH_SERIAL_IO_PINS_PER_UART_CONTROLLER_NO_FLOW_CTRL	2

/* Below defines are based on GPIO_CONFIG structure fields */
#define GPIO_CONF_PAD_MODE_MASK		0xF
#define GPIO_CONF_PAD_MODE_BIT_POS	0

/* GPIO Pad Mode */
#define B_PCH_GPIO_PAD_MODE	(0x1000 | 0x800 | 0x400)
#define N_PCH_GPIO_PAD_MODE	10

/* For any GpioPad usage in code use GPIO_PAD type*/
typedef u32 GPIO_PAD;

/* For any GpioGroup usage in code use GPIO_GROUP type */
typedef u32 GPIO_GROUP;

/*
 * GPIO configuration structure used for pin programming.
 * Structure contains fields that can be used to configure pad.
 */
typedef struct {
	/*
	Pad Mode
	Pad can be set as GPIO or one of its native functions.
	When in native mode setting Direction, OutputState, Interrupt is unnecessary.
	Refer to definition of GPIO_PAD_MODE.
	Refer to EDS for each native mode according to the pad.
	*/
	u32 PadMode	: 4;
	/*
	Host Software Pad Ownership
	Set pad to ACPI mode or GPIO Driver Mode.
	Refer to definition of GPIO_HOSTSW_OWN.
	*/
	u32 HostSoftPadOwn	: 2;
	/*
	GPIO Direction
	Can choose between In, In with inversion Out, both In and Out, both In with inversion and out or d
	isabling both.
	Refer to definition of GPIO_DIRECTION for supported settings.
	*/
	u32 Direction		: 5;
	/*
	Output State
	Set Pad output value.
	Refer to definition of GPIO_OUTPUT_STATE for supported settings.
	This setting takes place when output is enabled.
	*/
	u32 OutputState	: 2;
	/*
	GPIO Interrupt Configuration
	Set Pad to cause one of interrupts (IOxAPIC/SCI/SMI/NMI). This setting is applicable only if GPIO
	A is in input mode.
	If GPIO is set to cause an SCI then also Gpe is enabled for this pad.
	Refer to definition of GPIO_INT_CONFIG for supported settings.
	*/
	u32 InterruptConfig	: 8;
	/*
	GPIO Power Configuration.
	This setting controls Pad Reset Configuration and Power Rail Type.
	Refer to definition of GPIO_RESET_CONFIG for supported settings.
	*/
	u32 PowerConfig		: 4;
	/*
	GPIO Electrical Configuration
	This setting controls pads termination and voltage tolerance.
	Refer to definition of GPIO_ELECTRICAL_CONFIG for supported settings.
	*/
	u32 ElectricalConfig	: 7;
	/*
	GPIO Lock Configuration
	This setting controls pads lock.
	Refer to definition of GPIO_LOCK_CONFIG for supported settings.
	*/
	u32 LockConfig		: 3;
	/*
	Additional GPIO configuration
	Refer to definition of GPIO_OTHER_CONFIG for supported settings.
	*/
	u32 OtherSettings	: 2;
	u32 RsvdBits		: 27;
} GPIO_CONFIG;

typedef struct {
	GPIO_PAD	GpioPad;
	GPIO_CONFIG	GpioConfig;
} GPIO_INIT_CONFIG;

typedef enum {
	GpioHardwareDefault = 0x0
} GPIO_HARDWARE_DEFAULT;

/* GPIO Pad Mode */
typedef enum {
	GpioPadModeGpio		= 0x1,
	GpioPadModeNative1	= 0x3,
	GpioPadModeNative2	= 0x5,
	GpioPadModeNative3	= 0x7,
	GpioPadModeNative4	= 0x9,
} GPIO_PAD_MODE;

/* Host Software Pad Ownership modes */
typedef enum {
	GpioHostOwnDefault	= 0x0,	/* Leave ownership value unmodified */
	GpioHostOwnAcpi		= 0x1,	/* Set HOST ownership to ACPI */
	GpioHostOwnGpio		= 0x3	/* Set HOST ownership to GPIO */
} GPIO_HOSTSW_OWN;

/* GPIO Direction */
typedef enum {
	GpioDirDefault	= 0x0,	/* Leave pad direction setting unmodified */
	GpioDirInOut	= (0x1 | (0x1 << 3)),	/* Set pad for both output and input */
	GpioDirInInvOut	= (0x1 | (0x3 << 3)),	/* Set pad for both output and input with inversion */
	GpioDirIn	= (0x3 | (0x1 << 3)),	/* Set pad for input only */
	GpioDirInInv	= (0x3 | (0x3 << 3)),	/* Set pad for input with inversion */
	GpioDirOut	= 0x5,	/* Set pad for output only */
	GpioDirNone	= 0x7	/* Disable both output and input */
} GPIO_DIRECTION;

/* GPIO Output State */
typedef enum {
	GpioOutDefault	= 0x0,
	GpioOutLow	= 0x1,
	GpioOutHigh	= 0x3
} GPIO_OUTPUT_STATE;

/*
 * GPIO interrupt configuration
 * This setting is applicable only if GPIO is in input mode.
 * GPIO_INT_CONFIG allows to choose which interrupt is generted
 * (IOxAPIC/SCI/SMI/NMI) and how it is triggered (edge or level).
 * Field from GpioIntNmi to GpioIntApic can be OR'ed with GpioIntLevel to
 * GpioIntBothEdgecan to describe an interrupt e.g. GpioIntApic | GpioIntLevel
 * If GPIO is set to cause an SCI then also Gpe is enabled for this pad.
 * Not all GPIO are capable of generating an SMI or NMI interrupt
 */
typedef enum {
	GpioIntDefault	= 0x0,	/* Leave value of interrupt routing unmodified */
	GpioIntDis	= 0x1,	/* Disable IOxAPIC/SCI/SMI/NMI interrupt generation */
	GpioIntNmi	= 0x3,	/* Enable NMI interrupt only */
	GpioIntSmi	= 0x5,	/* Enable SMI interrupt only */
	GpioIntSci	= 0x9,	/* Enable SCI interrupt only */
	GpioIntApic	= 0x11,	/* Enable IOxAPIC interrupt only */
	GpioIntLevel	= (0x1 << 5),	/* Set interrupt as level triggered */
	GpioIntEdge	= (0x3 << 5),	/* Set interrupt as edge triggered */
	GpioIntLvlEdgDis	= (0x5 << 5),	/* Disable interrupt trigger */
	GpioIntBothEdge	= (0x7 << 5)	/* Set interrupt as both edge triggered */
} GPIO_INT_CONFIG;

/*
 * GPIO Power Configuration
 * GPIO_RESET_CONFIG allows to set GPIO Reset (used to reset the specified
 * Pad Register fields).
 */
typedef enum {
	GpioResetDefault	= 0x0,	/* Leave value of pad reset unmodified */
	GpioResetPwrGood	= 0x1,	/* Powergood reset */
	GpioResetDeep		= 0x3,	/* Deep GPIO Reset */
	GpioResetNormal		= 0x5,	/* GPIO Reset */
	GpioResetResume		= 0x7	/* Resume Reset */
} GPIO_RESET_CONFIG;

typedef int gpio_t;

/* Clear GPIO SMI Status */
void clear_all_smi(void);

/* Get GPIO SMI Status */
void get_smi_status(u32 status[GPIO_COMMUNITY_MAX]);

/* Enable GPIO SMI  */
void enable_all_smi(void);

/* Enable GPIO individual Group SMI  */
void enable_gpio_groupsmi(gpio_t gpio_num, u32 mask);

/*
 * GPIO Electrical Configuration
 * Set GPIO termination and Pad Tolerance (applicable only for some pads)
 * Field from GpioTermDefault to GpioTermNative can be OR'ed with
 * GpioTolerance1v8.
 */
typedef enum {
	GpioTermDefault	= 0x0,	/* Leave termination setting unmodified */
	GpioTermNone	= 0x1,	/* none */
	GpioTermWpd5K	= 0x5,	/* 5kOhm weak pull-down */
	GpioTermWpd20K	= 0x9,	/* 20kOhm weak pull-down */
	GpioTermWpu1K	= 0x13,	/* 1kOhm weak pull-up */
	GpioTermWpu2K	= 0x17,	/* 2kOhm weak pull-up */
	GpioTermWpu5K	= 0x15,	/* 5kOhm weak pull-up */
	GpioTermWpu20K	= 0x19,	/* 20kOhm weak pull-up */
	GpioTermWpu1K2K	= 0x1B,	/* 1kOhm & 2kOhm weak pull-up */
	GpioTermNative	= 0x1F,	/* Native function for pads termination */
	GpioNoTolerance1v8	= (0x1 << 5),	/* Disable 1.8V pad tolerance */
	GpioTolerance1v8	= (0x3 << 5)	/* Enable 1.8V pad tolerance */
} GPIO_ELECTRICAL_CONFIG;

/*
 * GPIO LockConfiguration
 * Set GPIO configuration lock and output state lock
 * GpioLockPadConfig and GpioLockOutputState can be OR'ed
 */
typedef enum {
	GpioLockDefault		= 0x0,	/* Leave lock setting unmodified */
	GpioPadConfigLock	= 0x3,	/* Lock Pad Configuration */
	GpioOutputStateLock	= 0x5	/* Lock GPIO pad output value */
} GPIO_LOCK_CONFIG;

/*
 * Other GPIO Configuration GPIO_OTHER_CONFIG is used for less often
 * settings and for future extensions Supported settings:
 * - RX raw override to '1' - allows to override input value to '1'
 * This is applicable only if in input mode (both in GPIO and native usage)
 * The override takes place at the internal pad state directly from buffer
 * and before the RXINV.
 */
typedef enum {
	GpioRxRaw1Default	= 0x0,	/* Use default input override value */
	GpioRxRaw1Dis	= 0x1,	/* Don't override input */
	GpioRxRaw1En	= 0x3	/* Override input to '1' */
} GPIO_OTHER_CONFIG;

/*
 * LP GPIO pins: Use below for functions from PCH GPIO Lib which
 * require GpioPad as argument. Encoding used here
 * has all information required by library functions
 */
#define GPIO_LP_GPP_A0		0x02000000
#define GPIO_LP_GPP_A1		0x02000001
#define GPIO_LP_GPP_A2		0x02000002
#define GPIO_LP_GPP_A3		0x02000003
#define GPIO_LP_GPP_A4		0x02000004
#define GPIO_LP_GPP_A5		0x02000005
#define GPIO_LP_GPP_A6		0x02000006
#define GPIO_LP_GPP_A7		0x02000007
#define GPIO_LP_GPP_A8		0x02000008
#define GPIO_LP_GPP_A9		0x02000009
#define GPIO_LP_GPP_A10		0x0200000A
#define GPIO_LP_GPP_A11		0x0200000B
#define GPIO_LP_GPP_A12		0x0200000C
#define GPIO_LP_GPP_A13		0x0200000D
#define GPIO_LP_GPP_A14		0x0200000E
#define GPIO_LP_GPP_A15		0x0200000F
#define GPIO_LP_GPP_A16		0x02000010
#define GPIO_LP_GPP_A17		0x02000011
#define GPIO_LP_GPP_A18		0x02000012
#define GPIO_LP_GPP_A19		0x02000013
#define GPIO_LP_GPP_A20		0x02000014
#define GPIO_LP_GPP_A21		0x02000015
#define GPIO_LP_GPP_A22		0x02000016
#define GPIO_LP_GPP_A23		0x02000017
#define GPIO_LP_GPP_B0		0x02010000
#define GPIO_LP_GPP_B1		0x02010001
#define GPIO_LP_GPP_B2		0x02010002
#define GPIO_LP_GPP_B3		0x02010003
#define GPIO_LP_GPP_B4		0x02010004
#define GPIO_LP_GPP_B5		0x02010005
#define GPIO_LP_GPP_B6		0x02010006
#define GPIO_LP_GPP_B7		0x02010007
#define GPIO_LP_GPP_B8		0x02010008
#define GPIO_LP_GPP_B9		0x02010009
#define GPIO_LP_GPP_B10		0x0201000A
#define GPIO_LP_GPP_B11		0x0201000B
#define GPIO_LP_GPP_B12		0x0201000C
#define GPIO_LP_GPP_B13		0x0201000D
#define GPIO_LP_GPP_B14		0x0201000E
#define GPIO_LP_GPP_B15		0x0201000F
#define GPIO_LP_GPP_B16		0x02010010
#define GPIO_LP_GPP_B17		0x02010011
#define GPIO_LP_GPP_B18		0x02010012
#define GPIO_LP_GPP_B19		0x02010013
#define GPIO_LP_GPP_B20		0x02010014
#define GPIO_LP_GPP_B21		0x02010015
#define GPIO_LP_GPP_B22		0x02010016
#define GPIO_LP_GPP_B23		0x02010017
#define GPIO_LP_GPP_C0		0x02020000
#define GPIO_LP_GPP_C1		0x02020001
#define GPIO_LP_GPP_C2		0x02020002
#define GPIO_LP_GPP_C3		0x02020003
#define GPIO_LP_GPP_C4		0x02020004
#define GPIO_LP_GPP_C5		0x02020005
#define GPIO_LP_GPP_C6		0x02020006
#define GPIO_LP_GPP_C7		0x02020007
#define GPIO_LP_GPP_C8		0x02020008
#define GPIO_LP_GPP_C9		0x02020009
#define GPIO_LP_GPP_C10		0x0202000A
#define GPIO_LP_GPP_C11		0x0202000B
#define GPIO_LP_GPP_C12		0x0202000C
#define GPIO_LP_GPP_C13		0x0202000D
#define GPIO_LP_GPP_C14		0x0202000E
#define GPIO_LP_GPP_C15		0x0202000F
#define GPIO_LP_GPP_C16		0x02020010
#define GPIO_LP_GPP_C17		0x02020011
#define GPIO_LP_GPP_C18		0x02020012
#define GPIO_LP_GPP_C19		0x02020013
#define GPIO_LP_GPP_C20		0x02020014
#define GPIO_LP_GPP_C21		0x02020015
#define GPIO_LP_GPP_C22		0x02020016
#define GPIO_LP_GPP_C23		0x02020017
#define GPIO_LP_GPP_D0		0x02030000
#define GPIO_LP_GPP_D1		0x02030001
#define GPIO_LP_GPP_D2		0x02030002
#define GPIO_LP_GPP_D3		0x02030003
#define GPIO_LP_GPP_D4		0x02030004
#define GPIO_LP_GPP_D5		0x02030005
#define GPIO_LP_GPP_D6		0x02030006
#define GPIO_LP_GPP_D7		0x02030007
#define GPIO_LP_GPP_D8		0x02030008
#define GPIO_LP_GPP_D9		0x02030009
#define GPIO_LP_GPP_D10		0x0203000A
#define GPIO_LP_GPP_D11		0x0203000B
#define GPIO_LP_GPP_D12		0x0203000C
#define GPIO_LP_GPP_D13		0x0203000D
#define GPIO_LP_GPP_D14		0x0203000E
#define GPIO_LP_GPP_D15		0x0203000F
#define GPIO_LP_GPP_D16		0x02030010
#define GPIO_LP_GPP_D17		0x02030011
#define GPIO_LP_GPP_D18		0x02030012
#define GPIO_LP_GPP_D19		0x02030013
#define GPIO_LP_GPP_D20		0x02030014
#define GPIO_LP_GPP_D21		0x02030015
#define GPIO_LP_GPP_D22		0x02030016
#define GPIO_LP_GPP_D23		0x02030017
#define GPIO_LP_GPP_E0		0x02040000
#define GPIO_LP_GPP_E1		0x02040001
#define GPIO_LP_GPP_E2		0x02040002
#define GPIO_LP_GPP_E3		0x02040003
#define GPIO_LP_GPP_E4		0x02040004
#define GPIO_LP_GPP_E5		0x02040005
#define GPIO_LP_GPP_E6		0x02040006
#define GPIO_LP_GPP_E7		0x02040007
#define GPIO_LP_GPP_E8		0x02040008
#define GPIO_LP_GPP_E9		0x02040009
#define GPIO_LP_GPP_E10		0x0204000A
#define GPIO_LP_GPP_E11		0x0204000B
#define GPIO_LP_GPP_E12		0x0204000C
#define GPIO_LP_GPP_E13		0x0204000D
#define GPIO_LP_GPP_E14		0x0204000E
#define GPIO_LP_GPP_E15		0x0204000F
#define GPIO_LP_GPP_E16		0x02040010
#define GPIO_LP_GPP_E17		0x02040011
#define GPIO_LP_GPP_E18		0x02040012
#define GPIO_LP_GPP_E19		0x02040013
#define GPIO_LP_GPP_E20		0x02040014
#define GPIO_LP_GPP_E21		0x02040015
#define GPIO_LP_GPP_E22		0x02040016
#define GPIO_LP_GPP_E23		0x02040017
#define GPIO_LP_GPP_F0		0x02050000
#define GPIO_LP_GPP_F1		0x02050001
#define GPIO_LP_GPP_F2		0x02050002
#define GPIO_LP_GPP_F3		0x02050003
#define GPIO_LP_GPP_F4		0x02050004
#define GPIO_LP_GPP_F5		0x02050005
#define GPIO_LP_GPP_F6		0x02050006
#define GPIO_LP_GPP_F7		0x02050007
#define GPIO_LP_GPP_F8		0x02050008
#define GPIO_LP_GPP_F9		0x02050009
#define GPIO_LP_GPP_F10		0x0205000A
#define GPIO_LP_GPP_F11		0x0205000B
#define GPIO_LP_GPP_F12		0x0205000C
#define GPIO_LP_GPP_F13		0x0205000D
#define GPIO_LP_GPP_F14		0x0205000E
#define GPIO_LP_GPP_F15		0x0205000F
#define GPIO_LP_GPP_F16		0x02050010
#define GPIO_LP_GPP_F17		0x02050011
#define GPIO_LP_GPP_F18		0x02050012
#define GPIO_LP_GPP_F19		0x02050013
#define GPIO_LP_GPP_F20		0x02050014
#define GPIO_LP_GPP_F21		0x02050015
#define GPIO_LP_GPP_F22		0x02050016
#define GPIO_LP_GPP_F23		0x02050017
#define GPIO_LP_GPP_G0		0x02060000
#define GPIO_LP_GPP_G1		0x02060001
#define GPIO_LP_GPP_G2		0x02060002
#define GPIO_LP_GPP_G3		0x02060003
#define GPIO_LP_GPP_G4		0x02060004
#define GPIO_LP_GPP_G5		0x02060005
#define GPIO_LP_GPP_G6		0x02060006
#define GPIO_LP_GPP_G7		0x02060007
#define GPIO_LP_GPD0		0x02070000
#define GPIO_LP_GPD1		0x02070001
#define GPIO_LP_GPD2		0x02070002
#define GPIO_LP_GPD3		0x02070003
#define GPIO_LP_GPD4		0x02070004
#define GPIO_LP_GPD5		0x02070005
#define GPIO_LP_GPD6		0x02070006
#define GPIO_LP_GPD7		0x02070007
#define GPIO_LP_GPD8		0x02070008
#define GPIO_LP_GPD9		0x02070009
#define GPIO_LP_GPD10		0x0207000A
#define GPIO_LP_GPD11		0x0207000B

#define END_OF_GPIO_TABLE	0xFFFFFFFF
#endif
