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

#ifndef DEVICE_LIB_H
#define DEVICE_LIB_H

#include <types.h>
#include <endian.h>
#include "compat/of.h"
#include "debug.h"

// a Expansion Header Struct as defined in Plug and Play BIOS Spec 1.0a Chapter 3.2
typedef struct {
	char signature[4];	// signature
	u8 structure_revision;
	u8 length;		// in 16 byte blocks
	u16 next_header_offset;	// offset to next Expansion Header as 16bit little-endian value, as offset from the start of the Expansion ROM
	u8 reserved;
	u8 checksum;	// the sum of all bytes of the Expansion Header must be 0
	u32 device_id;	// PnP Device ID as 32bit little-endian value
	u16 p_manufacturer_string;	//16bit little-endian offset from start of Expansion ROM
	u16 p_product_string;	//16bit little-endian offset from start of Expansion ROM
	u8 device_base_type;
	u8 device_sub_type;
	u8 device_if_type;
	u8 device_indicators;
	// the following vectors are all 16bit little-endian offsets from start of Expansion ROM
	u16 bcv;		// Boot Connection Vector
	u16 dv;		// Disconnect Vector
	u16 bev;		// Bootstrap Entry Vector
	u16 reserved_2;
	u16 sriv;		// Static Resource Information Vector
} __packed exp_header_struct_t;

// a PCI Data Struct as defined in PCI 2.3 Spec Chapter 6.3.1.2
typedef struct {
	u8 signature[4];	// signature, the String "PCIR"
	u16 vendor_id;
	u16 device_id;
	u16 reserved;
	u16 pci_ds_length;	// PCI Data Structure Length, 16bit little-endian value
	u8 pci_ds_revision;
	u8 class_code[3];
	u16 img_length;	// length of the Exp.ROM Image, 16bit little-endian value in 512 bytes
	u16 img_revision;
	u8 code_type;
	u8 indicator;
	u16 reserved_2;
} __packed pci_data_struct_t;

typedef struct {
	u8 bus;
	u8 devfn;
#if CONFIG(PCI_OPTION_ROM_RUN_YABEL)
	struct device* dev;
	u64 puid; /* unused */
#else
	void *dev;
	u64 puid;
	phandle_t phandle;
	ihandle_t ihandle;
#endif
	// store the address of the BAR that is used to simulate
	// legacy VGA memory accesses
	u64 vmem_addr;
	u64 vmem_size;
	// used to buffer I/O Accesses, that do not access the I/O Range of the device...
	// 64k might be overkill, but we can buffer all I/O accesses...
	u8 io_buffer[64 * 1024];
	u16 pci_vendor_id;
	u16 pci_device_id;
	// translated address of the "PC-Compatible" Expansion ROM Image for this device
	unsigned long img_addr;
	u32 img_size;	// size of the Expansion ROM Image (read from the PCI Data Structure)
} biosemu_device_t;

typedef struct {
#if CONFIG(PCI_OPTION_ROM_RUN_YABEL)
	unsigned long info;
#else
	u8 info;
#endif
	u8 bus;
	u8 devfn;
	u8 cfg_space_offset;
	u64 address;
	u64 address_offset;
	u64 size;
} __packed translate_address_t;

// array to store address translations for this
// device. Needed for faster address translation, so
// not every I/O or Memory Access needs to call translate_address_dev
// and access the device tree
// 6 BARs, 1 Exp. ROM, 1 Cfg.Space, and 3 Legacy, plus 2 "special"
// translations are supported... this should be enough for
// most devices... for VGA it is enough anyways...
extern translate_address_t translate_address_array[13];

// index of last translate_address_array entry
// set by get_dev_addr_info function
extern u8 taa_last_entry;

// add 1:1 mapped memory regions to translation table
void biosemu_add_special_memory(u32 start, u32 size);

/* the device we are working with... */
extern biosemu_device_t bios_device;

u8 biosemu_dev_init(struct device * device);
// NOTE: for dev_check_exprom to work, biosemu_dev_init MUST be called first!
u8 biosemu_dev_check_exprom(unsigned long rom_base_addr);

u8 biosemu_dev_translate_address(int type, unsigned long * addr);

/* endianness swap functions for 16 and 32 bit words
 * copied from axon_pciconfig.c
 */
static inline void
out32le(void *addr, u32 val)
{
#if ENV_X86 || ENV_ARM || ENV_ARM64
	*((u32*) addr) = cpu_to_le32(val);
#else
	asm volatile ("stwbrx  %0, 0, %1"::"r" (val), "r"(addr));
#endif
}

static inline u32
in32le(void *addr)
{
	u32 val;
#if ENV_X86 || ENV_ARM || ENV_ARM64
	val = cpu_to_le32(*((u32 *) addr));
#else
	asm volatile ("lwbrx  %0, 0, %1":"=r" (val):"r"(addr));
#endif
	return val;
}

static inline void
out16le(void *addr, u16 val)
{
#if ENV_X86 || ENV_ARM || ENV_ARM64
	*((u16*) addr) = cpu_to_le16(val);
#else
	asm volatile ("sthbrx  %0, 0, %1"::"r" (val), "r"(addr));
#endif
}

static inline u16
in16le(void *addr)
{
	u16 val;
#if ENV_X86 || ENV_ARM || ENV_ARM64
	val = cpu_to_le16(*((u16*) addr));
#else
	asm volatile ("lhbrx %0, 0, %1":"=r" (val):"r"(addr));
#endif
	return val;
}

/* debug function, dumps HID1 and HID4 to detect whether caches are on/off */
static inline void
dumpHID(void)
{
	u64 hid;
	//HID1 = 1009
	__asm__ __volatile__("mfspr %0, 1009":"=r"(hid));
	printf("HID1: %016llx\n", (unsigned long long)hid);
	//HID4 = 1012
	__asm__ __volatile__("mfspr %0, 1012":"=r"(hid));
	printf("HID4: %016llx\n", (unsigned long long)hid);
}

#endif
