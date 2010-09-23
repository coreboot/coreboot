/*
 * Minimalist ACPI DSDT table for EPIA-N / NL
 * Basic description of some hardware resources to allow
 * interrupt assignments to be done. This is expected to be included
 * into the PATA Device definition in ab_physical.asl
 * (C) Copyright 2009 Jon Harrison <jon.harrison@blueyonder.co.uk>
 *
 */

Name (TIM0, Package (0x07)
{
    Package (0x05)
    {
        0x78, 0xB4, 0xF0, 0x017F, 0x0258
    },

    Package (0x05)
    {
        0x20, 0x22, 0x33, 0x47, 0x5D
    },

    Package (0x05)
    {
        0x04, 0x03, 0x02, 0x01, 0x00
    },

    Package (0x04)
    {
        0x02, 0x01, 0x00, 0x00
    },

    Package (0x07)
    {
        0x78, 0x50, 0x3C, 0x2D, 0x1E, 0x14, 0x0F
    },

    Package (0x0F)
    {
        0x06, 0x05, 0x04, 0x04, 0x03, 0x03, 0x02, 0x02,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01,0x00
    },

    Package (0x07)
    {
        0x0E, 0x08, 0x06, 0x04, 0x02, 0x01, 0x00
    }
})


/* This method sets up the PATA Timing Control.
 * Note that a lot of this is done in the
 * coreboot VT8237R init code, but this is
 * already getting very cluttered with board
 * specific code. Using ACPI will allow this
 * to be de-cluttered a bit (so long as we're
 * running a ACPI capable OS!)
 */

Method (PMEX, 0, Serialized)
{
    If (REGF)
    {
		/* Check if these regs are still at defaults */
		/* Board specific timing improvement if not  */
		/* Already changed                           */
        If (LEqual (PMPT, 0xA8))
        {
            Store (0x5D, PMPT)
        }

        If (LEqual (PSPT, 0xA8))
        {
            Store (0x5D, PSPT)
        }

        If (LEqual (SMPT, 0xA8))
        {
            Store (0x5D, SMPT)
        }

        If (LEqual (SSPT, 0xA8))
        {
            Store (0x5D, SSPT)
        }

    }
}

/* This Method Provides the method that is used to */
/* Reset ATA Drives to POST reset condition        */
Method (GTF, 4, Serialized)
{
    Store (Buffer (0x07)
        {
            0x03, 0x00, 0x00, 0x00, 0x00, 0xA0, 0xEF
        }, Local1)
    Store (Buffer (0x07)
        {
            0x03, 0x00, 0x00, 0x00, 0x00, 0xA0, 0xEF
        }, Local2)
    CreateByteField (Local1, 0x01, MODE)
    CreateByteField (Local2, 0x01, UMOD)
    CreateByteField (Local1, 0x05, PCHA)
    CreateByteField (Local2, 0x05, UCHA)
    And (Arg0, 0x03, Local3)
    If (LEqual (And (Local3, 0x01), 0x01))
    {
        Store (0xB0, PCHA)
        Store (0xB0, UCHA)
    }

    If (Arg1)
    {
        Store (DerefOf (Index (DerefOf (Index (TIM0, 0x05)), Arg2)),
            UMOD)
        Or (UMOD, 0x40, UMOD)
    }
    Else
    {
        Store (Match (DerefOf (Index (TIM0, 0x01)), MEQ, Arg3, MTR,
            0x00, 0x00), Local0)
        Or (0x20, DerefOf (Index (DerefOf (Index (TIM0, 0x03)), Local0
            )), UMOD)
    }

    Store (Match (DerefOf (Index (TIM0, 0x01)), MEQ, Arg3, MTR,
        0x00, 0x00), Local0)
    Or (0x08, DerefOf (Index (DerefOf (Index (TIM0, 0x02)), Local0
        )), MODE)
    Concatenate (Local1, Local2, Local6)
    Return (Local6)
}

