/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMDTOOL_ACPIMMIO_H
#define AMDTOOL_ACPIMMIO_H 1

#include "amdtool.h"

const uint8_t *get_acpi_mmio_bar(struct pci_dev *sb);
void acpimmio_cleanup(void);

#endif
