/* SPDX-License-Identifier: GPL-2.0-only */

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)

PHDRS
{
	to_load PT_LOAD;
}

#if ENV_DECOMPRESSOR || ENV_BOOTBLOCK || ENV_RMODULE
ENTRY(_start)
#else
ENTRY(stage_entry)
#endif
