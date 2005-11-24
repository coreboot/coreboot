/*============================================================================
Copyright 2005 ADVANCED MICRO DEVICES, INC. All Rights Reserved.
This software and any related documentation (the "Materials") are the
confidential proprietary information of AMD. Unless otherwise provided in a
software agreement specifically licensing the Materials, the Materials are
provided in confidence and may not be distributed, modified, or reproduced in
whole or in part by any means.
LIMITATION OF LIABILITY: THE MATERIALS ARE PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO
WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY
PARTICULAR PURPOSE, OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR
USAGE OF TRADE. IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY
DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS,
BUSINESS INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF THE USE OF OR
INABILITY TO USE THE MATERIALS, EVEN IF AMD HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES. BECAUSE SOME JURISDICTIONS PROHIBIT THE EXCLUSION
OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE
LIMITATION MAY NOT APPLY TO YOU.
AMD does not assume any responsibility for any errors which may appear in the
Materials nor any responsibility to support or update the Materials. AMD
retains the right to modify the Materials at any time, without notice, and is
not obligated to provide such modified Materials to you.
NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
further information, software, technical information, know-how, or show-how
available to you.
U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to the
restrictions as set forth in FAR 52.227-14 and DFAR 252.227-7013, et seq., or
its successor. Use of the Materials by the Government constitutes
acknowledgement of AMD's proprietary rights in them.
============================================================================*/
// 2005.9 serengeti support
// by yhlu
//
//=
//AMD8111 isa

            Device (ISA)
            {
                /* lpc  0x00040000 */ 
                Method (_ADR, 0, NotSerialized)
                {
			Return (DADD(\_SB.PCI0.SBDN, 0x00010000))
                }

                OperationRegion (PIRY, PCI_Config, 0x51, 0x02) // LPC Decode Registers
                Field (PIRY, ByteAcc, NoLock, Preserve)
                {
                    Z000,   2,  // Parallel Port Range
                        ,   1, 
                    ECP,    1,  // ECP Enable
                    FDC1,   1,  // Floppy Drive Controller 1
                    FDC2,   1,  // Floppy Drive Controller 2
                    Offset (0x01), 
                    Z001,   3,  // Serial Port A Range
                    SAEN,   1,  // Serial Post A Enabled
                    Z002,   3,  // Serial Port B Range
                    SBEN,   1  // Serial Post B Enabled
                }

                Device (PIC)
                {
                    Name (_HID, EisaId ("PNP0000"))
                    Name (_CRS, ResourceTemplate ()
                    {
                        IO (Decode16, 0x0020, 0x0020, 0x01, 0x02)
                        IO (Decode16, 0x00A0, 0x00A0, 0x01, 0x02)
                        IRQ (Edge, ActiveHigh, Exclusive) {2}
                    })
                }

                Device (DMA1)
                {
                    Name (_HID, EisaId ("PNP0200"))
                    Name (_CRS, ResourceTemplate ()
                    {
                        IO (Decode16, 0x0000, 0x0000, 0x01, 0x10)
                        IO (Decode16, 0x0080, 0x0080, 0x01, 0x10)
                        IO (Decode16, 0x00C0, 0x00C0, 0x01, 0x20)
                        DMA (Compatibility, NotBusMaster, Transfer16) {4}
                    })
                }

                Device (TMR)
                {
                    Name (_HID, EisaId ("PNP0100"))
                    Name (_CRS, ResourceTemplate ()
                    {
                        IO (Decode16, 0x0040, 0x0040, 0x01, 0x04)
                        IRQ (Edge, ActiveHigh, Exclusive) {0}
                    })
                }

                Device (RTC)
                {
                    Name (_HID, EisaId ("PNP0B00"))
                    Name (_CRS, ResourceTemplate ()
                    {
                        IO (Decode16, 0x0070, 0x0070, 0x01, 0x06)
                        IRQ (Edge, ActiveHigh, Exclusive) {8}
                    })
                }

                Device (SPKR)
                {
                    Name (_HID, EisaId ("PNP0800"))
                    Name (_CRS, ResourceTemplate ()
                    {
                        IO (Decode16, 0x0061, 0x0061, 0x01, 0x01)
                    })
                }

                Device (COPR)
                {
                    Name (_HID, EisaId ("PNP0C04"))
                    Name (_CRS, ResourceTemplate ()
                    {
                        IO (Decode16, 0x00F0, 0x00F0, 0x01, 0x10)
                        IRQ (Edge, ActiveHigh, Exclusive) {13}
                    })
                }

                Device (SYSR)
                {
                    Name (_HID, EisaId ("PNP0C02"))
                    Name (_UID, 0x00)
                    Name (SYR1, ResourceTemplate ()
                    {
                        IO (Decode16, 0x04D0, 0x04D0, 0x01, 0x02) //wrh092302 - added to report Thor NVRAM
                        IO (Decode16, 0x1100, 0x117F, 0x01, 0x80) //wrh092302 - added to report Thor NVRAM
                        IO (Decode16, 0x1180, 0x11FF, 0x01, 0x80)
                        IO (Decode16, 0x0010, 0x0010, 0x01, 0x10)
                        IO (Decode16, 0x0022, 0x0022, 0x01, 0x1E)
                        IO (Decode16, 0x0044, 0x0044, 0x01, 0x1C)
                        IO (Decode16, 0x0062, 0x0062, 0x01, 0x02)
                        IO (Decode16, 0x0065, 0x0065, 0x01, 0x0B)
                        IO (Decode16, 0x0076, 0x0076, 0x01, 0x0A)
                        IO (Decode16, 0x0090, 0x0090, 0x01, 0x10)
                        IO (Decode16, 0x00A2, 0x00A2, 0x01, 0x1E)
                        IO (Decode16, 0x00E0, 0x00E0, 0x01, 0x10)
                        IO (Decode16, 0x0B78, 0x0B78, 0x01, 0x04) // Added this to remove ACPI Unrepoted IO Error 
                        IO (Decode16, 0x0190, 0x0190, 0x01, 0x04) // Added this to remove ACPI Unrepoted IO Error
                    })
                    Method (_CRS, 0, NotSerialized)
                    {
                        Return (SYR1)
                    }
                }

                Device (MEM)
                {
                    Name (_HID, EisaId ("PNP0C02"))
                    Name (_UID, 0x01)
                    Method (_CRS, 0, NotSerialized)
                    {
                        Name (BUF0, ResourceTemplate ()
                        {
                            Memory32Fixed (ReadWrite, 0x000E0000, 0x00020000) // BIOS E4000-FFFFF
                            Memory32Fixed (ReadWrite, 0x000C0000, 0x00000000) // video BIOS c0000-c8404
                            Memory32Fixed (ReadWrite, 0xFEC00000, 0x00001000) // I/O APIC
                            Memory32Fixed (ReadWrite, 0xFFC00000, 0x00380000) // LPC forwarded, 4 MB w/ROM
                            Memory32Fixed (ReadWrite, 0xFEE00000, 0x00001000) // Local APIC
                            Memory32Fixed (ReadWrite, 0xFFF80000, 0x00080000) // Overlay BIOS
                            Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) // Overlay BIOS
                            Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) // Overlay BIOS
                            Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) //Overlay BIOS
                            Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) //Overlay BIOS
                        })
			// Read the Video Memory length 
                        CreateDWordField (BUF0, 0x14, CLEN)
                        CreateDWordField (BUF0, 0x10, CBAS)

                        ShiftLeft (VGA1, 0x09, Local0)
                        Store (Local0, CLEN)

                        Return (BUF0)
                    }
                }

                Device (PS2M)
                {
                    Name (_HID, EisaId ("PNP0F13"))
                    Name (_CRS, ResourceTemplate ()
                    {
                        IRQNoFlags () {12}
                    })
                    Method (_STA, 0, NotSerialized)
                    {
                        And (FLG0, 0x04, Local0)
                        If (LEqual (Local0, 0x04)) { Return (0x0F) }
                        Else { Return (0x00) }
                    }
                }

                Device (PS2K)
                {
                    Name (_HID, EisaId ("PNP0303"))
                    Name (_CRS, ResourceTemplate ()
                    {
                        IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
                        IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
                        IRQNoFlags () {1}
                    })
                }
		Include ("superio.asl")

            }

