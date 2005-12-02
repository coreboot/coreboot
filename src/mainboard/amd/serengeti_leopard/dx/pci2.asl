/*
 * Copyright 2005 AMD
 */
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

