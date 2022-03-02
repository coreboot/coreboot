/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/amd/common/acpi/alib.asl>

Method (PNOT)
{
	/* Report AC/DC state to ALIB using WAL1() */
	\WAL1 ()
}
