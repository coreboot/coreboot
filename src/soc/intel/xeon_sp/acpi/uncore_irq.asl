/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/acpi_asl.h>

/*
 * Uncore devices PCI interrupt routing packages.
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

// Socket 0, IIOStack 1 device legacy interrupt routing
Name (PR10, Package ()
{
	// PCI Express Port 1A-1D
	GEN_PCIE_LEGACY_IRQ(),

	// Uncore CHAUTIL Devices
	GEN_UNCORE_LEGACY_IRQ(0x0008FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0009FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000AFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000BFFFF),

	// Uncore CHASAD Devices
	GEN_UNCORE_LEGACY_IRQ(0x000EFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000FFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0010FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0011FFFF),

	// Uncore CMSCHA Devices
	GEN_UNCORE_LEGACY_IRQ(0x0014FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0015FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0016FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0017FFFF),

	// Uncore CHASADALL Device
	GEN_UNCORE_LEGACY_IRQ(0x001DFFFF),

	// Uncore PCUCR Device
	GEN_UNCORE_LEGACY_IRQ(0x001EFFFF),

	// Uncore VCUCR Device
	GEN_UNCORE_LEGACY_IRQ(0x001FFFFF)
})

// Socket 0, IIOStack 1 device IOAPIC interrupt routing
Name (AR10, Package ()
{
	// PCI Express Port A-D
	GEN_PCIE_IOAPIC_IRQ(0x27,0x21,0x22,0x23),

	// Uncore CHAUTIL Devices
	GEN_UNCORE_IOAPIC_IRQ(0x0008FFFF, 0x20, 0x24, 0x25, 0x26),
	GEN_UNCORE_IOAPIC_IRQ(0x0009FFFF, 0x20, 0x24, 0x25, 0x26),
	GEN_UNCORE_IOAPIC_IRQ(0x000AFFFF, 0x20, 0x24, 0x25, 0x26),
	GEN_UNCORE_IOAPIC_IRQ(0x000BFFFF, 0x20, 0x24, 0x25, 0x26),

	// Uncore CHASAD Devices
	GEN_UNCORE_IOAPIC_IRQ(0x000EFFFF, 0x20, 0x24, 0x25, 0x26),
	GEN_UNCORE_IOAPIC_IRQ(0x000FFFFF, 0x20, 0x24, 0x25, 0x26),
	GEN_UNCORE_IOAPIC_IRQ(0x0010FFFF, 0x20, 0x24, 0x25, 0x26),
	GEN_UNCORE_IOAPIC_IRQ(0x0011FFFF, 0x20, 0x24, 0x25, 0x26),

	// Uncore CMSCHA Devices
	GEN_UNCORE_IOAPIC_IRQ(0x0014FFFF, 0x20, 0x24, 0x25, 0x26),
	GEN_UNCORE_IOAPIC_IRQ(0x0015FFFF, 0x20, 0x24, 0x25, 0x26),
	GEN_UNCORE_IOAPIC_IRQ(0x0016FFFF, 0x20, 0x24, 0x25, 0x26),
	GEN_UNCORE_IOAPIC_IRQ(0x0017FFFF, 0x20, 0x24, 0x25, 0x26),

	// Uncore CHASADALL Device
	GEN_UNCORE_IOAPIC_IRQ(0x001DFFFF, 0x20, 0x24, 0x25, 0x26),

	// Uncore PCUCR Device
	GEN_UNCORE_IOAPIC_IRQ(0x001EFFFF, 0x20, 0x24, 0x25, 0x26),

	// Uncore VCUCR Device
	GEN_UNCORE_IOAPIC_IRQ(0x001FFFFF, 0x20, 0x24, 0x25, 0x26)
})

// Socket 0, IIOStack 2 device legacy interrupt routing
Name (PR20, Package ()
{
	// PCI Express Port A-D on PC02
	GEN_PCIE_LEGACY_IRQ(),

	// Uncore M2MEM Devices
	GEN_UNCORE_LEGACY_IRQ(0x0008FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0009FFFF),

	// Uncore MCMAIN Device
	GEN_UNCORE_LEGACY_IRQ(0x000AFFFF),

	// Uncore MCDECS2 Device
	GEN_UNCORE_LEGACY_IRQ(0x000BFFFF),

	// Uncore MCMAIN Device
	GEN_UNCORE_LEGACY_IRQ(0x000CFFFF),

	// Uncore MCDECS Device
	GEN_UNCORE_LEGACY_IRQ(0x000DFFFF),

	// Uncore Unicast MC0 DDRIO0 Device
	GEN_UNCORE_LEGACY_IRQ(0x0016FFFF),

	// Uncore Unicast MC1 DDRIO0 Device
	GEN_UNCORE_LEGACY_IRQ(0x0017FFFF)
})

// Socket 0, IIOStack 2 device IOAPIC interrupt routing
Name (AR20, Package ()
{
	// PCI Express Port A-D on PC02
	GEN_PCIE_IOAPIC_IRQ(0x2F,0x29,0x2A,0x2B),

	// Uncore M2MEM Devices
	GEN_UNCORE_IOAPIC_IRQ(0x0008FFFF, 0x28, 0x2C, 0x2D, 0x2E),
	GEN_UNCORE_IOAPIC_IRQ(0x0009FFFF, 0x28, 0x2C, 0x2D, 0x2E),

	// Uncore MCMAIN Device
	GEN_UNCORE_IOAPIC_IRQ(0x000AFFFF, 0x28, 0x2C, 0x2D, 0x2E),

	// Uncore MCDECS2 Device
	GEN_UNCORE_IOAPIC_IRQ(0x000BFFFF, 0x28, 0x2C, 0x2D, 0x2E),

	// Uncore MCMAIN Device
	GEN_UNCORE_IOAPIC_IRQ(0x000CFFFF, 0x28, 0x2C, 0x2D, 0x2E),

	// Uncore MCDECS Device
	GEN_UNCORE_IOAPIC_IRQ(0x000DFFFF, 0x28, 0x2C, 0x2D, 0x2E),

	// Uncore Unicast MC0 DDRIO0 Device
	GEN_UNCORE_IOAPIC_IRQ(0x0016FFFF, 0x28, 0x2C, 0x2D, 0x2E),

	// Uncore Unicast MC1 DDRIO0 Device
	GEN_UNCORE_IOAPIC_IRQ(0x0017FFFF, 0x28, 0x2C, 0x2D, 0x2E)
})

// Socket 0, IIOStack 3 device legacy interrupt routing
Name (PR28, Package ()
{
	// PCI Express Port 3 on PC03
	GEN_PCIE_LEGACY_IRQ(),

	// KTI Devices
	GEN_UNCORE_LEGACY_IRQ(0x000EFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000FFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0010FFFF),

	// M3K Device
	GEN_UNCORE_LEGACY_IRQ(0x0012FFFF),

	// M2U Device
	GEN_UNCORE_LEGACY_IRQ(0x0015FFFF),

	// M2D Device
	GEN_UNCORE_LEGACY_IRQ(0x0016FFFF),

	// M20 Device
	GEN_UNCORE_LEGACY_IRQ(0x0017FFFF)
})

// Socket 0, IIOStack 3 device IOAPIC interrupt routing
Name (AR28, Package ()
{
	// PCI Express Port 3 A-D on PC03
	GEN_PCIE_IOAPIC_IRQ(0x37,0x31,0x32,0x33),

	// KTI Devices
	GEN_UNCORE_IOAPIC_IRQ(0x000EFFFF, 0x30, 0x34, 0x35, 0x36),
	GEN_UNCORE_IOAPIC_IRQ(0x000FFFFF, 0x30, 0x34, 0x35, 0x36),
	GEN_UNCORE_IOAPIC_IRQ(0x0010FFFF, 0x30, 0x34, 0x35, 0x36),

	// M3K Device
	GEN_UNCORE_IOAPIC_IRQ(0x0012FFFF, 0x30, 0x34, 0x35, 0x36),

	// M2U Device
	GEN_UNCORE_IOAPIC_IRQ(0x0015FFFF, 0x30, 0x34, 0x35, 0x36),

	// M2D Device
	GEN_UNCORE_IOAPIC_IRQ(0x0016FFFF, 0x30, 0x34, 0x35, 0x36),

	// M20 Device
	GEN_UNCORE_IOAPIC_IRQ(0x0017FFFF, 0x30, 0x34, 0x35, 0x36)
})

// Socket 1, IIOStack 0 device legacy interrupt routing
Name (PR40, Package ()
{
	// DMI
	Package () { 0x0000FFFF, 0x00, LNKA, 0x00 },

	// CBDMA
	GEN_UNCORE_LEGACY_IRQ(0x0004FFFF),

	// Ubox
	GEN_UNCORE_LEGACY_IRQ(0x0008FFFF)
})

// Socket 1, IIOStack 0 device IOAPIC interrupt routing
Name (AR40, Package ()
{
	// DMI
	Package () { 0x0000FFFF, 0x00, 0x00, 0x4F },

	// CBDMA
	GEN_UNCORE_IOAPIC_IRQ(0x0004FFFF, 0x4A, 0x4B, 0x4A, 0x4B),

	// Ubox
	GEN_UNCORE_IOAPIC_IRQ(0x0008FFFF, 0x48, 0x4C, 0x4D, 0x4E),
})

// Socket 1, IIOStack 1 device legacy interrupt routing
Name (PR50, Package ()
{
	// PCI Express Port
	GEN_PCIE_LEGACY_IRQ(),

	// CHA Devices
	GEN_UNCORE_LEGACY_IRQ(0x0008FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0009FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000AFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000BFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000EFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000FFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0010FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0011FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0014FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0015FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0016FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0017FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x001DFFFF),

	// PCU Devices
	GEN_UNCORE_LEGACY_IRQ(0x001EFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x001FFFFF)
})

// Socket 1, IIOStack 1 device IOAPIC interrupt routing
Name (AR50, Package ()
{
	// PCI Express Port A-D
	GEN_PCIE_IOAPIC_IRQ(0x57,0x51,0x52,0x53),

	// CHA Devices
	GEN_UNCORE_IOAPIC_IRQ(0x0008FFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x0009FFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x000AFFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x000BFFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x000EFFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x000FFFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x0010FFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x0011FFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x0014FFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x0015FFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x0016FFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x0017FFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x001DFFFF, 0x50, 0x54, 0x55, 0x56),

	// PCU Devices
	GEN_UNCORE_IOAPIC_IRQ(0x001EFFFF, 0x50, 0x54, 0x55, 0x56),
	GEN_UNCORE_IOAPIC_IRQ(0x001FFFFF, 0x50, 0x54, 0x55, 0x56)
})

// Socket 1, IIOStack 2 device legacy interrupt routing
Name (PR60, Package ()
{
	// PCI Express Port
	GEN_PCIE_LEGACY_IRQ(),

	// Integrated Memory Controller
	GEN_UNCORE_LEGACY_IRQ(0x0008FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0009FFFF),

	// Uncore Devices
	GEN_UNCORE_LEGACY_IRQ(0x000AFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000BFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000CFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000DFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0016FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0017FFFF)
})

// Socket 1, IIOStack 2 device IOAPIC interrupt routing
Name (AR60, Package ()
{
	// PCI Express Port A-D
	GEN_PCIE_IOAPIC_IRQ(0x5F,0x59,0x5A,0x5B),

	// Integrated Memory Controller
	GEN_UNCORE_IOAPIC_IRQ(0x0008FFFF, 0x58, 0x5C, 0x5D, 0x5E),
	GEN_UNCORE_IOAPIC_IRQ(0x0009FFFF, 0x58, 0x5C, 0x5D, 0x5E),

	// Uncore Devices
	GEN_UNCORE_IOAPIC_IRQ(0x000AFFFF, 0x58, 0x5C, 0x5D, 0x5E),
	GEN_UNCORE_IOAPIC_IRQ(0x000BFFFF, 0x58, 0x5C, 0x5D, 0x5E),
	GEN_UNCORE_IOAPIC_IRQ(0x000CFFFF, 0x58, 0x5C, 0x5D, 0x5E),
	GEN_UNCORE_IOAPIC_IRQ(0x000DFFFF, 0x58, 0x5C, 0x5D, 0x5E),
	GEN_UNCORE_IOAPIC_IRQ(0x0016FFFF, 0x58, 0x5C, 0x5D, 0x5E),
	GEN_UNCORE_IOAPIC_IRQ(0x0017FFFF, 0x58, 0x5C, 0x5D, 0x5E)
})

// Socket 1, IIOStack 3 device legacy interrupt routing
Name (PR68, Package ()
{
	// PCI Express Port
	GEN_PCIE_LEGACY_IRQ(),

	// Uncore Devices
	GEN_UNCORE_LEGACY_IRQ(0x000EFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x000FFFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0010FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0012FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0015FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0016FFFF),
	GEN_UNCORE_LEGACY_IRQ(0x0017FFFF)
})

// Socket 1, IIOStack 3 device legacy interrupt routing
Name (AR68, Package ()
{
	// PCI Express Port A-D
	GEN_PCIE_IOAPIC_IRQ(0x67,0x61,0x62,0x63),

	// Uncore Devices
	GEN_UNCORE_IOAPIC_IRQ(0x000EFFFF, 0x60, 0x64, 0x65, 0x66),
	GEN_UNCORE_IOAPIC_IRQ(0x000FFFFF, 0x60, 0x64, 0x65, 0x66),
	GEN_UNCORE_IOAPIC_IRQ(0x0010FFFF, 0x60, 0x64, 0x65, 0x66),
	GEN_UNCORE_IOAPIC_IRQ(0x0012FFFF, 0x60, 0x64, 0x65, 0x66),
	GEN_UNCORE_IOAPIC_IRQ(0x0015FFFF, 0x60, 0x64, 0x65, 0x66),
	GEN_UNCORE_IOAPIC_IRQ(0x0016FFFF, 0x60, 0x64, 0x65, 0x66),
	GEN_UNCORE_IOAPIC_IRQ(0x0017FFFF, 0x60, 0x64, 0x65, 0x66)
})
