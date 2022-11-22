/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_APOLLOLAKE_PM_H_
#define _SOC_APOLLOLAKE_PM_H_

#include <stdint.h>
#include <acpi/acpi.h>
#include <soc/gpe.h>
#include <soc/iomap.h>

/* ACPI_BASE_ADDRESS */

#define PM1_STS			0x00
#define   WAK_STS		(1 << 15)
#define   PCIEXPWAK_STS		(1 << 14)
#define   PRBTNOR_STS		(1 << 11)
#define   RTC_STS		(1 << 10)
#define   PWRBTN_STS		(1 << 8)
#define   GBL_STS		(1 << 5)

#define PM1_EN			0x02
#define   PCIEXPWAK_DIS		(1 << 14)
#define   RTC_EN		(1 << 10)
#define   PWRBTN_EN		(1 << 8)
#define   GBL_EN		(1 << 5)

#define PM1_CNT			0x04
#define   SCI_EN		(1 << 0)

#define PM1_TMR			0x08

#define SMI_EN			0x40

#define SMI_ESPI		28 /* This bit is present in GLK*/
#define SMI_OCP_CSE		27
#define SMI_SPI			26
#define SMI_SPI_SSMI		25
#define SMI_SCC2		21
#define SMI_PCIE		20
#define SMI_SCS			19
#define SMI_HOST_SMBUS		18
#define SMI_XHCI		17
#define SMI_SMBUS		16
#define SMI_SERIRQ		15
#define SMI_PERIODIC		14
#define SMI_TCO			13
#define SMI_MCSMI		12
#define SMI_GPIO_UNLOCK_SSMI	11
#define SMI_GPIO		10
#define SMI_BIOS_RLS		7
#define SMI_SWSMI_TMR		6
#define SMI_APMC		5
#define SMI_SLP			4
#define SMI_LEGACY_USB		3
#define SMI_BIOS		2
#define SMI_EOS			1
#define SMI_GBL			0

#if CONFIG(SOC_ESPI)
#define   ESPI_SMI_EN	(1 << SMI_ESPI) /* Valid for GLK with ESPI */
#else
#define   ESPI_SMI_EN	0
#endif
#define   USB_EN	(1 << SMI_XHCI) /* Legacy USB2 SMI logic */
#define   PERIODIC_EN	(1 << SMI_PERIODIC) /* SMI on PERIODIC_STS in SMI_STS */
#define   TCO_SMI_EN	(1 << SMI_TCO) /* Enable TCO Logic (BIOSWE et al) */
#define   GPIO_EN	(1 << SMI_GPIO) /* Enable GPIO SMI */
#define   BIOS_RLS	(1 << SMI_BIOS_RLS) /* asserts SCI on bit set */
/* start software smi timer on bit set */
#define   SWSMI_TMR_EN	(1 << SMI_SWSMI_TMR)
#define   APMC_EN	(1 << SMI_APMC) /* Writes to APM_CNT cause SMI# */
/* Write to SLP_EN in PM1_CNT asserts SMI# */
#define   SLP_SMI_EN	(1 << SMI_SLP)
#define   BIOS_EN	(1 << SMI_BIOS) /* Assert SMI# on GBL_RLS bit */
#define   EOS		(1 << SMI_EOS) /* End of SMI (deassert SMI#) */
#define   GBL_SMI_EN	(1 << SMI_GBL) /* Global SMI Enable */

/* SMI_EN Params for this platform to pass to enable_smi
 *
 * Enable SMI generation:
 *  - on APMC writes (io 0xb2)
 *  - on writes to SLP_EN (sleep states)
 *  - on writes to GBL_RLS (bios commands)
 *  - on eSPI events (does nothing on LPC systems)
 * No SMIs:
 *  - on microcontroller writes (io 0x62/0x66)
 *  - on TCO events, unless enabled in common code
 */
#define   ENABLE_SMI_PARAMS \
	(ESPI_SMI_EN | APMC_EN | SLP_SMI_EN | GBL_SMI_EN | EOS | GPIO_EN)

#define SMI_STS			0x44
#define  SMI_STS_BITS		32
/* Bits for SMI status */
#define  ESPI_SMI_STS_BIT	28
#define  PMC_OCP_SMI_STS_BIT	27
#define  SPI_SMI_STS_BIT	26
#define  SPI_SSMI_STS_BIT	25
#define  SCC2_SMI_STS_BIT	21
#define  PCI_EXP_SMI_STS_BIT	20
#define  SCS_SMI_STS_BIT	19
#define  HSMBUS_SMI_STS_BIT	18
#define  XHCI_SMI_STS_BIT	17
#define  SMBUS_SMI_STS_BIT	16
#define  SERIRQ_SMI_STS_BIT	15
#define  PERIODIC_STS_BIT	14
#define  TCO_STS_BIT		13
#define  MC_SMI_STS_BIT		12
#define  GPIO_UNLOCK_SMI_STS_BIT	11
#define  GPIO_STS_BIT		10
#define  GPE0_STS_BIT		9	/* Datasheet says this is reserved */
#define  PM1_STS_BIT		8
#define  SWSMI_TMR_STS_BIT	6
#define  APM_STS_BIT		5
#define  SMI_ON_SLP_EN_STS_BIT	4
#define  LEGACY_USB_STS_BIT	3
#define  BIOS_STS_BIT		2

#define GPE_CNTL		0x50
#define DEVACT_STS		0x4c

#define GPE0_REG_MAX		4
#define GPE0_REG_SIZE		32
#define GPE0_STS(x)		(0x20 + ((x) * 4))
#define  GPE0_A			0
#define  GPE0_B			1
#define  GPE0_C			2
#define  GPE0_D			3
#define GPE_STD			GPE0_A
#define   ESPI_STS              (1 << 20) /* This bit is present in GLK */
#define   SATA_PME_STS		(1 << 17)
#define   SMB_WAK_STS		(1 << 16)
#define   AVS_PME_STS		(1 << 14)
#define   XHCI_PME_STS		(1 << 13)
#define   XDCI_PME_STS		(1 << 12)
#define   CSE_PME_STS		(1 << 11)
#define   BATLOW_STS		(1 << 10)
#define   PCIE_GPE_STS		(1 << 9)
#define   SWGPE_STS		(1 << 2)
#define GPE0_EN(x)		(0x30 + ((x) * 4))
#define   ESPI_EN		(1 << 20) /* This bit is present in GLK */
#define   SATA_PME_EN		(1 << 17)
#define   SMB_WAK_EN		(1 << 16)
#define   AVS_PME_EN		(1 << 14)
#define   PME_B0_EN		(1 << 13)
#define   XDCI_PME_EN		(1 << 12)
#define   CSE_PME_EN		(1 << 11)
#define   BATLOW_EN		(1 << 10)
#define   PCIE_GPE_EN		(1 << 9)
#define   SWGPE_EN		(1 << 2)

/* P-state configuration */
#define PSS_MAX_ENTRIES		8
#define PSS_RATIO_STEP		2
#define PSS_LATENCY_TRANSITION	10
#define PSS_LATENCY_BUSMASTER	10

#if !defined(__ACPI__)

#include <soc/pmc.h>

/* Track power state from reset to log events. */
struct chipset_power_state {
	uint16_t pm1_sts;
	uint16_t pm1_en;
	uint32_t pm1_cnt;
	uint32_t gpe0_sts[GPE0_REG_MAX];
	uint32_t gpe0_en[GPE0_REG_MAX];
	uint16_t tco1_sts;
	uint16_t tco2_sts;
	uint32_t prsts;
	uint32_t gen_pmcon1;
	uint32_t gen_pmcon2;
	uint32_t gen_pmcon3;
	uint32_t prev_sleep_state;
} __packed;

void pch_log_state(void);

/* Get base address PMC memory mapped registers. */
uint8_t *pmc_mmio_regs(void);

/* STM Support */
uint16_t get_pmbase(void);
#endif /* !defined(__ACPI__) */
#endif
