/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2017 secunet Security Networks AG
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
