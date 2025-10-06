/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootstate.h>
#include "opensil.h"

__weak void opensil_smbios_fill_cbmem_meminfo(void) { }

static void prepare_dmi_17(void *unused)
{
	opensil_smbios_fill_cbmem_meminfo();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_ENTRY, prepare_dmi_17, NULL);
