/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef BOARD_VERIFIED_BOOT_H
#define BOARD_VERIFIED_BOOT_H

#include <vboot_check.h>

/* Define the items to be measured or verified */
#define FSP (const char *)"fsp.bin"
#define CMOS_LAYOUT (const char *)"cmos_layout.bin"
#define RAMSTAGE (const char *)CONFIG_CBFS_PREFIX"/ramstage"
#define ROMSTAGE (const char *)CONFIG_CBFS_PREFIX"/romstage"
#define PAYLOAD (const char *)CONFIG_CBFS_PREFIX"/payload"
#define POSTCAR (const char *)CONFIG_CBFS_PREFIX"/postcar"
#define OP_ROM_VBT (const char *)"vbt.bin"
#define MICROCODE (const char *)"cpu_microcode_blob.bin"

#endif
