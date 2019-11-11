/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Facebook, Inc.
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

#include <cpu/x86/msr.h>
#include <soc/msr.h>
#include <stddef.h>
#include <device/pci_ops.h>
#include <device/dram/ddr4.h>
#include <soc/pci_devs.h>
#include <soc/memory.h>
#include <spd_bin.h>

#define MAX_SPD_READ_TRIES		3

static uint32_t get_memory_dclk(void)
{
	uint32_t reg32 =
		pci_mmio_read_config32(PCI_DEV(QPI_BUS, PCU1_DEV, PCU1_FUNC), REG_MC_BIOS_REQ);
	return (reg32 & REG_MC_BIOS_REQ_FREQ_MSK) * REG_MC_MULTIPLIER;
}

void save_dimm_info(void)
{
	int index = 0;
	uint32_t dclk_mhz = 0, tries;

	/*
	 * When talking to SPD chips through IMC slave offset of 0x50 is automagically added
	 * by hardware. Real-world slave numbers translate to: 0xa0, 0xa2, 0xa4, 0xa6.
	 */
	struct spd_block blk = {.addr_map = {SPD_SLAVE_ADDR(0, 0), SPD_SLAVE_ADDR(0, 1),
					     SPD_SLAVE_ADDR(1, 0), SPD_SLAVE_ADDR(1, 1)} };

	get_spd_smbus(&blk);
	dump_spd_info(&blk);

	dclk_mhz = get_memory_dclk();

	/*
	 * The platform is limited to 2 channels and max 2 dimms per channel.
	 * It doesn't look like DDR3 is supported so we assume memory is all DDR4.
	 */

	for (int channel = 0; channel < IMC_MAX_CHANNELS; channel++) {
		for (int slot = 0; slot < CONFIG_DIMM_MAX / IMC_MAX_CHANNELS; slot++) {
			dimm_attr dimm = {0};
			u8 *spd_data = blk.spd_array[index];
			int res = SPD_STATUS_OK;
			tries = MAX_SPD_READ_TRIES;
			/*
			 * SMBus controller can't validate data integrity. So on CRC
			 * error retry a few times.
			 */
			do {
				res = spd_decode_ddr4(&dimm, spd_data);
				if (res == SPD_STATUS_CRC_ERROR) {
					printk(BIOS_ERR,
						"SPD CRC error, channel %u slot %u "
						"try %u\n", channel, slot, tries);
					get_spd_smbus(&blk);
				}
			} while (tries-- && res == SPD_STATUS_CRC_ERROR);

			index++;

			if (res == SPD_STATUS_CRC_ERROR) {
				printk(BIOS_WARNING, "Gave up reading CRC on channel %u"
					" slot %u\n", channel, slot);
				continue;
			}

			if (res == SPD_STATUS_OK) {
				spd_add_smbios17_ddr4(channel, slot, dclk_mhz, &dimm);
			}
		}
	}
}

bool memory_config_is_locked(void)
{
	msr_t msr = rdmsr(MSR_LT_MEMORY_LOCKED);
	return (msr.lo & (MSR_MEM_LOCK_BIT1 | MSR_MEM_LOCK_BIT2));
}
