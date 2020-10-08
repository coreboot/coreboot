/* SPDX-License-Identifier: GPL-2.0-only */

/****************************************************************
 * CPU hotplug
 ****************************************************************/

Scope(\_SB) {
    /* Objects filled in by run-time generated SSDT */
    External(NTFY, MethodObj)
    External(CPON, PkgObj)

    /* Methods called by run-time generated SSDT Processor objects */
    Method(CPMA, 1, NotSerialized) {
        // _MAT method - create an madt APIC buffer
        // Arg0 = Processor ID = Local APIC ID
        // Local0 = CPON flag for this cpu
        Local0 = DerefOf (CPON [Arg0])
        // Local1 = Buffer (in madt APIC form) to return
        Local1 = Buffer(8) {0x00, 0x08, 0x00, 0x00, 0x00, 0, 0, 0}
        // Update the processor id, Local APIC id, and enable/disable status
        Local1 [2] = Arg0
        Local1 [3] = Arg0
        Local1 [4] = Local0
        Return (Local1)
    }
    Method(CPST, 1, NotSerialized) {
        // _STA method - return ON status of cpu
        // Arg0 = Processor ID = Local APIC ID
        // Local0 = CPON flag for this cpu
        Local0 = DerefOf (CPON [Arg0])
        If (Local0) {
            Return (0xF)
        } Else {
            Return (0x0)
        }
    }
    Method(CPEJ, 2, NotSerialized) {
        // _EJ0 method - eject callback
        Sleep(200)
    }

    /* CPU hotplug notify method */
    OperationRegion(PRST, SystemIO, 0xaf00, 32)
    Field(PRST, ByteAcc, NoLock, Preserve) {
        PRS, 256
    }
    Method(PRSC, 0) {
        // Local5 = active CPU bitmap
        Local5 = PRS
        // Local2 = last read byte from bitmap
        Local2 = 0
        // Local0 = Processor ID / APIC ID iterator
        Local0 = 0
        While (Local0 < SizeOf(CPON)) {
            // Local1 = CPON flag for this cpu
            Local1 = DerefOf (CPON [Local0])
            If (Local0 & 0x07) {
                // Shift down previously read bitmap byte
                Local2 >>= 1
            } Else {
                // Read next byte from CPU bitmap
                Local2 = DerefOf (Local5 [Local0 >> 3])
            }
            // Local3 = active state for this cpu
            Local3 = Local2 & 1

            If (Local1 != Local3) {
                // State change - update CPON with new state
                CPON [Local0] = Local3
                // Do CPU notify
                If (Local3 == 1) {
                    NTFY(Local0, 1)
                } Else {
                    NTFY(Local0, 3)
                }
            }
            Local0++
        }
    }
}
