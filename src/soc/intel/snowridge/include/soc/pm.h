/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_PM_H_
#define _SOC_SNOWRIDGE_PM_H_

#include <acpi/acpi.h>
#include <intelpch/gpe.h>
#include <stdint.h>
#include <soc/iomap.h>
#include <soc/pmc.h>

#define PM1_STS                 0x00
#define WAK_STS                 BIT(15)
#define PWRBTN_STS              BIT(8)
#define PM1_EN                  0x02
#define PWRBTN_EN               BIT(8)
#define GBL_EN                  BIT(5)
#define PM1_CNT                 0x04
#define SCI_EN                  BIT(0)
#define PM1_TMR                 0x08
#define SMI_EN                  0x30
#define XHCI_SMI_EN             BIT(31)
#define ME_SMI_EN               BIT(30)
#define ESPI_SMI_EN             BIT(28)
#define GPIO_UNLOCK_SMI_EN      BIT(27)
#define SDX_SMI_EN              BIT(25)
#define THERM_SMI_EN            BIT(18)
#define LEGACY_USB2_EN          BIT(17)
#define PERIODIC_EN             BIT(14)
#define TCO_SMI_EN              BIT(13)
#define MCSMI_EN                BIT(11)
#define BIOS_RLS                BIT(7)
#define SWSMI_TMR_EN            BIT(6)
#define APMC_EN                 BIT(5)
#define SLP_SMI_EN              BIT(4)
#define LEGACY_USB_EN           BIT(3)
#define BIOS_EN                 BIT(2)
#define EOS                     BIT(1)
#define GBL_SMI_EN              BIT(0)
#define SMI_STS                 0x34
#define SMI_STS_BITS            32
#define XHCI_SMI_STS_BIT        31
#define ME_SMI_STS_BIT          30
#define ESPI_SMI_STS_BIT        28
#define GPIO_UNLOCK_SMI_STS_BIT 27
#define SPI_SMI_STS_BIT         26
#define SCC_SMI_STS_BIT         25
#define INTERNAL_TT_STS_BIT     22
#define MONITOR_STS_BIT         21
#define PCI_EXP_SMI_STS_BIT     20
#define RTC_UIP_SMI_STS_BIT     19
#define THERMAL_SMI_STS_BIT     18
#define LEGACY_USB2_STS_BIT     17
#define SMBUS_SMI_STS_BIT       16
#define SERIRQ_SMI_STS_BIT      15
#define PERIODIC_STS_BIT        14
#define TCO_STS_BIT             13
#define DEVMON_STS_BIT          12
#define MCSMI_STS_BIT           11
#define GPIO_STS_BIT            10
#define GPE0_STS_BIT            9
#define PM1_STS_BIT             8
#define SWSMI_TMR_STS_BIT       6
#define APM_STS_BIT             5
#define SMI_ON_SLP_EN_STS_BIT   4
#define LEGACY_USB_STS_BIT      3
#define BIOS_STS_BIT            2
#define PM2_CNT                 0x50

#define GPE0_REG_MAX 4
#define GPE0_STS(x)  (0x60 + ((x) * 4))
#define GPE_31_0     0 /**< 0x60 = GPE[31:0]. */
#define GPE_63_32    1 /**< 0x64 = GPE[63:32]. */
#define GPE_95_64    2 /**< 0x68 = GPE[95:64]. */
#define GPE_STD      3 /**< 0x6c = Standard GPE. */

#define GPE0_EN(x)     (0x70 + ((x) * 4))
#define WADT_EN        BIT(18)
#define USB_CON_DSX_EN BIT(17)
#define LANWAKE_EN     BIT(16)
#define GPIO_T2_EN     BIT(15)
#define ESPI_EN        BIT(14)
#define PME_B0_EN      BIT(13)
#define ME_SCI_EN      BIT(12)
#define PME_EN         BIT(11)
#define BATLOW_EN      BIT(10)
#define PCI_EXP_EN     BIT(9)
#define TCOSCI_EN      BIT(6)
#define THERM_EN       BIT(4)
#define SWGPE_EN       BIT(2)
#define HOT_PLUG_EN    BIT(1)

/**
 * @brief Enable SMI generation:
 *  - on APMC writes (io 0xb2)
 *  - on writes to SLP_EN (sleep states)
 *  - on writes to GBL_RLS (bios commands)
 *  - on eSPI events (does nothing on LPC systems)
 * No SMIs:
 *  - on TCO events, unless enabled in common code
 *  - on microcontroller writes (io 0x62/0x66)
 */
#define ENABLE_SMI_PARAMS (APMC_EN | SLP_SMI_EN | GBL_SMI_EN | ESPI_SMI_EN | EOS)

#define PSS_RATIO_STEP         2
#define PSS_MAX_ENTRIES        8
#define PSS_LATENCY_TRANSITION 10
#define PSS_LATENCY_BUSMASTER  10

struct chipset_power_state {
	uint16_t pm1_sts;
	uint16_t pm1_en;
	uint32_t pm1_cnt;
	uint32_t gpe0_sts[4];
	uint32_t gpe0_en[4];

	uint32_t prev_sleep_state;
} __packed;

/**
 * @brief Return base address of Power Management Controller memory mapped registers, definition
 * is not needed.
 *
 * @return uint8_t*
 */
uint8_t *pmc_mmio_regs(void);

#endif // _SOC_SNOWRIDGE_PM_H_
