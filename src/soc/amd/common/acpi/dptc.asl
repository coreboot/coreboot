/* SPDX-License-Identifier: GPL-2.0-only */

External(\_SB.DDEF, MethodObj)
External(\_SB.DTHL, MethodObj)
External(\_SB.DTAB, MethodObj)

Scope (\_SB)
{
    Method (DPTC, 0, Serialized)
    {
        /* If _SB.DDEF is not present, DPTC is not enabled so return early. */
        If (!CondRefOf (\_SB.DDEF))
        {
            Return (Zero)
        }

        /* If _SB.DTHL is not present, then DPTC Tablet Mode is not enabled.
         * Throttle the SOC if the battery is not present (BTEX), the battery level is critical
         * (BFCR), or the battery is cutoff (BFCT). */
        If (CondRefOf (\_SB.DTHL) &&
            (!\_SB.PCI0.LPCB.EC0.BTEX || \_SB.PCI0.LPCB.EC0.BFCR || \_SB.PCI0.LPCB.EC0.BFCT))
        {
            \_SB.DTHL()
            Return (Zero)
        }

        /* If _SB.DTAB is not present, then DPTC Tablet Mode is not enabled. */
        If (CondRefOf (\_SB.DTAB) && (\_SB.PCI0.LPCB.EC0.TBMD == 1))
        {
            \_SB.DTAB()
            Return (Zero)
        }

#if CONFIG(FEATURE_DYNAMIC_DPTC)
        \_SB.DTTS()
#else
        \_SB.DDEF()
#endif
        Return (Zero)
    }
}
