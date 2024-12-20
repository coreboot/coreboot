/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __MAINBOARD_GOOGLE_RAURU_STORAGE_H__
#define __MAINBOARD_GOOGLE_RAURU_STORAGE_H__

enum ufs_type {
	UFS_UNKNOWN = 0,
	UFS_31 = 0x310,
	UFS_40 = 0x400,
	UFS_40_HS = 0x401,
};

uint32_t storage_id(void);
enum ufs_type storage_type(uint32_t index);

#endif
