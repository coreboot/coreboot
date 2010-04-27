// AMD8151
            Device (AGPB)
            {
                Method (_ADR, 0, NotSerialized)
                {
                        Return (DADD(GHCD(HCIN, 0), 0x00010000))
                }

                Name (APIC, Package (0x04)
                {
                    Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x10 },
                    Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x11 },
                    Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x12 },
                    Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x13 }
                })
                Name (PICM, Package (0x04)
                {
                    Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },
                    Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
                    Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
                    Package (0x04) { 0x0000FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 }
                })
                Method (_PRT, 0, NotSerialized)
                {
                    If (LNot (PICF)) { Return (PICM) }
                    Else { Return (APIC) }
                }
            }

