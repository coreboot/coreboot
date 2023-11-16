/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <mainboard/addressmap.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20230621	// OEM revision
)
{
	#include <acpi/dsdt_top.asl>

	Device (COM0)
	{
		Name (_HID, "ARMH0011")  // _HID: Hardware ID
		Name (_UID, Zero)  // _UID: Unique ID
		Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
		{
			Memory32Fixed (ReadWrite,
				RDN2_UART_NS_BASE,	 // Address Base
				0x00001000,		 // Address Length
				)
			Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
			{
				RDN2_UART_NS_GSIV,
			}
		})
		Method (_STA, 0, NotSerialized)  // _STA: Status
		{
			Return (0xf)
		}
	}

	Device (VR00)
	{
		Name (_HID, "LNRO0005")  // _HID: Hardware ID
		Name (_UID, Zero)  // _UID: Unique ID
		Name (_CCA, One)  // _CCA: Cache Coherency Attribute
		Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
		{
			Memory32Fixed (ReadWrite,
				0x0C130000,		 // Address Base
				0x00010000,		 // Address Length
				)
			Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
			{
				0x000001CA,
			}
		})
	}

	Device (VR01)
	{
		Name (_HID, "LNRO0005")  // _HID: Hardware ID
		Name (_UID, One)  // _UID: Unique ID
		Name (_CCA, One)  // _CCA: Cache Coherency Attribute
		Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
		{
			Memory32Fixed (ReadWrite,
				0x0C150000,		 // Address Base
				0x00010000,		 // Address Length
				)
			Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
			{
				0x000001CC,
			}
		})
	}


        Device (\_SB.DMA0)
        {
            Name (_HID, "ARMH0330")  // _HID: Hardware ID
            Name (_UID, Zero)  // _UID: Unique ID
            Name (_CCA, One)  // _CCA: Cache Coherency Attribute
            Name (_STA, 0x0F)  // _STA: Status
            Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
            {
                Name (RBUF, ResourceTemplate ()
                {
                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000000000000000, // Range Minimum
                        0x0000000000000001, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000000000000002, // Length
                        ,, _Y02, AddressRangeMemory, TypeStatic)
                    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                    {
                        0x000001ED,
                        0x000001EE,
                        0x000001EF,
                        0x000001F0,
                        0x000001F1,
                        0x000001F2,
                        0x000001F3,
                        0x000001F4,
                        0x000001F5,
                    }
                })
                CreateQWordField (RBUF, \_SB.DMA0._CRS._Y02._MIN, MIN2)  // _MIN: Minimum Base Address
                CreateQWordField (RBUF, \_SB.DMA0._CRS._Y02._MAX, MAX2)  // _MAX: Maximum Base Address
                CreateQWordField (RBUF, \_SB.DMA0._CRS._Y02._LEN, LEN2)  // _LEN: Length
                MIN2 = RDN2_DMA0_BASE
                MAX2 = (MIN2 + 0xFFFF)
                LEN2 = 0x00010000
                Return (RBUF) /* \_SB_.DMA0._CRS.RBUF */
            }
        }

        Device (\_SB.DMA1)
        {
            Name (_HID, "ARMH0330")  // _HID: Hardware ID
            Name (_UID, One)  // _UID: Unique ID
            Name (_CCA, One)  // _CCA: Cache Coherency Attribute
            Name (_STA, 0x0F)  // _STA: Status
            Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
            {
                Name (RBUF, ResourceTemplate ()
                {
                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000000000000000, // Range Minimum
                        0x0000000000000001, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000000000000002, // Length
                        ,, _Y03, AddressRangeMemory, TypeStatic)
                    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                    {
                        0x000001F7,
                        0x000001F8,
                        0x000001F9,
                        0x000001FA,
                        0x000001FB,
                        0x000001FC,
                        0x000001FD,
                        0x000001FE,
                        0x000001FF,
                    }
                })
                CreateQWordField (RBUF, \_SB.DMA1._CRS._Y03._MIN, MIN2)  // _MIN: Minimum Base Address
                CreateQWordField (RBUF, \_SB.DMA1._CRS._Y03._MAX, MAX2)  // _MAX: Maximum Base Address
                CreateQWordField (RBUF, \_SB.DMA1._CRS._Y03._LEN, LEN2)  // _LEN: Length
                MIN2 = RDN2_DMA1_BASE
                MAX2 = (MIN2 + 0xFFFF)
                LEN2 = 0x00010000
                Return (RBUF) /* \_SB_.DMA1._CRS.RBUF */
            }
        }
	Device (PCI0)
	{
		Name (_HID, EisaId ("PNP0A08")) // PCI Express Bus _HID: Hardware ID
		Name (_CID, EisaId ("PNP0A03")) // PCI Bus _CID: Compatible ID
		Name (_SEG, Zero)  // _SEG: PCI Segment
		Name (_BBN, Zero)  // _BBN: BIOS Bus Number
		Name (_UID, "PCI0")  // _UID: Unique ID
		Name (_CCA, One)  // _CCA: Cache Coherency Attribute

		Method (_STA, 0, NotSerialized)  // _STA: Status
		{
			Return (0xf)
		}

		Method (_CBA, 0, NotSerialized)  // _CBA: Configuration Base Address
		{
			Return (RDN2_PCIE_ECAM_BASE)
		}

		Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
		{
			Name (RBUF, ResourceTemplate ()
			{
				WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
					0x0000,			 // Granularity
					0x0000,			 // Range Minimum
					0x00FF,			 // Range Maximum
					0x0000,			 // Translation Offset
					0x0100,			 // Length
					,, )
				DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
					0x00000000,		 // Granularity
					RDN2_PCIE_MMIO_BASE,		 // Range Minimum
					RDN2_PCIE_MMIO_LIMIT,		 // Range Maximum
					0x00000000,		 // Translation Offset
					RDN2_PCIE_MMIO_SIZE,		 // Length
					,, , AddressRangeMemory, TypeStatic)
				QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
					0x0000000000000000, // Granularity
					RDN2_PCIE_MMIO_HIGH_BASE, // Range Minimum
					RDN2_PCIE_MMIO_HIGH_LIMIT, // Range Maximum
					0x0000000000000000, // Translation Offset
					RDN2_PCIE_MMIO_HIGH_SIZE, // Length
					,, , AddressRangeMemory, TypeStatic)
				DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
					0x00000000,		 // Granularity
					0x00000000,		 // Range Minimum
					0x007FFFFF,		 // Range Maximum
					0x77800000,		 // Translation Offset
					0x00800000,		 // Length
					,, , TypeTranslation, DenseTranslation)
			})
			Return (RBUF) /* \PCI0._CRS.RBUF */
		}

		Device (RES0)
		{
			Name (_HID, "PNP0C02" /* PNP Motherboard Resources */)  // _HID: Hardware ID
			Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
			{
				QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
					0x0000000000000000, // Granularity
					RDN2_PCIE_ECAM_BASE, // Range Minimum
					RDN2_PCIE_ECAM_LIMIT, // Range Maximum
					0x0000000000000000, // Translation Offset
					RDN2_PCIE_ECAM_SIZE, // Length
					,, , AddressRangeMemory, TypeStatic)
			})
			Method (_STA, 0, NotSerialized)  // _STA: Status
			{
				Return (0xf)
			}
		}
	}
}
