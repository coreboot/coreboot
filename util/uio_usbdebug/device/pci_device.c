
#include <device/pci.h>
#include <device/device.h>

unsigned int pci_match_simple_dev(device_t dev, pci_devfn_t sdev)
{
	return 0;
}

void pci_dev_read_resources(struct device *dev)
{
}
