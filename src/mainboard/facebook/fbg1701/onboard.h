/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 * Copyright (C) 2018-2019 Eltan B.V.
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

#ifndef ONBOARD_H
#define ONBOARD_H

/* SD CARD gpio */
#define SDCARD_CD 81 /* Not used */

#define ITE8528_CMD_PORT 0x6E
#define ITE8528_DATA_PORT 0x6F

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
