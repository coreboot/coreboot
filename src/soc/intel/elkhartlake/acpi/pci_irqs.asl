/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/irq.h>

Name (PICP, Package () {
	/* D31 */
	Package () { 0x001FFFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x001FFFFF, 1, 0, PCH_IRQ_B },
	Package () { 0x001FFFFF, 2, 0, PCH_IRQ_C },
	Package () { 0x001FFFFF, 3, 0, PCH_IRQ_D },
	/* D30 */
	Package () { 0x001EFFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x001EFFFF, 1, 0, PCH_IRQ_B },
	Package () { 0x001EFFFF, 2, 0, PCH_IRQ_C },
	Package () { 0x001EFFFF, 3, 0, PCH_IRQ_D },
	/* Intel PSE Devices */
	Package () { 0x001DFFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x001DFFFF, 1, 0, PCH_IRQ34 },
	Package () { 0x001DFFFF, 2, 0, PCH_IRQ35 },
	Package () { 0x001DFFFF, 3, 0, PCH_IRQ36 },
	/* PCIe Root Ports */
	Package () { 0x001CFFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x001CFFFF, 1, 0, PCH_IRQ_B },
	Package () { 0x001CFFFF, 2, 0, PCH_IRQ_C },
	Package () { 0x001CFFFF, 3, 0, PCH_IRQ_D },
	/* Intel PSE I2C Devices */
	Package () { 0x001BFFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x001BFFFF, 1, 0, PCH_IRQ_B },
	Package () { 0x001BFFFF, 2, 0, PCH_IRQ_C },
	Package () { 0x001BFFFF, 3, 0, PCH_IRQ_D },
	/* D26 */
	Package () { 0x001AFFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x001AFFFF, 1, 0, PCH_IRQ_B },
	Package () { 0x001AFFFF, 2, 0, PCH_IRQ_C },
	Package () { 0x001AFFFF, 3, 0, PCH_IRQ_D },
	/* D25 */
	Package () { 0x0019FFFF, 0, 0, PCH_IRQ31 },
	Package () { 0x0019FFFF, 1, 0, PCH_IRQ32 },
	Package () { 0x0019FFFF, 2, 0, PCH_IRQ33 },
	/* Intel PSE Devices */
	Package () { 0x0018FFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x0018FFFF, 1, 0, PCH_IRQ_B },
	Package () { 0x0018FFFF, 2, 0, PCH_IRQ_C },
	Package () { 0x0018FFFF, 3, 0, PCH_IRQ_D },
	/* SATA */
	Package () { 0x0017FFFF, 0, 0, PCH_IRQ_A },
	/* ME Interfaces */
	Package () { 0x0016FFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x0016FFFF, 1, 0, PCH_IRQ_B },
	Package () { 0x0016FFFF, 2, 0, PCH_IRQ_C },
	Package () { 0x0016FFFF, 3, 0, PCH_IRQ_D },
	/* I2C Devices */
	Package () { 0x0015FFFF, 0, 0, PCH_IRQ27 },
	Package () { 0x0015FFFF, 1, 0, PCH_IRQ28 },
	Package () { 0x0015FFFF, 2, 0, PCH_IRQ29 },
	Package () { 0x0015FFFF, 3, 0, PCH_IRQ30 },
	/* USB Devices */
	Package () { 0x0014FFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x0014FFFF, 1, 0, PCH_IRQ_B },
	/* Intel PSE Devices */
	Package () { 0x0013FFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x0013FFFF, 1, 0, PCH_IRQ_B },
	Package () { 0x0013FFFF, 2, 0, PCH_IRQ_C },
	Package () { 0x0013FFFF, 3, 0, PCH_IRQ_D },
	/* D18 */
	Package () { 0x0012FFFF, 0, 0, PCH_IRQ24 },
	Package () { 0x0012FFFF, 1, 0, PCH_IRQ25 },
	Package () { 0x0012FFFF, 2, 0, PCH_IRQ26 },
	/* Intel PSE Devices */
	Package () { 0x0011FFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x0011FFFF, 1, 0, PCH_IRQ_B },
	Package () { 0x0011FFFF, 2, 0, PCH_IRQ_C },
	Package () { 0x0011FFFF, 3, 0, PCH_IRQ_D },
	/* D16 */
	Package () { 0x0010FFFF, 0, 0, PCH_IRQ_A },
	Package () { 0x0010FFFF, 1, 0, PCH_IRQ_B },
	Package () { 0x0010FFFF, 2, 0, PCH_IRQ_C },
	/* SA GNA Device */
	Package () { 0x0008FFFF, 0, 0, PCH_IRQ_A },
	/* SA Thermal Device */
	Package () { 0x0004FFFF, 0, 0, PCH_IRQ_A },
	/* SA IGFX Device */
	Package () { 0x0002FFFF, 0, 0, PCH_IRQ_A },
})

Name (PICN, Package () {
	/* D31 */
	Package () { 0x001FFFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x001FFFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x001FFFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x001FFFFF, 3, 0, PCH_IRQ11 },
	/* D30 */
	Package () { 0x001EFFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x001EFFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x001EFFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x001EFFFF, 3, 0, PCH_IRQ11 },
	/* Intel PSE Devices */
	Package () { 0x001DFFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x001DFFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x001DFFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x001DFFFF, 3, 0, PCH_IRQ11 },
	/* PCIe Root Ports */
	Package () { 0x001CFFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x001CFFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x001CFFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x001CFFFF, 3, 0, PCH_IRQ11 },
	/* Intel PSE I2C Devices */
	Package () { 0x001BFFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x001BFFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x001BFFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x001BFFFF, 3, 0, PCH_IRQ11 },
	/* D26 */
	Package () { 0x001AFFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x001AFFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x001AFFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x001AFFFF, 3, 0, PCH_IRQ11 },
	/* D25 */
	Package () { 0x0019FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0019FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0019FFFF, 2, 0, PCH_IRQ11 },
	/* Intel PSE Devices */
	Package () { 0x0018FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0018FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0018FFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x0018FFFF, 3, 0, PCH_IRQ11 },
	/* SATA */
	Package () { 0x0017FFFF, 0, 0, PCH_IRQ11 },
	/* ME Interfaces */
	Package () { 0x0016FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0016FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0016FFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x0016FFFF, 3, 0, PCH_IRQ11 },
	/* I2C Devices */
	Package () { 0x0015FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0015FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0015FFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x0015FFFF, 3, 0, PCH_IRQ11 },
	/* USB Devices */
	Package () { 0x0014FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0014FFFF, 1, 0, PCH_IRQ10 },
	/* Intel PSE Devices */
	Package () { 0x0013FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0013FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0013FFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x0013FFFF, 3, 0, PCH_IRQ11 },
	/* D18 */
	Package () { 0x0012FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0012FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0012FFFF, 2, 0, PCH_IRQ11 },
	/* Intel PSE Devices */
	Package () { 0x0011FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0011FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0011FFFF, 2, 0, PCH_IRQ11 },
	Package () { 0x0011FFFF, 3, 0, PCH_IRQ11 },
	/* D16 */
	Package () { 0x0010FFFF, 0, 0, PCH_IRQ11 },
	Package () { 0x0010FFFF, 1, 0, PCH_IRQ10 },
	Package () { 0x0010FFFF, 2, 0, PCH_IRQ11 },
	/* SA GNA Device */
	Package () { 0x0008FFFF, 0, 0, PCH_IRQ11 },
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
