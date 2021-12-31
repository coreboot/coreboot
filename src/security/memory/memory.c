/* SPDX-License-Identifier: GPL-2.0-only */

#include <security/intel/txt/txt.h>
#include <stdbool.h>

#include "memory.h"

/**
 * To be called after DRAM init.
 * Tells the caller if DRAM must be cleared as requested by the user,
 * firmware or security framework.
 */
bool security_clear_dram_request(void)
{
	if (CONFIG(SECURITY_CLEAR_DRAM_ON_REGULAR_BOOT))
		return true;

	if (CONFIG(INTEL_TXT) && intel_txt_memory_has_secrets())
		return true;

	/* TODO: Add TEE environments here */

	return false;
}
