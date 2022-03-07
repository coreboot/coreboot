/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <commonlib/endian.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <stdio.h>
#include <string.h>
#include <cbfs.h>
#include <cbmem.h>
#include <acpi/acpigen.h>

/* Rmodules don't like weak symbols. */
void __weak map_oprom_vendev_rev(u32 *vendev, u8 *rev) { return; }
u32 __weak map_oprom_vendev(u32 vendev) { return vendev; }

void vga_oprom_preload(void)
{
/* The CONFIG_VGA_BIOS_ID symbol is only defined when VGA_BIOS is selected */
#if CONFIG(VGA_BIOS)
	const char name[] = "pci" CONFIG_VGA_BIOS_ID ".rom";

	if (!CONFIG(CBFS_PRELOAD))
		return;

	printk(BIOS_DEBUG, "Preloading VGA ROM %s\n", name);

	cbfs_preload(name);
#endif
}

static void *cbfs_boot_map_optionrom(uint16_t vendor, uint16_t device)
{
	char name[17] = "pciXXXX,XXXX.rom";

	snprintf(name, sizeof(name), "pci%04hx,%04hx.rom", vendor, device);

	return cbfs_map(name, NULL);
}

static void *cbfs_boot_map_optionrom_revision(uint16_t vendor, uint16_t device, uint8_t rev)
{
	char name[20] = "pciXXXX,XXXX,XX.rom";

	snprintf(name, sizeof(name), "pci%04hx,%04hx,%02hhx.rom", vendor, device, rev);

	return cbfs_map(name, NULL);
}

struct rom_header *pci_rom_probe(const struct device *dev)
{
	struct rom_header *rom_header = NULL;
	struct pci_data *rom_data;
	u8 rev = pci_read_config8(dev, PCI_REVISION_ID);
	u8 mapped_rev = rev;
	u32 vendev = (dev->vendor << 16) | dev->device;
	u32 mapped_vendev = vendev;

	/* If the ROM is in flash, then don't check the PCI device for it. */
	if (CONFIG(CHECK_REV_IN_OPROM_NAME)) {
		rom_header = cbfs_boot_map_optionrom_revision(dev->vendor, dev->device, rev);
		map_oprom_vendev_rev(&mapped_vendev, &mapped_rev);
	} else {
		rom_header = cbfs_boot_map_optionrom(dev->vendor, dev->device);
		mapped_vendev = map_oprom_vendev(vendev);
	}

	if (!rom_header) {
		if (CONFIG(CHECK_REV_IN_OPROM_NAME) &&
				(vendev != mapped_vendev || rev != mapped_rev)) {
			rom_header = cbfs_boot_map_optionrom_revision(
					mapped_vendev >> 16,
					mapped_vendev & 0xffff, mapped_rev);
		} else if (vendev != mapped_vendev) {
			rom_header = cbfs_boot_map_optionrom(
					mapped_vendev >> 16,
					mapped_vendev & 0xffff);
		}
	}

	if (rom_header) {
		printk(BIOS_DEBUG, "In CBFS, ROM address for %s = %p\n",
		       dev_path(dev), rom_header);
	} else if (CONFIG(ON_DEVICE_ROM_LOAD)) {
		uintptr_t rom_address;

		rom_address = pci_read_config32(dev, PCI_ROM_ADDRESS);

		if (rom_address == 0x00000000 || rom_address == 0xffffffff) {
			if (CONFIG(CPU_QEMU_X86) && (dev->class >> 8) == PCI_CLASS_DISPLAY_VGA)
				rom_address = 0xc0000;
			else
				return NULL;
		} else {
			/* Enable expansion ROM address decoding. */
			pci_write_config32(dev, PCI_ROM_ADDRESS,
					   rom_address|PCI_ROM_ADDRESS_ENABLE);
		}

		rom_address &= PCI_ROM_ADDRESS_MASK;

		printk(BIOS_DEBUG, "Option ROM address for %s = %lx\n",
		       dev_path(dev), (unsigned long)rom_address);
		rom_header = (struct rom_header *)rom_address;
	} else {
		printk(BIOS_DEBUG, "PCI Option ROM loading disabled for %s\n",
		       dev_path(dev));
		return NULL;
	}

	printk(BIOS_SPEW,
	       "PCI expansion ROM, signature 0x%04x, INIT size 0x%04x, data ptr 0x%04x\n",
	       le32_to_cpu(rom_header->signature),
	       rom_header->size * 512, le32_to_cpu(rom_header->data));

	if (le32_to_cpu(rom_header->signature) != PCI_ROM_HDR) {
		printk(BIOS_ERR, "Incorrect expansion ROM header signature %04x\n",
		       le32_to_cpu(rom_header->signature));
		return NULL;
	}

	rom_data = (((void *)rom_header) + le32_to_cpu(rom_header->data));

	printk(BIOS_SPEW, "PCI ROM image, vendor ID %04x, device ID %04x,\n",
	       rom_data->vendor, rom_data->device);
	/* If the device id is mapped, a mismatch is expected */
	if ((dev->vendor != rom_data->vendor
	    || dev->device != rom_data->device)
	    && (vendev == mapped_vendev)) {
		printk(BIOS_ERR, "ID mismatch: vendor ID %04x, device ID %04x\n",
		       dev->vendor, dev->device);
		return NULL;
	}

	printk(BIOS_SPEW, "PCI ROM image, Class Code %04x%02x, Code Type %02x\n",
	       rom_data->class_hi, rom_data->class_lo,
	       rom_data->type);

	if (dev->class != ((rom_data->class_hi << 8) | rom_data->class_lo)) {
		printk(BIOS_DEBUG, "Class Code mismatch ROM %08x, dev %08x\n",
		       (rom_data->class_hi << 8) | rom_data->class_lo,
		       dev->class);
		// return NULL;
	}

	return rom_header;
}

static void *pci_ram_image_start = (void *)PCI_RAM_IMAGE_START;

struct rom_header *pci_rom_load(struct device *dev,
				struct rom_header *rom_header)
{
	struct pci_data * rom_data;
	unsigned int rom_size;
	unsigned int image_size=0;

	do {
		/* Get next image. */
		rom_header = (struct rom_header *)((void *) rom_header
							    + image_size);

		rom_data = (struct pci_data *)((void *) rom_header
				+ le32_to_cpu(rom_header->data));

		image_size = le32_to_cpu(rom_data->ilen) * 512;
	} while ((rom_data->type != 0) && (rom_data->indicator != 0)); // make sure we got x86 version

	if (rom_data->type != 0)
		return NULL;

	rom_size = rom_header->size * 512;

	/*
	 * We check to see if the device thinks it is a VGA device not
	 * whether the ROM image is for a VGA device because some
	 * devices have a mismatch between the hardware and the ROM.
	 */
	if ((dev->class >> 8) == PCI_CLASS_DISPLAY_VGA) {
#if !CONFIG(MULTIPLE_VGA_ADAPTERS)
		extern struct device *vga_pri; /* Primary VGA device (device.c). */
		if (dev != vga_pri) return NULL; /* Only one VGA supported. */
#endif
		if ((void *)PCI_VGA_RAM_IMAGE_START != rom_header) {
			printk(BIOS_DEBUG,
			       "Copying VGA ROM Image from %p to 0x%x, 0x%x bytes\n",
			       rom_header, PCI_VGA_RAM_IMAGE_START, rom_size);
			memcpy((void *)PCI_VGA_RAM_IMAGE_START, rom_header,
			       rom_size);
		}
		return (struct rom_header *) (PCI_VGA_RAM_IMAGE_START);
	}

	printk(BIOS_DEBUG, "Copying non-VGA ROM image from %p to %p, 0x%x bytes\n",
	       rom_header, pci_ram_image_start, rom_size);

	memcpy(pci_ram_image_start, rom_header, rom_size);
	pci_ram_image_start += rom_size;
	return (struct rom_header *) (pci_ram_image_start-rom_size);
}

/* ACPI */
#if CONFIG(HAVE_ACPI_TABLES)

/* VBIOS may be modified after oprom init so use the copy if present. */
static struct rom_header *check_initialized(const struct device *dev)
{
	struct rom_header *run_rom;
	struct pci_data *rom_data;

	if (!CONFIG(VGA_ROM_RUN) && !CONFIG(RUN_FSP_GOP))
		return NULL;

	run_rom = (struct rom_header *)(uintptr_t)PCI_VGA_RAM_IMAGE_START;
	if (read_le16(&run_rom->signature) != PCI_ROM_HDR)
		return NULL;

	rom_data = (struct pci_data *)((u8 *)run_rom
			+ read_le16(&run_rom->data));

	if (read_le32(&rom_data->signature) == PCI_DATA_HDR
			&& read_le16(&rom_data->device) == dev->device
			&& read_le16(&rom_data->vendor) == dev->vendor)
		return run_rom;
	else
		return NULL;
}

static unsigned long
pci_rom_acpi_fill_vfct(const struct device *device, acpi_vfct_t *vfct_struct,
		       unsigned long current)
{
	acpi_vfct_image_hdr_t *header = &vfct_struct->image_hdr;
	struct rom_header *rom;

	rom = check_initialized(device);
	if (!rom)
		rom = pci_rom_probe(device);
	if (!rom) {
		printk(BIOS_ERR, "%s failed\n", __func__);
		return current;
	}

	printk(BIOS_DEBUG, "           Copying %sVBIOS image from %p\n",
			rom == (struct rom_header *)
					(uintptr_t)PCI_VGA_RAM_IMAGE_START ?
			"initialized " : "",
			rom);

	header->DeviceID = device->device;
	header->VendorID = device->vendor;
	header->PCIBus = device->bus->secondary;
	header->PCIFunction = PCI_FUNC(device->path.pci.devfn);
	header->PCIDevice = PCI_SLOT(device->path.pci.devfn);
	header->ImageLength = rom->size * 512;
	memcpy((void *)&header->VbiosContent, rom, header->ImageLength);

	vfct_struct->VBIOSImageOffset = (size_t)header - (size_t)vfct_struct;

	current += header->ImageLength;
	return current;
}

unsigned long
pci_rom_write_acpi_tables(const struct device *device, unsigned long current,
			  struct acpi_rsdp *rsdp)
{
	/* Only handle VGA devices */
	if ((device->class >> 8) != PCI_CLASS_DISPLAY_VGA)
		return current;

	/* Only handle enabled devices */
	if (!device->enabled)
		return current;

	/* AMD/ATI uses VFCT */
	if (device->vendor == PCI_VID_ATI) {
		acpi_vfct_t *vfct;

		current = ALIGN_UP(current, 8);
		vfct = (acpi_vfct_t *)current;
		acpi_create_vfct(device, vfct, pci_rom_acpi_fill_vfct);
		if (vfct->header.length) {
			printk(BIOS_DEBUG, "ACPI:    * VFCT at %lx\n", current);
			current += vfct->header.length;
			acpi_add_table(rsdp, vfct);
		}
	}

	return current;
}

void pci_rom_ssdt(const struct device *device)
{
	static size_t ngfx;

	/* Only handle display devices */
	if ((device->class >> 16) != PCI_BASE_CLASS_DISPLAY)
		return;

	/* Only handle enabled devices */
	if (!device->enabled)
		return;

	/* Probe for option rom */
	const struct rom_header *rom = pci_rom_probe(device);
	if (!rom || !rom->size) {
		printk(BIOS_WARNING, "%s: Missing PCI Option ROM\n",
		       dev_path(device));
		return;
	}

	const char *scope = acpi_device_path(device);
	if (!scope) {
		printk(BIOS_ERR, "%s: Missing ACPI scope\n", dev_path(device));
		return;
	}

	/* Supports up to four devices. */
	if ((CBMEM_ID_ROM0 + ngfx) > CBMEM_ID_ROM3) {
		printk(BIOS_ERR, "%s: Out of CBMEM IDs.\n", dev_path(device));
		return;
	}

	/* Prepare memory */
	const size_t cbrom_length = rom->size * 512;
	if (!cbrom_length) {
		printk(BIOS_ERR, "%s: ROM has zero length!\n",
		       dev_path(device));
		return;
	}

	void *cbrom = cbmem_add(CBMEM_ID_ROM0 + ngfx, cbrom_length);
	if (!cbrom) {
		printk(BIOS_ERR, "%s: Failed to allocate CBMEM.\n",
		       dev_path(device));
		return;
	}
	/* Increment CBMEM id for next device */
	ngfx++;

	memcpy(cbrom, rom, cbrom_length);

	/* write _ROM method */
	acpigen_write_scope(scope);
	acpigen_write_rom(cbrom, cbrom_length);
	acpigen_pop_len(); /* pop scope */
}
#endif
