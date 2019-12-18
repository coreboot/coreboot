/*
 * This file is part of the coreboot project.
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
