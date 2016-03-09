/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Name (PR01, Package() {
    // [SL01]: PCI Express Slot 1 on 1A on PCI0
    Package() { 0x0000FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
    Package() { 0x0000FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
    Package() { 0x0000FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
    Package() { 0x0000FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
})

Name (AR01, Package() {
    // [SL01]: PCI Express Slot 1 on 1A on PCI0
    Package() { 0x0000FFFF, 0, 0, 16 },
    Package() { 0x0000FFFF, 1, 0, 17 },
    Package() { 0x0000FFFF, 2, 0, 18 },
    Package() { 0x0000FFFF, 3, 0, 19 },
})

Name (AH01, Package() {
    // [SL01]: PCI Express Slot 1 on 1A on PCI0
    Package() { 0x0000FFFF, 0, 0, 26 },
    Package() { 0x0000FFFF, 1, 0, 28 },
    Package() { 0x0000FFFF, 2, 0, 29 },
    Package() { 0x0000FFFF, 3, 0, 30 },
})

Name (PR02, Package() {
    // [SL02]: PCI Express Slot 2 on 1B on PCI0
    Package() { 0x0000FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
    Package() { 0x0000FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
    Package() { 0x0000FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
    Package() { 0x0000FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
})

Name (AR02, Package() {
    // [SL02]: PCI Express Slot 2 on 1B on PCI0
    Package() { 0x0000FFFF, 0, 0, 16 },
    Package() { 0x0000FFFF, 1, 0, 17 },
    Package() { 0x0000FFFF, 2, 0, 18 },
    Package() { 0x0000FFFF, 3, 0, 19 },
})

Name (AH02, Package() {
    // [SL02]: PCI Express Slot 2 on 1B on PCI0
    Package() { 0x0000FFFF, 0, 0, 27 },
    Package() { 0x0000FFFF, 1, 0, 30 },
    Package() { 0x0000FFFF, 2, 0, 28 },
    Package() { 0x0000FFFF, 3, 0, 29 },
})

Name (PR03, Package() {
    // [CB0I]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
    // [CB0J]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
    // [CB0K]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
    // [CB0L]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
})

Name (AR03, Package() {
    // [CB0I]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 0, 0, 16 },
    // [CB0J]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 1, 0, 17 },
    // [CB0K]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 2, 0, 18 },
    // [CB0L]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 3, 0, 19 },
})

Name (AH03, Package() {
    // [CB0I]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 0, 0, 32 },
    // [CB0J]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 1, 0, 36 },
    // [CB0K]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 2, 0, 37 },
    // [CB0L]: CB3DMA on IOSF
    Package() { 0x0000FFFF, 3, 0, 38 },
})

Name (PR04, Package() {
    // [SL04]: PCI Express Slot 4 on 2B on PCI0
    Package() { 0x0000FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
    Package() { 0x0000FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
    Package() { 0x0000FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
    Package() { 0x0000FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
})

Name (AR04, Package() {
    // [SL04]: PCI Express Slot 4 on 2B on PCI0
    Package() { 0x0000FFFF, 0, 0, 16 },
    Package() { 0x0000FFFF, 1, 0, 17 },
    Package() { 0x0000FFFF, 2, 0, 18 },
    Package() { 0x0000FFFF, 3, 0, 19 },
})

Name (AH04, Package() {
    // [SL04]: PCI Express Slot 4 on 2B on PCI0
    Package() { 0x0000FFFF, 0, 0, 33 },
    Package() { 0x0000FFFF, 1, 0, 37 },
    Package() { 0x0000FFFF, 2, 0, 38 },
    Package() { 0x0000FFFF, 3, 0, 36 },
})

Name (PR05, Package() {
    // [SL05]: PCI Express Slot 5 on 2C on PCI0
    Package() { 0x0000FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
    Package() { 0x0000FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
    Package() { 0x0000FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
    Package() { 0x0000FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
})

Name (AR05, Package() {
    // [SL05]: PCI Express Slot 5 on 2C on PCI0
    Package() { 0x0000FFFF, 0, 0, 16 },
    Package() { 0x0000FFFF, 1, 0, 17 },
    Package() { 0x0000FFFF, 2, 0, 18 },
    Package() { 0x0000FFFF, 3, 0, 19 },
})

Name (AH05, Package() {
    // [SL05]: PCI Express Slot 5 on 2C on PCI0
    Package() { 0x0000FFFF, 0, 0, 34 },
    Package() { 0x0000FFFF, 1, 0, 37 },
    Package() { 0x0000FFFF, 2, 0, 36 },
    Package() { 0x0000FFFF, 3, 0, 38 },
})

Name (PR06, Package() {
    // [SL06]: PCI Express Slot 6 on 2D on PCI0
    Package() { 0x0000FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
    Package() { 0x0000FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
    Package() { 0x0000FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
    Package() { 0x0000FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
})

Name (AR06, Package() {
    // [SL06]: PCI Express Slot 6 on 2D on PCI0
    Package() { 0x0000FFFF, 0, 0, 16 },
    Package() { 0x0000FFFF, 1, 0, 17 },
    Package() { 0x0000FFFF, 2, 0, 18 },
    Package() { 0x0000FFFF, 3, 0, 19 },
})

Name (AH06, Package() {
    // [SL06]: PCI Express Slot 6 on 2D on PCI0
    Package() { 0x0000FFFF, 0, 0, 35 },
    Package() { 0x0000FFFF, 1, 0, 36 },
    Package() { 0x0000FFFF, 2, 0, 38 },
    Package() { 0x0000FFFF, 3, 0, 37 },
})

Name (PR07, Package() {
    // [SL07]: PCI Express Slot 7 on 3A on PCI0
    Package() { 0x0000FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
    Package() { 0x0000FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
    Package() { 0x0000FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
    Package() { 0x0000FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
})

Name (AR07, Package() {
    // [SL07]: PCI Express Slot 7 on 3A on PCI0
    Package() { 0x0000FFFF, 0, 0, 16 },
    Package() { 0x0000FFFF, 1, 0, 17 },
    Package() { 0x0000FFFF, 2, 0, 18 },
    Package() { 0x0000FFFF, 3, 0, 19 },
})

Name (AH07, Package() {
    // [SL07]: PCI Express Slot 7 on 3A on PCI0
    Package() { 0x0000FFFF, 0, 0, 40 },
    Package() { 0x0000FFFF, 1, 0, 44 },
    Package() { 0x0000FFFF, 2, 0, 45 },
    Package() { 0x0000FFFF, 3, 0, 46 },
})

Name (PR08, Package() {
    // [SL08]: PCI Express Slot 8 on 3B on PCI0
    Package() { 0x0000FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
    Package() { 0x0000FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
    Package() { 0x0000FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
    Package() { 0x0000FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
})

Name (AR08, Package() {
    // [SL08]: PCI Express Slot 8 on 3B on PCI0
    Package() { 0x0000FFFF, 0, 0, 16 },
    Package() { 0x0000FFFF, 1, 0, 17 },
    Package() { 0x0000FFFF, 2, 0, 18 },
    Package() { 0x0000FFFF, 3, 0, 19 },
})

Name (AH08, Package() {
    // [SL08]: PCI Express Slot 8 on 3B on PCI0
    Package() { 0x0000FFFF, 0, 0, 41 },
    Package() { 0x0000FFFF, 1, 0, 45 },
    Package() { 0x0000FFFF, 2, 0, 46 },
    Package() { 0x0000FFFF, 3, 0, 44 },
})

Name (PR09, Package() {
    // [SL09]: PCI Express Slot 9 on 3C on PCI0
    Package() { 0x0000FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
    Package() { 0x0000FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
    Package() { 0x0000FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
    Package() { 0x0000FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
})

Name (AR09, Package() {
    // [SL09]: PCI Express Slot 9 on 3C on PCI0
    Package() { 0x0000FFFF, 0, 0, 16 },
    Package() { 0x0000FFFF, 1, 0, 17 },
    Package() { 0x0000FFFF, 2, 0, 18 },
    Package() { 0x0000FFFF, 3, 0, 19 },
})

Name (AH09, Package() {
    // [SL09]: PCI Express Slot 9 on 3C on PCI0
    Package() { 0x0000FFFF, 0, 0, 42 },
    Package() { 0x0000FFFF, 1, 0, 45 },
    Package() { 0x0000FFFF, 2, 0, 44 },
    Package() { 0x0000FFFF, 3, 0, 46 },
})

Name (PR0A, Package() {
    // [SL0A]: PCI Express Slot 10 on 3D on PCI0
    Package() { 0x0000FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
    Package() { 0x0000FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
    Package() { 0x0000FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
    Package() { 0x0000FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
})

Name (AR0A, Package() {
    // [SL0A]: PCI Express Slot 10 on 3D on PCI0
    Package() { 0x0000FFFF, 0, 0, 16 },
    Package() { 0x0000FFFF, 1, 0, 17 },
    Package() { 0x0000FFFF, 2, 0, 18 },
    Package() { 0x0000FFFF, 3, 0, 19 },
})

Name (AH0A, Package() {
    // [SL0A]: PCI Express Slot 10 on 3D on PCI0
    Package() { 0x0000FFFF, 0, 0, 43 },
    Package() { 0x0000FFFF, 1, 0, 44 },
    Package() { 0x0000FFFF, 2, 0, 46 },
    Package() { 0x0000FFFF, 3, 0, 45 },
})


 // PCI Express Port 1A on PCI0
Device (BR1A) {
    Name   (_ADR, 0x00010000)
    Method (_PRW, 0) {
        Return (Package (0x02) {0x09, 0x04})
    }
    Method (_PRT, 0) {
        If (LEqual(PICM, Zero)) {
            Return (PR01)
        }
        If (LEqual(APC1, One)) {
            Return (AH01)
        }
        Return (AR01)
    }

}

// PCI Express Port 1B on PCI0
Device (BR1B) {
    Name   (_ADR, 0x00010001)
    Method (_PRW, 0) {
        Return (Package (0x02) {0x09, 0x04})
    }
    Method (_PRT, 0) {
        If (LEqual(PICM, Zero)) {
            Return (PR02)
        }
        If (LEqual(APC1, One)) {
            Return (AH02)
        }
        Return (AR02)
    }

}

// PCI Express Port 2A on PCI0
Device (BR2A) {
    Name   (_ADR, 0x00020000)
    Method (_PRW, 0) {
        Return (Package (0x02) {0x09, 0x04})
    }
    Method (_PRT, 0) {
        If (LEqual(PICM, Zero)) {
            Return (PR03)
        }
        If (LEqual(APC1, One)) {
            Return (AH03)
        }
        Return (AR03)
    }


    // CB3DMA on IOSF
    Device (CB0I) {
        Name   (_ADR, 0x00000000)
    }

    // CB3DMA on IOSF
    Device (CB0J) {
        Name   (_ADR, 0x00000001)
    }

    // CB3DMA on IOSF
    Device (CB0K) {
        Name   (_ADR, 0x00000002)
    }

    // CB3DMA on IOSF
    Device (CB0L) {
        Name   (_ADR, 0x00000003)
    }
}

// PCI Express Port 2B on PCI0
Device (BR2B) {
    Name   (_ADR, 0x00020001)
    Method (_PRW, 0) {
        Return (Package (0x02) {0x09, 0x04})
    }
    Method (_PRT, 0) {
        If (LEqual(PICM, Zero)) {
            Return (PR04)
        }
        If (LEqual(APC1, One)) {
            Return (AH04)
        }
        Return (AR04)
    }

}

// PCI Express Port 2C on PCI0
Device (BR2C) {
    Name   (_ADR, 0x00020002)
    Method (_PRW, 0) {
        Return (Package (0x02) {0x09, 0x04})
    }
    Method (_PRT, 0) {
        If (LEqual(PICM, Zero)) {
            Return (PR05)
        }
        If (LEqual(APC1, One)) {
            Return (AH05)
        }
        Return (AR05)
    }

}

// PCI Express Port 2D on PCI0
Device (BR2D) {
    Name   (_ADR, 0x00020003)
    Method (_PRW, 0) {
        Return (Package (0x02) {0x09, 0x04})
    }
    Method (_PRT, 0) {
        If (LEqual(PICM, Zero)) {
            Return (PR06)
        }
        If (LEqual(APC1, One)) {
            Return (AH06)
        }
        Return (AR06)
    }

}

// PCI Express Port 3A on PCI0
Device (BR3A) {
    Name   (_ADR, 0x00030000)
    Method (_PRW, 0) {
        Return (Package (0x02) {0x09, 0x04})
    }
    Method (_PRT, 0) {
        If (LEqual(PICM, Zero)) {
            Return (PR07)
        }
        If (LEqual(APC1, One)) {
            Return (AH07)
        }
        Return (AR07)
    }

}

// PCI Express Port 3B on PCI0
Device (BR3B) {
    Name   (_ADR, 0x00030001)
    Method (_PRW, 0) {
        Return (Package (0x02) {0x09, 0x04})
    }
    Method (_PRT, 0) {
        If (LEqual(PICM, Zero)) {
            Return (PR08)
        }
        If (LEqual(APC1, One)) {
            Return (AH08)
        }
        Return (AR08)
    }

}

// PCI Express Port 3C on PCI0
Device (BR3C) {
    Name   (_ADR, 0x00030002)
    Method (_PRW, 0) {
        Return (Package (0x02) {0x09, 0x04})
    }
    Method (_PRT, 0) {
        If (LEqual(PICM, Zero)) {
            Return (PR09)
        }
        If (LEqual(APC1, One)) {
            Return (AH09)
        }
        Return (AR09)
    }

}

// PCI Express Port 3D on PCI0
Device (BR3D) {
    Name   (_ADR, 0x00030003)
    Method (_PRW, 0) {
        Return (Package (0x02) {0x09, 0x04})
    }
    Method (_PRT, 0) {
        If (LEqual(PICM, Zero)) {
            Return (PR0A)
        }
        If (LEqual(APC1, One)) {
            Return (AH0A)
        }
        Return (AR0A)
    }

}
