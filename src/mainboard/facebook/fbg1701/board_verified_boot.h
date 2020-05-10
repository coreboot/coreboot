/* SPDX-License-Identifier: GPL-2.0-only */

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
