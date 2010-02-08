/*
 * Minimalist ACPI DSDT table for EPIA-N / NL
 * Basic description of some hardware resources to allow
 * interrupt assignments to be done. This is expected to be included
 * into _SB.PCI0 namespace
 * (C) Copyright 2009 Jon Harrison <jon.harrison@blueyonder.co.uk>
 *
 */


/* Basic description of the VT8237R LPC Interface
 * PCI Configuration Space
 */

Device (VT8R)
{
    Name (_ADR, 0x00110000)
    OperationRegion (USBC, PCI_Config, 0x50, 0x02)
    Scope (\)
    {
    	Field (\_SB.PCI0.VT8R.USBC, ByteAcc, NoLock, Preserve)
    	{
    		IDEB,	8
    	}
    }

    OperationRegion (VTSB, PCI_Config, 0x00, 0xE8)
    Scope (\)
    {
    	Field (\_SB.PCI0.VT8R.VTSB, ByteAcc, NoLock, Preserve)
    	{
    				Offset (0x02),
    		DEID,	16,
    				Offset (0x2C),
    		ID2C,	8,
    		ID2D,	8,
    		ID2E,	8,
    		ID2F,	8,
    				Offset (0x44),
    		PIRE,	4,
    		PIRF,	4,
    		PIRG,	4,
    		PIRH,	4,
    		POLE,	1,
    		POLF,	1,
    		POLG,	1,
    		POLH,	1,
    		ENR8,	1,
    				Offset (0x50),
    		ESB4,	1,
    		ESB3,	1,
    		ESB2,	1,
    		EIDE,	1,
    		EUSB,	1,
    		ESB1,	1,
    		EAMC,	2,
    		EKBC,	1,
    		KBCC,	1,
    		EPS2,	1,
    		ERTC,	1,
    		ELAN,	1,
    			,	2,
    		USBD,	1,
    		SIRQ,	8,
    				Offset (0x55),
    		PIRA,	8,
    		PIBC,	8,
    		PIRD,	8,
    				Offset (0x75),
    		BSAT,	1,
    				Offset (0x94),
    		PWC1,	2,
    		GPO1,	1,
    		GPO2,	1,
    		GPO3,	1,
    		PLLD,	1
    	}
    }
}

/* Basic Description of Serial ATA Interface */
Device (SATA)
{
    Name (_ADR, 0x000F0000)
    Method (_STA, 0, NotSerialized)
    {
        If (LNotEqual (\_SB.PCI0.SATA.VID, 0x1106))
        {
            Return (0x00)
        }
        Else
        {
            If (LEqual (\_SB.PCI0.SATA.CMDR, 0x00))
            {
                Return (0x0D)
            }
            Else
            {
                Return (0x0F)
            }
        }
    }

    OperationRegion (SAPR, PCI_Config, 0x00, 0xC2)
    Field (SAPR, ByteAcc, NoLock, Preserve)
    {
        VID,    16,
                Offset (0x04),
        CMDR,   3,
                Offset (0x3C),
        IDEI,   8,
                Offset (0x49),
            ,   6,
        EPHY,   1
    }
}

/* Basic Description of Parallel ATA Interface */
/* An some initialisation of the interface     */
Device (PATA)
{
    Name (_ADR, 0x000F0001)
    Name (REGF, 0x01)
    Method (_STA, 0, NotSerialized)
    {
        If (LNotEqual (\_SB.PCI0.PATA.VID, 0x1106))
        {
            Return (0x00)
        }
        Else
        {
            PMEX ()
			/* Check if the Interface is Enabled */
            If (LEqual (\_SB.PCI0.PATA.CMDR, 0x00))
            {
                Return (0x0D)
            }
            Else
            {
                Return (0x0F)
            }
        }
    }

	/* ACPI Spec says to check that regions are accessible */
	/* before trying to access them                        */
    Method (_REG, 2, NotSerialized)
    {
		/* Arg0 = Operating Region (0x02 == PCI_Config) */
        If (LEqual (Arg0, 0x02))
        {
			/* Arg1 = Handler Connection Mode (0x01 == Connect) */
            Store (Arg1, REGF)
        }
    }

	#include "pata_methods.asl"


    OperationRegion (PAPR, PCI_Config, 0x00, 0xC2)
    Field (PAPR, ByteAcc, NoLock, Preserve)
    {
        VID,    16,
                Offset (0x04),
        CMDR,   3,
                Offset (0x09),
        ENAT,   4,
                Offset (0x3C),
        IDEI,   8,
                Offset (0x40),
        ESCH,   1,
        EPCH,   1,
                Offset (0x48),
        SSPT,   8,
        SMPT,   8,
        PSPT,   8,
        PMPT,   8,
                Offset (0x50),
        SSUT,   4,
        SSCT,   1,
        SSUE,   3,
        SMUT,   4,
        SMCT,   1,
        SMUE,   3,
        PSUT,   4,
        PSCT,   1,
        PSUE,   3,
        PMUT,   4,
        PMCT,   1,
        PMUE,   3
    }


    Device (CHN0)
    {
        Name (_ADR, 0x00)
        Method (_STA, 0, NotSerialized)
        {
            If (LNotEqual (\_SB.PCI0.PATA.EPCH, 0x01))
            {
                Return (0x00)
            }
            Else
            {
                Return (0x0F)
            }
        }

        Device (DRV0)
        {
            Name (_ADR, 0x00)
            Method (_GTF, 0, NotSerialized)
            {
                Return (GTF (0x00, PMUE, PMUT, PMPT))
            }
        }

        Device (DRV1)
        {
            Name (_ADR, 0x01)
            Method (_GTF, 0, NotSerialized)
            {
                Return (GTF (0x01, PSUE, PSUT, PSPT))
            }
        }
    }

    Device (CHN1)
    {
        Name (_ADR, 0x01)
        Method (_STA, 0, NotSerialized)
        {
            If (LNotEqual (ATFL, 0x02))
            {
                If (LEqual (\_SB.PCI0.SATA.EPHY, 0x01))
                {
                    Return (0x00)
                }
                Else
                {
                    If (LNotEqual (\_SB.PCI0.PATA.ESCH, 0x01))
                    {
                        Return (0x00)
                    }
                    Else
                    {
                        Return (0x0F)
                    }
                }
            }
			Else
			{
               If (LEqual (ATFL, 0x02))
               {
                   If (LNotEqual (\_SB.PCI0.PATA.ESCH, 0x01))
                   {
                	   Return (0x00)
                   }
                   Else
                   {
                	   Return (0x0F)
                   }
               }
			   Else
			   {
			   		Return(0x00)
			   }
			}
        }

        Device (DRV0)
        {
            Name (_ADR, 0x00)
            Method (_GTF, 0, NotSerialized)
            {
                Return (GTF (0x02, SMUE, SMUT, SMPT))
            }
        }

        Device (DRV1)
        {
            Name (_ADR, 0x01)
            Method (_GTF, 0, NotSerialized)
            {
                Return (GTF (0x03, SSUE, SSUT, SSPT))
            }
        }
    }
} // End of PATA Device


/* Implement Basic USB Presence detect and */
/* Power Management Event mask             */
Device (USB0)
{
    Name (_ADR, 0x00100000)
    Name (_PRW, Package (0x02)
    {
        0x0E,
        0x03
    })

    OperationRegion (U2F0, PCI_Config, 0x00, 0xC2)
    Field (U2F0, ByteAcc, NoLock, Preserve)
    {
        VID,    16,
                Offset (0x04),
        CMDR,   3,
                Offset (0x3C),
        U0IR,   4,
                Offset (0x84),
        ECDX,   2
    }

    Method (_STA, 0, NotSerialized)
    {
        If (LNotEqual (\_SB.PCI0.USB0.VID, 0x1106))
        {
            Return (0x00)
        }
        Else
        {
            If (LEqual (\_SB.PCI0.USB0.CMDR, 0x00))
            {
                Return (0x0D)
            }
            Else
            {
                Return (0x0F)
            }
        }
    }
}

Device (USB1)
{
    Name (_ADR, 0x00100001)
    Name (_PRW, Package (0x02)
    {
        0x0E,
        0x03
    })

    OperationRegion (U2F1, PCI_Config, 0x00, 0xC2)
    Field (U2F1, ByteAcc, NoLock, Preserve)
    {
        VID,    16,
                Offset (0x04),
        CMDR,   3,
                Offset (0x3C),
        U1IR,   4,
                Offset (0x84),
        ECDX,   2
    }

    Method (_STA, 0, NotSerialized)
    {
        If (LNotEqual (\_SB.PCI0.USB1.VID, 0x1106))
        {
            Return (0x00)
        }
        Else
        {
            If (LEqual (\_SB.PCI0.USB1.CMDR, 0x00))
            {
                Return (0x0D)
            }
            Else
            {
                Return (0x0F)
            }
        }
    }
}

Device (USB2)
{
    Name (_ADR, 0x00100002)
    Name (_PRW, Package (0x02)
    {
        0x0E,
        0x03
    })

    OperationRegion (U2F2, PCI_Config, 0x00, 0xC2)
    Field (U2F2, ByteAcc, NoLock, Preserve)
    {
        VID,    16,
                Offset (0x04),
        CMDR,   3,
                Offset (0x3C),
        U2IR,   4,
                Offset (0x84),
        ECDX,   2
    }

    Method (_STA, 0, NotSerialized)
    {
        If (LNotEqual (\_SB.PCI0.USB2.VID, 0x1106))
        {
            Return (0x00)
        }
        Else
        {
            If (LEqual (\_SB.PCI0.USB2.CMDR, 0x00))
            {
                Return (0x0D)
            }
            Else
            {
                Return (0x0F)
            }
        }
    }
}

Device (USB3)
{
    Name (_ADR, 0x00100003)
    Name (_PRW, Package (0x02)
    {
        0x0E,
        0x03
    })

    OperationRegion (U2F3, PCI_Config, 0x00, 0xC2)
    Field (U2F3, ByteAcc, NoLock, Preserve)
    {
        VID,    16,
                Offset (0x04),
        CMDR,   3,
                Offset (0x3C),
        U3IR,   4,
                Offset (0x84),
        ECDX,   2
    }

    Method (_STA, 0, NotSerialized)
    {
        If (LNotEqual (\_SB.PCI0.USB3.VID, 0x1106))
        {
            Return (0x00)
        }
        Else
        {
            If (LEqual (\_SB.PCI0.USB3.CMDR, 0x00))
            {
                Return (0x0D)
            }
            Else
            {
                Return (0x0F)
            }
        }
    }
}

Device (USB4)
{
    Name (_ADR, 0x00100004)
    Name (_PRW, Package (0x02)
    {
        0x0E,
        0x03
    })

    OperationRegion (U2F4, PCI_Config, 0x00, 0xC2)
    Field (U2F4, ByteAcc, NoLock, Preserve)
    {
        VID,    16,
                Offset (0x04),
        CMDR,   3,
                Offset (0x3C),
        U4IR,   4,
                Offset (0x84),
        ECDX,   2
    }

    Method (_STA, 0, NotSerialized)
    {
        If (LNotEqual (\_SB.PCI0.USB4.VID, 0x1106))
        {
            Return (0x00)
        }
        Else
        {
            If (LEqual (\_SB.PCI0.USB4.CMDR, 0x00))
            {
                Return (0x0D)
            }
            Else
            {
                Return (0x0F)
            }
        }
    }
}

/* Basic Definition of Ethernet Interface */
Device (NIC0)
{
    Name (_ADR, 0x00120000)
    Name (_PRW, Package (0x02)
    {
        0x03,
        0x05
    })

    OperationRegion (NIC0, PCI_Config, 0x00, 0xC2)
    Field (NIC0, ByteAcc, NoLock, Preserve)
    {
        VID,    16,
                Offset (0x04),
        CMDR,   3,
                Offset (0x3C),
        NIIR,   4,
    }

    Method (_STA, 0, NotSerialized)
    {
        If (LNotEqual (\_SB.PCI0.NIC0.VID, 0x1106))
        {
            Return (0x00)
        }
        Else
        {
            If (LEqual (\_SB.PCI0.NIC0.CMDR, 0x00))
            {
                Return (0x0D)
            }
            Else
            {
                Return (0x0F)
            }
        }
    }
}

/* Very Basic Definition of Sound Controller */
Device (AC97)
{
    Name (_ADR, 0x00110005)
    Name (_PRW, Package (0x02)
    {
        0x0D,
        0x05
    })
}
