#include <stdio.h>
#include <io.h>
#include <pci.h>

void *malloc(unsigned int size);
void *memset(void *s, int c, size_t count);

#define CONFIG_CMD(dev, where)   (0x80000000 | (dev->bus << 16) | (dev->devfn << 8) | (where & ~3))

static struct pci_dev *pcidevices = NULL;

struct pci_dev *pci_find_device(unsigned int vendor, unsigned int device,
				struct pci_dev *from)
{
	struct pci_dev *curr = from ? from : pcidevices;

	while (curr) {
		if (curr->vendor == vendor && curr->device == device)
			return curr;
		curr = curr->next;
	}
	return NULL;
}

struct pci_dev *pci_find_slot(unsigned int bus, unsigned int devfn)
{
	struct pci_dev *curr = pcidevices;

	while (curr) {
		if (curr->devfn == devfn && curr->bus == bus)
			return curr;
		curr = curr->next;
	}
	return NULL;
}

int pci_read_config_byte(struct pci_dev *dev, u8 where, u8 * val)
{
	outl(0xcf8, CONFIG_CMD(dev, where));
	*val = inb(0xCFC + (where & 3));
	return 0;
}

int pci_read_config_word(struct pci_dev *dev, u8 where, u16 * val)
{
	outl(0xcf8, CONFIG_CMD(dev, where));
	*val = inb(0xCFC + (where & 2));
	return 0;
}

int pci_read_config_dword(struct pci_dev *dev, u8 where, u32 * val)
{
	outl(0xcf8, CONFIG_CMD(dev, where));
	*val = inb(0xCFC);
	return 0;
}

int pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val)
{
	outl(0xcf8, CONFIG_CMD(dev, where));
	outb(0xCFC + (where & 3), val);
	return 0;
}

int pci_write_config_word(struct pci_dev *dev, u8 where, u16 val)
{
	outl(0xcf8, CONFIG_CMD(dev, where));
	outb(0xCFC + (where & 2), val);
	return 0;
}

int pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val)
{
	outl(0xcf8, CONFIG_CMD(dev, where));
	outb(0xCFC, where);
	return 0;
}

#define FUNC(x) ((x) & 7)
#define SLOT(x) ((x) >> 3)
#define MAX_DEV (20 << 3)

void pci_init(void)
{
	struct pci_dev *pdev, **p, current;
	int dev, bus = 0, multi = 0;

	pcidevices = NULL;

	for (dev = 0; dev < MAX_DEV; dev++) {
		u16 vendor, device;

		if (!multi && FUNC(dev))
			continue;

		current.bus = bus;	// FIXME
		current.devfn = dev;

		pci_read_config_word(&current, PCI_VENDOR_ID, &vendor);
		pci_read_config_word(&current, PCI_DEVICE_ID, &device);

		if (vendor == 0xffff || vendor == 0x0000 ||
		    device == 0xffff || device == 0x0000)
			continue;

		pdev = malloc(sizeof(*pdev));
		memset(pdev, 0, sizeof(*pdev));

		pdev->vendor = vendor;
		pdev->device = device;
		pdev->bus = current.bus;

		pci_read_config_byte(pdev, PCI_HEADER_TYPE, &(pdev->header));
		pci_read_config_word(pdev, PCI_COMMAND, &(pdev->command));
		pci_read_config_byte(pdev, PCI_CLASS_PROG, &(pdev->progif));
		pci_read_config_word(pdev, PCI_CLASS_DEVICE, &(pdev->class));
		pdev->devfn = dev;

		multi = pdev->header & 0x80;
		for (p = &pcidevices; *p; p = &(*p)->next);
		*p = pdev;

	}


}
