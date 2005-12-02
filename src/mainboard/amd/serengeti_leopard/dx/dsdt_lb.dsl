/*
 * Copyright 2005 AMD
 */
DefinitionBlock ("DSDT.aml", "DSDT", 1, "AMD-K8", "AMDACPI", 100925440)
{
    Scope (_PR)
    {
        Processor (CPU0, 0x00, 0x0000C010, 0x06) {}
        Processor (CPU1, 0x01, 0x00000000, 0x00) {}
        Processor (CPU2, 0x02, 0x00000000, 0x00) {}
        Processor (CPU3, 0x03, 0x00000000, 0x00) {}

    }

    Method (FWSO, 0, NotSerialized) { }

    Name (_S0, Package (0x04) { 0x00, 0x00, 0x00, 0x00 })
    Name (_S1, Package (0x04) { 0x01, 0x01, 0x01, 0x01 })
    Name (_S3, Package (0x04) { 0x05, 0x05, 0x05, 0x05 })
    Name (_S5, Package (0x04) { 0x07, 0x07, 0x07, 0x07 })

    Scope (_SB)
    {
        Device (PCI0)
        {
	    /* BUS0 root bus */

/*
//hardcode begin
            Name (BUSN, Package (0x04) { 0x04010003, 0x06050013, 0x00000000, 0x00000000 })
            Name (MMIO, Package (0x10) { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
                                         0x00f43003, 0x00f44f01, 0x0000d003, 0x00efff01, 0x00f40003, 0x00f42f00, 0x00f45003, 0x00f44f00 })
            Name (PCIO, Package (0x08) { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00001003, 0x00001000, 0x00002003, 0x00002001 })
            Name (SBLK, 0x00)
            Name (TOM1, 0x40000000)

	    // for AMD opteron we could have four chains, so we will have PCI1, PCI2, PCI3, PCI4
	    // PCI1 must be SBLK Chain
	    // If you have HT IO card that is connected to PCI2, PCI3, PCI4, then you man put Device in SSDT2, SSDT3, SSDT4,  
	    //    in acpi_tables.c you can link those SSDT to RSDT according to it's presence.
	    // Otherwise put the PCI2, PCI3, PCI4 in this dsdt
    	    Name (HCLK, Package (0x04) { 0x00000001, 0x00000011, 0x00000000, 0x00000000 }) //[0,3]=1 enable [4,7]=node_id, [8,15]=linkn

	    Name (SBDN, 3)  // 8111 UnitID Base
//hardcode end
*/
	    External (BUSN)
	    External (MMIO)
	    External (PCIO)
	    External (SBLK)
	    External (TOM1)
	    External (HCLK)
	    External (SBDN)

            Name (_HID, EisaId ("PNP0A03"))
            Name (_ADR, 0x00180000)
            Name (_UID, 0x01)
	    Name (_BBN, 0)


	    // define L1IC Link1 on node0 init completed, so node1 is installed
	    // We must make sure our bus is 0 ? 
            OperationRegion (LDT1, PCI_Config, 0xA4, 0x01)
            Field (LDT1, ByteAcc, Lock, Preserve)
            {
                    ,   5,
                L1IC,   1
            }

	}
	
	Device (PCI1)
	{
	    // BUS 1 first HT Chain
            Name (_HID, EisaId ("PNP0A03"))
            Name (_ADR, 0x00180000)  // Fake 
            Name (_UID, 0x02)
            Method (_BBN, 0, NotSerialized)
            {
                Return (GBUS (0x00, \_SB.PCI0.SBLK))
            }

            Method (_CRS, 0, NotSerialized)
            {
                Name (BUF0, ResourceTemplate ()
                {
                    IO (Decode16, 0x0CF8, 0x0CF8, 0x01, 0x08) //CF8-CFFh
                    IO (Decode16, 0xC000, 0xC000, 0x01, 0x80) //8000h
                    IO (Decode16, 0xC080, 0xC080, 0x01, 0x80) //8080h

                    WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                        0x0000, // Address Space Granularity
                        0x8100, // Address Range Minimum
                        0xFFFF, // Address Range Maximum
                        0x0000, // Address Translation Offset
                        0x7F00,,,
                        , TypeStatic)    //8100h-FFFFh

                    DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                        0x00000000, // Address Space Granularity
                        0x000C0000, // Address Range Minimum
                        0x00000000, // Address Range Maximum
                        0x00000000, // Address Translation Offset
                        0x00000000,,,
                        , AddressRangeMemory, TypeStatic)   //Video BIOS A0000h-C7FFFh

                    Memory32Fixed (ReadWrite, 0x000D8000, 0x00004000)//USB HC D8000-DBFFF

                    WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                        0x0000, // Address Space Granularity
                        0x0000, // Address Range Minimum
                        0x03AF, // Address Range Maximum
                        0x0000, // Address Translation Offset
                        0x03B0,,,
                        , TypeStatic)  //0-CF7h

                    WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                        0x0000, // Address Space Granularity
                        0x03E0, // Address Range Minimum
                        0x0CF7, // Address Range Maximum
                        0x0000, // Address Translation Offset
                        0x0918,,,
                        , TypeStatic)  //0-CF7h
                })
                \_SB.OSTP ()
                CreateDWordField (BUF0, 0x3E, VLEN)
                CreateDWordField (BUF0, 0x36, VMAX)
                CreateDWordField (BUF0, 0x32, VMIN)
                ShiftLeft (VGA1, 0x09, Local0)
                Add (VMIN, Local0, VMAX)
                Decrement (VMAX)
                Store (Local0, VLEN)
                Concatenate (\_SB.GMEM (0x00, \_SB.PCI0.SBLK), BUF0, Local1)
                Concatenate (\_SB.GIOR (0x00, \_SB.PCI0.SBLK), Local1, Local2)
                Concatenate (\_SB.GWBN (0x00, \_SB.PCI0.SBLK), Local2, Local3)
                Return (Local3) 
	    }

	    Include ("pci1_hc.asl")
		
        }
/*
        Device (PCI2)
        {

	    // BUS ? Second HT Chain
	    Name (HCIN, 0x01)  // HC2

	    Name (_HID, "PNP0A03") 

            Method (_ADR, 0, NotSerialized) //Fake bus should be 0
	    {
		Return (DADD(GHCN(HCIN), 0x00180000))
	    }
            Name (_UID, 0x03)

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
*/

    }

    Scope (_GPE)
    {
        Method (_L08, 0, NotSerialized)
        {
            Notify (\_SB.PCI1, 0x02) //PME# Wakeup
        }

        Method (_L0F, 0, NotSerialized)
        {
            Notify (\_SB.PCI1.TP2P.USB0, 0x02)  //USB Wakeup
        }

        Method (_L22, 0, NotSerialized) // GPIO18 (LID) - Pogo 0 Bridge B
        {
            Notify (\_SB.PCI1.PG0B, 0x02)
        }

        Method (_L29, 0, NotSerialized) // GPIO25 (Suspend) - Pogo 0 Bridge A 
        {
            Notify (\_SB.PCI1.PG0A, 0x02)
        }
    }

    Method (_PTS, 1, NotSerialized)
    {
        Or (Arg0, 0xF0, Local0)
        Store (Local0, DBG1)
    }
/*
    Method (_WAK, 1, NotSerialized)
    {
        Or (Arg0, 0xE0, Local0)
        Store (Local0, DBG1)
    }
*/
    Name (PICF, 0x00) //Flag Variable for PIC vs. I/O APIC Mode
    Method (_PIC, 1, NotSerialized) //PIC Flag and Interface Method
    {
        Store (Arg0, PICF)
    }

    OperationRegion (DEBG, SystemIO, 0x80, 0x01)
    Field (DEBG, ByteAcc, Lock, Preserve)
    {
        DBG1,   8
    }

    OperationRegion (EXTM, SystemMemory, 0x000FF83C, 0x04)
    Field (EXTM, WordAcc, Lock, Preserve)
    {
        AMEM,   32
    }

    OperationRegion (VGAM, SystemMemory, 0x000C0002, 0x01)
    Field (VGAM, ByteAcc, Lock, Preserve)
    {
        VGA1,   8
    }

    OperationRegion (GRAM, SystemMemory, 0x0400, 0x0100)
    Field (GRAM, ByteAcc, Lock, Preserve)
    {
        Offset (0x10), 
        FLG0,   8
    }

    OperationRegion (GSTS, SystemIO, 0xC028, 0x02)
    Field (GSTS, ByteAcc, NoLock, Preserve)
    {
            ,   4, 
        IRQR,   1
    }

    OperationRegion (Z007, SystemIO, 0x21, 0x01)
    Field (Z007, ByteAcc, NoLock, Preserve)
    {
        Z008,   8
    }

    OperationRegion (Z009, SystemIO, 0xA1, 0x01)
    Field (Z009, ByteAcc, NoLock, Preserve)
    {
        Z00A,   8
    }

    Include ("amdk8_util.asl")

}

