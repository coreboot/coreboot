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

                Name (APIC, Package (0x04)
                {
		    // Slot A - PIRQ BCDA
                    Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x0018 }, //Slot 2
                    Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x0019 },
                    Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x001A },
                    Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x001B },

                })
                Name (PICM, Package (0x04)
                {
                    Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },//Slot 2
                    Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
                    Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
                    Package (0x04) { 0x0000FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 },
                })

		Name (DNCG, Ones)

                Method (_PRT, 0, NotSerialized)
                {
                    If (LEqual (^DNCG, Ones)) {
			    Multiply (HCIN, 0x0008, Local2) // GSI for 8132 is 4 so we get 8
		            Store (0x00, Local1)
            		    While (LLess (Local1, 0x04))
            		    {
                        	// Update the GSI according to HCIN
	                        Store(DeRefOf(Index (DeRefOf (Index (APIC, Local1)), 3)), Local0)
				Add(Local2, Local0, Local0)
                	        Store(Local0, Index (DeRefOf (Index (APIC, Local1)), 3))
		                Increment (Local1)
            		    }

                        Store (0x00, ^DNCG)

                    }

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
                    Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x001F },// Slot 1
                    Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x0020 },
                    Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x0021 },
                    Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x0022 }
                })
                Name (PICM, Package (0x04)
                {
                    Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },//Slot 1
                    Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
                    Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
                    Package (0x04) { 0x0000FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 }
                })

                Name (DNCG, Ones)

                Method (_PRT, 0, NotSerialized)
                {
                    If (LEqual (^DNCG, Ones)) {
                            Multiply (HCIN, 0x0008, Local2) // GSI for 8132 is 4 so we get 8
                            Store (0x00, Local1)
                            While (LLess (Local1, 0x04))
                            {
                                // Update the GSI according to HCIN
                                Store(DeRefOf(Index (DeRefOf (Index (APIC, Local1)), 3)), Local0)
                                Add(Local2, Local0, Local0)
                                Store(Local0, Index (DeRefOf (Index (APIC, Local1)), 3))
                                Increment (Local1)
                            }

                        Store (0x00, ^DNCG)

                    }

                    If (LNot (PICF)) { Return (PICM) }
                    Else { Return (APIC) }
                }
            }
