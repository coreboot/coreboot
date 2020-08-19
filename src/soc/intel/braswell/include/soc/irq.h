/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_IRQ_H_
#define _SOC_IRQ_H_

#define PIRQA_APIC_IRQ			16
#define PIRQB_APIC_IRQ			17
#define PIRQC_APIC_IRQ			18
#define PIRQD_APIC_IRQ			19
#define PIRQE_APIC_IRQ			20
#define PIRQF_APIC_IRQ			21
#define PIRQG_APIC_IRQ			22
#define PIRQH_APIC_IRQ			23

/* The below IRQs are for when devices are in ACPI mode. Active low. */
#define LPE_DMA0_IRQ			24
#define LPE_DMA1_IRQ			25
#define LPE_SSP0_IRQ			26
#define LPE_SSP1_IRQ			27
#define LPE_SSP2_IRQ			28
#define LPE_IPC2HOST_IRQ		29
#define LPSS_I2C1_IRQ			32
#define LPSS_I2C2_IRQ			33
#define LPSS_I2C3_IRQ			34
#define LPSS_I2C4_IRQ			35
#define LPSS_I2C5_IRQ			36
#define LPSS_I2C6_IRQ			37
#define LPSS_I2C7_IRQ			38
#define LPSS_HSUART1_IRQ		39
#define LPSS_HSUART2_IRQ		40
#define LPSS_SPI_IRQ			41
#define LPSS_DMA1_IRQ			42
#define LPSS_DMA2_IRQ			43
#define SCC_EMMC_IRQ			45
#define SCC_SDIO_IRQ			46
#define SCC_SD_IRQ				47

#define GPIO_N_IRQ				48
#define GPIO_SW_IRQ				49
#define GPIO_E_IRQ				50

/* GPIO direct / dedicated IRQs. */

/* NORTH COMMUNITY */
#define GPIO_N_DED_IRQ_0		51
#define GPIO_N_DED_IRQ_1		52
#define GPIO_N_DED_IRQ_2		53
#define GPIO_N_DED_IRQ_3		54
#define GPIO_N_DED_IRQ_4		55
#define GPIO_N_DED_IRQ_5		56
#define GPIO_N_DED_IRQ_6		57
#define GPIO_N_DED_IRQ_7		58

/* SOUTH WEST COMMUNITY */
#define GPIO_SW_DED_IRQ_0		59
#define GPIO_SW_DED_IRQ_1		60
#define GPIO_SW_DED_IRQ_2		61
#define GPIO_SW_DED_IRQ_3		62
#define GPIO_SW_DED_IRQ_4		63
#define GPIO_SW_DED_IRQ_5		64
#define GPIO_SW_DED_IRQ_6		65
#define GPIO_SW_DED_IRQ_7		66

/* EAST COMMUNITY */
#define GPIO_E_DED_IRQ_0		67
#define GPIO_E_DED_IRQ_1		68
#define GPIO_E_DED_IRQ_2		69
#define GPIO_E_DED_IRQ_3		70
#define GPIO_E_DED_IRQ_4		71
#define GPIO_E_DED_IRQ_5		72
#define GPIO_E_DED_IRQ_6		73
#define GPIO_E_DED_IRQ_7		74
#define GPIO_E_DED_IRQ_8		75
#define GPIO_E_DED_IRQ_9		76
#define GPIO_E_DED_IRQ_10		77
#define GPIO_E_DED_IRQ_11		78
#define GPIO_E_DED_IRQ_12		79
#define GPIO_E_DED_IRQ_13		80
#define GPIO_E_DED_IRQ_14		81
#define GPIO_E_DED_IRQ_15		82

/* More IRQ */
#define LPSS_SPI2_IRQ			89
#define LPSS_SPI3_IRQ			90
#define GPIO_SE_IRQ			91

/* GPIO direct / dedicated IRQs. */
/* SOUTH EAST COMMUNITY */
#define GPIO_SE_DED_IRQ_0		92
#define GPIO_SE_DED_IRQ_1		93
#define GPIO_SE_DED_IRQ_2		94
#define GPIO_SE_DED_IRQ_3		95
#define GPIO_SE_DED_IRQ_4		96
#define GPIO_SE_DED_IRQ_5		97
#define GPIO_SE_DED_IRQ_6		98
#define GPIO_SE_DED_IRQ_7		99
#define GPIO_SE_DED_IRQ_8		100
#define GPIO_SE_DED_IRQ_9		101
#define GPIO_SE_DED_IRQ_10		102
#define GPIO_SE_DED_IRQ_11		103
#define GPIO_SE_DED_IRQ_12		104
#define GPIO_SE_DED_IRQ_13		105
#define GPIO_SE_DED_IRQ_14		106
#define GPIO_SE_DED_IRQ_15		107

/* OTHER IRQs */
#define GPIO_VIRTUAL			108
#define LPE_DMA2			109
#define LPE_SSP3			110
#define LPE_SSP4			111
#define LPE_SSP5			112

/* DIRQs - Two levels of expansion to evaluate to numeric constants for ASL. */
#define _GPIO_N_DED_IRQ(slot)		GPIO_N_DED_IRQ_##slot
#define _GPIO_SW_DED_IRQ(slot)		GPIO_SW_DED_IRQ_##slot
#define _GPIO_E_DED_IRQ(slot)		GPIO_E_DED_IRQ_##slot
#define _GPIO_SE_DED_IRQ(slot)		GPIO_SE_DED_IRQ_##slot
#define GPIO_N_DED_IRQ(slot)		_GPIO_N_DED_IRQ(slot)
#define GPIO_SW_DED_IRQ(slot)		_GPIO_SW_DED_IRQ(slot)
#define GPIO_E_DED_IRQ(slot)		_GPIO_E_DED_IRQ(slot)
#define GPIO_SE_DED_IRQ(slot)		_GPIO_SE_DED_IRQ(slot)

/* TODO NEED TO UPDATE THESE IN onboard.h */
#define _GPIO_S0_DED_IRQ(slot)		GPIO_N_DED_IRQ_##slot
#define _GPIO_S5_DED_IRQ(slot)		GPIO_SE_DED_IRQ_##slot
#define GPIO_S0_DED_IRQ(slot)		_GPIO_N_DED_IRQ(slot)
#define GPIO_S5_DED_IRQ(slot)		_GPIO_E_DED_IRQ(slot)

/* PIC IRQ settings. */
#define PIRQ_PIC_IRQDISABLE		0x80
#define PIRQ_PIC_IRQ3			0x3
#define PIRQ_PIC_IRQ4			0x4
#define PIRQ_PIC_IRQ5			0x5
#define PIRQ_PIC_IRQ6			0x6
#define PIRQ_PIC_IRQ7			0x7
#define PIRQ_PIC_IRQ9			0x9
#define PIRQ_PIC_IRQ10			0xa
#define PIRQ_PIC_IRQ11			0xb
#define PIRQ_PIC_IRQ12			0xc
#define PIRQ_PIC_IRQ14			0xe
#define PIRQ_PIC_IRQ15			0xf
#define PIRQ_PIC_UNKNOWN_UNUSED		0xff

/* Overloaded term, but these values determine the per device route. */
#define PIRQA				0
#define PIRQB				1
#define PIRQC				2
#define PIRQD				3
#define PIRQE				4
#define PIRQF				5
#define PIRQG				6
#define PIRQH				7

/* These registers live behind the ILB_BASE_ADDRESS */
#define ACTL				0x00
# define SCIS_MASK				0x07
# define SCIS_IRQ9				0x00
# define SCIS_IRQ10				0x01
# define SCIS_IRQ11				0x02
# define SCIS_IRQ20				0x04
# define SCIS_IRQ21				0x05
# define SCIS_IRQ22				0x06
# define SCIS_IRQ23				0x07

/*
 * In each mainboard directory there should exist a header file irqroute.h that
 * defines the PCI_DEV_PIRQ_ROUTES and PIRQ_PIC_ROUTES macros which
 * consist of PCI_DEV_PIRQ_ROUTE and PIRQ_PIC entries.
 */

#if !defined(__ASSEMBLER__) && !defined(__ACPI__)
#include <stdint.h>

#define NUM_IR_DEVS 32
#define NUM_PIRQS   8

struct soc_irq_route {
	/* Per device configuration. */
	uint16_t pcidev[NUM_IR_DEVS];
	/* Route path for each internal PIRQx in PIC mode. */
	uint8_t  pic[NUM_PIRQS];
};

extern const struct soc_irq_route global_soc_irq_route;

#define DEFINE_IRQ_ROUTES \
	const struct soc_irq_route global_soc_irq_route = { \
		.pcidev = { PCI_DEV_PIRQ_ROUTES, }, \
		.pic = { PIRQ_PIC_ROUTES, }, \
	}

/* The following macros are used for ACPI by the ASL compiler */
#define PCI_DEV_PIRQ_ROUTE(dev_, a_, b_, c_, d_) \
	[dev_] = (((PIRQ ## d_) << 12) | ((PIRQ ## c_) << 8) | \
		   ((PIRQ ## b_) <<  4) | ((PIRQ ## a_) << 0))

#define PIRQ_PIC(pirq_, pic_irq_) \
	[PIRQ ## pirq_] = PIRQ_PIC_IRQ ## pic_irq_

#endif /* !defined(__ASSEMBLER__) && !defined(__ACPI__) */

#endif /* _SOC_IRQ_H_ */
