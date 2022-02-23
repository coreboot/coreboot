/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>

/****************************************************************
 * HPET
 ****************************************************************/

Scope(\_SB) {
    Device(HPET) {
        Name(_HID, EISAID("PNP0103"))
        Name(_UID, 0)
        OperationRegion(HPTM, SystemMemory, HPET_BASE_ADDRESS, 0x400)
        Field(HPTM, DWordAcc, Lock, Preserve) {
            VEND, 32,
            PRD, 32,
        }
        Method(_STA, 0, NotSerialized) {
            Local0 = VEND
            Local1 = PRD
            Local0 >>= 16
            If ((Local0 == 0) || (Local0 == 0xffff)) {
                Return (0x0)
            }
            If ((Local1 == 0) || (Local1 > 100000000)) {
                Return (0x0)
            }
            Return (0x0F)
        }
        Name(_CRS, ResourceTemplate() {
            Memory32Fixed(ReadOnly,
                HPET_BASE_ADDRESS,  // Address Base
                0x00000400,         // Address Length
                )
        })
    }
}
