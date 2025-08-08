/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SCH5555_EC_H__
#define __SCH5555_EC_H__

uint8_t sch5555_mbox_read(uint8_t addr1, uint16_t addr2);

void sch5555_mbox_write(uint8_t addr1, uint16_t addr2, uint8_t val);

#endif
