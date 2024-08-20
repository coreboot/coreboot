/* SPDX-License-Identifier: GPL-2.0-only */

#if ENV_X86_64
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
#else
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
#endif

PHDRS
{
	to_load PT_LOAD;
#if ENV_SEPARATE_DATA_AND_BSS
	data_segment PT_LOAD;
#endif
}

ENTRY(_start)
