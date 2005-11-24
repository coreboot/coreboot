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
DefinitionBlock ("SSDT2.aml", "SSDT", 1, "AMD-K8", "AMDACPI", 100925440)
{
    Scope (_SB)
    {
	External (DADD, MethodObj)
	External (GHCE, MethodObj)
	External (GHCN, MethodObj)
	External (GHCL, MethodObj)
	External (GNUS, MethodObj)
	External (GIOR, MethodObj)
	External (GMEM, MethodObj)
	External (GWBN, MethodObj)
	External (GBUS, MethodObj)

	External (PICF)

	External (\_SB.PCI1.LNKA, DeviceObj)
	External (\_SB.PCI1.LNKB, DeviceObj)
	External (\_SB.PCI1.LNKC, DeviceObj)
	External (\_SB.PCI1.LNKD, DeviceObj)
	

        Device (PCI2)
        {

	    // BUS ? Second HT Chain
	    Name (HCIN, 0x01)  // HC2

	    Name (_HID, "PNP0A03") 

            Method (_ADR, 0, NotSerialized) //Fake bus should be 0
	    {
		Return (DADD(GHCN(HCIN), 0x00180000))
	    }
	
            Name (_UID,  0x03) 

            Method (_BBN, 0, NotSerialized)
            {
                Return (GBUS (GHCN(HCIN), GHCL(HCIN)))
            }

            Method (_STA, 0, NotSerialized)
            {
                Return (\_SB.GHCE(HCIN)) 
            }

            Method (_CRS, 0, NotSerialized)
            {
                Name (BUF0, ResourceTemplate () { })
		Store( GHCN(HCIN), Local4)
		Store( GHCL(HCIN), Local5)

                Concatenate (\_SB.GIOR (Local4, Local5), BUF0, Local1)
                Concatenate (\_SB.GMEM (Local4, Local5), Local1, Local2)
                Concatenate (\_SB.GWBN (Local4, Local5), Local2, Local3)
                Return (Local3)
            }

	    Include ("pci2_hc.asl")
        }
    }

}

