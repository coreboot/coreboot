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
//AMD8111
            Name (APIC, Package (0x04)
            {
                Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x10},// 0x0004ffff : assusme 8131 is present 
                Package (0x04) { 0x0004FFFF, 0x01, 0x00, 0x11}, 
                Package (0x04) { 0x0004FFFF, 0x02, 0x00, 0x12}, 
                Package (0x04) { 0x0004FFFF, 0x03, 0x00, 0x13}
            })

            Name (PICM, Package (0x04)
            {
                Package (0x04) { 0x0004FFFF, 0x00, \_SB.PCI1.LNKA, 0x00}, 
                Package (0x04) { 0x0004FFFF, 0x01, \_SB.PCI1.LNKB, 0x00}, 
                Package (0x04) { 0x0004FFFF, 0x02, \_SB.PCI1.LNKC, 0x00}, 
                Package (0x04) { 0x0004FFFF, 0x03, \_SB.PCI1.LNKD, 0x00}
            })

	    Name (DNCG, Ones)

            Method (_PRT, 0, NotSerialized)
            {
		If (LEqual (^DNCG, Ones)) {
			Store (DADD(\_SB.PCI0.SBDN, 0x0001ffff), Local0)
			// Update the Device Number according to SBDN
                        Store(Local0, Index (DeRefOf (Index (PICM, 0)), 0))
                        Store(Local0, Index (DeRefOf (Index (PICM, 1)), 0))
                        Store(Local0, Index (DeRefOf (Index (PICM, 2)), 0))
                        Store(Local0, Index (DeRefOf (Index (PICM, 3)), 0))

                        Store(Local0, Index (DeRefOf (Index (APIC, 0)), 0))
                        Store(Local0, Index (DeRefOf (Index (APIC, 1)), 0))
                        Store(Local0, Index (DeRefOf (Index (APIC, 2)), 0))
                        Store(Local0, Index (DeRefOf (Index (APIC, 3)), 0))
			
			Store (0x00, ^DNCG)
			
		}

                If (LNot (PICF)) { 
			Return (PICM) 
		}
                Else {
			Return (APIC) 
		}
            }

            Device (SBC3)
            {
                /*  acpi smbus   it should be 0x00040003 if 8131 present */
		Method (_ADR, 0, NotSerialized)
		{
			Return (DADD(\_SB.PCI0.SBDN, 0x00010003))
		}
                OperationRegion (PIRQ, PCI_Config, 0x56, 0x02)
                Field (PIRQ, ByteAcc, Lock, Preserve)
                {
                    PIBA,   8, 
                    PIDC,   8
                }
/*
                OperationRegion (TS3_, PCI_Config, 0xC4, 0x02)
                Field (TS3_, DWordAcc, NoLock, Preserve)
                {
                    PTS3,   16
                }
*/
            }

            Device (HPET)
            {
                Name (HPT, 0x00)
                Name (_HID, EisaId ("PNP0103"))
                Name (_UID, 0x00)
                Method (_STA, 0, NotSerialized)
                {
                    Return (0x0F)
                }

                Method (_CRS, 0, NotSerialized)
                {
                    Name (BUF0, ResourceTemplate ()
                    {
                        Memory32Fixed (ReadWrite, 0xFED00000, 0x00000400)
                    })
                    Return (BUF0)
                }
            }

	    Include ("amd8111_pic.asl")

	    Include ("amd8111_isa.asl")

            Device (TP2P)
            {
                /* 8111 P2P and it should 0x00030000 when 8131 present*/
                Method (_ADR, 0, NotSerialized)
                {
			Return (DADD(\_SB.PCI0.SBDN, 0x00000000))
                }

                Method (_PRW, 0, NotSerialized)
                {
                    If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x08, 0x03 }) }
                    Else { Return (Package (0x02) { 0x08, 0x01 }) }
                }

                Device (USB0)
                {
                    Name (_ADR, 0x00000000)
                    Method (_PRW, 0, NotSerialized)
                    {
                        If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x0F, 0x03 }) }
                        Else { Return (Package (0x02) { 0x0F, 0x01 }) }
                    }
                }

                Device (USB1)
                {
                    Name (_ADR, 0x00000001)
                    Method (_PRW, 0, NotSerialized)
                    {
                        If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x0F, 0x03 }) }
                        Else { Return (Package (0x02) { 0x0F, 0x01 }) }
                    }
                }

                Name (APIC, Package (0x0C)
                {
                    Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x10 }, //USB
                    Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x11 },
                    Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x12 },
                    Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x13 },

                    Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x10 }, //Slot 4
                    Package (0x04) { 0x0004FFFF, 0x01, 0x00, 0x11 },
                    Package (0x04) { 0x0004FFFF, 0x02, 0x00, 0x12 },
                    Package (0x04) { 0x0004FFFF, 0x03, 0x00, 0x13 },

                    Package (0x04) { 0x0005FFFF, 0x00, 0x00, 0x11 }, //Slot 3
                    Package (0x04) { 0x0005FFFF, 0x01, 0x00, 0x12 },
                    Package (0x04) { 0x0005FFFF, 0x02, 0x00, 0x13 },
                    Package (0x04) { 0x0005FFFF, 0x03, 0x00, 0x10 }
                })
	
                Name (PICM, Package (0x0C)
                {
                    Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI1.LNKA, 0x00 }, //USB
                    Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI1.LNKB, 0x00 },
                    Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI1.LNKC, 0x00 },
                    Package (0x04) { 0x0000FFFF, 0x03, \_SB.PCI1.LNKD, 0x00 },

                    Package (0x04) { 0x0004FFFF, 0x00, \_SB.PCI1.LNKA, 0x00 }, //Slot 4
                    Package (0x04) { 0x0004FFFF, 0x01, \_SB.PCI1.LNKB, 0x00 },
                    Package (0x04) { 0x0004FFFF, 0x02, \_SB.PCI1.LNKC, 0x00 },
                    Package (0x04) { 0x0004FFFF, 0x03, \_SB.PCI1.LNKD, 0x00 },

                    Package (0x04) { 0x0005FFFF, 0x00, \_SB.PCI1.LNKB, 0x00 }, //Slot 3
                    Package (0x04) { 0x0005FFFF, 0x01, \_SB.PCI1.LNKC, 0x00 },
                    Package (0x04) { 0x0005FFFF, 0x02, \_SB.PCI1.LNKD, 0x00 },
                    Package (0x04) { 0x0005FFFF, 0x03, \_SB.PCI1.LNKA, 0x00 }
                })

                Method (_PRT, 0, NotSerialized)
                {
                    If (LNot (PICF)) { Return (PICM) }
                    Else { Return (APIC) }
                }
            }

