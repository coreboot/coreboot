/*
 * Copyright 2006 AMD
 */

            Device (HTXA)
            {
                /*  HTX */
                Method (_ADR, 0, NotSerialized)
                {
                        Return (DADD(GHCD(HCIN, 0), 0x00000000))
                }

                Method (_PRW, 0, NotSerialized)
                {
                    If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x29, 0x03 }) }
                    Else { Return (Package (0x02) { 0x29, 0x01 }) }
                }

            }

