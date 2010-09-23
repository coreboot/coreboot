/*
 * Minimalist ACPI DSDT table for EPIA-N / NL
 * Basic description of PCI Interrupt Assignments.
 * This is expected to be included into _SB.PCI0 namespace
 * (C) Copyright 2009 Jon Harrison <jon.harrison@blueyonder.co.uk>
 *
 */

 /* PCI PnP Routing Links */

 /* Define how interrupt Link A is plumbed in */
 Device (LNKA)
 {
	 Name (_HID, EisaId ("PNP0C0F"))
	 Name (_UID, 0x01)
 	 /* Status - always return ready */
	 Method (_STA, 0, NotSerialized)
	 {
	 	/* See if coreboot has allocated INTA# */
	 	And (PIRA, 0xF0, Local0)
		If (LEqual (Local0, 0x00))
		{
			Return (0x09)
		}
		Else
		{
		 	Return (0x0B)
		}
	 }

	 Method (_PRS, 0, NotSerialized)
	 {
		 Name (BUFA, ResourceTemplate ()
		 {
			 IRQ (Level, ActiveLow, Shared, )
				 {3,4,6,7,10,11,12}
		 })
		 Return (BUFA)
	 }

	 Method (_CRS, 0, NotSerialized)
	 {
		 Name (BUFA, ResourceTemplate ()
		 {
			 IRQ (Level, ActiveLow, Shared, _Y07)
			 {}
		 })
		 /* Read the Binary Encoded Field and Map this        */
		 /* onto the bitwise _INT field in the IRQ descriptor */
		 /* See ACPI Spec for detail of _IRQ Descriptor       */
         CreateByteField (BUFA, \_SB.PCI0.LNKA._CRS._Y07._INT, IRA1)
         CreateByteField (BUFA, 0x02, IRA2)
         Store (0x00, Local3)
         Store (0x00, Local4)
         And (PIRA, 0xF0, Local1)
         ShiftRight (Local1, 0x04, Local1)
         If (LNotEqual (Local1, 0x00))
         {
         	 If (LGreater (Local1, 0x07))
         	 {
         		 Subtract (Local1, 0x08, Local2)
         		 ShiftLeft (One, Local2, Local4)
         	 }
         	 Else
         	 {
         		 If (LGreater (Local1, 0x00))
         		 {
         			 ShiftLeft (One, Local1, Local3)
         		 }
         	 }

         	 Store (Local3, IRA1)
         	 Store (Local4, IRA2)
         }
		 Return (BUFA)
	 }

 	 /* Set Resources - dummy function to keep Linux ACPI happy
	  * Linux is more than happy not to tinker with irq
 	  * assignments as long as the CRS and STA functions
 	  * return good values
 	 */
	 Method (_SRS, 1, NotSerialized) {}
 	 /* Disable - Set PnP Routing Reg to 0 */
 	 Method (_DIS, 0, NotSerialized )
	 {
	 	And (PIRA, 0x0F, PIRA)
	 }
 } // End of LNKA

 Device (LNKB)
 {
	 Name (_HID, EisaId ("PNP0C0F"))
	 Name (_UID, 0x02)
	 Method (_STA, 0, NotSerialized)
	 {
	 	/* See if coreboot has allocated INTB# */
	 	And (PIBC, 0x0F, Local0)
		If (LEqual (Local0, 0x00))
		{
			Return (0x09)
		}
		Else
		{
		 	Return (0x0B)
		}
	 }

	 Method (_PRS, 0, NotSerialized)
	 {
		 Name (BUFB, ResourceTemplate ()
		 {
			 IRQ (Level, ActiveLow, Shared, )
				 {3,4,6,7,10,11,12}
		 })
		 Return (BUFB)
	 }

	 Method (_CRS, 0, NotSerialized)
	 {
		 Name (BUFB, ResourceTemplate ()
		 {
			 IRQ (Level, ActiveLow, Shared, _Y08)
			 {}
		 })
		 /* Read the Binary Encoded Field and Map this        */
		 /* onto the bitwise _INT field in the IRQ descriptor */
		 /* See ACPI Spec for detail of _IRQ Descriptor       */
         CreateByteField (BUFB, \_SB.PCI0.LNKB._CRS._Y08._INT, IRB1)
         CreateByteField (BUFB, 0x02, IRB2)
         Store (0x00, Local3)
         Store (0x00, Local4)
         And (PIBC, 0x0F, Local1)
         If (LNotEqual (Local1, 0x00))
         {
         	 If (LGreater (Local1, 0x07))
         	 {
         		 Subtract (Local1, 0x08, Local2)
         		 ShiftLeft (One, Local2, Local4)
         	 }
         	 Else
         	 {
         		 If (LGreater (Local1, 0x00))
         		 {
         			 ShiftLeft (One, Local1, Local3)
         		 }
         	 }

         	 Store (Local3, IRB1)
         	 Store (Local4, IRB2)
         }
		 Return (BUFB)
	 }

 	 /* Set Resources - dummy function to keep Linux ACPI happy
	  * Linux is more than happy not to tinker with irq
 	  * assignments as long as the CRS and STA functions
 	  * return good values
 	 */
	 Method (_SRS, 1, NotSerialized) {}
 	 /* Disable - Set PnP Routing Reg to 0 */
 	 Method (_DIS, 0, NotSerialized )
	 {
	 	And (PIBC, 0xF0, PIBC)
	 }

 } // End of LNKB

 Device (LNKC)
 {
	 Name (_HID, EisaId ("PNP0C0F"))
	 Name (_UID, 0x03)
	 Method (_STA, 0, NotSerialized)
	 {
	 	/* See if coreboot has allocated INTC# */
	 	And (PIBC, 0xF0, Local0)
		If (LEqual (Local0, 0x00))
		{
			Return (0x09)
		}
		Else
		{
		 	Return (0x0B)
		}
	 }

	 Method (_PRS, 0, NotSerialized)
	 {
		 Name (BUFC, ResourceTemplate ()
		 {
			 IRQ (Level, ActiveLow, Shared, )
				 {3,4,6,7,10,11,12}
		 })
		 Return (BUFC)
	 }

	 Method (_CRS, 0, NotSerialized)
	 {
		 Name (BUFC, ResourceTemplate ()
		 {
			 IRQ (Level, ActiveLow, Shared, _Y09)
			 {}
		 })
		 /* Read the Binary Encoded Field and Map this        */
		 /* onto the bitwise _INT field in the IRQ descriptor */
		 /* See ACPI Spec for detail of _IRQ Descriptor       */
         CreateByteField (BUFC, \_SB.PCI0.LNKC._CRS._Y09._INT, IRC1)
         CreateByteField (BUFC, 0x02, IRC2)
         Store (0x00, Local3)
         Store (0x00, Local4)
         And (PIBC, 0xF0, Local1)
         ShiftRight (Local1, 0x04, Local1)
         If (LNotEqual (Local1, 0x00))
         {
         	 If (LGreater (Local1, 0x07))
         	 {
         		 Subtract (Local1, 0x08, Local2)
         		 ShiftLeft (One, Local2, Local4)
         	 }
         	 Else
         	 {
         		 If (LGreater (Local1, 0x00))
         		 {
         			 ShiftLeft (One, Local1, Local3)
         		 }
         	 }

         	 Store (Local3, IRC1)
         	 Store (Local4, IRC2)
         }
		 Return (BUFC)
	 }

 	 /* Set Resources - dummy function to keep Linux ACPI happy
	  * Linux is more than happy not to tinker with irq
 	  * assignments as long as the CRS and STA functions
 	  * return good values
 	 */
	 Method (_SRS, 1, NotSerialized) {}
 	 /* Disable - Set PnP Routing Reg to 0 */
 	 Method (_DIS, 0, NotSerialized )
	 {
	 	And (PIBC, 0x0F, PIBC)
	 }

} // End of LNKC

Device (LNKD)
{
	 Name (_HID, EisaId ("PNP0C0F"))
	 Name (_UID, 0x04)
	 Method (_STA, 0, NotSerialized)
	 {
	 	/* See if coreboot has allocated INTD# */
	 	And (PIRD, 0xF0, Local0)
		If (LEqual (Local0, 0x00))
		{
			Return (0x09)
		}
		Else
		{
		 	Return (0x0B)
		}
	 }

	 Method (_PRS, 0, NotSerialized)
	 {
		 Name (BUFD, ResourceTemplate ()
		 {
			 IRQ (Level, ActiveLow, Shared, )
				 {3,4,6,7,10,11,12}
		 })
		 Return (BUFD)
	 }

	 Method (_CRS, 0, NotSerialized)
	 {
		 Name (BUFD, ResourceTemplate ()
		 {
			 IRQ (Level, ActiveLow, Shared, _Y0A)
			 {}
		 })
		 /* Read the Binary Encoded Field and Map this        */
		 /* onto the bitwise _INT field in the IRQ descriptor */
		 /* See ACPI Spec for detail of _IRQ Descriptor       */
         CreateByteField (BUFD, \_SB.PCI0.LNKD._CRS._Y0A._INT, IRD1)
         CreateByteField (BUFD, 0x02, IRD2)
         Store (0x00, Local3)
         Store (0x00, Local4)
         And (PIRD, 0xF0, Local1)
         ShiftRight (Local1, 0x04, Local1)
         If (LNotEqual (Local1, 0x00))
         {
         	 If (LGreater (Local1, 0x07))
         	 {
         		 Subtract (Local1, 0x08, Local2)
         		 ShiftLeft (One, Local2, Local4)
         	 }
         	 Else
         	 {
         		 If (LGreater (Local1, 0x00))
         		 {
         			 ShiftLeft (One, Local1, Local3)
         		 }
         	 }

         	 Store (Local3, IRD1)
         	 Store (Local4, IRD2)
         }
		 Return (BUFD)
	 }

 	 /* Set Resources - dummy function to keep Linux ACPI happy
	  * Linux is more than happy not to tinker with irq
 	  * assignments as long as the CRS and STA functions
 	  * return good values
 	 */
	 Method (_SRS, 1, NotSerialized) {}
 	 /* Disable - Set PnP Routing Reg to 0 */
 	 Method (_DIS, 0, NotSerialized )
	 {
	 	And (PIRD, 0x0F, PIRD)
	 }

} // End of LNKD


/* APIC IRQ Links */

Device (ATAI)
{
	 Name (_HID, EisaId ("PNP0C0F"))
	 Name (_UID, 0x05)
	 Method (_STA, 0, NotSerialized)
	 {
	 	/* ATFL == 0x02 if SATA Enabled */
      	If (LNotEqual (ATFL, 0x02))
      	{
			/* Double Check By Reading SATA VID */
			/* Otherwise Compatibility Mode     */
        	If (LNotEqual (\_SB.PCI0.SATA.VID, 0x1106))
          	{
        	  	Return (0x09)
          	}
          	Else
          	{
        	  	Return (0x0B)
          	}
      	}
      	Else
      	{
			/* Serial ATA Enabled Check if PATA is in */
			/* Compatibility Mode 					  */
          	If (LEqual (\_SB.PCI0.PATA.ENAT, 0x0A))
          	{
        	  	Return (0x09)
          	}
          	Else
          	{
        	  	Return (0x0B)
          	}
      	}
	 }

     Method (_PRS, 0, NotSerialized)
     {
          Name (ATAN, ResourceTemplate ()
          {
              Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, )
              {
            	  0x00000014,
              }
          })
		  Return (ATAN)
	 }

     Method (_CRS, 0, NotSerialized)
     {
     	Name (ATAB, ResourceTemplate ()
        {
            Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, _Y10)
            {
                0x00000000,
            }
        })
        CreateByteField (ATAB, \_SB.PCI0.ATAI._CRS._Y10._INT, IRAI)
        Store (0x14, IRAI)
        Return (ATAB)

	 }

 	 /* Set Resources - dummy function to keep Linux ACPI happy
	  * Linux is more than happy not to tinker with irq
 	  * assignments as long as the CRS and STA functions
 	  * return good values
 	 */
	 Method (_SRS, 1, NotSerialized) {}
 	 /* Disable - dummy function to keep Linux ACPI happy */
 	 Method (_DIS, 0, NotSerialized ) {}

} // End of ATA Interface Link


Device (USBI)
{
	Name (_HID, EisaId ("PNP0C0F"))
    Name (_UID, 0x0A)
    Method (_STA, 0, NotSerialized)
    {
		/* Check that at least one of the USB */
		/* functions is enabled               */
         And (IDEB, 0x37, Local0)
         If (LEqual (Local0, 0x37))
         {
         	 Return (0x09)
         }
         Else
         {
         	 Return (0x0B)
         }
	}

    Method (_PRS, 0, NotSerialized)
    {
        Name (USBB, ResourceTemplate ()
        {
            Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, )
            {
                0x00000015,
            }
        })

		Return(USBB)
	}

    Method (_CRS, 0, NotSerialized)
    {
        Name (USBB, ResourceTemplate ()
        {
            Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, _Y12)
            {
                0x00000000,
            }
        })
        CreateByteField (USBB, \_SB.PCI0.USBI._CRS._Y12._INT, IRBI)
        Store (0x15, IRBI)
        Return (USBB)
	}


 	/* Set Resources - dummy function to keep Linux ACPI happy
	 * Linux is more than happy not to tinker with irq
 	 * assignments as long as the CRS and STA functions
 	 * return good values
 	*/
	Method (_SRS, 1, NotSerialized) {}
 	/* Disable - dummy function to keep Linux ACPI happy */
 	Method (_DIS, 0, NotSerialized ) {}
}

Device (VT8I)
{
    Name (_HID, EisaId ("PNP0C0F"))
    Name (_UID, 0x0B)
    Method (_STA, 0, NotSerialized)
    {
		/* Check Whether Sound and/or Modem are Activated */
        If (LEqual (EAMC, 0x03))
        {
            Return (0x09)
        }
        Else
        {
            Return (0x0B)
        }
    }

    Method (_PRS, 0, NotSerialized)
    {
        Name (A97C, ResourceTemplate ()
        {
            Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, )
            {
                0x00000016,
            }
        })
        Return (A97C)
	}

    Method (_CRS, 0, NotSerialized)
    {
        Name (A97B, ResourceTemplate ()
        {
            Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, _Y14)
            {
                0x00000000,
            }
        })
        CreateByteField (A97B, \_SB.PCI0.VT8I._CRS._Y14._INT, IRCI)
        Store (0x16, IRCI)
        Return (A97B)
	}

 	/* Set Resources - dummy function to keep Linux ACPI happy
	 * Linux is more than happy not to tinker with irq
 	 * assignments as long as the CRS and STA functions
 	 * return good values
 	*/
	Method (_SRS, 1, NotSerialized) {}
 	/* Disable - dummy function to keep Linux ACPI happy */
 	Method (_DIS, 0, NotSerialized ) {}

}


Device (NICI)
{
    Name (_HID, EisaId ("PNP0C0F"))
    Name (_UID, 0x0C)
    Method (_STA, 0, NotSerialized)
    {
		/* Check if LAN Function is Enabled           */
		/* Note that LAN Enable Polarity is different */
		/* from other functions in VT8237R !?         */
    	If (LEqual (ELAN, 0x00))
    	{
    		Return (0x09)
    	}
    	Else
    	{
    		Return (0x0B)
    	}
	}

    Method (_PRS, 0, NotSerialized)
    {
        Name (NICB, ResourceTemplate ()
        {
            Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, )
            {
                0x00000017,
            }
        })
        Return (NICB)
	}

    Method (_CRS, 0, NotSerialized)
    {
        Name (NICD, ResourceTemplate ()
        {
            Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, _Y16)
            {
                0x00000000,
            }
        })
        CreateByteField (NICD, \_SB.PCI0.NICI._CRS._Y16._INT, IRDI)
        Store (0x17, IRDI)
        Return (NICD)
	}

  	/* Set Resources - dummy function to keep Linux ACPI happy
	 * Linux is more than happy not to tinker with irq
 	 * assignments as long as the CRS and STA functions
 	 * return good values
 	*/
	Method (_SRS, 1, NotSerialized) {}
 	/* Disable - dummy function to keep Linux ACPI happy */
 	Method (_DIS, 0, NotSerialized ) {}


}
