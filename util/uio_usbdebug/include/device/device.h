/* SPDX-License-Identifier: GPL-2.0-only */


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
