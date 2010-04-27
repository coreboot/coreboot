/*
 * Minimalist ACPI DSDT table for EPIA-N / NL
 * (C) Copyright 2009 Jon Harrison <jon.harrison@blueyonder.co.uk>
 * Heavily based on EPIA-M dstd.asl
 * (C) Copyright 2004 Nick Barker <Nick.Barker9@btinternet.com>
 *
 */
DefinitionBlock ("dsdt.aml", "DSDT", 1, "CBT-V2", "CBT-DSDT", 1)
{
    Scope (\_PR)
    {
        Processor (\_PR.CPU0, 0x00, 0x00000000, 0x00) {}
    }

	/* For now only define 2 power states:
	 *  - S0 which is fully on
	 *  - S5 which is soft off
	 * any others would involve declaring the wake up methods
	 */
    Name (\_S0, Package (0x04)
    {
        0x00,
        0x00,
        0x00,
        0x00
    })
    Name (\_S5, Package (0x04)
    {
        0x02,
        0x02,
        0x02,
        0x02
    })

	/* Global Flag Used to Indicate State of */
	/* ATA Interface                         */
    Name (ATFL, 0x00)

	/* Root of the bus hierarchy */
	Scope (\_SB)
    {

       Device (PCI0)
       {
           Name (_HID, EisaId ("PNP0A03"))
           Name (_ADR, 0x00)
           Name (_UID, 0x01)
           Name (_BBN, 0x00)

	       /* PCI Routing Table */
	       Name (_PRT, Package () {

        	   Package (0x04) {0x000FFFFF, 0x00, ATAI, 0x00}, // SATA Link A
        	   Package (0x04) {0x000FFFFF, 0x01, ATAI, 0x00}, // SATA Link B
        	   Package (0x04) {0x000FFFFF, 0x02, ATAI, 0x00}, // SATA Link C
        	   Package (0x04) {0x000FFFFF, 0x03, ATAI, 0x00}, // SATA Link D

        	   Package (0x04) {0x0010FFFF, 0x00, USBI, 0x00}, // USB Link A
        	   Package (0x04) {0x0010FFFF, 0x01, USBI, 0x00}, // USB Link B
        	   Package (0x04) {0x0010FFFF, 0x02, USBI, 0x00}, // USB Link C
        	   Package (0x04) {0x0010FFFF, 0x03, USBI, 0x00}, // USB Link D

        	   Package (0x04) {0x0011FFFF, 0x00, VT8I, 0x00}, // VT8237 Link A
        	   Package (0x04) {0x0011FFFF, 0x01, VT8I, 0x00}, // VT8237 Link B
        	   Package (0x04) {0x0011FFFF, 0x02, VT8I, 0x00}, // VT8237 Link C
        	   Package (0x04) {0x0011FFFF, 0x03, VT8I, 0x00}, // VT8237 Link D

        	   Package (0x04) {0x0012FFFF, 0x00, NICI, 0x00}, // LAN Link A
        	   Package (0x04) {0x0012FFFF, 0x01, NICI, 0x00}, // LAN Link B
        	   Package (0x04) {0x0012FFFF, 0x02, NICI, 0x00}, // LAN Link C
        	   Package (0x04) {0x0012FFFF, 0x03, NICI, 0x00}, // LAN Link D

        	   Package (0x04) {0x0001FFFF, 0x00, 0, 0x10}, // VGA Link A (GSI)
        	   Package (0x04) {0x0001FFFF, 0x01, 0, 0x11}, // VGA Link B (GSI)
        	   Package (0x04) {0x0001FFFF, 0x02, 0, 0x12}, // VGA Link C (GSI)
        	   Package (0x04) {0x0001FFFF, 0x03, 0, 0x13}, // VGA Link D (GSI)

        	   Package (0x04) {0x0014FFFF, 0x00, 0, 0x12}, // Slot 1 Link C (GSI)
        	   Package (0x04) {0x0014FFFF, 0x01, 0, 0x13}, // Slot 1 Link D (GSI)
        	   Package (0x04) {0x0014FFFF, 0x02, 0, 0x10}, // Slot 1 Link A (GSI)
        	   Package (0x04) {0x0014FFFF, 0x03, 0, 0x11}, // Slot 1 Link B (GSI)

        	   Package (0x04) {0x0013FFFF, 0x00, 0, 0x13}, // Riser Slot Link D (GSI)
        	   Package (0x04) {0x0013FFFF, 0x01, 0, 0x12}, // Riser Slot Link C (GSI)
        	   Package (0x04) {0x0013FFFF, 0x02, 0, 0x11}, // Riser Slot Link B (GSI)
        	   Package (0x04) {0x0013FFFF, 0x03, 0, 0x10} // Riser Slot Link A (GSI)

	       })

		   /* PCI Devices Included Here */
		   #include "acpi/sb_physical.asl"

		   /* Legacy PNP Devices Defined Here */

		   /* Disable PS2 Mouse Support */
           Device (PS2M)
           {
        	   Name (_HID, EisaId ("PNP0F13"))
        	   Method (_STA, 0, NotSerialized)
        	   {
       			   Return (0x09)
        	   }

        	   Method (_CRS, 0, NotSerialized)
        	   {
        		   Name (BUF1, ResourceTemplate ()
        		   {
        			   IRQNoFlags ()
        				   {12}
        		   })
        		   Return (BUF1)
        	   }
           }

		   /* Disable Legacy PS2 Keyboard Support */
           Device (PS2K)
           {
         	   Name (_HID, EisaId ("PNP0303"))
         	   Name (_CID, 0x0B03D041)
         	   Method (_STA, 0, NotSerialized)
         	   {
         			Return (0x09)
         	   }

         	   Name (_CRS, ResourceTemplate ()
         	   {
         		   IO (Decode16,
         			   0x0060,  		   // Range Minimum
         			   0x0060,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x01,			   // Length
         			   )
         		   IO (Decode16,
         			   0x0064,  		   // Range Minimum
         			   0x0064,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x01,			   // Length
         			   )
         		   IRQNoFlags ()
         			   {1}
         	   })
           }

		   /* Legacy PIC Description */
           Device (PIC)
           {
         	   Name (_HID, EisaId ("PNP0000"))
         	   Name (_CRS, ResourceTemplate ()
         	   {
         		   IO (Decode16,
         			   0x0020,  		   // Range Minimum
         			   0x0020,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x02,			   // Length
         			   )
         		   IO (Decode16,
         			   0x00A0,  		   // Range Minimum
         			   0x00A0,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x02,			   // Length
         			   )
         		   IRQNoFlags ()
         			   {2}
         	   })
           }

		   /* Legacy DMA Description */
           Device (DMA1)
           {
         	   Name (_HID, EisaId ("PNP0200"))
         	   Name (_CRS, ResourceTemplate ()
         	   {
         		   DMA (Compatibility, BusMaster, Transfer8, )
         			   {4}
         		   IO (Decode16,
         			   0x0000,  		   // Range Minimum
         			   0x0000,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x10,			   // Length
         			   )
         		   IO (Decode16,
         			   0x0080,  		   // Range Minimum
         			   0x0080,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x11,			   // Length
         			   )
         		   IO (Decode16,
         			   0x0094,  		   // Range Minimum
         			   0x0094,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x0C,			   // Length
         			   )
         		   IO (Decode16,
         			   0x00C0,  		   // Range Minimum
         			   0x00C0,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x20,			   // Length
         			   )
         	   })
           }

		   /* Legacy Timer Description */
           Device (TMR)
           {
         	   Name (_HID, EisaId ("PNP0100"))
         	   Name (_CRS, ResourceTemplate ()
         	   {
         		   IO (Decode16,
         			   0x0040,  		   // Range Minimum
         			   0x0040,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x04,			   // Length
         			   )
         		   IRQNoFlags ()
         			   {0}
         	   })
           }

		   /* Legacy RTC Description */
           Device (RTC)
           {
         	   Name (_HID, EisaId ("PNP0B00"))
         	   Name (_CRS, ResourceTemplate ()
         	   {
         		   IO (Decode16,
         			   0x0070,  		   // Range Minimum
         			   0x0070,  		   // Range Maximum
         			   0x04,			   // Alignment
         			   0x04,			   // Length
         			   )
         		   IRQNoFlags ()
         			   {8}
         	   })
           }

		   /* Legacy Speaker Description */
           Device (SPKR)
           {
         	   Name (_HID, EisaId ("PNP0800"))
         	   Name (_CRS, ResourceTemplate ()
         	   {
         		   IO (Decode16,
         			   0x0061,  		   // Range Minimum
         			   0x0061,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x01,			   // Length
         			   )
         	   })
           }

		   /* Legacy Math Co-Processor Description */
           Device (COPR)
           {
         	   Name (_HID, EisaId ("PNP0C04"))
         	   Name (_CRS, ResourceTemplate ()
         	   {
         		   IO (Decode16,
         			   0x00F0,  		   // Range Minimum
         			   0x00F0,  		   // Range Maximum
         			   0x01,			   // Alignment
         			   0x10,			   // Length
         			   )
         		   IRQNoFlags ()
         			   {13}
         	   })
           }

		   /* General Legacy IO Reservations                   */
		   /* Covering items that are not explicitly reserved  */
		   /* from coreboot.                                   */
           Device (SYSR)
           {
               Name (_HID, EisaId ("PNP0C02"))
               Name (_UID, 0x01)
               Name (_CRS, ResourceTemplate ()
               {
            	   IO (Decode16,
            		   0x0010,  		   // Range Minimum
            		   0x0010,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x10,			   // Length
            		   )
            	   IO (Decode16,
            		   0x0022,  		   // Range Minimum
            		   0x0022,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x1E,			   // Length
            		   )
            	   IO (Decode16,
            		   0x0044,  		   // Range Minimum
            		   0x0044,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x1C,			   // Length
            		   )
            	   IO (Decode16,
            		   0x0062,  		   // Range Minimum
            		   0x0062,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x02,			   // Length
            		   )
            	   IO (Decode16,
            		   0x0065,  		   // Range Minimum
            		   0x0065,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x0B,			   // Length
            		   )
            	   IO (Decode16,
            		   0x0074,  		   // Range Minimum
            		   0x0074,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x0C,			   // Length
            		   )
            	   IO (Decode16,
            		   0x0091,  		   // Range Minimum
            		   0x0091,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x03,			   // Length
            		   )
            	   IO (Decode16,
            		   0x00A2,  		   // Range Minimum
            		   0x00A2,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x1E,			   // Length
            		   )
            	   IO (Decode16,
            		   0x00E0,  		   // Range Minimum
            		   0x00E0,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x10,			   // Length
            		   )
            	   IO (Decode16,
            		   0x04D0,  		   // Range Minimum
            		   0x04D0,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x02,			   // Length
            		   )
            	   IO (Decode16,
            		   0x0294,  		   // Range Minimum
            		   0x0294,  		   // Range Maximum
            		   0x01,			   // Alignment
            		   0x04,			   // Length
            		   )
               })
           }

		   #include "acpi/irq_links.asl"
		   #include "acpi/pci_init.asl"

	   } //End of PCI0

	} // End of _SB

} // End of Definition Block

