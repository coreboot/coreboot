/*
 * Copyright (C) 2014 Nico Huber <nico.h@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */


#ifndef _DEVICE_DEVICE_H
#define _DEVICE_DEVICE_H

#include <pci/pci.h>

typedef struct pci_dev *pci_devfn_t;

#define pci_read_config8 pci_read_byte
#define pci_read_config16 pci_read_word
#define pci_read_config32 pci_read_long

#define PCI_CAP_ID_EHCI_DEBUG PCI_CAP_ID_DBG

extern struct pci_access *pci_access;
#define PCI_DEV(b, d, f) pci_get_dev(pci_access, 0, b, d, f)

#endif
