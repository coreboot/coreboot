/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_PMC_H_
#define _DENVERTON_NS_PMC_H_

/* Memory mapped IO registers behind PMC_BASE_ADDRESS */
#define PMC_ACPI_BASE 0x40 /* IO BAR */
#define MASK_PMC_ACPI_BASE 0xfffc
#define PMC_ACPI_CNT 0x44
#define PMC_ACPI_CNT_PWRM_EN (1 << 8)			   /* PWRM enable */
#define PMC_ACPI_CNT_ACPI_EN (1 << 7)			   /* ACPI enable */
#define PMC_ACPI_CNT_SCIS ((1 << 2) | (1 << 1) | (1 << 0)) /* SCI IRQ select \
							      */
#define PMC_ACPI_CNT_SCIS_MASK 0x07
#define PMC_ACPI_CNT_SCIS_IRQ9 0x00
#define PMC_ACPI_CNT_SCIS_IRQ10 0x01
#define PMC_ACPI_CNT_SCIS_IRQ11 0x02
#define PMC_ACPI_CNT_SCIS_DISABLE 0x03
#define PMC_ACPI_CNT_SCIS_IRQ20 0x04
#define PMC_ACPI_CNT_SCIS_IRQ21 0x05
#define PMC_ACPI_CNT_SCIS_IRQ22 0x06
#define PMC_ACPI_CNT_SCIS_IRQ23 0x07

#define SCI_IRQ_ADJUST		0
#define SCI_IRQ_SEL		(0x07 << SCI_IRQ_ADJUST)
#define SCIS_IRQ9		0x00
#define SCIS_IRQ10		0x01
#define SCIS_IRQ11		0x02
#define SCIS_IRQ20		0x04
#define SCIS_IRQ21		0x05
#define SCIS_IRQ22		0x06
#define SCIS_IRQ23		0x07

#define PMC_PWRM_BASE   0x48 /* MEM BAR */
#define   MASK_PMC_PWRM_BASE    0xfffff000 /* 4K alignment */
#define GEN_PMCON_A               0xA0
#define   MS4V                    (1 << 18)
#define   GBL_RST_STS             (1 << 16)
#define GEN_PMCON_B               0xA4
#define   GEN_PMCON_B_SUS_PWR_FLR   0x4000
#define   SUS_PWR_FLR               GEN_PMCON_B_SUS_PWR_FLR
#define   GEN_PMCON_B_RTC_PWR_STS   0x04
#define   GEN_PMCON_B_PWR_FLR       0x02
#define   PWR_FLR                   GEN_PMCON_B_PWR_FLR
#define   GEN_PMCON_B_AFTERG3_EN    0x00
#define ETR3                      0xAC
#define   ETR3_CF9LOCK              BIT31 ///< CF9h Lockdown
#define   ETR3_CF9GR                BIT20 ///< CF9h Global Reset

/* IO Mapped registers behind ACPI_BASE_ADDRESS */
#define PM1_STS 0x00
#define WAK_STS (1 << 15)
#define PCIEXPWAK_STS (1 << 14)
#define PRBTNOR_STS (1 << 11)
#define RTC_STS (1 << 10)
#define PWRBTN_STS (1 << 8)
#define GBL_STS (1 << 5)
#define BM_STS (1 << 4)
#define TMROF_STS (1 << 0)
#define PM1_EN 0x02
#define PCIEXPWAK_DIS (1 << 14)
#define RTC_EN (1 << 10)
#define PWRBTN_EN (1 << 8)
#define GBL_EN (1 << 5)
#define TMROF_EN (1 << 0)
#define PM1_CNT 0x04
#define SLP_EN (1 << 13)
#define SLP_TYP_SHIFT 10
#define SLP_TYP (7 << SLP_TYP_SHIFT)
#define SLP_TYP_S0 0
#define SLP_TYP_S1 1
#define SLP_TYP_S3 5
#define SLP_TYP_S4 6
#define SLP_TYP_S5 7
#define GBL_RLS (1 << 2)
#define BM_RLD (1 << 1)
#define SCI_EN (1 << 0)
#define PM1_TMR 0x08
#define SMI_EN 0x30
#define LEGACY_USB3_EN (1 << 31) // Legacy USB3 SMI logic
#define GPIO_UNLOCK_EN (1 << 27) // GPIO unlock SMI
#define INTEL_USB2_EN (1 << 18)  // Intel-Specific USB2 SMI logic
#define LEGACY_USB2_EN (1 << 17) // Legacy USB2 SMI logic
#define PERIODIC_EN (1 << 14)    // SMI on PERIODIC_STS in SMI_STS
#define TCO_EN (1 << 13)	 // Enable TCO Logic (BIOSWE et al)
#define MCSMI_EN (1 << 11)       // Trap microcontroller range access
#define BIOS_RLS (1 << 7)	// asserts SCI on bit set
#define SWSMI_TMR_EN (1 << 6)    // start software smi timer on bit set
#define APMC_EN (1 << 5)	 // Writes to APM_CNT cause SMI#
#define SLP_SMI_EN (1 << 4)      // Write to SLP_EN in PM1_CNT asserts SMI#
#define LEGACY_USB_EN (1 << 3)   // Legacy USB circuit SMI logic
#define BIOS_EN (1 << 2)	 // Assert SMI# on setting GBL_RLS bit
#define EOS (1 << 1)		 // End of SMI (deassert SMI#)
#define GBL_SMI_EN (1 << 0)      // SMI# generation at all?
#define SMI_STS 0x34
#define SMI_STS_LEGACY_USB3 (1 << 31)
#define SMI_STS_GPIO_UNLOCK (1 << 27)
#define SMI_STS_SPI (1 << 26)
#define SMI_STS_MONITOR (1 << 21)
#define SMI_STS_PCI_EXP (1 << 20)
#define SMI_STS_PATCH (1 << 19)
#define SMI_STS_INTEL_USB2 (1 << 18)
#define SMI_STS_LEGACY_USB2 (1 << 17)
#define SMI_STS_SMBUS (1 << 16)
#define SMI_STS_SERIRQ (1 << 15)
#define SMI_STS_PERIODIC (1 << 14)
#define SMI_STS_TCO (1 << 13)
#define SMI_STS_DEVMON (1 << 12)
#define SMI_STS_MCSMI (1 << 11)
#define SMI_STS_GPE1 (1 << 10)
#define SMI_STS_GPE0 (1 << 9)
#define SMI_STS_PM1 (1 << 8)
#define SMI_STS_SWSMI_TMR (1 << 6)
#define SMI_STS_APMC (1 << 5)
#define SMI_STS_SLP_SMI (1 << 4)
#define SMI_STS_LEGACY_USB (1 << 3)
#define SMI_STS_BIOS (1 << 2)
#define GPE_CTRL 0x40
#define SWGPE_CTRL	(1 << 17)
#define PM2_CNT 0x50
#define GPE0_REG_MAX		4
#define GPE0_REG_SIZE		32
#define GPE0_STS(x) (0x80 + (x * 4))
#define  GPE_STD 0
#define GPIO31_STS (1 << 31)
#define GPIO30_STS (1 << 30)
#define GPIO29_STS (1 << 29)
#define GPIO28_STS (1 << 28)
#define GPIO27_STS (1 << 27)
#define GPIO26_STS (1 << 26)
#define GPIO25_STS (1 << 25)
#define GPIO24_STS (1 << 24)
#define GPIO23_STS (1 << 23)
#define GPIO22_STS (1 << 22)
#define GPIO21_STS (1 << 21)
#define GPIO20_STS (1 << 20)
#define GPIO19_STS (1 << 19)
#define GPIO18_STS (1 << 18)
#define GPIO17_STS (1 << 17)
#define GPIO16_STS (1 << 16)
#define GPIO15_STS (1 << 15)
#define GPIO14_STS (1 << 14)
#define GPIO13_STS (1 << 13)
#define GPIO12_STS (1 << 12)
#define GPIO11_STS (1 << 11)
#define GPIO10_STS (1 << 10)
#define GPIO09_STS (1 << 09)
#define GPIO08_STS (1 << 08)
#define GPIO07_STS (1 << 07)
#define GPIO06_STS (1 << 06)
#define GPIO05_STS (1 << 05)
#define GPIO04_STS (1 << 04)
#define GPIO03_STS (1 << 03)
#define GPIO02_STS (1 << 02)
#define GPIO01_STS (1 << 01)
#define GPIO00_STS (1 << 00)
#define GPE0_STS_63_32 0x84
#define GPE0_STS_95_64 0x88
#define GPE0_STS_127_96 0x8c
#define PME_B0_STS (1 << 13)
#define ME_SCI_STS (1 << 12)
#define PME_STS (1 << 11)
#define PCI_EXP_STS (1 << 9)
#define RI_STS (1 << 8)
#define SMB_WAK_STS (1 << 7)
#define TCOSCI_STS (1 << 6)
#define IE_SCI_STS (1 << 3)
#define SWGPE_STS (1 << 2)
#define HOT_PLUG_STS (1 << 1)
#define GPE0_EN(x) (0x90 + (x * 4))
#define GPIO31_EN (1 << 31)
#define GPIO30_EN (1 << 30)
#define GPIO29_EN (1 << 29)
#define GPIO28_EN (1 << 28)
#define GPIO27_EN (1 << 27)
#define GPIO26_EN (1 << 26)
#define GPIO25_EN (1 << 25)
#define GPIO24_EN (1 << 24)
#define GPIO23_EN (1 << 23)
#define GPIO22_EN (1 << 22)
#define GPIO21_EN (1 << 21)
#define GPIO20_EN (1 << 20)
#define GPIO19_EN (1 << 19)
#define GPIO18_EN (1 << 18)
#define GPIO17_EN (1 << 17)
#define GPIO16_EN (1 << 16)
#define GPIO15_EN (1 << 15)
#define GPIO14_EN (1 << 14)
#define GPIO13_EN (1 << 13)
#define GPIO12_EN (1 << 12)
#define GPIO11_EN (1 << 11)
#define GPIO10_EN (1 << 10)
#define GPIO09_EN (1 << 09)
#define GPIO08_EN (1 << 08)
#define GPIO07_EN (1 << 07)
#define GPIO06_EN (1 << 06)
#define GPIO05_EN (1 << 05)
#define GPIO04_EN (1 << 04)
#define GPIO03_EN (1 << 03)
#define GPIO02_EN (1 << 02)
#define GPIO01_EN (1 << 01)
#define GPIO00_EN (1 << 00)
#define GPE0_EN_63_32 0x94
#define GPE0_EN_95_64 0x98
#define GPE0_EN_127_96 0x9c
#define PME_B0_EN (1 << 13)
#define ME_SCI_EN (1 << 12)
#define PME_EN (1 << 11)
#define PCI_EXP_EN (1 << 9)
#define RI_EN (1 << 8)
#define SMB_WAK_EN (1 << 7)
#define TCOSCI_EN (1 << 6)
#define IE_SCI_EN (1 << 3)
#define SWGPE_EN (1 << 2)
#define HOT_PLUG_EN (1 << 1)

/* TCO registers and fields live behind TCOBASE I/O bar in SMBus device. */
#define TCO_RLD 0x00
#define TCO1_STS 0x04
#define TCO1_STS_TCO_SLVSEL (1 << 13)
#define TCO1_STS_CPUSERR (1 << 12)
#define TCO1_STS_CPUSMI (1 << 10)
#define TCO1_STS_CPUSCI (1 << 9)
#define TCO1_STS_BIOSWR (1 << 8)
#define TCO1_STS_NEWCENTURY (1 << 7)
#define TCO1_STS_TIMEOUT (1 << 3)
#define TCO1_STS_TCO_INT (1 << 2)
#define TCO1_STS_OS_TCO_SMI (1 << 1)
#define TCO1_STS_NMI2SMI (1 << 0)
#define TCO2_STS 0x06
#define TCO2_STS_SMLINK_SLAVE_SMI 0x04
#define TCO2_STS_SECOND_TO 0x02
#define TCO2_STS_INTRD_DET 0x01
#define TCO1_CNT 0x08
#define TCO_LOCK (1 << 12)
#define TCO_TMR_HLT (1 << 11)
#define TCO2_CNT 0x0a
#define TCO_TMR 0x12

/* Memory mapped IO registers behind PMC_BASE_ADDRESS */
#define PRSTS			0x10
#define GPIO_GPE_CFG		0x120
#define  GPE0_DWX_MASK		0x7
#define GPE0_DW_SHIFT(x)	(4 + 4*(x))
#define PCH_PWRM_ACPI_TMR_CTL	0xfc
#define  ACPI_TIM_DIS		(1 << 1)

/* I/O ports */
#define RST_CNT 0xcf9
#define FULL_RST (1 << 3)
#define RST_CPU (1 << 2)
#define SYS_RST (1 << 1)

#endif /* _DENVERTON_NS_PMC_H_ */
