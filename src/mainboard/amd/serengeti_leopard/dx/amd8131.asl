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
//8131
		
            Device (PG0A)
            {
                Name (_ADR, 0x00010000) /*  8132 pcix bridge*/
                Method (_PRW, 0, NotSerialized)
                {
                    If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x29, 0x03 }) }
                    Else { Return (Package (0x02) { 0x29, 0x01 }) }
                }

                Name (APIC, Package (0x14)
                {
		    // Slot A - PIRQ BCDA
                    Package (0x04) { 0x0001FFFF, 0x00, 0x00, 0x19 }, //Slot 2 
                    Package (0x04) { 0x0001FFFF, 0x01, 0x00, 0x1A }, 
                    Package (0x04) { 0x0001FFFF, 0x02, 0x00, 0x1B }, 
                    Package (0x04) { 0x0001FFFF, 0x03, 0x00, 0x18 },
 
		    //Cypress Slot A - PIRQ BCDA
                    Package (0x04) { 0x0003FFFF, 0x00, 0x00, 0x19 }, //?
                    Package (0x04) { 0x0003FFFF, 0x01, 0x00, 0x1A }, 
                    Package (0x04) { 0x0003FFFF, 0x02, 0x00, 0x1B }, 
                    Package (0x04) { 0x0003FFFF, 0x03, 0x00, 0x18 }, 

		    //Cypress Slot B - PIRQ CDAB
                    Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x1A }, //?
                    Package (0x04) { 0x0004FFFF, 0x01, 0x00, 0x1B }, 
                    Package (0x04) { 0x0004FFFF, 0x02, 0x00, 0x18 }, 
                    Package (0x04) { 0x0004FFFF, 0x03, 0x00, 0x19 }, 

		    //Cypress Slot C - PIRQ DABC
                    Package (0x04) { 0x0005FFFF, 0x00, 0x00, 0x1B }, //?
                    Package (0x04) { 0x0005FFFF, 0x01, 0x00, 0x18 }, 
                    Package (0x04) { 0x0005FFFF, 0x02, 0x00, 0x19 }, 
                    Package (0x04) { 0x0005FFFF, 0x03, 0x00, 0x1A }, 

		    //Cypress Slot D - PIRQ ABCD
                    Package (0x04) { 0x0006FFFF, 0x00, 0x00, 0x18 }, //?
                    Package (0x04) { 0x0006FFFF, 0x01, 0x00, 0x19 }, 
                    Package (0x04) { 0x0006FFFF, 0x02, 0x00, 0x1A }, 
                    Package (0x04) { 0x0006FFFF, 0x03, 0x00, 0x1B }
                })
                Name (PICM, Package (0x14)
                {
                    Package (0x04) { 0x0001FFFF, 0x00, \_SB.PCI1.LNKB, 0x00 },//Slot 2 
                    Package (0x04) { 0x0001FFFF, 0x01, \_SB.PCI1.LNKC, 0x00 }, 
                    Package (0x04) { 0x0001FFFF, 0x02, \_SB.PCI1.LNKD, 0x00 }, 
                    Package (0x04) { 0x0001FFFF, 0x03, \_SB.PCI1.LNKA, 0x00 }, 

                    Package (0x04) { 0x0003FFFF, 0x00, \_SB.PCI1.LNKB, 0x00 }, 
                    Package (0x04) { 0x0003FFFF, 0x01, \_SB.PCI1.LNKC, 0x00 }, 
                    Package (0x04) { 0x0003FFFF, 0x02, \_SB.PCI1.LNKD, 0x00 }, 
                    Package (0x04) { 0x0003FFFF, 0x03, \_SB.PCI1.LNKA, 0x00 }, 

                    Package (0x04) { 0x0004FFFF, 0x00, \_SB.PCI1.LNKC, 0x00 }, 
                    Package (0x04) { 0x0004FFFF, 0x01, \_SB.PCI1.LNKD, 0x00 }, 
                    Package (0x04) { 0x0004FFFF, 0x02, \_SB.PCI1.LNKA, 0x00 }, 
                    Package (0x04) { 0x0004FFFF, 0x03, \_SB.PCI1.LNKB, 0x00 }, 

                    Package (0x04) { 0x0005FFFF, 0x00, \_SB.PCI1.LNKD, 0x00 }, 
                    Package (0x04) { 0x0005FFFF, 0x01, \_SB.PCI1.LNKA, 0x00 }, 
                    Package (0x04) { 0x0005FFFF, 0x02, \_SB.PCI1.LNKB, 0x00 }, 
                    Package (0x04) { 0x0005FFFF, 0x03, \_SB.PCI1.LNKC, 0x00 }, 

                    Package (0x04) { 0x0006FFFF, 0x00, \_SB.PCI1.LNKA, 0x00 }, 
                    Package (0x04) { 0x0006FFFF, 0x01, \_SB.PCI1.LNKB, 0x00 }, 
                    Package (0x04) { 0x0006FFFF, 0x02, \_SB.PCI1.LNKC, 0x00 }, 
                    Package (0x04) { 0x0006FFFF, 0x03, \_SB.PCI1.LNKD, 0x00 }
                })
                Method (_PRT, 0, NotSerialized)
                {
                    If (LNot (PICF)) { Return (PICM) }
                    Else { Return (APIC) }
                }
            }

            Device (PG0B)
            {
                Name (_ADR, 0x00020000) /* 8132 pcix bridge 2 */
                Method (_PRW, 0, NotSerialized)
                {
                    If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x22, 0x03 }) }
                    Else { Return (Package (0x02) { 0x22, 0x01 }) }
                }

                Name (APIC, Package (0x04)
                {
		    // Slot A - PIRQ ABCD
                    Package (0x04) { 0x0001FFFF, 0x00, 0x00, 0x1F },// Slot 1
                    Package (0x04) { 0x0001FFFF, 0x01, 0x00, 0x20 }, 
                    Package (0x04) { 0x0001FFFF, 0x02, 0x00, 0x21 }, 
                    Package (0x04) { 0x0001FFFF, 0x03, 0x00, 0x22 }
                })
                Name (PICM, Package (0x04)
                {
                    Package (0x04) { 0x0001FFFF, 0x00, \_SB.PCI1.LNKA, 0x00 },//Slot 1 
                    Package (0x04) { 0x0001FFFF, 0x01, \_SB.PCI1.LNKB, 0x00 }, 
                    Package (0x04) { 0x0001FFFF, 0x02, \_SB.PCI1.LNKC, 0x00 }, 
                    Package (0x04) { 0x0001FFFF, 0x03, \_SB.PCI1.LNKD, 0x00 }
                })
                Method (_PRT, 0, NotSerialized)
                {
                    If (LNot (PICF)) { Return (PICM) }
                    Else { Return (APIC) }
                }
            }
