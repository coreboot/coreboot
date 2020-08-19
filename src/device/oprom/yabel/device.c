/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * Copyright (c) 2008, 2009 Pattrick Hueper <phueper@hueper.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#include "device.h"
#include "compat/rtas.h"
#include <string.h>
#include "debug.h"

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/resource.h>

/* the device we are working with... */
biosemu_device_t bios_device;
//max. 6 BARs and 1 Exp.ROM plus CfgSpace and 3 legacy ranges, plus 2 "special" memory ranges
translate_address_t translate_address_array[13];
u8 taa_last_entry;

typedef struct {
	u8 info;
	u8 bus;
	u8 devfn;
	u8 cfg_space_offset;
	u64 address;
	u64 size;
} __packed assigned_address_t;

#if CONFIG(PCI_OPTION_ROM_RUN_YABEL)
/* coreboot version */

static void
biosemu_dev_get_addr_info(void)
{
	int taa_index = 0;
	struct resource *r;
	u8 bus = bios_device.dev->bus->secondary;
	u16 devfn = bios_device.dev->path.pci.devfn;

	bios_device.bus =  bus;
	bios_device.devfn = devfn;

	DEBUG_PRINTF("bus: %x, devfn: %x\n", bus, devfn);
	for (r = bios_device.dev->resource_list; r; r = r->next) {
		translate_address_array[taa_index].info = r->flags;
		translate_address_array[taa_index].bus = bus;
		translate_address_array[taa_index].devfn = devfn;
		translate_address_array[taa_index].cfg_space_offset =
		    r->index;
		translate_address_array[taa_index].address = r->base;
		translate_address_array[taa_index].size = r->size;
		/* don't translate addresses... all addresses are 1:1 */
		translate_address_array[taa_index].address_offset = 0;
		taa_index++;
	}
	/* Expansion ROM */
	translate_address_array[taa_index].info = IORESOURCE_MEM | IORESOURCE_READONLY;
	translate_address_array[taa_index].bus = bus;
	translate_address_array[taa_index].devfn = devfn;
	translate_address_array[taa_index].cfg_space_offset = 0x30;
	translate_address_array[taa_index].address = bios_device.img_addr;
	translate_address_array[taa_index].size = 0; /* TODO: do we need the size? */
	/* don't translate addresses... all addresses are 1:1 */
	translate_address_array[taa_index].address_offset = 0;
	taa_index++;
	/* legacy ranges if its a VGA card... */
	if ((bios_device.dev->class & 0xFF0000) == 0x030000) {
		DEBUG_PRINTF("%s: VGA device found, adding legacy resources...\n", __func__);
		/* I/O 0x3B0-0x3BB */
		translate_address_array[taa_index].info = IORESOURCE_FIXED | IORESOURCE_IO;
		translate_address_array[taa_index].bus = bus;
		translate_address_array[taa_index].devfn = devfn;
		translate_address_array[taa_index].cfg_space_offset = 0;
		translate_address_array[taa_index].address = 0x3b0;
		translate_address_array[taa_index].size = 0xc;
		/* don't translate addresses... all addresses are 1:1 */
		translate_address_array[taa_index].address_offset = 0;
		taa_index++;
		/* I/O 0x3C0-0x3DF */
		translate_address_array[taa_index].info = IORESOURCE_FIXED | IORESOURCE_IO;
		translate_address_array[taa_index].bus = bus;
		translate_address_array[taa_index].devfn = devfn;
		translate_address_array[taa_index].cfg_space_offset = 0;
		translate_address_array[taa_index].address = 0x3c0;
		translate_address_array[taa_index].size = 0x20;
		/* don't translate addresses... all addresses are 1:1 */
		translate_address_array[taa_index].address_offset = 0;
		taa_index++;
		/* Mem 0xA0000-0xBFFFF */
		translate_address_array[taa_index].info = IORESOURCE_FIXED | IORESOURCE_MEM;
		translate_address_array[taa_index].bus = bus;
		translate_address_array[taa_index].devfn = devfn;
		translate_address_array[taa_index].cfg_space_offset = 0;
		translate_address_array[taa_index].address = 0xa0000;
		translate_address_array[taa_index].size = 0x20000;
		/* don't translate addresses... all addresses are 1:1 */
		translate_address_array[taa_index].address_offset = 0;
		taa_index++;
	}
	// store last entry index of translate_address_array
	taa_last_entry = taa_index - 1;
#if CONFIG(X86EMU_DEBUG)
	//dump translate_address_array
	printf("translate_address_array:\n");
	translate_address_t ta;
	int i;
	for (i = 0; i <= taa_last_entry; i++) {
		ta = translate_address_array[i];
		printf
		    ("%d: info: %08lx bus: %02x devfn: %02x cfg_space_offset: %02x\n\taddr: %016llx\n\toffs: %016llx\n\tsize: %016llx\n",
		     i, ta.info, ta.bus, ta.devfn, ta.cfg_space_offset,
		     ta.address, ta.address_offset, ta.size);
	}
#endif
}
#else
// use translate_address_dev and get_puid from net-snk's net_support.c
void translate_address_dev(u64 *, phandle_t);
u64 get_puid(phandle_t node);

// scan all addresses assigned to the device ("assigned-addresses" and "reg")
// store in translate_address_array for faster translation using dev_translate_address
void
biosemu_dev_get_addr_info(void)
{
	// get bus/dev/fn from assigned-addresses
	int32_t len;
	//max. 6 BARs and 1 Exp.ROM plus CfgSpace and 3 legacy ranges
	assigned_address_t buf[11];
	len =
	    of_getprop(bios_device.phandle, "assigned-addresses", buf,
		       sizeof(buf));
	bios_device.bus = buf[0].bus;
	bios_device.devfn = buf[0].devfn;
	DEBUG_PRINTF("bus: %x, devfn: %x\n", bios_device.bus,
		     bios_device.devfn);
	//store address translations for all assigned-addresses and regs in
	//translate_address_array for faster translation later on...
	int i = 0;
	// index to insert data into translate_address_array
	int taa_index = 0;
	u64 address_offset;
	for (i = 0; i < (len / sizeof(assigned_address_t)); i++, taa_index++) {
		//copy all info stored in assigned-addresses
		translate_address_array[taa_index].info = buf[i].info;
		translate_address_array[taa_index].bus = buf[i].bus;
		translate_address_array[taa_index].devfn = buf[i].devfn;
		translate_address_array[taa_index].cfg_space_offset =
		    buf[i].cfg_space_offset;
		translate_address_array[taa_index].address = buf[i].address;
		translate_address_array[taa_index].size = buf[i].size;
		// translate first address and store it as address_offset
		address_offset = buf[i].address;
		translate_address_dev(&address_offset, bios_device.phandle);
		translate_address_array[taa_index].address_offset =
		    address_offset - buf[i].address;
	}
	//get "reg" property
	len = of_getprop(bios_device.phandle, "reg", buf, sizeof(buf));
	for (i = 0; i < (len / sizeof(assigned_address_t)); i++) {
		if ((buf[i].size == 0) || (buf[i].cfg_space_offset != 0)) {
			// we don't care for ranges with size 0 and
			// BARs and Expansion ROM must be in assigned-addresses... so in reg
			// we only look for those without config space offset set...
			// i.e. the legacy ranges
			continue;
		}
		//copy all info stored in assigned-addresses
		translate_address_array[taa_index].info = buf[i].info;
		translate_address_array[taa_index].bus = buf[i].bus;
		translate_address_array[taa_index].devfn = buf[i].devfn;
		translate_address_array[taa_index].cfg_space_offset =
		    buf[i].cfg_space_offset;
		translate_address_array[taa_index].address = buf[i].address;
		translate_address_array[taa_index].size = buf[i].size;
		// translate first address and store it as address_offset
		address_offset = buf[i].address;
		translate_address_dev(&address_offset, bios_device.phandle);
		translate_address_array[taa_index].address_offset =
		    address_offset - buf[i].address;
		taa_index++;
	}
	// store last entry index of translate_address_array
	taa_last_entry = taa_index - 1;
#if CONFIG(X86EMU_DEBUG)
	//dump translate_address_array
	printf("translate_address_array:\n");
	translate_address_t ta;
	for (i = 0; i <= taa_last_entry; i++) {
		ta = translate_address_array[i];
		printf
		    ("%d: %02x%02x%02x%02x\n\taddr: %016llx\n\toffs: %016llx\n\tsize: %016llx\n",
		     i, ta.info, ta.bus, ta.devfn, ta.cfg_space_offset,
		     ta.address, ta.address_offset, ta.size);
	}
#endif
}
#endif

// "special memory" is a hack to make some parts of memory fall through to real memory
// (ie. no translation). Necessary if option ROMs attempt DMA there, map registers or
// do similarly crazy things.
void
biosemu_add_special_memory(u32 start, u32 size)
{
	int taa_index = ++taa_last_entry;
	translate_address_array[taa_index].info = IORESOURCE_FIXED | IORESOURCE_MEM;
	translate_address_array[taa_index].bus = 0;
	translate_address_array[taa_index].devfn = 0;
	translate_address_array[taa_index].cfg_space_offset = 0;
	translate_address_array[taa_index].address = start;
	translate_address_array[taa_index].size = size;
	/* don't translate addresses... all addresses are 1:1 */
	translate_address_array[taa_index].address_offset = 0;
}

#if !CONFIG(PCI_OPTION_ROM_RUN_YABEL)
// to simulate accesses to legacy VGA Memory (0xA0000-0xBFFFF)
// we look for the first prefetchable memory BAR, if no prefetchable BAR found,
// we use the first memory BAR
// dev_translate_addr will translate accesses to the legacy VGA Memory into the found vmem BAR
static void
biosemu_dev_find_vmem_addr(void)
{
	int i = 0;
	translate_address_t ta;
	s8 tai_np = -1, tai_p = -1;	// translate_address_array index for non-prefetchable and prefetchable memory
	//search backwards to find first entry
	for (i = taa_last_entry; i >= 0; i--) {
		ta = translate_address_array[i];
		if ((ta.cfg_space_offset >= 0x10)
		    && (ta.cfg_space_offset <= 0x24)) {
			//only BARs
			if ((ta.info & 0x03) >= 0x02) {
				//32/64bit memory
				tai_np = i;
				if ((ta.info & 0x40) != 0) {
					// prefetchable
					tai_p = i;
				}
			}
		}
	}
	if (tai_p != -1) {
		ta = translate_address_array[tai_p];
		bios_device.vmem_addr = ta.address;
		bios_device.vmem_size = ta.size;
		DEBUG_PRINTF
		    ("%s: Found prefetchable Virtual Legacy Memory BAR: %llx, size: %llx\n",
		     __func__, bios_device.vmem_addr,
		     bios_device.vmem_size);
	} else if (tai_np != -1) {
		ta = translate_address_array[tai_np];
		bios_device.vmem_addr = ta.address;
		bios_device.vmem_size = ta.size;
		DEBUG_PRINTF
		    ("%s: Found non-prefetchable Virtual Legacy Memory BAR: %llx, size: %llx",
		     __func__, bios_device.vmem_addr,
		     bios_device.vmem_size);
	}
	// disable vmem
	//bios_device.vmem_size = 0;
}

void
biosemu_dev_get_puid(void)
{
	// get puid
	bios_device.puid = get_puid(bios_device.phandle);
	DEBUG_PRINTF("puid: 0x%llx\n", bios_device.puid);
}
#endif

static void
biosemu_dev_get_device_vendor_id(void)
{

	u32 pci_config_0;
#if CONFIG(PCI_OPTION_ROM_RUN_YABEL)
	pci_config_0 = pci_read_config32(bios_device.dev, 0x0);
#else
	pci_config_0 =
	    rtas_pci_config_read(bios_device.puid, 4, bios_device.bus,
				 bios_device.devfn, 0x0);
#endif
	bios_device.pci_device_id =
	    (u16) ((pci_config_0 & 0xFFFF0000) >> 16);
	bios_device.pci_vendor_id = (u16) (pci_config_0 & 0x0000FFFF);
	DEBUG_PRINTF("PCI Device ID: %04x, PCI Vendor ID: %x\n",
		     bios_device.pci_device_id, bios_device.pci_vendor_id);
}

/* Check whether the device has a valid Expansion ROM and search the PCI Data
 * Structure and any Expansion ROM Header (using dev_scan_exp_header()) for
 * needed information.  If the rom_addr parameter is != 0, it is the address of
 * the Expansion ROM image and will be used, if it is == 0, the Expansion ROM
 * BAR address will be used.
 */
u8
biosemu_dev_check_exprom(unsigned long rom_base_addr)
{
	int i = 0;
	translate_address_t ta;
	u16 pci_ds_offset;
	pci_data_struct_t pci_ds;
	if (rom_base_addr == 0) {
		// check for ExpROM Address (Offset 30) in taa
		for (i = 0; i <= taa_last_entry; i++) {
			ta = translate_address_array[i];
			if (ta.cfg_space_offset == 0x30) {
				//translated address
				rom_base_addr = ta.address + ta.address_offset;
				break;
			}
		}
	}
	/* In the ROM there could be multiple Expansion ROM Images... start
	 * searching them for an x86 image.
	 */
	do {
		if (rom_base_addr == 0) {
			printf("Error: no Expansion ROM address found!\n");
			return -1;
		}
		set_ci();
		u16 rom_signature = in16le((void *) rom_base_addr);
		clr_ci();
		if (rom_signature != 0xaa55) {
			printf
			    ("Error: invalid Expansion ROM signature: %02x!\n",
			     *((u16 *) rom_base_addr));
			return -1;
		}
		set_ci();
		// at offset 0x18 is the (16bit little-endian) pointer to the PCI Data Structure
		pci_ds_offset = in16le((void *) (rom_base_addr + 0x18));
		//copy the PCI Data Structure
		memcpy(&pci_ds, (void *) (rom_base_addr + pci_ds_offset),
		       sizeof(pci_ds));
		clr_ci();
#if CONFIG(X86EMU_DEBUG)
		DEBUG_PRINTF("PCI Data Structure @%lx:\n",
			     rom_base_addr + pci_ds_offset);
		dump((void *) &pci_ds, sizeof(pci_ds));
#endif
		if (strncmp((const char *) pci_ds.signature, "PCIR", 4) != 0) {
			printf("Invalid PCI Data Structure found!\n");
			break;
		}
		//little-endian conversion
		pci_ds.vendor_id = in16le(&pci_ds.vendor_id);
		pci_ds.device_id = in16le(&pci_ds.device_id);
		pci_ds.img_length = in16le(&pci_ds.img_length);
		pci_ds.pci_ds_length = in16le(&pci_ds.pci_ds_length);
#ifdef DO_THIS_TEST_TWICE
		if (pci_ds.vendor_id != bios_device.pci_vendor_id) {
			printf
			    ("Image has invalid Vendor ID: %04x, expected: %04x\n",
			     pci_ds.vendor_id, bios_device.pci_vendor_id);
			break;
		}
		if (pci_ds.device_id != bios_device.pci_device_id) {
			printf
			    ("Image has invalid Device ID: %04x, expected: %04x\n",
			     pci_ds.device_id, bios_device.pci_device_id);
			break;
		}
#endif
		DEBUG_PRINTF("Image Length: %d\n", pci_ds.img_length * 512);
		DEBUG_PRINTF("Image Code Type: %d\n", pci_ds.code_type);
		if (pci_ds.code_type == 0) {
			//x86 image
			//store image address and image length in bios_device struct
			bios_device.img_addr = rom_base_addr;
			bios_device.img_size = pci_ds.img_length * 512;
			// we found the image, exit the loop
			break;
		} else {
			// no x86 image, check next image (if any)
			rom_base_addr += pci_ds.img_length * 512;
		}
		if ((pci_ds.indicator & 0x80) == 0x80) {
			//last image found, exit the loop
			DEBUG_PRINTF("Last PCI Expansion ROM Image found.\n");
			break;
		}
	}
	while (bios_device.img_addr == 0);
	// in case we did not find a valid x86 Expansion ROM Image
	if (bios_device.img_addr == 0) {
		printf("Error: no valid x86 Expansion ROM Image found!\n");
		return -1;
	}
	return 0;
}

u8
biosemu_dev_init(struct device * device)
{
	u8 rval = 0;
	//init bios_device struct
	DEBUG_PRINTF("%s\n", __func__);
	memset(&bios_device, 0, sizeof(bios_device));

#if !CONFIG(PCI_OPTION_ROM_RUN_YABEL)
	bios_device.ihandle = of_open(device_name);
	if (bios_device.ihandle == 0) {
		DEBUG_PRINTF("%s is no valid device!\n", device_name);
		return -1;
	}
	bios_device.phandle = of_finddevice(device_name);
#else
	bios_device.dev = device;
#endif
	biosemu_dev_get_addr_info();
#if !CONFIG(PCI_OPTION_ROM_RUN_YABEL)
	biosemu_dev_find_vmem_addr();
	biosemu_dev_get_puid();
#endif
	biosemu_dev_get_device_vendor_id();
	return rval;
}

// translate address function using translate_address_array assembled
// by dev_get_addr_info... MUCH faster than calling translate_address_dev
// and accessing client interface for every translation...
// returns: 0 if addr not found in translate_address_array, 1 if found.
u8
biosemu_dev_translate_address(int type, unsigned long * addr)
{
	int i = 0;
	translate_address_t ta;
#if !CONFIG(PCI_OPTION_ROM_RUN_YABEL)
	/* we don't need this hack for coreboot... we can access legacy areas */
	//check if it is an access to legacy VGA Mem... if it is, map the address
	//to the vmem BAR and then translate it...
	// (translation info provided by Ben Herrenschmidt)
	// NOTE: the translation seems to only work for NVIDIA cards... but it is needed
	// to make some NVIDIA cards work at all...
	if ((bios_device.vmem_size > 0)
	    && ((*addr >= 0xA0000) && (*addr < 0xB8000))) {
		*addr = (*addr - 0xA0000) * 4 + 2 + bios_device.vmem_addr;
	}
	if ((bios_device.vmem_size > 0)
	    && ((*addr >= 0xB8000) && (*addr < 0xC0000))) {
		u8 shift = *addr & 1;
		*addr &= 0xfffffffe;
		*addr = (*addr - 0xB8000) * 4 + shift + bios_device.vmem_addr;
	}
#endif
	for (i = 0; i <= taa_last_entry; i++) {
		ta = translate_address_array[i];
		if ((*addr >= ta.address) && (*addr <= (ta.address + ta.size)) && (ta.info & type)) {
			*addr += ta.address_offset;
			return 1;
		}
	}
	return 0;
}
