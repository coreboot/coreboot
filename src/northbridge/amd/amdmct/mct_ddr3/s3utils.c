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
#include <arch/cpu.h>
#include <arch/acpi.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cbfs.h>
#include <cbmem.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <pc80/mc146818rtc.h>
#include <inttypes.h>
#include <console/console.h>
#include <string.h>
#include "mct_d.h"
#include "mct_d_gcc.h"

#include "s3utils.h"

#define S3NV_FILE_NAME "s3nv"

static uint8_t is_fam15h(void)
{
	uint8_t fam15h = 0;
	uint32_t family;

	family = cpuid_eax(0x80000001);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f)
		/* Family 15h or later */
		fam15h = 1;

	return fam15h;
}

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

static uint32_t read_config32_dct(device_t dev, uint8_t node, uint8_t dct, uint32_t reg) {
	if (is_fam15h()) {
		uint32_t dword;
#ifdef __PRE_RAM__
		pci_devfn_t dev_fn1 = PCI_DEV(0, 0x18 + node, 1);
#else
		struct device *dev_fn1 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 1));
#endif

		/* Select DCT */
		dword = pci_read_config32(dev_fn1, 0x10c);
		dword &= ~0x1;
		dword |= (dct & 0x1);
		pci_write_config32(dev_fn1, 0x10c, dword);
	} else {
		/* Apply offset */
		reg += dct * 0x100;
	}

	return pci_read_config32(dev, reg);
}

static void write_config32_dct(device_t dev, uint8_t node, uint8_t dct, uint32_t reg, uint32_t value) {
	if (is_fam15h()) {
		uint32_t dword;
#ifdef __PRE_RAM__
		pci_devfn_t dev_fn1 = PCI_DEV(0, 0x18 + node, 1);
#else
		struct device *dev_fn1 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 1));
#endif

		/* Select DCT */
		dword = pci_read_config32(dev_fn1, 0x10c);
		dword &= ~0x1;
		dword |= (dct & 0x1);
		pci_write_config32(dev_fn1, 0x10c, dword);
	} else {
		/* Apply offset */
		reg += dct * 0x100;
	}

	pci_write_config32(dev, reg, value);
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

static uint32_t read_amd_dct_index_register_dct(device_t dev, uint8_t node, uint8_t dct, uint32_t index_ctl_reg, uint32_t index)
{
	if (is_fam15h()) {
		uint32_t dword;
#ifdef __PRE_RAM__
		pci_devfn_t dev_fn1 = PCI_DEV(0, 0x18 + node, 1);
#else
		struct device *dev_fn1 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 1));
#endif

		/* Select DCT */
		dword = pci_read_config32(dev_fn1, 0x10c);
		dword &= ~0x1;
		dword |= (dct & 0x1);
		pci_write_config32(dev_fn1, 0x10c, dword);
	} else {
		/* Apply offset */
		index_ctl_reg += dct * 0x100;
	}

	return read_amd_dct_index_register(dev, index_ctl_reg, index);
}

/* Non-cryptographic 64-bit hash function taken from Stack Overflow:
 * http://stackoverflow.com/a/13326345
 * Any 64-bit hash with sufficiently low collision potential
 * could be used instead.
 */
void calculate_spd_hash(uint8_t *spd_data, uint64_t *spd_hash)
{
	const unsigned long long prime = 2654435789ULL;
	uint16_t byte;
	*spd_hash = 104395301;

	for (byte = 0; byte < 256; byte++)
		*spd_hash += (spd_data[byte] * prime) ^ (*spd_hash >> 23);

	*spd_hash = *spd_hash ^ (*spd_hash << 37);
}

uint16_t calculate_nvram_mct_hash(void)
{
	uint32_t nvram;
	uint16_t ret;

	ret = 0;
	if (get_option(&nvram, "max_mem_clock") == CB_SUCCESS)
		ret |= nvram & 0xf;
	if (get_option(&nvram, "minimum_memory_voltage") == CB_SUCCESS)
		ret |= (nvram & 0x3) << 4;
	if (get_option(&nvram, "ECC_memory") == CB_SUCCESS)
		ret |= (nvram & 0x1) << 6;
	if (get_option(&nvram, "ECC_redirection") == CB_SUCCESS)
		ret |= (nvram & 0x1) << 7;
	if (get_option(&nvram, "ecc_scrub_rate") == CB_SUCCESS)
		ret |= (nvram & 0x1) << 8;
	if (get_option(&nvram, "interleave_chip_selects") == CB_SUCCESS)
		ret |= (nvram & 0x1) << 9;
	if (get_option(&nvram, "interleave_nodes") == CB_SUCCESS)
		ret |= (nvram & 0x1) << 10;
	if (get_option(&nvram, "interleave_memory_channels") == CB_SUCCESS)
		ret |= (nvram & 0x1) << 11;
	if (get_option(&nvram, "cpu_c_states") == CB_SUCCESS)
		ret |= (nvram & 0x1) << 12;
	if (get_option(&nvram, "cpu_cc6_state") == CB_SUCCESS)
		ret |= (nvram & 0x1) << 13;

	return ret;
}

static struct amd_s3_persistent_data *map_s3nv_in_nvram(void)
{
	ssize_t s3nv_offset;
	ssize_t s3nv_file_offset;
	void *s3nv_cbfs_file_ptr;
	struct amd_s3_persistent_data *persistent_data;

	/* Obtain CBFS file offset */
	s3nv_offset = get_s3nv_file_offset();
	if (s3nv_offset == -1)
		return NULL;

	/* Align flash pointer to nearest boundary */
	s3nv_file_offset = s3nv_offset;
	s3nv_offset &= ~(CONFIG_S3_DATA_SIZE-1);
	s3nv_offset += CONFIG_S3_DATA_SIZE;
	s3nv_file_offset = s3nv_offset - s3nv_file_offset;

	/* Map data structure in CBFS and restore settings */
	s3nv_cbfs_file_ptr = cbfs_boot_map_with_leak(S3NV_FILE_NAME, CBFS_TYPE_RAW, NULL);
	if (!s3nv_cbfs_file_ptr) {
		printk(BIOS_DEBUG, "S3 state file could not be mapped: %s\n", S3NV_FILE_NAME);
		return NULL;
	}
	persistent_data = (s3nv_cbfs_file_ptr + s3nv_file_offset);

	return persistent_data;
}

#ifdef __PRE_RAM__
int8_t load_spd_hashes_from_nvram(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
{
	struct amd_s3_persistent_data *persistent_data;

	persistent_data = map_s3nv_in_nvram();
	if (!persistent_data)
		return -1;

	memcpy(pDCTstat->spd_data.nvram_spd_hash, persistent_data->node[pDCTstat->Node_ID].spd_hash, sizeof(pDCTstat->spd_data.nvram_spd_hash));
	memcpy(pDCTstat->spd_data.nvram_memclk, persistent_data->node[pDCTstat->Node_ID].memclk, sizeof(pDCTstat->spd_data.nvram_memclk));

	pMCTstat->nvram_checksum = persistent_data->nvram_checksum;

	return 0;
}
#endif

#ifdef __RAMSTAGE__
static uint64_t rdmsr_uint64_t(unsigned long index) {
	msr_t msr = rdmsr(index);
	return (((uint64_t)msr.hi) << 32) | ((uint64_t)msr.lo);
}

static uint32_t read_config32_dct_nbpstate(struct device *dev, uint8_t node,
					   uint8_t dct, uint8_t nb_pstate,
					   uint32_t reg)
{
	uint32_t dword;
	struct device *dev_fn1 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 1));

	/* Select DCT */
	dword = pci_read_config32(dev_fn1, 0x10c);
	dword &= ~0x1;
	dword |= (dct & 0x1);
	pci_write_config32(dev_fn1, 0x10c, dword);

	/* Select NB Pstate index */
	dword = pci_read_config32(dev_fn1, 0x10c);
	dword &= ~(0x3 << 4);
	dword |= (nb_pstate & 0x3) << 4;
	pci_write_config32(dev_fn1, 0x10c, dword);

	return pci_read_config32(dev, reg);
}

static void copy_cbmem_spd_data_to_save_variable(struct amd_s3_persistent_data *persistent_data, uint8_t *restored)
{
	uint8_t node;
	uint8_t dimm;
	uint8_t channel;
	struct amdmct_memory_info *mem_info;
	mem_info = cbmem_find(CBMEM_ID_AMDMCT_MEMINFO);
	if (mem_info == NULL) {
		/* can't find amdmct information in cbmem */
		for (node = 0; node < MAX_NODES_SUPPORTED; node++)
			for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm++)
				persistent_data->node[node].spd_hash[dimm] = 0xffffffffffffffffULL;

		return;
	}

	for (node = 0; node < MAX_NODES_SUPPORTED; node++)
		for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm++)
			calculate_spd_hash(mem_info->dct_stat[node].spd_data.spd_bytes[dimm], &persistent_data->node[node].spd_hash[dimm]);

	for (node = 0; node < MAX_NODES_SUPPORTED; node++)
		for (channel = 0; channel < 2; channel++)
			persistent_data->node[node].memclk[channel] = mem_info->dct_stat[node].Speed;

	persistent_data->nvram_checksum = calculate_nvram_mct_hash();

	if (restored) {
		if (mem_info->mct_stat.GStatus & (1 << GSB_ConfigRestored))
			*restored = 1;
		else
			*restored = 0;
	}
}

void copy_mct_data_to_save_variable(struct amd_s3_persistent_data *persistent_data)
{
	uint8_t i;
	uint8_t j;
	uint8_t node;
	uint8_t channel;

	/* Zero out data structure */
	memset(persistent_data, 0, sizeof(struct amd_s3_persistent_data));

	/* Load data from DCTs into data structure */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		struct device *dev_fn1 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 1));
		struct device *dev_fn2 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 2));
		struct device *dev_fn3 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 3));
		/* Test for node presence */
		if ((!dev_fn1) || (pci_read_config32(dev_fn1, PCI_VENDOR_ID) == 0xffffffff)) {
			persistent_data->node[node].node_present = 0;
			continue;
		}
		persistent_data->node[node].node_present = 1;

		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];

			/* Stage 1 */
			data->f2x110 = pci_read_config32(dev_fn2, 0x110);

			/* Stage 2 */
			data->f1x40 = read_config32_dct(dev_fn1, node, channel, 0x40);
			data->f1x44 = read_config32_dct(dev_fn1, node, channel, 0x44);
			data->f1x48 = read_config32_dct(dev_fn1, node, channel, 0x48);
			data->f1x4c = read_config32_dct(dev_fn1, node, channel, 0x4c);
			data->f1x50 = read_config32_dct(dev_fn1, node, channel, 0x50);
			data->f1x54 = read_config32_dct(dev_fn1, node, channel, 0x54);
			data->f1x58 = read_config32_dct(dev_fn1, node, channel, 0x58);
			data->f1x5c = read_config32_dct(dev_fn1, node, channel, 0x5c);
			data->f1x60 = read_config32_dct(dev_fn1, node, channel, 0x60);
			data->f1x64 = read_config32_dct(dev_fn1, node, channel, 0x64);
			data->f1x68 = read_config32_dct(dev_fn1, node, channel, 0x68);
			data->f1x6c = read_config32_dct(dev_fn1, node, channel, 0x6c);
			data->f1x70 = read_config32_dct(dev_fn1, node, channel, 0x70);
			data->f1x74 = read_config32_dct(dev_fn1, node, channel, 0x74);
			data->f1x78 = read_config32_dct(dev_fn1, node, channel, 0x78);
			data->f1x7c = read_config32_dct(dev_fn1, node, channel, 0x7c);
			data->f1xf0 = pci_read_config32(dev_fn1, 0xf0);
			data->f1x120 = pci_read_config32(dev_fn1, 0x120);
			data->f1x124 = pci_read_config32(dev_fn1, 0x124);
			data->f2x10c = pci_read_config32(dev_fn2, 0x10c);
			data->f2x114 = pci_read_config32(dev_fn2, 0x114);
			data->f2x118 = pci_read_config32(dev_fn2, 0x118);
			data->f2x11c = pci_read_config32(dev_fn2, 0x11c);
			data->f2x1b0 = pci_read_config32(dev_fn2, 0x1b0);
			data->f3x44 = pci_read_config32(dev_fn3, 0x44);
			for (i = 0; i < 16; i++) {
				data->msr0000020[i] = rdmsr_uint64_t(0x00000200 | i);
			}
			data->msr00000250 = rdmsr_uint64_t(0x00000250);
			data->msr00000258 = rdmsr_uint64_t(0x00000258);
			for (i = 0; i < 8; i++)
				data->msr0000026[i] = rdmsr_uint64_t(0x00000260 | (i + 8));
			data->msr000002ff = rdmsr_uint64_t(0x000002ff);
			data->msrc0010010 = rdmsr_uint64_t(0xc0010010);
			data->msrc001001a = rdmsr_uint64_t(0xc001001a);
			data->msrc001001d = rdmsr_uint64_t(0xc001001d);
			data->msrc001001f = rdmsr_uint64_t(0xc001001f);

			/* Stage 3 */
			data->f2x40 = read_config32_dct(dev_fn2, node, channel, 0x40);
			data->f2x44 = read_config32_dct(dev_fn2, node, channel, 0x44);
			data->f2x48 = read_config32_dct(dev_fn2, node, channel, 0x48);
			data->f2x4c = read_config32_dct(dev_fn2, node, channel, 0x4c);
			data->f2x50 = read_config32_dct(dev_fn2, node, channel, 0x50);
			data->f2x54 = read_config32_dct(dev_fn2, node, channel, 0x54);
			data->f2x58 = read_config32_dct(dev_fn2, node, channel, 0x58);
			data->f2x5c = read_config32_dct(dev_fn2, node, channel, 0x5c);
			data->f2x60 = read_config32_dct(dev_fn2, node, channel, 0x60);
			data->f2x64 = read_config32_dct(dev_fn2, node, channel, 0x64);
			data->f2x68 = read_config32_dct(dev_fn2, node, channel, 0x68);
			data->f2x6c = read_config32_dct(dev_fn2, node, channel, 0x6c);
			data->f2x78 = read_config32_dct(dev_fn2, node, channel, 0x78);
			data->f2x7c = read_config32_dct(dev_fn2, node, channel, 0x7c);
			data->f2x80 = read_config32_dct(dev_fn2, node, channel, 0x80);
			data->f2x84 = read_config32_dct(dev_fn2, node, channel, 0x84);
			data->f2x88 = read_config32_dct(dev_fn2, node, channel, 0x88);
			data->f2x8c = read_config32_dct(dev_fn2, node, channel, 0x8c);
			data->f2x90 = read_config32_dct(dev_fn2, node, channel, 0x90);
			data->f2xa4 = read_config32_dct(dev_fn2, node, channel, 0xa4);
			data->f2xa8 = read_config32_dct(dev_fn2, node, channel, 0xa8);

			/* Family 15h-specific configuration */
			if (is_fam15h()) {
				data->f2x200 = read_config32_dct(dev_fn2, node, channel, 0x200);
				data->f2x204 = read_config32_dct(dev_fn2, node, channel, 0x204);
				data->f2x208 = read_config32_dct(dev_fn2, node, channel, 0x208);
				data->f2x20c = read_config32_dct(dev_fn2, node, channel, 0x20c);
				for (i = 0; i < 4; i++)
					data->f2x210[i] = read_config32_dct_nbpstate(dev_fn2, node, channel, i, 0x210);
				data->f2x214 = read_config32_dct(dev_fn2, node, channel, 0x214);
				data->f2x218 = read_config32_dct(dev_fn2, node, channel, 0x218);
				data->f2x21c = read_config32_dct(dev_fn2, node, channel, 0x21c);
				data->f2x22c = read_config32_dct(dev_fn2, node, channel, 0x22c);
				data->f2x230 = read_config32_dct(dev_fn2, node, channel, 0x230);
				data->f2x234 = read_config32_dct(dev_fn2, node, channel, 0x234);
				data->f2x238 = read_config32_dct(dev_fn2, node, channel, 0x238);
				data->f2x23c = read_config32_dct(dev_fn2, node, channel, 0x23c);
				data->f2x240 = read_config32_dct(dev_fn2, node, channel, 0x240);

				data->f2x9cx0d0fe003 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fe003);
				data->f2x9cx0d0fe013 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fe013);
				for (i = 0; i < 9; i++)
					data->f2x9cx0d0f0_8_0_1f[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f001f | (i << 8));
				data->f2x9cx0d0f201f = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f201f);
				data->f2x9cx0d0f211f = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f211f);
				data->f2x9cx0d0f221f = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f221f);
				data->f2x9cx0d0f801f = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f801f);
				data->f2x9cx0d0f811f = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f811f);
				data->f2x9cx0d0f821f = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f821f);
				data->f2x9cx0d0fc01f = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc01f);
				data->f2x9cx0d0fc11f = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc11f);
				data->f2x9cx0d0fc21f = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc21f);
				data->f2x9cx0d0f4009 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f4009);
				for (i = 0; i < 9; i++)
					data->f2x9cx0d0f0_8_0_02[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f0002 | (i << 8));
				for (i = 0; i < 9; i++)
					data->f2x9cx0d0f0_8_0_06[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f0006 | (i << 8));
				for (i = 0; i < 9; i++)
					data->f2x9cx0d0f0_8_0_0a[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f000a | (i << 8));

				data->f2x9cx0d0f2002 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f2002);
				data->f2x9cx0d0f2102 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f2102);
				data->f2x9cx0d0f2202 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f2202);
				data->f2x9cx0d0f8002 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f8002);
				data->f2x9cx0d0f8006 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f8006);
				data->f2x9cx0d0f800a = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f800a);
				data->f2x9cx0d0f8102 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f8102);
				data->f2x9cx0d0f8106 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f8106);
				data->f2x9cx0d0f810a = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f810a);
				data->f2x9cx0d0fc002 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc002);
				data->f2x9cx0d0fc006 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc006);
				data->f2x9cx0d0fc00a = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc00a);
				data->f2x9cx0d0fc00e = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc00e);
				data->f2x9cx0d0fc012 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc012);

				data->f2x9cx0d0f2031 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f2031);
				data->f2x9cx0d0f2131 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f2131);
				data->f2x9cx0d0f2231 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f2231);
				data->f2x9cx0d0f8031 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f8031);
				data->f2x9cx0d0f8131 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f8131);
				data->f2x9cx0d0f8231 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f8231);
				data->f2x9cx0d0fc031 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc031);
				data->f2x9cx0d0fc131 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc131);
				data->f2x9cx0d0fc231 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fc231);
				for (i = 0; i < 9; i++)
					data->f2x9cx0d0f0_0_f_31[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f0031 | (i << 8));

				data->f2x9cx0d0f8021 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f8021);

				if (channel == 1)
					data->f2x9cx0d0fe00a = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fe00a);
			}

			/* Stage 4 */
			data->f2x94 = read_config32_dct(dev_fn2, node, channel, 0x94);

			/* Stage 6 */
			for (i = 0; i < 9; i++)
				for (j = 0; j < 3; j++)
					data->f2x9cx0d0f0_f_8_0_0_8_4_0[i][j] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f0000 | (i << 8) | (j * 4));
			data->f2x9cx00 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x00);
			data->f2x9cx0a = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0a);
			data->f2x9cx0c = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0c);

			/* Stage 7 */
			data->f2x9cx04 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x04);

			/* Stage 9 */
			data->f2x9cx0d0fe006 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fe006);
			data->f2x9cx0d0fe007 = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0fe007);

			/* Stage 10 */
			for (i = 0; i < 12; i++)
				data->f2x9cx10[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x10 + i);
			for (i = 0; i < 12; i++)
				data->f2x9cx20[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x20 + i);
			for (i = 0; i < 4; i++)
				for (j = 0; j < 3; j++)
					data->f2x9cx3_0_0_3_1[i][j] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, (0x01 + i) + (0x100 * j));
			for (i = 0; i < 4; i++)
				for (j = 0; j < 3; j++)
					data->f2x9cx3_0_0_7_5[i][j] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, (0x05 + i) + (0x100 * j));
			data->f2x9cx0d = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d);
			for (i = 0; i < 9; i++)
				data->f2x9cx0d0f0_f_0_13[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f0013 | (i << 8));
			for (i = 0; i < 9; i++)
				data->f2x9cx0d0f0_f_0_30[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f0030 | (i << 8));
			for (i = 0; i < 4; i++)
				data->f2x9cx0d0f2_f_0_30[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f2030 | (i << 8));
			for (i = 0; i < 2; i++)
				for (j = 0; j < 3; j++)
					data->f2x9cx0d0f8_8_4_0[i][j] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f0000 | (i << 8) | (j * 4));
			data->f2x9cx0d0f812f = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x0d0f812f);

			/* Stage 11 */
			if (IS_ENABLED(CONFIG_DIMM_DDR3)) {
				for (i = 0; i < 12; i++)
					data->f2x9cx30[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x30 + i);
				for (i = 0; i < 12; i++)
					data->f2x9cx40[i] = read_amd_dct_index_register_dct(dev_fn2, node, channel, 0x98, 0x40 + i);
			}

			/* Other */
			/* ECC scrub rate control */
			data->f3x58 = read_config32_dct(dev_fn3, node, 0, 0x58);

			/* ECC scrub location */
			write_config32_dct(dev_fn3, node, 0, 0x58, 0x0);		/* Disable sequential scrub to work around non-atomic location read */
			data->f3x5c = read_config32_dct(dev_fn3, node, 0, 0x5c);
			data->f3x60 = read_config32_dct(dev_fn3, node, 0, 0x60);
			write_config32_dct(dev_fn3, node, 0, 0x58, data->f3x58);	/* Re-enable sequential scrub */
		}
	}
}
#else
static void write_config32_dct_nbpstate(pci_devfn_t dev, uint8_t node,
					uint8_t dct, uint8_t nb_pstate,
					uint32_t reg, uint32_t value)
{
	uint32_t dword;
	pci_devfn_t dev_fn1 = PCI_DEV(0, 0x18 + node, 1);

	/* Select DCT */
	dword = pci_read_config32(dev_fn1, 0x10c);
	dword &= ~0x1;
	dword |= (dct & 0x1);
	pci_write_config32(dev_fn1, 0x10c, dword);

	/* Select NB Pstate index */
	dword = pci_read_config32(dev_fn1, 0x10c);
	dword &= ~(0x3 << 4);
	dword |= (nb_pstate & 0x3) << 4;
	pci_write_config32(dev_fn1, 0x10c, dword);

	pci_write_config32(dev, reg, value);
}

static void write_amd_dct_index_register(pci_devfn_t dev,
					 uint32_t index_ctl_reg, uint32_t index,
					 uint32_t value)
{
	uint32_t dword;

	pci_write_config32(dev, index_ctl_reg + 0x04, value);
	index |= (1 << 30);
	pci_write_config32(dev, index_ctl_reg, index);
	do {
		dword = pci_read_config32(dev, index_ctl_reg);
	} while (!(dword & (1 << 31)));
}

static void write_amd_dct_index_register_dct(pci_devfn_t dev, uint8_t node,
					     uint8_t dct,
					     uint32_t index_ctl_reg,
					     uint32_t index, uint32_t value)
{
	if (is_fam15h()) {
		uint32_t dword;
		pci_devfn_t dev_fn1 = PCI_DEV(0, 0x18 + node, 1);

		/* Select DCT */
		dword = pci_read_config32(dev_fn1, 0x10c);
		dword &= ~0x1;
		dword |= (dct & 0x1);
		pci_write_config32(dev_fn1, 0x10c, dword);
	} else {
		/* Apply offset */
		index_ctl_reg += dct * 0x100;
	}

	return write_amd_dct_index_register(dev, index_ctl_reg, index, value);
}
#endif

#ifdef __PRE_RAM__
static void wrmsr_uint64_t(unsigned long index, uint64_t value) {
	msr_t msr;
	msr.hi = (value & 0xffffffff00000000ULL) >> 32;
	msr.lo = (value & 0xffffffff);
	wrmsr(index, msr);
}

void restore_mct_data_from_save_variable(struct amd_s3_persistent_data *persistent_data, uint8_t training_only)
{
	uint8_t i;
	uint8_t j;
	uint8_t node;
	uint8_t channel;
	uint8_t ganged;
	uint8_t dct_enabled;
	uint32_t dword;

	if (training_only) {
		/* Only restore the Receiver Enable and DQS training registers */
		for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
			for (channel = 0; channel < 2; channel++) {
				struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
				if (!persistent_data->node[node].node_present)
					continue;

				/* Restore training parameters */
				for (i = 0; i < 4; i++)
					for (j = 0; j < 3; j++)
						write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, (0x01 + i) + (0x100 * j), data->f2x9cx3_0_0_3_1[i][j]);
				for (i = 0; i < 4; i++)
					for (j = 0; j < 3; j++)
						write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, (0x05 + i) + (0x100 * j), data->f2x9cx3_0_0_7_5[i][j]);

				for (i = 0; i < 12; i++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x10 + i, data->f2x9cx10[i]);
				for (i = 0; i < 12; i++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x20 + i, data->f2x9cx20[i]);

				if (IS_ENABLED(CONFIG_DIMM_DDR3)) {
					for (i = 0; i < 12; i++)
						write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x30 + i, data->f2x9cx30[i]);
					for (i = 0; i < 12; i++)
						write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x40 + i, data->f2x9cx40[i]);
				}

				/* Restore MaxRdLatency */
				if (is_fam15h()) {
					for (i = 0; i < 4; i++)
						write_config32_dct_nbpstate(PCI_DEV(0, 0x18 + node, 2), node, channel, i, 0x210, data->f2x210[i]);
				} else {
					write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x78, data->f2x78);
				}

				/* Other timing control registers */
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x8c, data->f2x8c);
			}
		}

		return;
	}

	/* Load data from data structure into DCTs */
	/* Stage 1 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			pci_write_config32(PCI_DEV(0, 0x18 + node, 2), 0x110, data->f2x110);
		}
	}

	/* Stage 2 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x40, data->f1x40);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x44, data->f1x44);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x48, data->f1x48);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x4c, data->f1x4c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x50, data->f1x50);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x54, data->f1x54);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x58, data->f1x58);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x5c, data->f1x5c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x60, data->f1x60);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x64, data->f1x64);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x68, data->f1x68);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x6c, data->f1x6c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x70, data->f1x70);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x74, data->f1x74);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x78, data->f1x78);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x7c, data->f1x7c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0xf0, data->f1xf0);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x120, data->f1x120);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 1), node, channel, 0x124, data->f1x124);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x10c, data->f2x10c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x114, data->f2x114);
			if (is_fam15h())
				/* Do not set LockDramCfg or CC6SaveEn at this time */
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x118, data->f2x118 & ~(0x3 << 18));
			else
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x118, data->f2x118);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x11c, data->f2x11c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x1b0, data->f2x1b0);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 3), node, channel, 0x44, data->f3x44);
			for (i = 0; i < 16; i++) {
				wrmsr_uint64_t(0x00000200 | i, data->msr0000020[i]);
			}
			wrmsr_uint64_t(0x00000250, data->msr00000250);
			wrmsr_uint64_t(0x00000258, data->msr00000258);
			/* FIXME
			 * Restoring these MSRs causes a hang on resume due to
			 * destroying CAR while still executing from CAR!
			 * For now, skip restoration...
			 */
			// for (i = 0; i < 8; i++)
			//	wrmsr_uint64_t(0x00000260 | (i + 8), data->msr0000026[i]);
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
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			if (is_fam15h())
				ganged = 0;
			else
				ganged = !!(data->f2x110 & 0x10);
			if ((ganged == 1) && (channel > 0))
				continue;

			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x40, data->f2x40);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x44, data->f2x44);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x48, data->f2x48);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x4c, data->f2x4c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x50, data->f2x50);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x54, data->f2x54);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x58, data->f2x58);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x5c, data->f2x5c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x60, data->f2x60);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x64, data->f2x64);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x68, data->f2x68);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x6c, data->f2x6c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x78, data->f2x78);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x7c, data->f2x7c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x80, data->f2x80);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x84, data->f2x84);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x88, data->f2x88);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x8c, data->f2x8c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x90, data->f2x90);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0xa4, data->f2xa4);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0xa8, data->f2xa8);
		}
	}

	/* Family 15h-specific configuration */
	if (is_fam15h()) {
		for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
			for (channel = 0; channel < 2; channel++) {
				struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
				if (!persistent_data->node[node].node_present)
					continue;

				/* Initialize DCT */
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0000000b, 0x80000000);
				dword = read_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe013);
				dword &= ~0xffff;
				dword |= 0x118;
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe013, dword);

				/* Restore values */
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x200, data->f2x200);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x204, data->f2x204);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x208, data->f2x208);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x20c, data->f2x20c);
				for (i = 0; i < 4; i++)
					write_config32_dct_nbpstate(PCI_DEV(0, 0x18 + node, 2), node, channel, i, 0x210, data->f2x210[i]);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x214, data->f2x214);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x218, data->f2x218);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x21c, data->f2x21c);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x22c, data->f2x22c);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x230, data->f2x230);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x234, data->f2x234);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x238, data->f2x238);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x23c, data->f2x23c);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x240, data->f2x240);

				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe013, data->f2x9cx0d0fe013);
				for (i = 0; i < 9; i++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f001f | (i << 8), data->f2x9cx0d0f0_8_0_1f[i]);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f201f, data->f2x9cx0d0f201f);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f211f, data->f2x9cx0d0f211f);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f221f, data->f2x9cx0d0f221f);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f801f, data->f2x9cx0d0f801f);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f811f, data->f2x9cx0d0f811f);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f821f, data->f2x9cx0d0f821f);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc01f, data->f2x9cx0d0fc01f);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc11f, data->f2x9cx0d0fc11f);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc21f, data->f2x9cx0d0fc21f);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f4009, data->f2x9cx0d0f4009);

				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f2031, data->f2x9cx0d0f2031);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f2131, data->f2x9cx0d0f2131);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f2231, data->f2x9cx0d0f2231);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f8031, data->f2x9cx0d0f8031);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f8131, data->f2x9cx0d0f8131);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f8231, data->f2x9cx0d0f8231);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc031, data->f2x9cx0d0fc031);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc131, data->f2x9cx0d0fc131);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc231, data->f2x9cx0d0fc231);
				for (i = 0; i < 9; i++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f0031 | (i << 8), data->f2x9cx0d0f0_0_f_31[i]);

				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f8021, data->f2x9cx0d0f8021);

				if (channel == 1)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe00a, data->f2x9cx0d0fe00a);
			}
		}
	}

	/* Stage 4 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			if (is_fam15h())
				ganged = 0;
			else
				ganged = !!(data->f2x110 & 0x10);
			if ((ganged == 1) && (channel > 0))
				continue;

			if (is_fam15h()) {
				/* Program PllLockTime = 0x190 */
				dword = read_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe006);
				dword &= ~0xffff;
				dword |= 0x190;
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe006, dword);

				/* Program MemClkFreqVal = 0 */
				dword = read_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x94);
				dword &= (0x1 << 7);
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x94, dword);

				/* Restore DRAM Adddress/Timing Control Register */
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x04, data->f2x9cx04);
			} else {
				/* Disable PHY auto-compensation engine */
				dword = read_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x08);
				if (!(dword & (1 << 30))) {
					dword |= (1 << 30);
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x08, dword);

					/* Wait for 5us */
					mct_Wait(100);
				}
			}

			/* Restore DRAM Configuration High Register */
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x94, data->f2x94);
		}
	}

	/* Stage 5 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			if (is_fam15h())
				ganged = 0;
			else
				ganged = !!(data->f2x110 & 0x10);
			if ((ganged == 1) && (channel > 0))
				continue;

			dct_enabled = !(data->f2x94 & (1 << 14));
			if (!dct_enabled)
				continue;

			/* Wait for any pending PHY frequency changes to complete */
			do {
				dword = read_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x94);
			} while (dword & (1 << 21));

			if (is_fam15h()) {
				/* Program PllLockTime = 0xf */
				dword = read_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe006);
				dword &= ~0xffff;
				dword |= 0xf;
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe006, dword);
			} else {
				/* Enable PHY auto-compensation engine */
				dword = read_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x08);
				dword &= ~(1 << 30);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x08, dword);
			}
		}
	}

	/* Wait for 750us */
	mct_Wait(15000);

	/* Stage 6 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			for (i = 0; i < 9; i++)
				for (j = 0; j < 3; j++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f0000 | (i << 8) | (j * 4), data->f2x9cx0d0f0_f_8_0_0_8_4_0[i][j]);
			write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x00, data->f2x9cx00);
			write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0a, data->f2x9cx0a);
			write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0c, data->f2x9cx0c);
		}
	}

	/* Family 15h-specific configuration */
	if (is_fam15h()) {
		for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
			for (channel = 0; channel < 2; channel++) {
				struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
				if (!persistent_data->node[node].node_present)
					continue;

				dword = read_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe003);
				dword |= (0x3 << 13);			/* DisAutoComp, DisablePredriverCal = 1 */
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe003, dword);

				for (i = 0; i < 9; i++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f0006 | (i << 8), data->f2x9cx0d0f0_8_0_06[i]);
				for (i = 0; i < 9; i++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f000a | (i << 8), data->f2x9cx0d0f0_8_0_0a[i]);
				for (i = 0; i < 9; i++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f0002 | (i << 8), (0x8000 | data->f2x9cx0d0f0_8_0_02[i]));

				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f8006, data->f2x9cx0d0f8006);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f800a, data->f2x9cx0d0f800a);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f8106, data->f2x9cx0d0f8106);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f810a, data->f2x9cx0d0f810a);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc006, data->f2x9cx0d0fc006);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc00a, data->f2x9cx0d0fc00a);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc00e, data->f2x9cx0d0fc00e);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc012, data->f2x9cx0d0fc012);
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f8002, (0x8000 | data->f2x9cx0d0f8002));
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f8102, (0x8000 | data->f2x9cx0d0f8102));
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fc002, (0x8000 | data->f2x9cx0d0fc002));
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f2002, (0x8000 | data->f2x9cx0d0f2002));
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f2102, (0x8000 | data->f2x9cx0d0f2102));
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f2202, (0x8000 | data->f2x9cx0d0f2202));

				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe003, data->f2x9cx0d0fe003);
			}
		}
	}

	/* Stage 7 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			if (is_fam15h())
				ganged = 0;
			else
				ganged = !!(data->f2x110 & 0x10);
			if ((ganged == 1) && (channel > 0))
				continue;

			if (!is_fam15h())
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x04, data->f2x9cx04);
		}
	}

	/* Stage 8 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			dct_enabled = !(data->f2x94 & (1 << 14));
			if (!dct_enabled)
				continue;

			if (is_fam15h())
				ganged = 0;
			else
				ganged = !!(data->f2x110 & 0x10);
			if ((ganged == 1) && (channel > 0))
				continue;

			printk(BIOS_SPEW, "Taking DIMMs out of self refresh node: %d channel: %d\n", node, channel);

			/* Exit self refresh mode */
			dword = read_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x90);
			dword |= (1 << 1);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x90, dword);
		}
	}

	/* Stage 9 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			dct_enabled = !(data->f2x94 & (1 << 14));
			if (!dct_enabled)
				continue;

			printk(BIOS_SPEW, "Waiting for DIMMs to exit self refresh node: %d channel: %d\n", node, channel);

			/* Wait for transition from self refresh mode to complete */
			do {
				dword = read_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x90);
			} while (dword & (1 << 1));

			/* Restore registers */
			write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe006, data->f2x9cx0d0fe006);
			write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0fe007, data->f2x9cx0d0fe007);
		}
	}

	/* Stage 10 */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			for (i = 0; i < 12; i++)
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x10 + i, data->f2x9cx10[i]);
			for (i = 0; i < 12; i++)
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x20 + i, data->f2x9cx20[i]);
			for (i = 0; i < 4; i++)
				for (j = 0; j < 3; j++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, (0x01 + i) + (0x100 * j), data->f2x9cx3_0_0_3_1[i][j]);
			for (i = 0; i < 4; i++)
				for (j = 0; j < 3; j++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, (0x05 + i) + (0x100 * j), data->f2x9cx3_0_0_7_5[i][j]);
			write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d, data->f2x9cx0d);
			for (i = 0; i < 9; i++)
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f0013 | (i << 8), data->f2x9cx0d0f0_f_0_13[i]);
			for (i = 0; i < 9; i++)
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f0030 | (i << 8), data->f2x9cx0d0f0_f_0_30[i]);
			for (i = 0; i < 4; i++)
				write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f2030 | (i << 8), data->f2x9cx0d0f2_f_0_30[i]);
			for (i = 0; i < 2; i++)
				for (j = 0; j < 3; j++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f0000 | (i << 8) | (j * 4), data->f2x9cx0d0f8_8_4_0[i][j]);
			write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x0d0f812f, data->f2x9cx0d0f812f);
		}
	}

	/* Stage 11 */
	if (IS_ENABLED(CONFIG_DIMM_DDR3)) {
		for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
			for (channel = 0; channel < 2; channel++) {
				struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
				if (!persistent_data->node[node].node_present)
					continue;

				for (i = 0; i < 12; i++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x30 + i, data->f2x9cx30[i]);
				for (i = 0; i < 12; i++)
					write_amd_dct_index_register_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x98, 0x40 + i, data->f2x9cx40[i]);
			}
		}
	}

	/* Other */
	for (node = 0; node < MAX_NODES_SUPPORTED; node++) {
		for (channel = 0; channel < 2; channel++) {
			struct amd_s3_persistent_mct_channel_data *data = &persistent_data->node[node].channel[channel];
			if (!persistent_data->node[node].node_present)
				continue;

			/* ECC scrub location */
			write_config32_dct(PCI_DEV(0, 0x18 + node, 3), node, 0, 0x5c, data->f3x5c);
			write_config32_dct(PCI_DEV(0, 0x18 + node, 3), node, 0, 0x60, data->f3x60);

			/* ECC scrub rate control */
			write_config32_dct(PCI_DEV(0, 0x18 + node, 3), node, 0, 0x58, data->f3x58);

			if (is_fam15h())
				/* Set LockDramCfg and CC6SaveEn */
				write_config32_dct(PCI_DEV(0, 0x18 + node, 2), node, channel, 0x118, data->f2x118);
		}
	}
}
#endif

#ifdef __RAMSTAGE__
int8_t save_mct_information_to_nvram(void)
{
	uint8_t nvram;
	uint8_t restored = 0;

	if (acpi_is_wakeup_s3())
		return 0;

	printk(BIOS_DEBUG, "Writing AMD DCT configuration to Flash\n");

	struct spi_flash flash;
	ssize_t s3nv_offset;
	struct amd_s3_persistent_data *persistent_data;

	/* Allocate temporary data structures */
	persistent_data = malloc(sizeof(struct amd_s3_persistent_data));
	if (!persistent_data) {
		printk(BIOS_DEBUG, "Could not allocate S3 data structure in RAM\n");
		return -1;
	}

	/* Obtain MCT configuration data */
	copy_mct_data_to_save_variable(persistent_data);

	/* Save RAM SPD data at the same time */
	copy_cbmem_spd_data_to_save_variable(persistent_data, &restored);

	if (restored) {
		/* Allow training bypass if DIMM configuration is unchanged on next boot */
		nvram = 1;
		set_option("allow_spd_nvram_cache_restore", &nvram);

		printk(BIOS_DEBUG, "Hardware configuration unchanged since last boot; skipping write\n");
		free(persistent_data);
		return 0;
	}

	/* Obtain CBFS file offset */
	s3nv_offset = get_s3nv_file_offset();
	if (s3nv_offset == -1) {
		free(persistent_data);
		return -1;
	}

	/* Align flash pointer to nearest boundary */
	s3nv_offset &= ~(CONFIG_S3_DATA_SIZE-1);
	s3nv_offset += CONFIG_S3_DATA_SIZE;

	/* Initialize SPI and detect devices */
	spi_init();
	if (spi_flash_probe(0, 0, &flash)) {
		printk(BIOS_DEBUG, "Could not find SPI device\n");
		return -1;
	}

	spi_flash_volatile_group_begin(&flash);

	/* Erase and write data structure */
	spi_flash_erase(&flash, s3nv_offset, CONFIG_S3_DATA_SIZE);
	spi_flash_write(&flash, s3nv_offset,
			sizeof(struct amd_s3_persistent_data), persistent_data);

	/* Deallocate temporary data structures */
	free(persistent_data);

	spi_flash_volatile_group_end(&flash);

	/* Allow training bypass if DIMM configuration is unchanged on next boot */
	nvram = 1;
	set_option("allow_spd_nvram_cache_restore", &nvram);

	return 0;
}
#endif

int8_t restore_mct_information_from_nvram(uint8_t training_only)
{
	struct amd_s3_persistent_data *persistent_data;

	persistent_data = map_s3nv_in_nvram();
	if (!persistent_data)
		return -1;

	restore_mct_data_from_save_variable(persistent_data, training_only);

	return 0;
}

void calculate_and_store_spd_hashes(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	uint8_t dimm;

	for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm++) {
		calculate_spd_hash(pDCTstat->spd_data.spd_bytes[dimm], &pDCTstat->spd_data.spd_hash[dimm]);
	}
}

void compare_nvram_spd_hashes(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	uint8_t dimm;

	pDCTstat->spd_data.nvram_spd_match = 1;
	for (dimm = 0; dimm < MAX_DIMMS_SUPPORTED; dimm++) {
		if (pDCTstat->spd_data.spd_hash[dimm] != pDCTstat->spd_data.nvram_spd_hash[dimm])
			pDCTstat->spd_data.nvram_spd_match = 0;
	}
}
