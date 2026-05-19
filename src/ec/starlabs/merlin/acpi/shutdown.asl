/* SPDX-License-Identifier: GPL-2.0-only */

Method (_Q3F, 0, NotSerialized)			// Event: Graceful Shutdown Request
{
	Notify (\_SB, 0x81)
}
