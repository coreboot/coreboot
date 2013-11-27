/*
 * Copyright 2005 AMD
 */
DefinitionBlock ("SSDT3.aml", "SSDT", 1, "AMD-K8", "AMDACPI", 100925440)
{
    Scope (_SB)
    {
	External (DADD, MethodObj)
	External (GHCE, MethodObj)
	External (GHCN, MethodObj)
	External (GHCL, MethodObj)
	External (GHCD, MethodObj)
	External (GNUS, MethodObj)
	External (GIOR, MethodObj)
	External (GMEM, MethodObj)
	External (GWBN, MethodObj)
	External (GBUS, MethodObj)

	External (PICF)

	External (\_SB.PCI0.LNKA, DeviceObj)
	External (\_SB.PCI0.LNKB, DeviceObj)
	External (\_SB.PCI0.LNKC, DeviceObj)
	External (\_SB.PCI0.LNKD, DeviceObj)

	Device (PCIX)
	{

	    // BUS ? Second HT Chain
	    Name (HCIN, 0xcc)  // HC2 0x01

	    Name (_UID,  0xdd)  // HC 0x03

	    Name (_HID, "PNP0A03")

	    Method (_ADR, 0, NotSerialized) //Fake bus should be 0
	    {
		Return (DADD(GHCN(HCIN), 0x00000000))
	    }

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

	    #include "acpi/pci3_hc.asl"
	}
    }

}

