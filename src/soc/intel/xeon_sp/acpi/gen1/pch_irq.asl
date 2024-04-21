/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/acpi_asl.h>

/*
 * PCH devices PCI interrupt routing packages.
 *
 * Note: The PCH routing PR10-PR68 and AR10-AR68 are defined in uncore_irq.asl
 *
 * See ACPI spec 6.2.13 _PRT (PCI routing table) for details.
 * The mapping fields ae Address, Pin, Source, Source Index.
 */

// Socket 0, IIOStack 0 device legacy interrupt routing
Name (PR00, Package ()
{
	// [DMI0]: Legacy PCI Express Port 0
	Package () { 0x0000FFFF, 0x00, LNKA, 0x00 },
	// [CB0A]: CBDMA
	// [CB0E]: CBDMA
	Package () { 0x0004FFFF, 0x00, LNKA, 0x00 },
	// [CB0B]: CBDMA
	// [CB0F]: CBDMA
	Package () { 0x0004FFFF, 0x01, LNKB, 0x00 },
	// [CB0C]: CBDMA
	// [CB0G]: CBDMA
	Package () { 0x0004FFFF, 0x02, LNKC, 0x00 },
	// [CB0D]: CBDMA
	// [CB0H]: CBDMA
	Package () { 0x0004FFFF, 0x03, LNKD, 0x00 },
	// Uncore 0 UBOX Device
	Package () { 0x0008FFFF, 0x00, LNKA, 0x00 },
	Package () { 0x0008FFFF, 0x01, LNKB, 0x00 },
	Package () { 0x0008FFFF, 0x02, LNKC, 0x00 },
	Package () { 0x0008FFFF, 0x03, LNKD, 0x00 },
	// [DISP]: Display Controller
	Package () { 0x000FFFFF, 0x00, LNKA, 0x00 },
	// [IHC1]: HECI #1
	// [IHC3]: HECI #3
	Package () { 0x0010FFFF, 0x00, LNKA, 0x00 },
	// [IHC2]: HECI #2
	Package () { 0x0010FFFF, 0x01, LNKB, 0x00 },
	// [IIDR]: IDE-Redirection (IDE-R)
	Package () { 0x0010FFFF, 0x02, LNKC, 0x00 },
	// [IMKT]: Keyboard and Text (KT) Redirection
	Package () { 0x0010FFFF, 0x03, LNKD, 0x00 },
	// [SAT2]: sSATA Host controller 2 on PCH
	Package () { 0x0011FFFF, 0x00, LNKA, 0x00 },
	// // [XHCI]: xHCI controller 1 on PCH
	Package () { 0x0014FFFF, 0x00, LNKA, 0x00 },
	// [OTG0]: USB Device Controller (OTG) on PCH
	Package () { 0x0014FFFF, 0x01, LNKB, 0x00 },
	// [TERM]: Thermal Subsystem on PCH
	Package () { 0x0014FFFF, 0x02, LNKC, 0x00 },
	// [CAMR]: Camera IO Host Controller on PCH
	Package () { 0x0014FFFF, 0x03, LNKD, 0x00 },
	// [HEC1]: HECI #1 on PCH
	// [HEC3]: HECI #3 on PCH
	Package () { 0x0016FFFF, 0x00, LNKA, 0x00 },
	// [HEC2]: HECI #2 on PCH
	Package () { 0x0016FFFF, 0x01, LNKB, 0x00 },
	// [IDER]: ME IDE redirect on PCH
	Package () { 0x0016FFFF, 0x02, LNKC, 0x00 },
	// [MEKT]: MEKT on PCH
	Package () { 0x0016FFFF, 0x03, LNKD, 0x00 },
	// [SAT1]: SATA controller 1 on PCH
	Package () { 0x0017FFFF, 0x00, LNKA, 0x00 },
	// [NAN1]: NAND Cycle Router on PCH
	Package () { 0x0018FFFF, 0x00, LNKA, 0x00 },
	// [RP17]: PCIE PCH Root Port #17
	Package () { 0x001BFFFF, 0x00, LNKA, 0x00 },
	// [RP18]: PCIE PCH Root Port #18
	Package () { 0x001BFFFF, 0x01, LNKB, 0x00 },
	// [RP19]: PCIE PCH Root Port #19
	Package () { 0x001BFFFF, 0x02, LNKC, 0x00 },
	// [RP20]: PCIE PCH Root Port #20
	Package () { 0x001BFFFF, 0x03, LNKD, 0x00 },
	// [RP01]: PCIE PCH Root Port #1
	// [RP05]: PCIE PCH Root Port #5
	Package () { 0x001CFFFF, 0x00, LNKA, 0x00 },
	// [RP02]: PCIE PCH Root Port #2
	// [RP06]: PCIE PCH Root Port #6
	Package () { 0x001CFFFF, 0x01, LNKB, 0x00 },
	// [RP03]: PCIE PCH Root Port #3
	// [RP07]: PCIE PCH Root Port #7
	Package () { 0x001CFFFF, 0x02, LNKC, 0x00 },
	// [RP04]: PCIE PCH Root Port #4
	// [RP08]: PCIE PCH Root Port #8
	Package () { 0x001CFFFF, 0x03, LNKD, 0x00 },
	// [RP09]: PCIE PCH Root Port #9
	// [RP13]: PCIE PCH Root Port #13
	Package () { 0x001DFFFF, 0x00, LNKA, 0x00 },
	// [RP10]: PCIE PCH Root Port #10
	// [RP14]: PCIE PCH Root Port #14
	Package () { 0x001DFFFF, 0x01, LNKB, 0x00 },
	// [RP11]: PCIE PCH Root Port #11
	// [RP15]: PCIE PCH Root Port #15
	Package () { 0x001DFFFF, 0x02, LNKC, 0x00 },
	// [RP12]: PCIE PCH Root Port #12
	// [RP16]: PCIE PCH Root Port #16
	Package () { 0x001DFFFF, 0x03, LNKD, 0x00 },
	// [UAR0]: UART #0 on PCH
	Package () { 0x001EFFFF, 0x02, LNKC, 0x00 },
	// [UAR1]: UART #1 on PCH
	Package () { 0x001EFFFF, 0x03, LNKD, 0x00 },
	// [CAVS]: HD Audio Subsystem Controller on PCH
	// [SMBS]: SMBus controller on PCH
	// [GBE1]: GbE Controller on PCH
	// [NTPK]: Northpeak Controller on PCH
	Package () { 0x001FFFFF, 0x00, LNKA, 0x00 },
})

// Socket 0, IIOStack 0 device IOAPIC interrupt routing
Name (AR00, Package ()
{
	// [DMI0]: Legacy PCI Express Port 0
	Package () { 0x0000FFFF, 0x00, 0x00, 0x1F },
	// [CB0A]: CB3DMA
	// [CB0E]: CB3DMA
	Package () { 0x0004FFFF, 0x00, 0x00, 0x1A },
	// [CB0B]: CB3DMA
	// [CB0F]: CB3DMA
	Package () { 0x0004FFFF, 0x01, 0x00, 0x1B },
	// [CB0C]: CB3DMA
	// [CB0G]: CB3DMA
	Package () { 0x0004FFFF, 0x02, 0x00, 0x1A },
	// [CB0D]: CB3DMA
	// [CB0H]: CB3DMA
	Package () { 0x0004FFFF, 0x03, 0x00, 0x1B },
	// [UBX0]: Uncore 0 UBOX Device
	Package () { 0x0008FFFF, 0x00, 0x00, 0x18 },
	Package () { 0x0008FFFF, 0x01, 0x00, 0x1C },
	Package () { 0x0008FFFF, 0x02, 0x00, 0x1D },
	Package () { 0x0008FFFF, 0x03, 0x00, 0x1E },
	// [DISP]: Display Controller
	Package () { 0x000FFFFF, 0x00, 0x00, 0x10 },
	// [IHC1]: HECI #1
	// [IHC3]: HECI #3
	Package () { 0x0010FFFF, 0x00, 0x00, 0x10 },
	// [IHC2]: HECI #2
	Package () { 0x0010FFFF, 0x01, 0x00, 0x11 },
	// [IIDR]: IDE-Redirection (IDE-R)
	Package () { 0x0010FFFF, 0x02, 0x00, 0x12 },
	// [IMKT]: Keyboard and Text (KT) Redirection
	Package () { 0x0010FFFF, 0x03, 0x00, 0x13 },
	// [SAT2]: sSATA Host controller 2 on PCH
	Package () { 0x0011FFFF, 0x00, 0x00, 0x10 },
	// [XHCI]: xHCI controller 1 on PCH
	Package () { 0x0014FFFF, 0x00, 0x00, 0x10 },
	// [OTG0]: USB Device Controller (OTG) on PCH
	Package () { 0x0014FFFF, 0x01, 0x00, 0x11 },
	// [TERM]: Thermal Subsystem on PCH
	Package () { 0x0014FFFF, 0x02, 0x00, 0x12 },
	// [CAMR]: Camera IO Host Controller on PCH
	Package () { 0x0014FFFF, 0x03, 0x00, 0x13 },
	// [HEC1]: HECI #1 on PCH
	// [HEC3]: HECI #3 on PCH
	Package () { 0x0016FFFF, 0x00, 0x00, 0x10 },
	// [HEC2]: HECI #2 on PCH
	Package () { 0x0016FFFF, 0x01, 0x00, 0x11 },
	// [IDER]: ME IDE redirect on PCH
	Package () { 0x0016FFFF, 0x02, 0x00, 0x12 },
	// [MEKT]: MEKT on PCH
	Package () { 0x0016FFFF, 0x03, 0x00, 0x13 },
	// [SAT1]: SATA controller 1 on PCH
	Package () { 0x0017FFFF, 0x00, 0x00, 0x10 },
	// [NAN1]: NAND Cycle Router on PCH
	Package () { 0x0018FFFF, 0x00, 0x00, 0x10 },
	// [RP17]: PCIE PCH Root Port #17
	Package () { 0x001BFFFF, 0x00, 0x00, 0x10 },
	// [RP18]: PCIE PCH Root Port #18
	Package () { 0x001BFFFF, 0x01, 0x00, 0x11 },
	// [RP19]: PCIE PCH Root Port #19
	Package () { 0x001BFFFF, 0x02, 0x00, 0x12 },
	// [RP20]: PCIE PCH Root Port #20
	Package () { 0x001BFFFF, 0x03, 0x00, 0x13 },
	// [RP01]: PCIE PCH Root Port #1
	// [RP05]: PCIE PCH Root Port #5
	Package () { 0x001CFFFF, 0x00, 0x00, 0x10 },
	// [RP02]: PCIE PCH Root Port #2
	// [RP06]: PCIE PCH Root Port #6
	Package () { 0x001CFFFF, 0x01, 0x00, 0x11 },
	// [RP03]: PCIE PCH Root Port #3
	// [RP07]: PCIE PCH Root Port #7
	Package () { 0x001CFFFF, 0x02, 0x00, 0x12 },
	// [RP04]: PCIE PCH Root Port #4
	// [RP08]: PCIE PCH Root Port #8
	Package () { 0x001CFFFF, 0x03, 0x00, 0x13 },
	// [RP09]: PCIE PCH Root Port #9
	// [RP13]: PCIE PCH Root Port #13
	Package () { 0x001DFFFF, 0x00, 0x00, 0x10 },
	// [RP10]: PCIE PCH Root Port #10
	// [RP14]: PCIE PCH Root Port #14
	Package () { 0x001DFFFF, 0x01, 0x00, 0x11 },
	// [RP11]: PCIE PCH Root Port #11
	// [RP15]: PCIE PCH Root Port #15
	Package () { 0x001DFFFF, 0x02, 0x00, 0x12 },
	// [RP12]: PCIE PCH Root Port #12
	// [RP16]: PCIE PCH Root Port #16
	Package () { 0x001DFFFF, 0x03, 0x00, 0x13 },
	// [UAR0]: UART #0 on PCH
	Package () { 0x001EFFFF, 0x02, 0x00, 0x16 },
	// [UAR1]: UART #1 on PCH
	Package () { 0x001EFFFF, 0x03, 0x00, 0x17 },
	// [CAVS]: HD Audio Subsystem Controller on PCH
	// [SMBS]: SMBus controller on PCH
	// [GBE1]: GbE Controller on PCH
	// [NTPK]: Northpeak Controller on PCH
	Package () { 0x001FFFFF, 0x00, 0x00, 0x10 },
})
