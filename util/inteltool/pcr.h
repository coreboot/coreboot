/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef INTELTOOL_PCR_H
#define INTELTOOL_PCR_H 1

#include <stdint.h>
#include "inteltool.h"

#define SBBAR_SIZE	(16 * MiB)
#define PCR_PORT_SIZE	(64 * KiB)

uint32_t read_pcr32(uint8_t port, uint16_t offset);

void print_pcr_ports(struct pci_dev *sb, const uint8_t *ports, size_t count);

void pcr_init(struct pci_dev *sb);
void pcr_cleanup(void);

#endif
