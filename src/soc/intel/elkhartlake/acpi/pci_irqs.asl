/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/irq.h>

Name (PICP, Package () {
	Package(){0x001FFFFF, 0, 0, PCH_IRQ_16 },
	Package(){0x001FFFFF, 1, 0, PCH_IRQ_17 },
	Package(){0x001FFFFF, 2, 0, PCH_IRQ_18 },
	Package(){0x001FFFFF, 3, 0, PCH_IRQ_19 },

	Package(){0x001EFFFF, 0, 0, LPSS_UART0_IRQ },
	Package(){0x001EFFFF, 1, 0, LPSS_UART1_IRQ },
	Package(){0x001EFFFF, 2, 0, LPSS_SPI0_IRQ },
	Package(){0x001EFFFF, 3, 0, LPSS_SPI1_IRQ },

	Package(){0x001CFFFF, 0, 0, PCH_IRQ_16 },
	Package(){0x001CFFFF, 1, 0, PCH_IRQ_17 },
	Package(){0x001CFFFF, 2, 0, PCH_IRQ_18 },
	Package(){0x001CFFFF, 3, 0, PCH_IRQ_19 },

	Package(){0x001AFFFF, 0, 0, PCH_IRQ_16 },

	Package(){0x0019FFFF, 0, 0, LPSS_I2C4_IRQ },
	Package(){0x0019FFFF, 1, 0, LPSS_I2C5_IRQ },
	Package(){0x0019FFFF, 2, 0, LPSS_UART2_IRQ },

	Package(){0x0017FFFF, 0, 0, PCH_IRQ_16 },

	Package(){0x0016FFFF, 0, 0, PCH_IRQ_16 },
	Package(){0x0016FFFF, 1, 0, PCH_IRQ_17 },
	Package(){0x0016FFFF, 2, 0, PCH_IRQ_18 },
	Package(){0x0016FFFF, 3, 0, PCH_IRQ_19 },

	Package(){0x0015FFFF, 0, 0, LPSS_I2C0_IRQ },
	Package(){0x0015FFFF, 1, 0, LPSS_I2C1_IRQ },
	Package(){0x0015FFFF, 2, 0, LPSS_I2C2_IRQ },
	Package(){0x0015FFFF, 3, 0, LPSS_I2C3_IRQ },

	Package(){0x0014FFFF, 0, 0, PCH_IRQ_16 },
	Package(){0x0014FFFF, 1, 0, PCH_IRQ_17 },
	Package(){0x0014FFFF, 2, 0, PCH_IRQ_18 },
	Package(){0x0014FFFF, 3, 0, PCH_IRQ_19 },

	Package(){0x0012FFFF, 1, 0, LPSS_SPI2_IRQ },
	/* SA GNA Device */
	Package(){0x0008FFFF, 0, 0, PCH_IRQ_16 },
	/* SA IPU Device */
	Package(){0x0005FFFF, 0, 0, PCH_IRQ_16 },
	/* SA Thermal Device */
	Package(){0x0004FFFF, 0, 0, PCH_IRQ_16 },
	/* SA IGFX Device */
	Package(){0x0002FFFF, 0, 0, PCH_IRQ_16 },
})

Name (PICN, Package () {
	Package () { 0x001FFFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x001FFFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x001FFFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x001FFFFF, 3, 0, PCH_IRQ11 },

	Package () { 0x001EFFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x001EFFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x001EFFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x001EFFFF, 3, 0, PCH_IRQ11 },

	Package () { 0x001CFFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x001CFFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x001CFFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x001CFFFF, 3, 0, PCH_IRQ11 },

	Package () { 0x001AFFFF, 0, 0, PCH_IRQ11 },

	Package () { 0x0019FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0019FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0019FFFF, 2, 0, PCH_IRQ11 },

	Package () { 0x0017FFFF, 0, 0, PCH_IRQ11 },

	Package () { 0x0016FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0016FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0016FFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x0016FFFF, 3, 0, PCH_IRQ11 },

	Package () { 0x0015FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0015FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0015FFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x0015FFFF, 3, 0, PCH_IRQ11 },

	Package () { 0x0014FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0014FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0014FFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x0014FFFF, 3, 0, PCH_IRQ11 },

	Package () { 0x0012FFFF, 1, 0, PCH_IRQ10 },
	/* SA GNA Device */
	Package () { 0x0008FFFF, 0, 0, PCH_IRQ11 },
	/* SA IPU Device */
	Package () { 0x0005FFFF, 0, 0, PCH_IRQ11 },
	/* SA Thermal Device */
	Package () { 0x0004FFFF, 0, 0, PCH_IRQ11 },
	/* SA IGFX Device */
	Package () { 0x0002FFFF, 0, 0, PCH_IRQ11 },
})

Method (_PRT)
{
	If (PICM) {
		Return (^PICP)
	} Else {
		Return (^PICN)
	}
}
