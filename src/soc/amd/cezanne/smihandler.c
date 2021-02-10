/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/smm.h>
#include <cpu/x86/smm.h>

int southbridge_io_trap_handler(int smif)
{
	return 0;
}

void *get_smi_source_handler(int source)
{
	return NULL;
}
