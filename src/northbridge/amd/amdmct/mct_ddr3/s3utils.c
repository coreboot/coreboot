/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <string.h>
#include <arch/acpi.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cbfs.h>
#include <spi-generic.h>
#include <spi_flash.h>

#include "s3utils.h"

#define S3NV_FILE_NAME "s3nv"

static ssize_t get_s3nv_file_offset(void);

ssize_t get_s3nv_file_offset(void)
{
	struct region_device s3nv_region;
	struct cbfsf s3nv_cbfs_file;
	if (cbfs_boot_locate(&s3nv_cbfs_file, S3NV_FILE_NAME, NULL)) {
		printk(BIOS_DEBUG, "S3 state file not found in CBFS: %s\n", S3NV_FILE_NAME);
		return -1;
	}
	cbfs_file_data(&s3nv_region, &s3nv_cbfs_file);

	return s3nv_region.region.offset;
}

static uint32_t read_amd_dct_index_register(device_t dev, uint32_t index_ctl_reg, uint32_t index)
{
	uint32_t dword;

	index &= ~(1 << 30);
	pci_write_config32(dev, index_ctl_reg, index);
	do {
		dword = pci_read_config32(dev, index_ctl_reg);
	} while (!(dword & (1 << 31)));
	dword = pci_read_config32(dev, index_ctl_reg + 0x04);

	return dword;
}

#ifdef __RAMSTAGE__
static uint64_t rdmsr_uint64_t(unsigned long index) {
	msr_t msr = rdmsr(index);
	return (((uint64_t)msr.hi) << 32) | ((uint64_t)msr.lo);
}

void copy_mct_data_to_save_variable(struct amd_s3_persistent_data* persistent_data)
{
	uint8_t i;
	uint8_t j;
	uint8_t node;
	uint8_t channel;

	/* Zero out data structure */
	memset(persistent_data, 0, sizeof(struct amd_s3_persistent_data));

	/* Load data from DCTs into data structure */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		device_t dev_fn1 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 1));
		device_t dev_fn2 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 2));
		device_t dev_fn3 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 3));
		if ((!dev_fn1) || (!dev_fn2) || (!dev_fn3)) {
			persistent_data->node[node].node_present = 0;
			continue;
		}
		persistent_data->node[node].node_present = 1;

		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];

			/* Stage 1 */
			data->f2x110 = pci_read_config32(dev_fn2, 0x110);

			/* Stage 2 */
			data->f1x40 = pci_read_config32(dev_fn1, 0x40 + (0x100 * channel));
			data->f1x44 = pci_read_config32(dev_fn1, 0x44 + (0x100 * channel));
			data->f1x48 = pci_read_config32(dev_fn1, 0x48 + (0x100 * channel));
			data->f1x4c = pci_read_config32(dev_fn1, 0x4c + (0x100 * channel));
			data->f1x50 = pci_read_config32(dev_fn1, 0x50 + (0x100 * channel));
			data->f1x54 = pci_read_config32(dev_fn1, 0x54 + (0x100 * channel));
			data->f1x58 = pci_read_config32(dev_fn1, 0x58 + (0x100 * channel));
			data->f1x5c = pci_read_config32(dev_fn1, 0x5c + (0x100 * channel));
			data->f1x60 = pci_read_config32(dev_fn1, 0x60 + (0x100 * channel));
			data->f1x64 = pci_read_config32(dev_fn1, 0x64 + (0x100 * channel));
			data->f1x68 = pci_read_config32(dev_fn1, 0x68 + (0x100 * channel));
			data->f1x6c = pci_read_config32(dev_fn1, 0x6c + (0x100 * channel));
			data->f1x70 = pci_read_config32(dev_fn1, 0x70 + (0x100 * channel));
			data->f1x74 = pci_read_config32(dev_fn1, 0x74 + (0x100 * channel));
			data->f1x78 = pci_read_config32(dev_fn1, 0x78 + (0x100 * channel));
			data->f1x7c = pci_read_config32(dev_fn1, 0x7c + (0x100 * channel));
			data->f1xf0 = pci_read_config32(dev_fn1, 0xf0);
			data->f1x120 = pci_read_config32(dev_fn1, 0x120);
			data->f1x124 = pci_read_config32(dev_fn1, 0x124);
			data->f2x10c = pci_read_config32(dev_fn2, 0x10c);
			data->f2x114 = pci_read_config32(dev_fn2, 0x114);
			data->f2x118 = pci_read_config32(dev_fn2, 0x118);
			data->f2x11c = pci_read_config32(dev_fn2, 0x11c);
			data->f2x1b0 = pci_read_config32(dev_fn2, 0x1b0);
			data->f3x44 = pci_read_config32(dev_fn3, 0x44);
			for (i=0; i<16; i++) {
				data->msr0000020[i] = rdmsr_uint64_t(0x00000200 | i);
			}
			data->msr00000250 = rdmsr_uint64_t(0x00000250);
			data->msr00000258 = rdmsr_uint64_t(0x00000258);
			for (i=0; i<8; i++)
				data->msr0000026[i] = rdmsr_uint64_t(0x00000260 | (i + 8));
			data->msr000002ff = rdmsr_uint64_t(0x000002ff);
			data->msrc0010010 = rdmsr_uint64_t(0xc0010010);
			data->msrc001001a = rdmsr_uint64_t(0xc001001a);
			data->msrc001001d = rdmsr_uint64_t(0xc001001d);
			data->msrc001001f = rdmsr_uint64_t(0xc001001f);

			/* Stage 3 */
			data->f2x40 = pci_read_config32(dev_fn2, 0x40 + (0x100 * channel));
			data->f2x44 = pci_read_config32(dev_fn2, 0x44 + (0x100 * channel));
			data->f2x48 = pci_read_config32(dev_fn2, 0x48 + (0x100 * channel));
			data->f2x4c = pci_read_config32(dev_fn2, 0x4c + (0x100 * channel));
			data->f2x50 = pci_read_config32(dev_fn2, 0x50 + (0x100 * channel));
			data->f2x54 = pci_read_config32(dev_fn2, 0x54 + (0x100 * channel));
			data->f2x58 = pci_read_config32(dev_fn2, 0x58 + (0x100 * channel));
			data->f2x5c = pci_read_config32(dev_fn2, 0x5c + (0x100 * channel));
			data->f2x60 = pci_read_config32(dev_fn2, 0x60 + (0x100 * channel));
			data->f2x64 = pci_read_config32(dev_fn2, 0x64 + (0x100 * channel));
			data->f2x68 = pci_read_config32(dev_fn2, 0x68 + (0x100 * channel));
			data->f2x6c = pci_read_config32(dev_fn2, 0x6c + (0x100 * channel));
			data->f2x78 = pci_read_config32(dev_fn2, 0x78 + (0x100 * channel));
			data->f2x7c = pci_read_config32(dev_fn2, 0x7c + (0x100 * channel));
			data->f2x80 = pci_read_config32(dev_fn2, 0x80 + (0x100 * channel));
			data->f2x84 = pci_read_config32(dev_fn2, 0x84 + (0x100 * channel));
			data->f2x88 = pci_read_config32(dev_fn2, 0x88 + (0x100 * channel));
			data->f2x8c = pci_read_config32(dev_fn2, 0x8c + (0x100 * channel));
			data->f2x90 = pci_read_config32(dev_fn2, 0x90 + (0x100 * channel));
			data->f2xa4 = pci_read_config32(dev_fn2, 0xa4 + (0x100 * channel));
			data->f2xa8 = pci_read_config32(dev_fn2, 0xa8 + (0x100 * channel));

			/* Stage 4 */
			data->f2x94 = pci_read_config32(dev_fn2, 0x94 + (0x100 * channel));

			/* Stage 6 */
			for (i=0; i<9; i++)
				for (j=0; j<3; j++)
					data->f2x9cx0d0f0_f_8_0_0_8_4_0[i][j] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0d0f0000 | (i << 8) | (j * 4));
			data->f2x9cx00 = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x00);
			data->f2x9cx0a = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0a);
			data->f2x9cx0c = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0c);

			/* Stage 7 */
			data->f2x9cx04 = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x04);

			/* Stage 9 */
			data->f2x9cx0d0fe006 = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0d0fe006);
			data->f2x9cx0d0fe007 = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0d0fe007);

			/* Stage 10 */
			for (i=0; i<12; i++)
				data->f2x9cx10[i] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x10 + i);
			for (i=0; i<12; i++)
				data->f2x9cx20[i] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x20 + i);
			for (i=0; i<4; i++)
				for (j=0; j<3; j++)
					data->f2x9cx3_0_0_3_1[i][j] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), (0x01 + i) + (0x100 * j));
			for (i=0; i<4; i++)
				for (j=0; j<3; j++)
					data->f2x9cx3_0_0_7_5[i][j] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), (0x05 + i) + (0x100 * j));
			data->f2x9cx0d = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0d);
			for (i=0; i<9; i++)
				data->f2x9cx0d0f0_f_0_13[i] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0d0f0013 | (i << 8));
			for (i=0; i<9; i++)
				data->f2x9cx0d0f0_f_0_30[i] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0d0f0030 | (i << 8));
			for (i=0; i<4; i++)
				data->f2x9cx0d0f2_f_0_30[i] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0d0f2030 | (i << 8));
			for (i=0; i<2; i++)
				for (j=0; j<3; j++)
					data->f2x9cx0d0f8_8_4_0[i][j] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0d0f0000 | (i << 8) | (j * 4));
			data->f2x9cx0d0f812f = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x0d0f812f);

			/* Stage 11 */
			if (IS_ENABLED(CONFIG_DIMM_DDR3)) {
				for (i=0; i<12; i++)
					data->f2x9cx30[i] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x30 + i);
				for (i=0; i<12; i++)
					data->f2x9cx40[i] = read_amd_dct_index_register(dev_fn2, 0x98 + (0x100 * channel), 0x40 + i);
			}

			/* Other */
			/* ECC scrub rate control */
			data->f3x58 = pci_read_config32(dev_fn3, 0x58);
		}
	}
}
#else
static void write_amd_dct_index_register(device_t dev, uint32_t index_ctl_reg, uint32_t index, uint32_t value)
{
	uint32_t dword;

	pci_write_config32(dev, index_ctl_reg + 0x04, value);
	index |= (1 << 30);
	pci_write_config32(dev, index_ctl_reg, index);
	do {
		dword = pci_read_config32(dev, index_ctl_reg);
	} while (!(dword & (1 << 31)));
}
#endif

#ifdef __PRE_RAM__
static void wrmsr_uint64_t(unsigned long index, uint64_t value) {
	msr_t msr;
	msr.hi = (value & 0xffffffff00000000ULL) >> 32;
	msr.lo = (value & 0xffffffff);
	wrmsr(index, msr);
}

void restore_mct_data_from_save_variable(struct amd_s3_persistent_data* persistent_data)
{
	uint8_t i;
	uint8_t j;
	uint8_t node;
	uint8_t channel;
	uint8_t ganged;
	uint8_t dct_enabled;
	uint32_t dword;

	/* Load data from data structure into DCTs */
	/* Stage 1 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x110, data->f2x110);
		}
	}

	/* Stage 2 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x40 + (0x100 * channel), data->f1x40);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x44 + (0x100 * channel), data->f1x44);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x48 + (0x100 * channel), data->f1x48);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x4c + (0x100 * channel), data->f1x4c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x50 + (0x100 * channel), data->f1x50);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x54 + (0x100 * channel), data->f1x54);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x58 + (0x100 * channel), data->f1x58);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x5c + (0x100 * channel), data->f1x5c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x60 + (0x100 * channel), data->f1x60);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x64 + (0x100 * channel), data->f1x64);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x68 + (0x100 * channel), data->f1x68);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x6c + (0x100 * channel), data->f1x6c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x70 + (0x100 * channel), data->f1x70);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x74 + (0x100 * channel), data->f1x74);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x78 + (0x100 * channel), data->f1x78);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x7c + (0x100 * channel), data->f1x7c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0xf0 + (0x100 * channel), data->f1xf0);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x120 + (0x100 * channel), data->f1x120);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 1), 0x124 + (0x100 * channel), data->f1x124);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x10c + (0x100 * channel), data->f2x10c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x114 + (0x100 * channel), data->f2x114);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x118 + (0x100 * channel), data->f2x118);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x11c + (0x100 * channel), data->f2x11c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x1b0 + (0x100 * channel), data->f2x1b0);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 3), 0x44 + (0x100 * channel), data->f3x44);
			for (i=0; i<16; i++) {
				wrmsr_uint64_t(0x00000200 | i, data->msr0000020[i]);
			}
			wrmsr_uint64_t(0x00000250, data->msr00000250);
			wrmsr_uint64_t(0x00000258, data->msr00000258);
			/* FIXME
			 * Restoring these MSRs causes a hang on resume
			 * For now, skip restoration...
			 */
			// for (i=0; i<8; i++)
			// 	wrmsr_uint64_t(0x00000260 | (i + 8), data->msr0000026[i]);
			wrmsr_uint64_t(0x000002ff, data->msr000002ff);
			wrmsr_uint64_t(0xc0010010, data->msrc0010010);
			wrmsr_uint64_t(0xc001001a, data->msrc001001a);
			wrmsr_uint64_t(0xc001001d, data->msrc001001d);
			wrmsr_uint64_t(0xc001001f, data->msrc001001f);
		}
	}

	/* Stage 3 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			ganged = !!(data->f2x110 & 0x10);
			if ((ganged == 1) && (channel > 0))
				continue;

			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x40 + (0x100 * channel), data->f2x40);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x44 + (0x100 * channel), data->f2x44);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x48 + (0x100 * channel), data->f2x48);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x4c + (0x100 * channel), data->f2x4c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x50 + (0x100 * channel), data->f2x50);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x54 + (0x100 * channel), data->f2x54);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x58 + (0x100 * channel), data->f2x58);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x5c + (0x100 * channel), data->f2x5c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x60 + (0x100 * channel), data->f2x60);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x64 + (0x100 * channel), data->f2x64);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x68 + (0x100 * channel), data->f2x68);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x6c + (0x100 * channel), data->f2x6c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x78 + (0x100 * channel), data->f2x78);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x7c + (0x100 * channel), data->f2x7c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x80 + (0x100 * channel), data->f2x80);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x84 + (0x100 * channel), data->f2x84);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x88 + (0x100 * channel), data->f2x88);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x8c + (0x100 * channel), data->f2x8c);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x90 + (0x100 * channel), data->f2x90);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0xa4 + (0x100 * channel), data->f2xa4);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0xa8 + (0x100 * channel), data->f2xa8);
		}
	}

	/* Stage 4 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			ganged = !!(data->f2x110 & 0x10);
			if ((ganged == 1) && (channel > 0))
				continue;

			/* Disable PHY auto-compensation engine */
			dword = read_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x08);
			if (!(dword & (1 << 30))) {
				dword |= (1 << 30);
				write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x08, dword);

				/* Wait for 5us */
				mct_Wait(100);
			}

			/* Restore DRAM Configuration High Register */
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x94 + (0x100 * channel), data->f2x94);

			/* Enable PHY auto-compensation engine */
			dword = read_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x08);
			dword &= ~(1 << 30);
			write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x08, dword);
		}
	}

	/* Wait for 750us */
	mct_Wait(15000);

	/* Stage 5 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			ganged = !!(data->f2x110 & 0x10);
			if ((ganged == 1) && (channel > 0))
				continue;

			/* Wait for any pending PHY frequency changes to complete */
			do {
				dword = read_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x08);
			} while (dword & (1 << 21));
		}
	}

	/* Stage 6 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			for (i=0; i<9; i++)
				for (j=0; j<3; j++)
					write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0d0f0000 | (i << 8) | (j * 4), data->f2x9cx0d0f0_f_8_0_0_8_4_0[i][j]);
			write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x00, data->f2x9cx00);
			write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0a, data->f2x9cx0a);
			write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0c, data->f2x9cx0c);
		}
	}

	/* Stage 7 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			ganged = !!(data->f2x110 & 0x10);
			if ((ganged == 1) && (channel > 0))
				continue;

			write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x04, data->f2x9cx04);
		}
	}

	/* Stage 8 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			dct_enabled = !(data->f2x94 & (1 << 14));
			if (!dct_enabled)
				continue;

			ganged = !!(data->f2x110 & 0x10);
			if ((ganged == 1) && (channel > 0))
				continue;

			printk(BIOS_SPEW, "Taking DIMMs out of self refresh node: %d channel: %d\n", node, channel);

			/* Exit self refresh mode */
			dword = pci_read_config32(PCI_DEV(0, 0x18 + node, 2), 0x90 + (0x100 * channel));
			dword |= (1 << 1);
			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x90 + (0x100 * channel), dword);
		}
	}

	/* Stage 9 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			dct_enabled = !(data->f2x94 & (1 << 14));
			if (!dct_enabled)
				continue;

			printk(BIOS_SPEW, "Waiting for DIMMs to exit self refresh node: %d channel: %d\n", node, channel);

			/* Wait for transition from self refresh mode to complete */
			do {
				dword = pci_read_config32(PCI_DEV(0, 0x18 + node, 2), 0x90 + (0x100 * channel));
			} while (dword & (1 << 1));

			/* Restore registers */
			write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0d0fe006, data->f2x9cx0d0fe006);
			write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0d0fe007, data->f2x9cx0d0fe007);
		}
	}

	/* Stage 10 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			for (i=0; i<12; i++)
				write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x10 + i, data->f2x9cx10[i]);
			for (i=0; i<12; i++)
				write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x20 + i, data->f2x9cx20[i]);
			for (i=0; i<4; i++)
				for (j=0; j<3; j++)
					write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), (0x01 + i) + (0x100 * j), data->f2x9cx3_0_0_3_1[i][j]);
			for (i=0; i<4; i++)
				for (j=0; j<3; j++)
					write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), (0x05 + i) + (0x100 * j), data->f2x9cx3_0_0_7_5[i][j]);
			write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0d, data->f2x9cx0d);
			for (i=0; i<9; i++)
				write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0d0f0013 | (i << 8), data->f2x9cx0d0f0_f_0_13[i]);
			for (i=0; i<9; i++)
				write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0d0f0030 | (i << 8), data->f2x9cx0d0f0_f_0_30[i]);
			for (i=0; i<4; i++)
				write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0d0f2030 | (i << 8), data->f2x9cx0d0f2_f_0_30[i]);
			for (i=0; i<2; i++)
				for (j=0; j<3; j++)
					write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0d0f0000 | (i << 8) | (j * 4), data->f2x9cx0d0f8_8_4_0[i][j]);
			write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x0d0f812f, data->f2x9cx0d0f812f);
		}
	}

	/* Stage 11 */
	if (IS_ENABLED(CONFIG_DIMM_DDR3)) {
		for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
			for (channel = 0; channel < 2; channel++) {
				struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
				if (!persistent_data->node[node].node_present)
					continue;

				for (i=0; i<12; i++)
					write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x30 + i, data->f2x9cx30[i]);
				for (i=0; i<12; i++)
					write_amd_dct_index_register(PCI_DEV(0, 0x18 + node, 2), 0x98 + (0x100 * channel), 0x40 + i, data->f2x9cx40[i]);
			}
		}
	}

	/* Other */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data* data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			/* ECC scrub rate control */
			pci_write_config32(PCI_DEV(0, 0x18 + node, 3), 0x58, data->f3x58);
		}
	}
}
#endif

#ifdef __RAMSTAGE__
int8_t save_mct_information_to_nvram(void)
{
	if (acpi_is_wakeup_s3())
		return 0;

	printk(BIOS_DEBUG, "Writing AMD DCT configuration to Flash\n");

	struct spi_flash *flash;
	ssize_t s3nv_offset;
	struct amd_s3_persistent_data persistent_data;

	/* Obtain MCT configuration data */
	copy_mct_data_to_save_variable(&persistent_data);

	/* Obtain CBFS file offset */
	s3nv_offset = get_s3nv_file_offset();
	if (s3nv_offset == -1)
		return -1;

	/* Align flash pointer to nearest boundary */
	s3nv_offset &= ~(CONFIG_S3_DATA_SIZE-1);
	s3nv_offset += CONFIG_S3_DATA_SIZE;

	/* Set temporary SPI MMIO address */
	device_t lpc_dev = dev_find_slot(0, PCI_DEVFN(0x14, 3));
	uint32_t spi_mmio_prev = pci_read_config32(lpc_dev, 0xa0);
	pci_write_config32(lpc_dev, 0xa0, (spi_mmio_prev & 0x1f) | 0xf0000000);

	/* Initialize SPI and detect devices */
	spi_init();
	flash = spi_flash_probe(0, 0);
	if (!flash) {
		printk(BIOS_DEBUG, "Could not find SPI device\n");
		return -1;
	}

	/* Set up SPI flash access */
	flash->spi->rw = SPI_WRITE_FLAG;
	spi_claim_bus(flash->spi);

	/* Erase and write data structure */
	flash->erase(flash, s3nv_offset, CONFIG_S3_DATA_SIZE);
	flash->write(flash, s3nv_offset, sizeof(struct amd_s3_persistent_data), &persistent_data);

	/* Tear down SPI flash access */
	flash->spi->rw = SPI_WRITE_FLAG;
	spi_release_bus(flash->spi);

	/* Restore SPI MMIO address */
	pci_write_config32(lpc_dev, 0xa0, spi_mmio_prev);

	return 0;
}
#endif

int8_t restore_mct_information_from_nvram(void)
{
	ssize_t s3nv_offset;
	ssize_t s3nv_file_offset;
	void * s3nv_cbfs_file_ptr;
	struct amd_s3_persistent_data *persistent_data;

	/* Obtain CBFS file offset */
	s3nv_offset = get_s3nv_file_offset();
	if (s3nv_offset == -1)
		return -1;

	/* Align flash pointer to nearest boundary */
	s3nv_file_offset = s3nv_offset;
	s3nv_offset &= ~(CONFIG_S3_DATA_SIZE-1);
	s3nv_offset += CONFIG_S3_DATA_SIZE;
	s3nv_file_offset = s3nv_offset - s3nv_file_offset;

	/* Map data structure in CBFS and restore settings */
	s3nv_cbfs_file_ptr = cbfs_boot_map_with_leak(S3NV_FILE_NAME, CBFS_TYPE_RAW, NULL);
	if (!s3nv_cbfs_file_ptr) {
		printk(BIOS_DEBUG, "S3 state file could not be mapped: %s\n", S3NV_FILE_NAME);
		return -1;
	}
	persistent_data = (s3nv_cbfs_file_ptr + s3nv_file_offset);
	restore_mct_data_from_save_variable(persistent_data);

	return 0;
}