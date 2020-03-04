/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <rules.h>

PHDRS
{
	to_load PT_LOAD;
}

ENTRY(_start)
