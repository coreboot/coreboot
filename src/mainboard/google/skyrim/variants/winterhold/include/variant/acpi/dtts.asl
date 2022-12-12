/* SPDX-License-Identifier: GPL-2.0-only */

External(\_SB.DTTB, MethodObj)
External(\_SB.DTTC, MethodObj)
External(\_SB.DTTD, MethodObj)
External(\_SB.DTTE, MethodObj)
External(\_SB.DTTF, MethodObj)

Scope (\_SB)
{
    //---------------------------------------------
    // Table | A | B | C | D | E | F | First boot |
    //---------------------------------------------
    // PRTN  | 0 | 1 | 2 | 3 | 4 | 5 |     7      |
    //---------------------------------------------
    Name (PRTN, 7)

    Method (DTTS, 0, Serialized)
    {
        // Set table A as default table after power on device
        If (\_SB.PRTN == 7)
        {
            \_SB.DDEF()
            \_SB.PRTN = 0
            Return (Zero)
        }

        If (\_SB.PCI0.LPCB.EC0.STTB == 0) { // Desktop
            If (\_SB.PCI0.LPCB.EC0.LIDS == 1) { // Lid-open
                // Table A/B
                If ((\_SB.PRTN == 0) || (\_SB.PRTN == 1)) {
                    // AMB sensor trigger point
                    // 50C will store 123(0x7B) in mapped memory
                    // 50C=323K, 323-200(offset)=123(0x7B)
                    If (\_SB.PCI0.LPCB.EC0.TIN4 >= 123) {
                        \_SB.DTTB()
                        \_SB.PRTN = 1
                        Return (Zero)
                    }
                    // AMB sensor release point
                    If ((\_SB.PCI0.LPCB.EC0.TIN4 <= 118)) {
                        \_SB.DDEF()
                        \_SB.PRTN = 0
                        Return (Zero)
                    }
                    // Keep tht previous thermal table
                    Return (Zero)
                } Else {
                    If (\_SB.PRTN == 3 || \_SB.PRTN == 5) {
                        \_SB.DTTB()
                        \_SB.PRTN = 1
                        Return (Zero)
                    } Else {
                        \_SB.DDEF()
                        \_SB.PRTN = 0
                        Return (Zero)
                    }
                }
            } Else { // Lid-close
                // Table C/D
                If (\_SB.PRTN == 2 || \_SB.PRTN == 3) {
                    If (\_SB.PCI0.LPCB.EC0.TIN4 >= 128) {
                        \_SB.DTTD()
                        \_SB.PRTN = 3
                        Return (Zero)
                    }
                    If(\_SB.PCI0.LPCB.EC0.TIN4 <= 123) {
                        \_SB.DTTC()
                        \_SB.PRTN = 2
                        Return (Zero)
                    }
                    // Keep tht previous thermal table
                    Return (Zero)
                } Else {
                    If (\_SB.PRTN == 1 || \_SB.PRTN == 5) {
                        \_SB.DTTD()
                        \_SB.PRTN = 3
                        Return (Zero)
                    } Else {
                        \_SB.DTTC()
                        \_SB.PRTN = 2
                        Return (Zero)
                    }
                }
            }
        } Else { // Laptop
            // Table E/F
            If (\_SB.PRTN == 4 || \_SB.PRTN == 5) {
                // AMB sensor trigger point
                If (\_SB.PCI0.LPCB.EC0.TIN4 >= 118) {
                    \_SB.DTTF()
                    \_SB.PRTN = 5
                    Return (Zero)
                }
                // AMB sensor release point
                If ((\_SB.PCI0.LPCB.EC0.TIN4 <= 113)) {
                    \_SB.DTTE()
                    \_SB.PRTN = 4
                    Return (Zero)
                }
                // Keep tht previous thermal table
                Return (Zero)
            } Else {
                If (\_SB.PRTN == 1 || \_SB.PRTN == 3) {
                    \_SB.DTTF()
                    \_SB.PRTN = 5
                    Return (Zero)
                } Else {
                    \_SB.DTTE()
                    \_SB.PRTN = 4
                    Return (Zero)
                }
            }
        } // Desktop/Laptop End
    }
}
