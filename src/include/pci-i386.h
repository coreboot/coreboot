/*
 *  This file was copied verbatim from Linux-2.4.0-test4 sources.
 *
 *	Low-Level PCI Access for i386 machines.
 *
 *	(c) 1999 Martin Mares <mj@ucw.cz>
 */

#define PCI_PROBE_BIOS 1
#define PCI_PROBE_CONF1 2
#define PCI_PROBE_CONF2 4
#define PCI_NO_SORT 0x100
#define PCI_BIOS_SORT 0x200
#define PCI_NO_CHECKS 0x400
#define PCI_ASSIGN_ROMS 0x1000
#define PCI_BIOS_IRQ_SCAN 0x2000

extern unsigned int pci_probe;

/* pci-i386.c */

extern unsigned int pcibios_max_latency;

void pcibios_resource_survey(void);
int pcibios_enable_resources(struct pci_dev *);

/* pci-pc.c */

extern int pcibios_last_bus;
extern struct pci_bus *pci_root_bus;
extern struct pci_ops *pci_root_ops;

struct irq_routing_table *pcibios_get_irq_routing_table(void);
int pcibios_set_irq_routing(struct pci_dev *dev, int pin, int irq);

/* pci-irq.c */

extern unsigned int pcibios_irq_mask;

void pcibios_irq_init(void);
void pcibios_fixup_irqs(void);
void pcibios_enable_irq(struct pci_dev *dev);
