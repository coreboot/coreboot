/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp_efs.h>
#include <assert.h>
#include <fmap_config.h>
#include <soc/iomap.h>

_Static_assert(FMAP_SECTION_EFS_START == (FLASH_BASE_ADDR + EFS_OFFSET),
	"FMAP EFS Offset does not match EFS Offset - check your config");
