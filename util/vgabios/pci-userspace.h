/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PCI_USERSPACE_H__
#define __PCI_USERSPACE_H__

struct device {
	int busno;
	int slot;
	int func;
};

int pci_initialize(void);
int pci_exit(void);

u8 pci_read_config8(struct device *dev, unsigned int where);
u16 pci_read_config16(struct device *dev, unsigned int where);
u32 pci_read_config32(struct device *dev, unsigned int where);
void pci_write_config8(struct device *dev, unsigned int where, u8 val);
void pci_write_config16(struct device *dev, unsigned int where, u16 val);
void pci_write_config32(struct device *dev, unsigned int where, u32 val);

#endif
