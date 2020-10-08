/* SPDX-License-Identifier: GPL-2.0-only */

/****************************************************************
 * Debugging
 ****************************************************************/

Scope(\) {
    /* Debug Output */
    OperationRegion(DBG, SystemIO, 0x0402, 0x01)
    Field(DBG, ByteAcc, NoLock, Preserve) {
        DBGB,   8,
    }

    /* Debug method - use this method to send output to the QEMU
     * BIOS debug port.  This method handles strings, integers,
     * and buffers.  For example: DBUG("abc") DBUG(0x123) */
    Method(DBUG, 1) {
        ToHexString(Arg0, Local0)
        ToBuffer(Local0, Local0)
        Local1 = SizeOf(Local0) - 1
        Local2 = 0
        While (Local2 < Local1) {
            DBGB = DerefOf( Local0 [Local2])
            Local2++
        }
        DBGB = 0x0A
    }
}
