/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/irq.h>

Name (PICP, Package () {
	/* cAVS, SMBus, GbE, Northpeak */
	Package(){0x001FFFFF, 3, 0, cAVS_INTA_IRQ },
	Package(){0x001FFFFF, 4, 0, SMBUS_INTB_IRQ },
	Package(){0x001FFFFF, 6, 0, GbE_INTC_IRQ },
	Package(){0x001FFFFF, 7, 0, TRACE_HUB_INTD_IRQ },
	/* SerialIo */
	Package(){0x001EFFFF, 0, 0, LPSS_UART0_IRQ },
	Package(){0x001EFFFF, 1, 0, LPSS_UART1_IRQ },
	Package(){0x001EFFFF, 2, 0, LPSS_SPI0_IRQ },
	Package(){0x001EFFFF, 3, 0, LPSS_SPI1_IRQ },
	/* PCI Express Port 1-8 */
	Package(){0x001CFFFF, 0, 0, PCIE_1_IRQ },
	Package(){0x001CFFFF, 1, 0, PCIE_2_IRQ },
	Package(){0x001CFFFF, 2, 0, PCIE_3_IRQ },
	Package(){0x001CFFFF, 3, 0, PCIE_4_IRQ },
	Package(){0x001CFFFF, 4, 0, PCIE_5_IRQ },
	Package(){0x001CFFFF, 5, 0, PCIE_6_IRQ },
	Package(){0x001CFFFF, 6, 0, PCIE_7_IRQ },
	Package(){0x001CFFFF, 7, 0, PCIE_8_IRQ },
	/* eMMC */
	Package(){0x001AFFFF, 0, 0, eMMC_IRQ },
	/* SerialIo */
	Package(){0x0019FFFF, 0, 0, LPSS_I2C4_IRQ },
	Package(){0x0019FFFF, 1, 0, LPSS_I2C5_IRQ },
	Package(){0x0019FFFF, 2, 0, LPSS_UART2_IRQ },
	/* SATA controller */
	Package(){0x0017FFFF, 0, 0, SATA_IRQ },
	/* CSME (HECI, IDE-R, Keyboard and Text redirection */
	Package(){0x0016FFFF, 0, 0, HECI_1_IRQ },
	Package(){0x0016FFFF, 1, 0, HECI_2_IRQ },
	Package(){0x0016FFFF, 2, 0, IDER_IRQ },
	Package(){0x0016FFFF, 3, 0, KT_IRQ },
	Package(){0x0016FFFF, 4, 0, HECI_3_IRQ },
	Package(){0x0016FFFF, 5, 0, HECI_4_IRQ },
	/* SerialIo */
	Package(){0x0015FFFF, 0, 0, LPSS_I2C0_IRQ },
	Package(){0x0015FFFF, 1, 0, LPSS_I2C1_IRQ },
	Package(){0x0015FFFF, 2, 0, LPSS_I2C2_IRQ },
	Package(){0x0015FFFF, 3, 0, LPSS_I2C3_IRQ },
	/* D20: xHCI, OTG, SRAM, CNVi WiFi, SD */
	Package(){0x0014FFFF, 0, 0, XHCI_IRQ },
	Package(){0x0014FFFF, 1, 0, OTG_IRQ },
	Package(){0x0014FFFF, 2, 0, PMC_SRAM_IRQ },
	Package(){0x0014FFFF, 3, 0, CNViWIFI_IRQ },
	Package(){0x0014FFFF, 5, 0, SD_IRQ },
	/* SerialIo */
	Package(){0x0012FFFF, 6, 0, LPSS_SPI2_IRQ },
	/* SA IGFX Device */
	Package(){0x0002FFFF, 0, 0, IGFX_IRQ },
	/* SA Thermal Device */
	Package(){0x0004FFFF, 0, 0, SA_THERMAL_IRQ },
	/* SA IPU Device */
	Package(){0x0005FFFF, 0, 0, IPU_IRQ },
	/* SA GNA Device */
	Package(){0x0008FFFF, 0, 0, GNA_IRQ },
})

Name (PICN, Package () {
	/* D31: cAVS, SMBus, GbE, Northpeak */
	Package () { 0x001FFFFF, 3, 0, 11 },
	Package () { 0x001FFFFF, 4, 0, 10 },
	Package () { 0x001FFFFF, 6, 0, 11 },
	Package () { 0x001FFFFF, 7, 0, 11 },
	/* D30: SerialIo */
	Package () {0x001EFFFF, 0, 0, 11 },
	Package () {0x001EFFFF, 1, 0, 10 },
	Package () {0x001EFFFF, 2, 0, 11 },
	Package () {0x001EFFFF, 3, 0, 11 },
	/* D28: PCI Express Port 1-8 */
	Package () { 0x001CFFFF, 0, 0, 11 },
	Package () { 0x001CFFFF, 1, 0, 10 },
	Package () { 0x001CFFFF, 2, 0, 11 },
	Package () { 0x001CFFFF, 3, 0, 11 },
	Package () { 0x001CFFFF, 4, 0, 11 },
	Package () { 0x001CFFFF, 5, 0, 10 },
	Package () { 0x001CFFFF, 6, 0, 11 },
	Package () { 0x001CFFFF, 7, 0, 11 },
	/* D26: eMMC */
	Package(){0x001AFFFF, 0, 0, 11 },
	/* D25: SerialIo */
	Package () {0x0019FFFF, 0, 0, 11 },
	Package () {0x0019FFFF, 1, 0, 10 },
	Package () {0x0019FFFF, 2, 0, 11 },
	/* D23: SATA controller */
	Package () { 0x0017FFFF, 0, 0, 11 },
	/* D22: CSME (HECI, IDE-R, KT redirection */
	Package () { 0x0016FFFF, 0, 0, 11 },
	Package () { 0x0016FFFF, 1, 0, 10 },
	Package () { 0x0016FFFF, 2, 0, 11 },
	Package () { 0x0016FFFF, 3, 0, 11 },
	Package () { 0x0016FFFF, 4, 0, 11 },
	Package () { 0x0016FFFF, 5, 0, 11 },
	/* D21: SerialIo */
	Package () {0x0015FFFF, 0, 0, 11 },
	Package () {0x0015FFFF, 1, 0, 10 },
	Package () {0x0015FFFF, 2, 0, 11 },
	Package () {0x0015FFFF, 3, 0, 11 },
	/* D20: xHCI, OTG, SRAM, CNVi WiFi, SD */
	Package () { 0x0014FFFF, 0, 0, 11 },
	Package () { 0x0014FFFF, 1, 0, 10 },
	Package () { 0x0014FFFF, 2, 0, 11 },
	Package () { 0x0014FFFF, 3, 0, 11 },
	Package () { 0x0014FFFF, 5, 0, 11 },
	/* D18: SerialIo */
	Package () {0x0012FFFF, 6, 0, 11 },
	/* SA IGFX Device */
	Package () {0x0002FFFF, 0, 0, 11 },
	/* SA Thermal Device */
	Package () { 0x0004FFFF, 0, 0, 11 },
	/* SA IPU Device */
	Package () { 0x0005FFFF, 0, 0, 11 },
	/* SA GNA Device */
	Package () { 0x0008FFFF, 0, 0, 11 },
})

Method (_PRT)
{
	If (PICM) {
		Return (^PICP)
	} Else {
		Return (^PICN)
	}
}
