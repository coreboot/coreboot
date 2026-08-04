/* SPDX-License-Identifier: GPL-2.0-only */
#include <acpi/acpi.h>
#include <acpi/acpi_vfct.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_rom.h>
#include <device/pci_def.h>

/*
 * When the device is an enabled ATI GPU, fill the VFCT image header and append the
 * VBIOS image after the VFCT image header.
 *
 * @param device The device to fill the VFCT table for.
 * @param header (Optional) The VFCT image header to fill.
 *
 * @return The number of bytes including the image header,
 *         or 0 if no Option ROM was found or the device is not supported.
 */
static unsigned long
ati_rom_acpi_fill_vfct(struct device *device, acpi_vfct_image_hdr_t *header)
{
	struct rom_header *rom;

	if (device->path.type != DEVICE_PATH_PCI || !device->enabled ||
	    (device->class >> 16) != PCI_BASE_CLASS_DISPLAY ||
	    device->vendor != PCI_VID_ATI)
		return 0;

	/* Already loaded into DRAM? */
	rom = device->pci_vga_option_rom;
	if (!rom)
		rom = pci_rom_probe(device);
	if (!rom) {
		printk(BIOS_ERR, "%s failed\n", __func__);
		return 0;
	}
	if (device->upstream->segment_group) {
		printk(BIOS_ERR, "VFCT only supports GPU in first PCI segment group.\n");
		return 0;
	}

	if (header) {
		printk(BIOS_DEBUG, "           Copying VBIOS image from %p\n", rom);

		header->DeviceID = device->device;
		header->VendorID = device->vendor;
		header->PCIBus = device->upstream->secondary;
		header->PCIFunction = PCI_FUNC(device->path.pci.devfn);
		header->PCIDevice = PCI_SLOT(device->path.pci.devfn);
		header->ImageLength = rom->size * 512;
		memcpy((void *)header->VbiosContent, rom, header->ImageLength);

		if (!device->pci_vga_option_rom) {
			/* In case _ROM is also emitted, set the pointer to the copy in DRAM. */
			device->pci_vga_option_rom = (struct rom_header *)header->VbiosContent;
		}
	}
	if (rom->size > 0)
		return sizeof(acpi_vfct_image_hdr_t) + rom->size * 512;

	return 0;
}

/*
 * This method allocates and fills the ACPI VFCT table. It can contain multiple VBIOS
 * images for different PCI devices. Each image is described by an acpi_vfct_image_hdr_t
 * structure. The actual VBIOS image is copied to the table for each PCI device.
 *
 * Since the VFCT table can be very large, it is allocated in a separate CBMEM buffer
 * to avoid polluting the CBMEM_ID_ACPI buffer.
 *
 * @param header Pointer to the VFCT table header.
 * @param arg1   Pointer to a pointer to the VFCT table.
 *               The pointer will be set to the allocated VFCT table.
 */
acpi_vfct_t *acpi_allocate_and_fill_vfct(void)
{
	/* pci_rom_probe() is available when PCI is supported */
	if (!CONFIG(PCI))
		return NULL;

	/* Calculate final size of VFCT table */
	size_t image_hdr_len = 0;
	for (struct device *dev = all_devices; dev; dev = dev->next)
		image_hdr_len += ati_rom_acpi_fill_vfct(dev, NULL);
	if (!image_hdr_len)
		return NULL;	/* No Option ROMs found. */

	/* Account for the VFCT table header itself */
	image_hdr_len += sizeof(acpi_vfct_t);

	/* VFCT can contain multiple OptionROMs, each can be as big as 128KiB.
	 * Allocate memory in a separate CBMEM buffer to no polute CBMEM_ID_ACPI.
	 */
	acpi_vfct_t *vfct = cbmem_add(CBMEM_ID_ACPI_VFCT, image_hdr_len);
	if (!vfct) {
		printk(BIOS_ERR, "%s: Failed to allocate %zd bytes for VFCT table in CBMEM\n",
		       __func__, image_hdr_len);
		return NULL;
	}

	if (acpi_fill_header(&vfct->header, "VFCT", VFCT, sizeof(acpi_vfct_t)) != CB_SUCCESS)
		return NULL;

	acpi_vfct_image_hdr_t *image_hdr = (acpi_vfct_image_hdr_t *)(vfct + 1);
	/* Point to first image header */
	vfct->VBIOSImageOffset = (uintptr_t)image_hdr - (uintptr_t)&vfct->header;

	for (struct device *dev = all_devices; dev; dev = dev->next) {
		/* Try to locate the PCI Option ROM and write it to the table */
		unsigned long written = ati_rom_acpi_fill_vfct(dev, image_hdr);
		if (!written)
			continue;

		vfct->header.length += written;
		image_hdr = (acpi_vfct_image_hdr_t *)(((char *)image_hdr) + written);
	}

	/* (Re)calculate length and checksum. */
	vfct->header.checksum = acpi_checksum((void *)vfct, vfct->header.length);

	return vfct;
}
