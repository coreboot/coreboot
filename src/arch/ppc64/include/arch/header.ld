/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_ARCH(powerpc)

PHDRS
{
	to_load PT_LOAD;
}

#if ENV_BOOTBLOCK
ENTRY(_start)
#else
ENTRY(stage_entry)
#endif
