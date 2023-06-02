/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef TRANSFORMERS_IPMI_H
#define TRANSFORMERS_IPMI_H

#include <types.h>

void init_frb2_wdt(void);
enum cb_err ipmi_get_slot_id(uint8_t *slot_id);

#endif
