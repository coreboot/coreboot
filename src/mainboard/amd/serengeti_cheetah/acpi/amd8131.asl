/*
 * Copyright 2005 AMD
 */

            Device (PG0A)
            {
                /*  8132 pcix bridge*/
                Method (_ADR, 0, NotSerialized)
                {
                        Return (DADD(GHCD(HCIN, 0), 0x00000000))
                }

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
                    Package (0x04) { 0x0001FFFF, 0x00, \_SB.PCI0.LNKB, 0x00 },//Slot 2
                    Package (0x04) { 0x0001FFFF, 0x01, \_SB.PCI0.LNKC, 0x00 },
                    Package (0x04) { 0x0001FFFF, 0x02, \_SB.PCI0.LNKD, 0x00 },
                    Package (0x04) { 0x0001FFFF, 0x03, \_SB.PCI0.LNKA, 0x00 },

                    Package (0x04) { 0x0003FFFF, 0x00, \_SB.PCI0.LNKB, 0x00 },
                    Package (0x04) { 0x0003FFFF, 0x01, \_SB.PCI0.LNKC, 0x00 },
                    Package (0x04) { 0x0003FFFF, 0x02, \_SB.PCI0.LNKD, 0x00 },
                    Package (0x04) { 0x0003FFFF, 0x03, \_SB.PCI0.LNKA, 0x00 },

                    Package (0x04) { 0x0004FFFF, 0x00, \_SB.PCI0.LNKC, 0x00 },
                    Package (0x04) { 0x0004FFFF, 0x01, \_SB.PCI0.LNKD, 0x00 },
                    Package (0x04) { 0x0004FFFF, 0x02, \_SB.PCI0.LNKA, 0x00 },
                    Package (0x04) { 0x0004FFFF, 0x03, \_SB.PCI0.LNKB, 0x00 },

                    Package (0x04) { 0x0005FFFF, 0x00, \_SB.PCI0.LNKD, 0x00 },
                    Package (0x04) { 0x0005FFFF, 0x01, \_SB.PCI0.LNKA, 0x00 },
                    Package (0x04) { 0x0005FFFF, 0x02, \_SB.PCI0.LNKB, 0x00 },
                    Package (0x04) { 0x0005FFFF, 0x03, \_SB.PCI0.LNKC, 0x00 },

                    Package (0x04) { 0x0006FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },
                    Package (0x04) { 0x0006FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
                    Package (0x04) { 0x0006FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
                    Package (0x04) { 0x0006FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 }
                })
                Method (_PRT, 0, NotSerialized)
                {
                    If (LNot (PICF)) { Return (PICM) }
                    Else { Return (APIC) }
                }
            }

            Device (PG0B)
            {
                /* 8132 pcix bridge 2 */
                Method (_ADR, 0, NotSerialized)
                {
                        Return (DADD(GHCD(HCIN, 0), 0x00010000))
                }

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
                    Package (0x04) { 0x0001FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },//Slot 1
                    Package (0x04) { 0x0001FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
                    Package (0x04) { 0x0001FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
                    Package (0x04) { 0x0001FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 }
                })
                Method (_PRT, 0, NotSerialized)
                {
                    If (LNot (PICF)) { Return (PICM) }
                    Else { Return (APIC) }
                }
            }
