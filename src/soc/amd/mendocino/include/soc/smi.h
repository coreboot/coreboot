/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_MENDOCINO_SMI_H
#define AMD_MENDOCINO_SMI_H

#include <types.h>

#define SMI_GEVENTS			24
#define SCIMAPS				64 /* 0..63 */
#define SCI_GPES			32
#define NUMBER_SMITYPES			160

#define SMI_EVENT_STATUS		0x0
#define SMI_EVENT_ENABLE		0x04
#define SMI_SCI_TRIG			0x08
#define SMI_SCI_LEVEL			0x0c
#define SMI_SCI_STATUS			0x10
#define SMI_SCI_EN			0x14
#define SMI_SCI_MAP0			0x40
# define SMI_SCI_MAP(X)			(SMI_SCI_MAP0 + (X))

/* SMI source and status */
#define SMITYPE_G_GENINT1_L		0
#define SMITYPE_G_GENINT2_L		1
#define SMITYPE_G_AGPIO3		2
#define SMITYPE_G_ESPI_ALERT_L		3
#define SMITYPE_G_AGPIO4		4
#define SMITYPE_G_BLINK			5
#define SMITYPE_G_SPKR			6
#define SMITYPE_G_AGPIO5		7
#define SMITYPE_G_WAKE_L		8
#define SMITYPE_G_SPI_TPM_CS_L		9
#define SMITYPE_G_AGPIO6		10
#define SMITYPE_G_AGPIO7		11
#define SMITYPE_G_USBOC0_L		12
#define SMITYPE_G_USBOC1_L		13
#define SMITYPE_G_USBOC2_L		14
#define SMITYPE_G_USBOC3_L		15
#define SMITYPE_G_AGPIO23		16
#define SMITYPE_G_AGPIO32		17
#define SMITYPE_G_FANIN0		18
#define SMITYPE_G_SYSRESET_L		19
#define SMITYPE_G_AGPIO40		20
#define SMITYPE_G_PWR_BTN_L		21
#define SMITYPE_G_AGPIO9		22
#define SMITYPE_G_AGPIO8		23
#define GEVENT_MASK ((1 << SMITYPE_G_GENINT1_L)		\
		   | (1 << SMITYPE_G_GENINT2_L)		\
		   | (1 << SMITYPE_G_AGPIO3)		\
		   | (1 << SMITYPE_G_ESPI_ALERT_L)	\
		   | (1 << SMITYPE_G_AGPIO4)		\
		   | (1 << SMITYPE_G_BLINK)		\
		   | (1 << SMITYPE_G_SPKR)		\
		   | (1 << SMITYPE_G_AGPIO5)		\
		   | (1 << SMITYPE_G_WAKE_L)		\
		   | (1 << SMITYPE_G_SPI_TPM_CS_L)	\
		   | (1 << SMITYPE_G_AGPIO6)		\
		   | (1 << SMITYPE_G_AGPIO7)		\
		   | (1 << SMITYPE_G_USBOC0_L)		\
		   | (1 << SMITYPE_G_USBOC1_L)		\
		   | (1 << SMITYPE_G_USBOC2_L)		\
		   | (1 << SMITYPE_G_USBOC3_L)		\
		   | (1 << SMITYPE_G_AGPIO23)		\
		   | (1 << SMITYPE_G_AGPIO32)		\
		   | (1 << SMITYPE_G_FANIN0)		\
		   | (1 << SMITYPE_G_SYSRESET_L)	\
		   | (1 << SMITYPE_G_AGPIO40)		\
		   | (1 << SMITYPE_G_PWR_BTN_L)		\
		   | (1 << SMITYPE_G_AGPIO9)		\
		   | (1 << SMITYPE_G_AGPIO8))
#define SMITYPE_MP2_WAKE		24
#define SMITYPE_MP2_GPIO0		25
#define SMITYPE_ESPI_SYS		26
#define SMITYPE_ESPI_WAKE_PME		27
#define SMITYPE_MP2_GPIO1		28
#define SMITYPE_GPP_PME			29
#define SMITYPE_NB_GPP_HOT_PLUG		30
/* 31 Reserved */
#define SMITYPE_WAKE_L2			32
#define SMITYPE_PSP			33
/* 34,35 Reserved */
#define SMITYPE_ESPI_SCI_B		36
#define SMITYPE_CIO_FCH_PME_S5_0	37
#define SMITYPE_CIO_FCH_PME_S5_1	38
#define SMITYPE_AZPME			39
#define SMITYPE_USB_PD_I2C4		40
#define SMITYPE_GPIO_CTL		41
#define SMITYPE_XHC2_PME		42
#define SMITYPE_ALT_HPET_ALARM		43
#define SMITYPE_FAN_THERMAL		44
#define SMITYPE_ASF_MASTER_SLAVE	45
#define SMITYPE_I2S_WAKE		46
#define SMITYPE_SMBUS0_MASTER		47
#define SMITYPE_TWARN			48
#define SMITYPE_TRAFFIC_MON		49
#define SMITYPE_ILLB			50
#define SMITYPE_PWRBUTTON_UP		51
#define SMITYPE_PROCHOT			52
#define SMITYPE_APU_HW			53
#define SMITYPE_NB_SCI			54
#define SMITYPE_RAS_SERR		55
#define SMITYPE_XHC0_PME		56
#define SMITYPE_XHC1_PME		57
#define SMITYPE_ACDC_TIMER		58
/* 59-60 Reserved */
#define SMITYPE_XHC3_PME		61
#define SMITYPE_XHC4_PME		62
#define SMITYPE_CUR_TEMP_STATUS_5	63
#define SMITYPE_KB_RESET		64
#define SMITYPE_SLP_TYP			65
#define SMITYPE_AL2H_ACPI		66
/* 67 Reserved */
#define SMITYPE_NB_GPP_PME_PULSE	68
#define SMITYPE_NB_GPP_HP_PULSE		69
#define SMITYPE_USB_PD_I2C4_INTR2	70
/* 71 Reserved */
#define SMITYPE_GBL_RLS			72
#define SMITYPE_BIOS_RLS		73
#define SMITYPE_PWRBUTTON_DOWN		74
#define SMITYPE_SMI_CMD_PORT		75
#define SMITYPE_USB_SMI			76
#define SMITYPE_SERIRQ			77
#define SMITYPE_SMBUS0_INTR		78
/* 79-80 Reserved */
#define SMITYPE_INTRUDER		81
#define SMITYPE_VBAT_LOW		82
#define SMITYPE_PROTHOT			83
#define SMITYPE_PCI_SERR		84
/* 85-89 Reserved */
#define SMITYPE_EMUL60_64		90
/* 91-132 Reserved */
#define SMITYPE_FANIN0			133
/* 134-140 Reserved */
#define SMITYPE_CF9_WRITE		141
#define SMITYPE_SHORT_TIMER		142
#define SMITYPE_LONG_TIMER		143
#define SMITYPE_AB_SMI			144
#define SMITYPE_ANY_RESET		145
#define SMITYPE_ESPI_SMI		146
/* 147 Reserved */
#define SMITYPE_IOTRAP0			148
#define SMITYPE_IOTRAP1			149
#define SMITYPE_IOTRAP2			150
#define SMITYPE_IOTRAP3			151
#define SMITYPE_MEMTRAP0		152
/* 153-155 Reserved */
#define SMITYPE_CFGTRAP0		156
/* 157-159 Reserved */

#define TYPE_TO_MASK(X)				(1 << (X) % 32)

#define SMI_REG_SMISTS0			0x80
#define SMI_REG_SMISTS1			0x84
#define SMI_REG_SMISTS2			0x88
#define SMI_REG_SMISTS3			0x8c
#define SMI_REG_SMISTS4			0x90

#define SMI_REG_POINTER			0x94
# define SMI_STATUS_SRC_SCI			(1 << 0)
# define SMI_STATUS_SRC_0			(1 << 1) /* SMIx80 */
# define SMI_STATUS_SRC_1			(1 << 2) /* SMIx84... */
# define SMI_STATUS_SRC_2			(1 << 3)
# define SMI_STATUS_SRC_3			(1 << 4)
# define SMI_STATUS_SRC_4			(1 << 5)

#define SMI_TIMER			0x96
#define SMI_TIMER_MASK				0x7fff
#define SMI_TIMER_EN				(1 << 15)

#define SMI_REG_SMITRIG0		0x98
# define SMITRIG0_PSP				(1 << 25)
# define SMITRG0_EOS				(1 << 28)
# define SMI_TIMER_SEL				(1 << 29)
# define SMITRG0_SMIENB				(1 << 31)

#define SMI_REG_CONTROL0		0xa0
#define SMI_REG_CONTROL1		0xa4
#define SMI_REG_CONTROL2		0xa8
#define SMI_REG_CONTROL3		0xac
#define SMI_REG_CONTROL4		0xb0
#define SMI_REG_CONTROL5		0xb4
#define SMI_REG_CONTROL6		0xb8
#define SMI_REG_CONTROL7		0xbc
#define SMI_REG_CONTROL8		0xc0
#define SMI_REG_CONTROL9		0xc4

#define SMI_MODE_MASK			0x03

#endif /* AMD_MENDOCINO_SMI_H */
