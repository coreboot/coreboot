/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <soc/irq.h>

Name (PICP, Package () {
	/* D31: HDA, SMBUS, TRACEHUB */
	Package(){0x001FFFFF, 3, 0, HDA_IRQ },
	Package(){0x001FFFFF, 4, 0, SMBUS_IRQ },
	Package(){0x001FFFFF, 7, 0, TRACEHUB_IRQ },
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
	Package(){0x001CFFFF, 0, 0, PCIE_1_IRQ },
	Package(){0x001CFFFF, 1, 0, PCIE_2_IRQ },
	Package(){0x001CFFFF, 2, 0, PCIE_3_IRQ },
	Package(){0x001CFFFF, 3, 0, PCIE_4_IRQ },
	Package(){0x001CFFFF, 4, 0, PCIE_5_IRQ },
	Package(){0x001CFFFF, 5, 0, PCIE_6_IRQ },
	Package(){0x001CFFFF, 6, 0, PCIE_7_IRQ },
	Package(){0x001CFFFF, 7, 0, PCIE_8_IRQ },
	/* D25: I2C4, I2C5, UART2 */
	Package(){0x0019FFFF, 0, 0, LPSS_I2C4_IRQ },
	Package(){0x0019FFFF, 1, 0, LPSS_I2C5_IRQ },
	Package(){0x0019FFFF, 2, 0, LPSS_UART2_IRQ },
	/* D23: SATA */
	Package(){0x0017FFFF, 0, 0, SATA_IRQ },
	/* D22: CSME */
	Package(){0x0016FFFF, 0, 0, HECI_1_IRQ },
	Package(){0x0016FFFF, 1, 0, HECI_2_IRQ },
	Package(){0x0016FFFF, 4, 0, HECI_3_IRQ },
	Package(){0x0016FFFF, 5, 0, HECI_4_IRQ },
	/* D21: I2C0 ~ I2C3 */
	Package(){0x0015FFFF, 0, 0, LPSS_I2C0_IRQ },
	Package(){0x0015FFFF, 1, 0, LPSS_I2C1_IRQ },
	Package(){0x0015FFFF, 2, 0, LPSS_I2C2_IRQ },
	Package(){0x0015FFFF, 3, 0, LPSS_I2C3_IRQ },
	/* D20: xHCI, xDCI, SRAM, CNVI_WIFI */
	Package(){0x0014FFFF, 0, 0, xHCI_IRQ },
	Package(){0x0014FFFF, 1, 0, xDCI_IRQ },
	Package(){0x0014FFFF, 3, 0, CNVI_WIFI_IRQ },
	/* D19: SPI3 */
	Package(){0x0013FFFF, 0, 0, LPSS_SPI3_IRQ },
	/* D18: ISH, SPI2 */
	Package(){0x0012FFFF, 0, 0, ISH_IRQ },
	Package(){0x0012FFFF, 6, 0, LPSS_SPI2_IRQ },
	/* D16: CNVI_BT, TCH0, TCH1 */
	Package(){0x0010FFFF, 2, 0, CNVI_BT_IRQ },
	Package(){0x0010FFFF, 6, 0, THC0_IRQ },
	Package(){0x0010FFFF, 7, 0, THC1_IRQ },
	/* D13: xHCI, xDCI */
	Package(){0x000DFFFF, 0, 0, xHCI_IRQ },
	Package(){0x000DFFFF, 1, 0, xDCI_IRQ },
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
	Package(){0x001FFFFF, 3, 0, 11 },
	Package(){0x001FFFFF, 4, 0, 11 },
	Package(){0x001FFFFF, 7, 0, 11 },
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
	Package(){0x001CFFFF, 4, 0, 11 },
	Package(){0x001CFFFF, 5, 0, 10 },
	Package(){0x001CFFFF, 6, 0, 11 },
	Package(){0x001CFFFF, 7, 0, 11 },
	/* D25: I2C4, I2C5, UART2 */
	Package(){0x0019FFFF, 0, 0, 11 },
	Package(){0x0019FFFF, 1, 0, 10 },
	Package(){0x0019FFFF, 2, 0, 11 },
	/* D23: SATA */
	Package(){0x0017FFFF, 0, 0, 11 },
	/* D22: CSME */
	Package(){0x0016FFFF, 0, 0, 11 },
	Package(){0x0016FFFF, 1, 0, 10 },
	Package(){0x0016FFFF, 4, 0, 11 },
	Package(){0x0016FFFF, 5, 0, 10 },
	/* D21: I2C0 ~ I2C3 */
	Package(){0x0015FFFF, 0, 0, 11 },
	Package(){0x0015FFFF, 1, 0, 10 },
	Package(){0x0015FFFF, 2, 0, 11 },
	Package(){0x0015FFFF, 3, 0, 11 },
	/* D20: xHCI, xDCI, CNVI_WIFI */
	Package(){0x0014FFFF, 0, 0, 11 },
	Package(){0x0014FFFF, 1, 0, 10 },
	Package(){0x0014FFFF, 3, 0, 11 },
	/* D19: SPI3 */
	Package(){0x0013FFFF, 0, 0, 11 },
	/* D18: ISH, SPI2 */
	Package(){0x0012FFFF, 0, 0, 11 },
	Package(){0x0012FFFF, 6, 0, 11 },
	/* D16: CNVI_BT, TCH0, TCH1 */
	Package(){0x0010FFFF, 2, 0, 11 },
	Package(){0x0010FFFF, 6, 0, 11 },
	Package(){0x0010FFFF, 7, 0, 11 },
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
