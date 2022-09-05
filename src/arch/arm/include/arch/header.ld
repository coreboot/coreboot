/* SPDX-License-Identifier: GPL-2.0-only */

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

PHDRS
{
	to_load PT_LOAD;
}

#if ENV_DECOMPRESSOR || ENV_BOOTBLOCK || ENV_RMODULE
ENTRY(_start)
#else
ENTRY(stage_entry)
#endif
