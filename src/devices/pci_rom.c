#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include "../drivers/pci/onboard/chip.h"

struct rom_header * pci_rom_probe(struct device *dev)
{
	unsigned long rom_address;
	struct rom_header *rom_header;
	struct pci_data *rom_data;

        if (dev->on_mainboard && (dev->rom_address != 0) ) {
                rom_address = dev->rom_address;
        }
        else {
                rom_address = pci_read_config32(dev, PCI_ROM_ADDRESS);
        }

	rom_address = pci_read_config32(dev, PCI_ROM_ADDRESS);
	if (rom_address == 0x00000000 || rom_address == 0xffffffff) {
		return NULL;
	}

	printk_spew("%s, rom address for %s = %x\n",
		    __func__, dev_path(dev), rom_address);

	/* enable expansion ROM address decoding */
	pci_write_config32(dev, PCI_ROM_ADDRESS, rom_address|PCI_ROM_ADDRESS_ENABLE);

	rom_header = rom_address;
	printk_spew("%s, PCI Expansion ROM, signature 0x%04x, \n\t"
		    "INIT size 0x%04x, data ptr 0x%04x\n",
		    __func__, le32_to_cpu(rom_header->signature),
		    rom_header->size * 512, le32_to_cpu(rom_header->data));
	if (le32_to_cpu(rom_header->signature) != PCI_ROM_HDR) {
		printk_err("%s, Incorrect Expansion ROM Header Signature %04x\n",
			   __func__, le32_to_cpu(rom_header->signature));
		return NULL;
	}

	rom_data = (unsigned char *) rom_header + le32_to_cpu(rom_header->data);
	printk_spew("%s, PCI ROM Image,  Vendor %04x, Device %04x,\n",
		    __func__, rom_data->vendor, rom_data->device);
	if (dev->vendor != rom_data->vendor || dev->device != rom_data->device) {
		printk_err("%s, Device or Vendor ID mismatch\n");
		return NULL;
	}

	printk_spew("%s, PCI ROM Image,  Class Code %02x%04x, Code Type %02x\n",
		    __func__, rom_data->class_hi, rom_data->class_lo,
		    rom_data->type);
	if ((dev->class >> 8) != (rom_data->class_hi << 16 | rom_data->class_lo)) {
		printk_err("%s, Class Code mismatch %x\n",
			   __func__, dev->class);
		return NULL;
	}

	return rom_header;
}

static void *pci_ram_image_start = PCI_RAM_IMAGE_START;
struct rom_header *pci_rom_load(struct device *dev, struct rom_header *rom_header)
{
	struct pci_data * rom_data;
	unsigned long rom_address;
	unsigned int rom_size;

	rom_address = pci_read_config32(dev, PCI_ROM_ADDRESS);
	rom_data = (unsigned char *) rom_header + le32_to_cpu(rom_header->data);
	rom_size = rom_header->size*512;

	if (PCI_CLASS_DISPLAY_VGA == (rom_data->class_hi << 16 | rom_data->class_lo)) {
		printk_spew("%s, copying VGA ROM Image from %x to %x, %x bytes\n",
			    __func__, rom_header, PCI_VGA_RAM_IMAGE_START, rom_size);
		memcpy(PCI_VGA_RAM_IMAGE_START, rom_header, rom_size);
		return (struct rom_header *) (PCI_VGA_RAM_IMAGE_START);
	} else {
		printk_spew("%s, copying non-VGA ROM Image from %x to %x, %x bytes\n",
			    __func__, rom_header, pci_ram_image_start, rom_size);
		memcpy(pci_ram_image_start, rom_header, rom_size);
		pci_ram_image_start += rom_size;
		return (struct rom_header *) pci_ram_image_start;
	}
	/* disable expansion ROM address decoding */
	pci_write_config32(dev, PCI_ROM_ADDRESS, rom_address & ~PCI_ROM_ADDRESS_ENABLE);
}
