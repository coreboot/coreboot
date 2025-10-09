/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMDTOOL_SMN_H
#define AMDTOOL_SMN_H 1

#include <stdint.h>
#include "amdtool.h"

uint32_t smn_read32(uint32_t addr);
uint16_t smn_read16(uint32_t addr);
uint8_t smn_read8(uint32_t addr);
void init_smn(struct pci_dev *nb);

#endif
