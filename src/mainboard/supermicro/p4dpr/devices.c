#include <stdlib.h>
#include <resource.h>
#include <pci.h>
#include <printk.h>
#include <string.h>

struct cdev {
	unsigned int devfn;
	struct cdev *next;
	struct cdev *children;
	void *private;
};
static struct cdev nodev;

#define DEV(NAME, DEVICE, FUNCTION, NEXT, CHILDREN, PRIVATE) \
static struct cdev NEXT; \
static struct cdev CHILDREN; \
static struct cdev NAME = { \
	.devfn = PCI_DEVFN(DEVICE, FUNCTION), \
	.next = &NEXT, \
	.children = &CHILDREN, \
 	.private = PRIVATE, \
}

DEV(dev0_2_0,  0x02, 0, dev0_1f_0, dev1_1c_0, 0);
DEV(dev0_1f_0, 0x1f, 0, nodev,     nodev,     0);
DEV(dev1_1c_0, 0x1c, 0, dev1_1e_0, nodev,     0);
DEV(dev1_1e_0, 0x1e, 0, nodev,     nodev,     0);

static void add_dev_list(struct pci_dev *bus, struct cdev *dev);

static void add_device(struct pci_dev *bus, struct cdev *dev)
{
	struct pci_dev *pdev;

#if 0
	printk_debug("Adding%s %02x.%01x\n",
		(dev->children == &nodev)?"":" bus",
		PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn));
#endif

	pdev = malloc(sizeof(*pdev));
	if (!pdev) 
		goto no_mem;

	memset(pdev, 0, sizeof(*pdev));
	pdev->bus = bus;
	pdev->devfn = dev->devfn;

	/* Add to the list of devices on the parent bus. */
	pdev->sibling = bus->children;
	bus->children = pdev;

	/* Add any children we might have */
	add_dev_list(pdev, dev->children);
	return;
 no_mem:
	printk_err("PCI: out of memory.\n");
	return;
}

static void add_dev_list(struct pci_dev *bus, struct cdev *dev)
{
	while(dev != &nodev) {
		add_device(bus, dev);
		dev = dev->next;
	}
}
void enumerate_static_devices(void)
{
	struct cdev *dev = &dev0_2_0;
	add_dev_list(&pci_root, dev);
}
