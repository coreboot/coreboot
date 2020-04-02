/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* These come from the dynamically created CPU SSDT */
External (\_SB.CNOT, MethodObj)

/* Notify OS to re-read CPU tables */
Method (PNOT)
{
	\_SB.CNOT (0x81)
}

/* Notify OS to re-read CPU _PPC limit */
Method (PPCN)
{
	\_SB.CNOT (0x80)
}

/* Notify OS to re-read Throttle Limit tables */
Method (TNOT)
{
	\_SB.CNOT (0x82)
}
