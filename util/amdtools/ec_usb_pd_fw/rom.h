/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef EC_USB_PD_FW__ROM_H__
#define EC_USB_PD_FW__ROM_H__

#include <stdbool.h>

bool rom_set_pd_fw_ptr(const char rom_file[], const char *fmap_region_name, int slot, bool automatic);

#endif // EC_USB_PD_FW__ROM_H__
