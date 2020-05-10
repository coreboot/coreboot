/* uio_ehci_pci - UIO driver for PCI EHCI devices */
/* This only implements MMIO access (no interrupts). */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/uio_driver.h>

#define DRIVER_VERSION	"0.0.1"
#define DRIVER_AUTHOR	"Nico Huber <nico.h@gmx.de>"
#define DRIVER_DESC	"UIO driver for PCI EHCI devices"
#define DRIVER_TAG	"uio_ehci_pci"

static int probe(struct pci_dev *const pci_dev,
		 const struct pci_device_id *const did)
{
	struct uio_info *info;
	int ret;

	ret = pci_enable_device(pci_dev);
	if (ret)
		goto return_;

	ret = pci_request_regions(pci_dev, DRIVER_TAG);
	if (ret)
		goto return_disable;

	info = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
	if (!info) {
		ret = -ENOMEM;
		goto return_release;
	}

	info->name = DRIVER_TAG;
	info->version = DRIVER_VERSION;

	info->mem[0].name = "EHCI MMIO area";
	info->mem[0].addr = pci_resource_start(pci_dev, 0);
	if (!info->mem[0].addr) {
		ret = -ENODEV;
		goto return_free;
	}
	info->mem[0].size = pci_resource_len(pci_dev, 0);
	info->mem[0].memtype = UIO_MEM_PHYS;

	ret = uio_register_device(&pci_dev->dev, info);
	if (ret)
		goto return_free;
	pci_set_drvdata(pci_dev, info);

	return 0;
return_free:
	kfree(info);
return_release:
	pci_release_regions(pci_dev);
return_disable:
	pci_disable_device(pci_dev);
return_:
	return ret;
}

static void remove(struct pci_dev *const pci_dev)
{
	struct uio_info *const info = pci_get_drvdata(pci_dev);

	uio_unregister_device(info);
	kfree(info);
	pci_release_regions(pci_dev);
	pci_disable_device(pci_dev);
}

static DEFINE_PCI_DEVICE_TABLE(ehci_pci_ids) = {
	{ PCI_DEVICE(0x8086, 0x27cc) },
	{ 0, }
};

static struct pci_driver uio_ehci_pci_driver = {
	.name		= DRIVER_TAG,
	.id_table	= ehci_pci_ids,
	.probe		= probe,
	.remove		= remove,
};

module_pci_driver(uio_ehci_pci_driver);
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
