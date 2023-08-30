/* SPDX-License-Identifier: GPL-2.0-only */

PHDRS
{
	to_load PT_LOAD;
#if ENV_SEPARATE_DATA_AND_BSS
	data_segment PT_LOAD;
#endif
}

ENTRY(_start)
