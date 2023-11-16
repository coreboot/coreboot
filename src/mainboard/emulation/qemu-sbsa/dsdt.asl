/* SPDX-License-Identifier: GPL-2.0-only */

#define LINK_DEVICE(Uid, LinkName, Irq)                                                    \
	Device (LinkName) {                                                                \
		Name (_HID, EISAID("PNP0C0F"))                                             \
		Name (_UID, Uid)                                                           \
		Name (_CRS, ResourceTemplate() {                                           \
			Interrupt (ResourceProducer, Level, ActiveHigh, Exclusive) { Irq } \
		})                                                                         \
	}

#define USB_PORT(PortName, Adr)                                                 \
	Device (PortName) {                                                     \
		Name (_ADR, Adr)                                                \
		Name (_UPC, Package() {                                         \
			0xFF,		                                        \
			0x00,		                                        \
			0x00000000,                                             \
			0x00000000                                              \
		})                                                              \
		Name (_PLD, Package() {                                         \
			Buffer(0x10) {                                          \
				0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
				0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  \
			}                                                       \
		})                                                              \
	}

#define PRT_ENTRY(Address, Pin, Link)    \
	Package (4) {                    \
		Address, Pin, Link, Zero \
	}

#define PRT_ENTRY_GROUP(Address, Link0, Link1, Link2, Link3) \
	PRT_ENTRY (Address, 0, Link0),                       \
	PRT_ENTRY (Address, 1, Link1),                       \
	PRT_ENTRY (Address, 2, Link2),                       \
	PRT_ENTRY (Address, 3, Link3)

#include <acpi/acpi.h>
#include <mainboard/addressmap.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20230621
)
{
	#include <acpi/dsdt_top.asl>

	Scope (_SB) {
		// UART PL011
		Device (COM0) {
			Name (_HID, "ARMH0011")
			Name (_UID, Zero)
			Name (_CRS, ResourceTemplate () {
				Memory32Fixed (ReadWrite, SBSA_UART_BASE, 0x00001000)
				Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 33 }
			})
		}

		// AHCI Host Controller
		Device (AHC0) {
			Name (_HID, "LNRO001E")
			Name (_CLS, Package (3) {
				0x01,
				0x06,
				0x01,
			})
			Name (_CCA, 1)
			Name (_CRS, ResourceTemplate() {
				Memory32Fixed (ReadWrite, SBSA_AHCI_BASE, 0x00010000)
				Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 42 }
			})
		}

		// USB EHCI Host Controller
		Device (USB0) {
			Name (_HID, "LNRO0D20")
			Name (_CID, "PNP0D20")
			Method (_CRS, 0x0, Serialized) {
				Name (RBUF, ResourceTemplate() {
					Memory32Fixed (ReadWrite, SBSA_EHCI_BASE, 0x00010000)
					Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 43 }
				})
				Return (RBUF)
			}

			// Root Hub
			Device (RHUB) {
				Name (_ADR, 0x00000000)	// Address of Root Hub should be 0 as per ACPI 5.0 spec

				// Ports connected to Root Hub
				Device (HUB1) {
					Name (_ADR, 0x00000001)
					Name (_UPC, Package() {
						0x00,		// Port is NOT connectable
						0xFF,		// Don't care
						0x00000000,	// Reserved 0 must be zero
						0x00000000	// Reserved 1 must be zero
					})
					USB_PORT (PRT1, 0x00000001) // USB0_RHUB_HUB1_PRT1
					USB_PORT (PRT2, 0x00000002) // USB0_RHUB_HUB1_PRT2
					USB_PORT (PRT3, 0x00000003) // USB0_RHUB_HUB1_PRT3
					USB_PORT (PRT4, 0x00000004) // USB0_RHUB_HUB1_PRT4
				} // USB0_RHUB_HUB1
			} // USB0_RHUB
		} // USB0

		Device (PCI0)
		{
			Name (_HID, EISAID ("PNP0A08")) // PCI Express Root Bridge
			Name (_CID, EISAID ("PNP0A03")) // Compatible PCI Root Bridge
			Name (_SEG, Zero)		// PCI Segment Group number
			Name (_BBN, Zero)		// PCI Base Bus Number
			Name (_UID, "PCI0")
			Name (_CCA, One)		// Initially mark the PCI coherent (for JunoR1)

			Method (_CBA, 0, NotSerialized) {
				return (SBSA_PCIE_ECAM_BASE)
			}

			LINK_DEVICE (0, GSI0, 0x23)
			LINK_DEVICE (1, GSI1, 0x24)
			LINK_DEVICE (2, GSI2, 0x25)
			LINK_DEVICE (3, GSI3, 0x26)

			Name (_PRT, Package () {

				// _PRT: PCI Routing Table
				PRT_ENTRY_GROUP (0x0000FFFF, GSI0, GSI1, GSI2, GSI3),
				PRT_ENTRY_GROUP (0x0001FFFF, GSI1, GSI2, GSI3, GSI0),
				PRT_ENTRY_GROUP (0x0002FFFF, GSI2, GSI3, GSI0, GSI1),
				PRT_ENTRY_GROUP (0x0003FFFF, GSI3, GSI0, GSI1, GSI2),
				PRT_ENTRY_GROUP (0x0004FFFF, GSI0, GSI1, GSI2, GSI3),
				PRT_ENTRY_GROUP (0x0005FFFF, GSI1, GSI2, GSI3, GSI0),
				PRT_ENTRY_GROUP (0x0006FFFF, GSI2, GSI3, GSI0, GSI1),
				PRT_ENTRY_GROUP (0x0007FFFF, GSI3, GSI0, GSI1, GSI2),
				PRT_ENTRY_GROUP (0x0008FFFF, GSI0, GSI1, GSI2, GSI3),
				PRT_ENTRY_GROUP (0x0009FFFF, GSI1, GSI2, GSI3, GSI0),
				PRT_ENTRY_GROUP (0x000AFFFF, GSI2, GSI3, GSI0, GSI1),
				PRT_ENTRY_GROUP (0x000BFFFF, GSI3, GSI0, GSI1, GSI2),
				PRT_ENTRY_GROUP (0x000CFFFF, GSI0, GSI1, GSI2, GSI3),
				PRT_ENTRY_GROUP (0x000DFFFF, GSI1, GSI2, GSI3, GSI0),
				PRT_ENTRY_GROUP (0x000EFFFF, GSI2, GSI3, GSI0, GSI1),
				PRT_ENTRY_GROUP (0x000FFFFF, GSI3, GSI0, GSI1, GSI2),
				PRT_ENTRY_GROUP (0x0010FFFF, GSI0, GSI1, GSI2, GSI3),
				PRT_ENTRY_GROUP (0x0011FFFF, GSI1, GSI2, GSI3, GSI0),
				PRT_ENTRY_GROUP (0x0012FFFF, GSI2, GSI3, GSI0, GSI1),
				PRT_ENTRY_GROUP (0x0013FFFF, GSI3, GSI0, GSI1, GSI2),
				PRT_ENTRY_GROUP (0x0014FFFF, GSI0, GSI1, GSI2, GSI3),
				PRT_ENTRY_GROUP (0x0015FFFF, GSI1, GSI2, GSI3, GSI0),
				PRT_ENTRY_GROUP (0x0016FFFF, GSI2, GSI3, GSI0, GSI1),
				PRT_ENTRY_GROUP (0x0017FFFF, GSI3, GSI0, GSI1, GSI2),
				PRT_ENTRY_GROUP (0x0018FFFF, GSI0, GSI1, GSI2, GSI3),
				PRT_ENTRY_GROUP (0x0019FFFF, GSI1, GSI2, GSI3, GSI0),
				PRT_ENTRY_GROUP (0x001AFFFF, GSI2, GSI3, GSI0, GSI1),
				PRT_ENTRY_GROUP (0x001BFFFF, GSI3, GSI0, GSI1, GSI2),
				PRT_ENTRY_GROUP (0x001CFFFF, GSI0, GSI1, GSI2, GSI3),
				PRT_ENTRY_GROUP (0x001DFFFF, GSI1, GSI2, GSI3, GSI0),
				PRT_ENTRY_GROUP (0x001EFFFF, GSI2, GSI3, GSI0, GSI1),
				PRT_ENTRY_GROUP (0x001FFFFF, GSI3, GSI0, GSI1, GSI2),
			})

			// Root complex resources
			Method (_CRS, 0, Serialized) {
			Name (RBUF, ResourceTemplate () {
				WordBusNumber ( // Bus numbers assigned to this root
				ResourceProducer,
				MinFixed, MaxFixed, PosDecode,
				0,	 // AddressGranularity
				0,	 // AddressMinimum - Minimum Bus Number
				255,	 // AddressMaximum - Maximum Bus Number
				0,	 // AddressTranslation - Set to 0
				256	// RangeLength - Number of Busses
				)

				DWordMemory ( // 32-bit BAR Windows
					ResourceProducer, PosDecode,
					MinFixed, MaxFixed,
					Cacheable, ReadWrite,
					0x00000000,           // Granularity
					SBSA_PCIE_MMIO_BASE,  // Min Base Address
					SBSA_PCIE_MMIO_LIMIT, // Max Base Address
					0,                    // Translate
					SBSA_PCIE_MMIO_SIZE   // Length
					)

				QWordMemory ( // 64-bit BAR Windows
					ResourceProducer, PosDecode,
					MinFixed, MaxFixed,
					Cacheable, ReadWrite,
					0x00000000,                // Granularity
					SBSA_PCIE_MMIO_HIGH_BASE,  // Min Base Address
					SBSA_PCIE_MMIO_HIGH_LIMIT, // Max Base Address
					0,                         // Translate
					SBSA_PCIE_MMIO_HIGH_SIZE   // Length
					)

				DWordIo ( // IO window
					ResourceProducer,
					MinFixed,
					MaxFixed,
					PosDecode,
					EntireRange,
					0x00000000,         // Granularity
					0,                  // Min Base Address
					0xffff,             // Max Base Address
					SBSA_PCIE_PIO_BASE, // Translate
					0x10000,            // Length
					,,,TypeTranslation
					)
				}) // Name(RBUF)

				Return (RBUF)
			} // Method(_CRS)

			// OS Control Handoff
			Name (SUPP, Zero) // PCI _OSC Support Field value
			Name (CTRL, Zero) // PCI _OSC Control Field value

			/*
			 * See [1] 6.2.10, [2] 4.5
			 */
			Method (_OSC,4) {
				// Check for proper UUID
				If (Arg0 == ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766")) {
					// Create DWord-adressable fields from the Capabilities Buffer
					CreateDWordField (Arg3,0,CDW1)
					CreateDWordField (Arg3,4,CDW2)
					CreateDWordField (Arg3,8,CDW3)

					// Save Capabilities DWord2 & 3
					SUPP = CDW2
					CTRL = CDW3

					// Only allow native hot plug control if OS supports:
					// * ASPM
					// * Clock PM
					// * MSI/MSI-X
					If ((SUPP & 0x16) != 0x16) {
						CTRL &= 0x1E // Mask bit 0 (and undefined bits)
					}

					// Always allow native PME, AER (no dependencies)

					// Never allow SHPC (no SHPC controller in this system)
					CTRL &= 0x1D

					If (Arg1 != One) { // Unknown revision
						CDW1 |= 0x08
					}

					If (CDW3 != CTRL) { // Capabilities bits were masked
						CDW1 |= 0x10
					}

					// Update DWORD3 in the buffer
					CDW3 = CTRL
					Return (Arg3)
				} Else {
					CDW1 |= 4 // Unrecognized UUID
					Return (Arg3)
				}
			} // End _OSC
		}
	} // Scope (_SB)
}
