/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <soc/irq.h>

Name (PICP, Package () {
	/* D31: HDA, SMBus, TraceHub, GbE */
	Package(){0x001FFFFF, 0, 0, TRACEHUB_IRQ },
	/* D30: UART0, UART1, SPI0, SPI1 */
	Package(){0x001EFFFF, 0, 0, LPSS_UART0_IRQ },
	Package(){0x001EFFFF, 1, 0, LPSS_UART1_IRQ },
	Package(){0x001EFFFF, 2, 0, LPSS_SPI0_IRQ },
	Package(){0x001EFFFF, 3, 0, LPSS_SPI1_IRQ },
	/* D29: RP9 ~ RP12 */
	Package(){0x001DFFFF, 0, 0, PCIE_9_IRQ },
	Package(){0x001DFFFF, 1, 0, PCIE_10_IRQ },
	Package(){0x001DFFFF, 2, 0, PCIE_11_IRQ },
	Package(){0x001DFFFF, 3, 0, PCIE_12_IRQ },
	/* D28: RP1 ~ RP8 */
	Package(){0x001CFFFF, 0, 0, PCIE_1_IRQ }, /* RP 1 and 5 */
	Package(){0x001CFFFF, 1, 0, PCIE_2_IRQ }, /* RP 2 and 6 */
	Package(){0x001CFFFF, 2, 0, PCIE_3_IRQ }, /* RP 3 and 7 */
	Package(){0x001CFFFF, 3, 0, PCIE_4_IRQ }, /* RP 4 and 8 */
	/* D25: I2C4, I2C5, UART2 */
	Package(){0x0019FFFF, 0, 0, LPSS_I2C4_IRQ },
	Package(){0x0019FFFF, 1, 0, LPSS_I2C5_IRQ },
	Package(){0x0019FFFF, 2, 0, LPSS_UART2_IRQ },
	/* D23: SATA */
	Package(){0x0017FFFF, 0, 0, SATA_IRQ },
	/* D22: CSME */
	Package(){0x0016FFFF, 0, 0, HECI_1_IRQ },
	Package(){0x0016FFFF, 1, 0, HECI_2_IRQ },
	Package(){0x0016FFFF, 2, 0, CSME_IDE_IRQ },
	Package(){0x0016FFFF, 3, 0, CSME_KT_IRQ },
	/* D21: I2C0 ~ I2C3 */
	Package(){0x0015FFFF, 0, 0, LPSS_I2C0_IRQ },
	Package(){0x0015FFFF, 1, 0, LPSS_I2C1_IRQ },
	Package(){0x0015FFFF, 2, 0, LPSS_I2C2_IRQ },
	Package(){0x0015FFFF, 3, 0, LPSS_I2C3_IRQ },
	/* D20: xHCI, xDCI, SRAM, CNVI_WIFI */
	Package(){0x0014FFFF, 0, 0, xHCI_IRQ },
	Package(){0x0014FFFF, 1, 0, xDCI_IRQ },
	/* D18: ISH, SPI2 */
	Package(){0x0012FFFF, 0, 0, ISH_IRQ },
	Package(){0x0012FFFF, 1, 0, LPSS_SPI2_IRQ },
	/* D17: UART3 */
	Package(){0x0011FFFF, 0, 0, LPSS_UART3_IRQ },
	/* D16: THC0, THC1 */
	Package(){0x0010FFFF, 0, 0, THC0_IRQ },
	Package(){0x0010FFFF, 1, 0, THC1_IRQ },
	/* D13: xHCI, xDCI */
	Package(){0x000DFFFF, 0, 0, CPU_xHCI_IRQ },
	Package(){0x000DFFFF, 1, 0, CPU_xDCI_IRQ },
	/* D8: GNA */
	Package(){0x0008FFFF, 0, 0, GNA_IRQ },
	/* D7: TBT PCIe */
	Package(){0x0007FFFF, 0, 0, TBT_PCIe0_IRQ },
	Package(){0x0007FFFF, 1, 0, TBT_PCIe1_IRQ },
	Package(){0x0007FFFF, 2, 0, TBT_PCIe2_IRQ },
	Package(){0x0007FFFF, 3, 0, TBT_PCIe3_IRQ },
	/* D6: PEG60 */
	Package(){0x0006FFFF, 0, 0, PEG_IRQ },
	/* D5: IPU Device */
	Package(){0x0005FFFF, 0, 0, IPU_IRQ },
	/* D4:  Thermal Device */
	Package(){0x0004FFFF, 0, 0, THERMAL_IRQ },
	/* D2: IGFX */
	Package(){0x0002FFFF, 0, 0, IGFX_IRQ },
})

Name (PICN, Package () {
	/* D31: HDA, SMBUS, TRACEHUB */
	Package(){0x001FFFFF, 0, 0, 11 },
	Package(){0x001FFFFF, 1, 0, 10 },
	Package(){0x001FFFFF, 2, 0, 11 },
	Package(){0x001FFFFF, 3, 0, 11 },
	/* D30: UART0, UART1, SPI0, SPI1 */
	Package(){0x001EFFFF, 0, 0, 11 },
	Package(){0x001EFFFF, 1, 0, 10 },
	Package(){0x001EFFFF, 2, 0, 11 },
	Package(){0x001EFFFF, 3, 0, 11 },
	/* D29: RP9 ~ RP12 */
	Package(){0x001DFFFF, 0, 0, 11 },
	Package(){0x001DFFFF, 1, 0, 10 },
	Package(){0x001DFFFF, 2, 0, 11 },
	Package(){0x001DFFFF, 3, 0, 11 },
	/* D28: RP1 ~ RP8 */
	Package(){0x001CFFFF, 0, 0, 11 },
	Package(){0x001CFFFF, 1, 0, 10 },
	Package(){0x001CFFFF, 2, 0, 11 },
	Package(){0x001CFFFF, 3, 0, 11 },
	/* D25: I2C4, I2C5, UART2 */
	Package(){0x0019FFFF, 0, 0, 11 },
	Package(){0x0019FFFF, 1, 0, 10 },
	Package(){0x0019FFFF, 2, 0, 11 },
	/* D23: SATA */
	Package(){0x0017FFFF, 0, 0, 11 },
	/* D22: CSME */
	Package(){0x0016FFFF, 0, 0, 11 },
	Package(){0x0016FFFF, 1, 0, 10 },
	Package(){0x0016FFFF, 2, 0, 11 },
	Package(){0x0016FFFF, 3, 0, 11 },
	/* D21: I2C0 ~ I2C3 */
	Package(){0x0015FFFF, 0, 0, 11 },
	Package(){0x0015FFFF, 1, 0, 10 },
	Package(){0x0015FFFF, 2, 0, 11 },
	Package(){0x0015FFFF, 3, 0, 11 },
	/* D20: xHCI, xDCI, SRAM, CNVI_WIFI */
	Package(){0x0014FFFF, 0, 0, 11 },
	Package(){0x0014FFFF, 1, 0, 10 },
	Package(){0x0014FFFF, 2, 0, 11 },
	/* D19: SPI3 */
	Package(){0x0013FFFF, 0, 0, 11 },
	/* D18: ISH, SPI2 */
	Package(){0x0012FFFF, 0, 0, 11 },
	Package(){0x0012FFFF, 1, 0, 10 },
	/* D17: UART3 */
	Package(){0x0011FFFF, 0, 0, 11 },
	/* D16: THC0, THC1 */
	Package(){0x0010FFFF, 0, 0, 11 },
	Package(){0x0010FFFF, 1, 0, 10 },
	/* D13: xHCI, xDCI */
	Package(){0x000DFFFF, 0, 0, 11 },
	Package(){0x000DFFFF, 1, 0, 10 },
	/* D8: GNA */
	Package(){0x0008FFFF, 0, 0, 11 },
	/* D7: TBT PCIe */
	Package(){0x0007FFFF, 0, 0, 11 },
	Package(){0x0007FFFF, 1, 0, 10 },
	Package(){0x0007FFFF, 2, 0, 11 },
	Package(){0x0007FFFF, 3, 0, 11 },
	/* D6: PEG60 */
	Package(){0x0006FFFF, 0, 0, 11 },
	/* D5: IPU Device */
	Package(){0x0005FFFF, 0, 0, 11 },
	/* D4:  Thermal Device */
	Package(){0x0004FFFF, 0, 0, 11 },
	/* D2: IGFX */
	Package(){0x0002FFFF, 0, 0, 11 },
})

Method (_PRT)
{
	If (PICM) {
		Return (^PICP)
	} Else {
		Return (^PICN)
	}
}
