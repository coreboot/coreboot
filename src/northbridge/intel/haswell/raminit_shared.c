/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <device/dram/ddr3.h>
#include <device/pci_ops.h>
#include <string.h>
#include <types.h>

#include "chip.h"
#include "haswell.h"
#include "raminit.h"

void get_spd_info(struct spd_info *spdi, const struct northbridge_intel_haswell_config *cfg)
{
	if (CONFIG(HAVE_SPD_IN_CBFS)) {
		/* With memory down: from mainboard code */
		mb_get_spd_map(spdi);
	} else {
		/* Without memory down: from devicetree */
		memcpy(spdi->addresses, cfg->spd_addresses, ARRAY_SIZE(spdi->addresses));
	}
}

static const char *const ecc_decoder[] = {
	"inactive",
	"active on IO",
	"disabled on IO",
	"active",
};

/* Print out the memory controller configuration, as per the values in its registers. */
void report_memory_config(void)
{
	const uint32_t addr_decoder_common = mchbar_read32(MAD_CHNL);

	printk(BIOS_DEBUG, "memcfg DDR3 clock %d MHz\n",
	       DIV_ROUND_CLOSEST(mchbar_read32(MC_BIOS_DATA) * 13333 * 2, 100));

	printk(BIOS_DEBUG, "memcfg channel assignment: A: %d, B % d, C % d\n",
	       (addr_decoder_common >> 0) & 3,
	       (addr_decoder_common >> 2) & 3,
	       (addr_decoder_common >> 4) & 3);

	for (unsigned int i = 0; i < NUM_CHANNELS; i++) {
		const uint32_t ch_conf = mchbar_read32(MAD_DIMM(i));

		printk(BIOS_DEBUG, "memcfg channel[%d] config (%8.8x):\n", i, ch_conf);
		printk(BIOS_DEBUG, "   ECC %s\n", ecc_decoder[(ch_conf >> 24) & 3]);
		printk(BIOS_DEBUG, "   enhanced interleave mode %s\n",
		       ((ch_conf >> 22) & 1) ? "on" : "off");

		printk(BIOS_DEBUG, "   rank interleave %s\n",
		       ((ch_conf >> 21) & 1) ? "on" : "off");

		printk(BIOS_DEBUG, "   DIMMA %d MB width %s %s rank%s\n",
		       ((ch_conf >> 0) & 0xff) * 256,
		       ((ch_conf >> 19) & 1) ? "x16" : "x8 or x32",
		       ((ch_conf >> 17) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? "" : ", selected");

		printk(BIOS_DEBUG, "   DIMMB %d MB width %s %s rank%s\n",
		       ((ch_conf >> 8) & 0xff) * 256,
		       ((ch_conf >> 20) & 1) ? "x16" : "x8 or x32",
		       ((ch_conf >> 18) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? ", selected" : "");
	}
}

static uint8_t nb_get_ecc_type(const uint32_t capid0_a)
{
	return capid0_a & CAPID_ECCDIS ? MEMORY_ARRAY_ECC_NONE : MEMORY_ARRAY_ECC_SINGLE_BIT;
}

static uint16_t nb_slots_per_channel(const uint32_t capid0_a)
{
	return !(capid0_a & CAPID_DDPCD) + 1;
}

static uint16_t nb_number_of_channels(const uint32_t capid0_a)
{
	return !(capid0_a & CAPID_PDCD) + 1;
}

static uint32_t nb_max_chan_capacity_mib(const uint32_t capid0_a)
{
	uint32_t ddrsz;

	/* Values from documentation, which assume two DIMMs per channel */
	switch (CAPID_DDRSZ(capid0_a)) {
	case 1:
		ddrsz = 8192;
		break;
	case 2:
		ddrsz = 2048;
		break;
	case 3:
		ddrsz = 512;
		break;
	default:
		ddrsz = 16384;
		break;
	}

	/* Account for the maximum number of DIMMs per channel */
	return (ddrsz / 2) * nb_slots_per_channel(capid0_a);
}

void setup_sdram_meminfo(const uint8_t *spd_data[NUM_CHANNELS][NUM_SLOTS])
{
	struct memory_info *mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (!mem_info)
		die("Failed to add memory info to CBMEM.\n");

	memset(mem_info, 0, sizeof(struct memory_info));

	/* TODO: This looks like an open-coded DIV_ROUND_CLOSEST() */
	const uint32_t ddr_freq_mhz = (mchbar_read32(MC_BIOS_DATA) * 13333 * 2 + 50) / 100;

	unsigned int dimm_cnt = 0;
	for (unsigned int channel = 0; channel < NUM_CHANNELS; channel++) {
		const uint32_t ch_conf = mchbar_read32(MAD_DIMM(channel));
		for (unsigned int slot = 0; slot < NUM_SLOTS; slot++) {
			const uint32_t dimm_size = ((ch_conf >> (slot * 8)) & 0xff) * 256;
			if (dimm_size) {
				struct dimm_info *dimm = &mem_info->dimm[dimm_cnt];
				dimm->dimm_size = dimm_size;
				/* TODO: Hardcoded, could also be LPDDR3 */
				dimm->ddr_type = MEMORY_TYPE_DDR3;
				dimm->ddr_frequency = ddr_freq_mhz * 2; /* In MT/s */
				dimm->rank_per_dimm = 1 + ((ch_conf >> (17 + slot)) & 1);
				dimm->channel_num = channel;
				dimm->dimm_num = slot;
				dimm->bank_locator = channel * 2;
				memcpy(dimm->serial,
					&spd_data[channel][slot][SPD_DDR3_SERIAL_NUM],
					SPD_DDR3_SERIAL_LEN);
				memcpy(dimm->module_part_number,
					&spd_data[channel][slot][SPD_DDR3_PART_NUM],
					SPD_DDR3_PART_LEN);
				dimm->mod_id =
					(spd_data[channel][slot][SPD_DDR3_MOD_ID2] << 8) |
					(spd_data[channel][slot][SPD_DDR3_MOD_ID1] & 0xff);
				/* TODO: Should be taken from SPD instead */
				dimm->mod_type = SPD_DDR3_DIMM_TYPE_SO_DIMM;
				dimm->bus_width = MEMORY_BUS_WIDTH_64;
				dimm_cnt++;
			}
		}
	}
	mem_info->dimm_cnt = dimm_cnt;

	const uint32_t capid0_a = pci_read_config32(HOST_BRIDGE, CAPID0_A);
	const uint16_t num_channels = nb_number_of_channels(capid0_a);

	mem_info->ecc_type = nb_get_ecc_type(capid0_a);
	mem_info->max_capacity_mib = num_channels * nb_max_chan_capacity_mib(capid0_a);
	mem_info->number_of_devices = num_channels * nb_slots_per_channel(capid0_a);
}
