/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <fsp/util.h>
#include <intelblocks/graphics.h>
#include <types.h>

uintptr_t fsp_soc_get_igd_bar(void)
{
	return graphics_get_memory_base();
}
