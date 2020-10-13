/*
 *
 * Copyright (C) 2010 coresystems GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _PCI_PCI_H
#define _PCI_PCI_H

/* we implement at least this version */
#define PCI_LIB_VERSION 0x020200

#include <pci.h>

#define PCI_REVISION_ID		REG_REVISION_ID
#define PCI_CLASS_PROG		REG_PROG_IF
#define PCI_CLASS_DEVICE	REG_SUBCLASS
#define PCI_SUBSYSTEM_VENDOR_ID REG_SUBSYS_VENDOR_ID
#define PCI_SUBSYSTEM_ID	REG_SUBSYS_ID

#define PCI_COMMAND		REG_COMMAND
#define PCI_COMMAND_IO		REG_COMMAND_IO
#define PCI_COMMAND_MEMORY	REG_COMMAND_MEM
#define PCI_COMMAND_MASTER	REG_COMMAND_BM

#define PCI_HEADER_TYPE		REG_HEADER_TYPE
#define PCI_HEADER_TYPE_NORMAL	HEADER_TYPE_NORMAL
#define PCI_HEADER_TYPE_BRIDGE	HEADER_TYPE_BRIDGE
#define PCI_HEADER_TYPE_CARDBUS	HEADER_TYPE_CARDBUS

#define PCI_BASE_ADDRESS_0	0x10
#define PCI_BASE_ADDRESS_1	0x14
#define PCI_BASE_ADDRESS_2	0x18
#define PCI_BASE_ADDRESS_3	0x1c
#define PCI_BASE_ADDRESS_4	0x20
#define PCI_BASE_ADDRESS_5	0x24
#define PCI_BASE_ADDRESS_SPACE	1 // mask
#define PCI_BASE_ADDRESS_SPACE_IO	1
#define PCI_BASE_ADDRESS_SPACE_MEM	0
#define PCI_BASE_ADDRESS_MEM_MASK	~0xf
#define PCI_BASE_ADDRESS_IO_MASK	~0x3

#define PCI_ROM_ADDRESS		0x30
#define PCI_ROM_ADDRESS1	0x38 // on bridges
#define PCI_ROM_ADDRESS_MASK	~0x7ff

#define PCI_CLASS_MEMORY_OTHER	0x0580

#define PCI_VENDOR_ID_INTEL 0x8086

struct pci_dev {
	u16 domain;
	u8 bus, dev, func;
	u16 vendor_id, device_id;
	u16 device_class;
	struct pci_dev *next;
};

/*
 * values to match devices against.
 * "-1" means "don't care", everything else requires an exact match
 */
struct pci_filter {
	int domain, bus, dev, func;
	int vendor, device;
	struct pci_dev *devices;
};

enum pci_access_type { /* dummy for code compatibility */
	PCI_ACCESS_AUTO,
	PCI_ACCESS_I386_TYPE1,
	PCI_ACCESS_MAX
};

struct pci_access {
	unsigned int method; /* dummy for code compatibility */
	struct pci_dev *devices;
};

u8 pci_read_byte(struct pci_dev *dev, int pos);
u16 pci_read_word(struct pci_dev *dev, int pos);
u32 pci_read_long(struct pci_dev *dev, int pos);

int pci_write_byte(struct pci_dev *dev, int pos, u8 data);
int pci_write_word(struct pci_dev *dev, int pos, u16 data);
int pci_write_long(struct pci_dev *dev, int pos, u32 data);

struct pci_access *pci_alloc(void);
void pci_init(struct pci_access*);
void pci_cleanup(struct pci_access*);
char *pci_filter_parse_slot(struct pci_filter*, const char*);
int pci_filter_match(struct pci_filter*, struct pci_dev*);
void pci_filter_init(struct pci_access*, struct pci_filter*);
void pci_scan_bus(struct pci_access*);
struct pci_dev *pci_get_dev(struct pci_access*, u16, u8, u8, u8);
void pci_free_dev(struct pci_dev *);

#endif
