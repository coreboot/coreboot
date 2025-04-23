/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef MEC15X1__ROM_H__
#define MEC15X1__ROM_H__

#include <stdbool.h>

bool rom_set_ec_fw_ptr(const char rom_file[], const char *fmap_region_name);

#endif // MEC15X1__ROM_H__
