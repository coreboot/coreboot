/*
 * Copyright 2005 AMD
 */
DefinitionBlock ("SSDT.aml", "SSDT", 1, "AMD-K8", "AMD-ACPI", 100925440)
{
    /*
     * These objects were referenced but not defined in this table
     */
    External (\_SB_.PCI0, DeviceObj)

    Scope (\_SB.PCI0)
    {
         Name (BUSN, Package (0x04)
        {
            0x11111111,
            0x22222222,
            0x33333333,
            0x44444444
        })
        Name (MMIO, Package (0x10)
        {
            0x11111111,
            0x22222222,
            0x33333333,
            0x44444444,
            0x55555555,
            0x66666666,
            0x77777777,
            0x88888888,
            0x99999999,
            0xaaaaaaaa,
            0xbbbbbbbb,
            0xcccccccc,
            0xdddddddd,
            0xeeeeeeee,
            0x11111111,
            0x22222222
        })
        Name (PCIO, Package (0x08)
        {
            0x77777777,
            0x88888888,
            0x99999999,
            0xaaaaaaaa,
            0xbbbbbbbb,
            0xcccccccc,
            0xdddddddd,
            0xeeeeeeee
        })
        Name (SBLK, 0x11)
        Name (TOM1, 0xaaaaaaaa)
	Name (SBDN, 0xbbbbbbbb)
	Name (HCLK, Package (0x08)
	{
	    0x11111111,
	    0x22222222,
	    0x33333333,
	    0x44444444,
            0x55555555,
            0x66666666,
            0x77777777,
            0x88888888
	})
        Name (HCDN, Package (0x08)
        {
            0x11111111,
            0x22222222,
            0x33333333,
            0x44444444,
            0x55555555,
            0x66666666,
            0x77777777,
            0x88888888
        })
    }
}

