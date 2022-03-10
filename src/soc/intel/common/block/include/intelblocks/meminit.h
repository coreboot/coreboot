/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_INTEL_COMMON_BLOCK_MEMINIT_H__
#define __SOC_INTEL_COMMON_BLOCK_MEMINIT_H__

#include <fsp/api.h>
#include <types.h>

/*
 * Calculates the number of channels depending upon the data bus width of the
 * platform and the channel width.
 */
#define CHANNEL_COUNT(ch_width)		(CONFIG_DATA_BUS_WIDTH / (ch_width))

/*
 * UPDs for FSP-M are organized depending upon the MRC's view of channel. Thus,
 * the number of channels as seen by the MRC are dependent on the channel width
 * assumption in the UPDs. These channels might not necessarily be the same as
 * the physical channels supported by the platform.
 */
#define MRC_CHANNELS			CHANNEL_COUNT(CONFIG_MRC_CHANNEL_WIDTH)

/* Different memory topologies supported by the platform. */
enum mem_topology {
	MEM_TOPO_MEMORY_DOWN = BIT(0),
	MEM_TOPO_DIMM_MODULE = BIT(1),
	MEM_TOPO_MIXED = MEM_TOPO_MEMORY_DOWN | MEM_TOPO_DIMM_MODULE,
};

/*
 * SPD provides information about the memory module. Depending upon the memory
 * topology, the SPD data can be obtained from different sources. Example: for
 * memory down topology, SPD is read from CBFS using cbfs_index. For DIMM
 * modules, SPD is read from EEPROM using the DIMM addresses provided by the
 * mainboard.
 */
struct mem_spd {
	enum mem_topology topo;
	/*
	 * SPD data is read from CBFS spd.bin file using cbfs_index to locate
	 * the entry. This is used in case of MEM_TOPO_MEMORY_DOWN and
	 * MEM_TOPO_MIXED topologies.
	 */
	size_t cbfs_index;

	/*
	 * SPD data is read from on-module EEPROM using the DIMM addresses
	 * provided by the mainboard. This is used in case of
	 * MEM_TOPO_DIMM_MODULE and MEM_TOPO_MIXED topologies.
	 *
	 * Up to a maximum of MRC_CHANNELS * CONFIG_DIMMS_PER_CHANNEL addresses
	 * can be provided by mainboard. However, depending upon the memory
	 * technology being used and the number of physical channels supported
	 * by that technology, the actual channels might be less than
	 * MRC_CHANNELS.
	 */
	struct {
		uint8_t addr_dimm[CONFIG_DIMMS_PER_CHANNEL];
	} smbus[MRC_CHANNELS];
};

/* Information about memory technology supported by SoC */
struct soc_mem_cfg {
	/*
	 * Number of physical channels that are supported by the memory
	 * technology.
	 */
	size_t num_phys_channels;

	/*
	 * Map of physical channel numbers to MRC channel numbers. This is
	 * helpful in identifying what SPD entries need to be filled for a
	 * physical channel.
	 *
	 * Example: MRC supports 8 channels 0 - 7, but a memory technology
	 * supports only 2 physical channels 0 - 1. In this case, the map could
	 * be:
	 * [0] = 0,
	 * [1] = 4,
	 * indicating that physical channel 0 is mapped to MRC channel 0 and
	 * physical channel 1 is mapped to MRC channel 4.
	 */
	size_t phys_to_mrc_map[MRC_CHANNELS];

	/*
	 * Masks to be applied in case of memory down topology. For memory down
	 * topology, there is no separate EEPROM. Thus, the masks need to be
	 * hard-coded by the SoC to indicate what combinations are supported.
	 * This is a mask of physical channels for the memory technology.
	 *
	 * Example: For the memory technology supporting 2 physical channels,
	 * where the population rules restrict use of channel 0 for
	 * half-channel, half_channel mask would be set to 0x1 indicating
	 * channel 0 is always populated.
	 */
	struct {
		/*
		 * Mask of physical channels that are populated in case of
		 * half-channel configuration.
		 */
		uint32_t half_channel;
		/*
		 * Mask of physical channels that are populated with memory
		 * down parts in case of mixed topology.
		 */
		uint32_t mixed_topo;
	} md_phy_masks;
};

/* Flags indicating how the channels are populated. */
enum channel_population {
	NO_CHANNEL_POPULATED = 0,
	TOP_HALF_POPULATED = BIT(0),
	BOTTOM_HALF_POPULATED = BIT(1),
	FULLY_POPULATED = TOP_HALF_POPULATED | BOTTOM_HALF_POPULATED,
};

/*
 * Data for the memory channels that can be used by SoC code to populate FSP
 * UPDs.
 */
struct mem_channel_data {
	/* Pointer to SPD data for each DIMM of each channel */
	uintptr_t spd[MRC_CHANNELS][CONFIG_DIMMS_PER_CHANNEL];
	/* Length of SPD data */
	size_t spd_len;
	/* Flags indicating how channels are populated */
	enum channel_population ch_population_flags;
};

/*
 * This change populates data regarding memory channels in `struct
 * mem_channel_data` using the following inputs from SoC code:
 * memupd        : FSP-M UPD configuration.
 * soc_mem_cfg   : SoC-specific information about the memory technology used by
 *                 the mainboard.
 * spd_info      : Information about the memory topology.
 * half_populated: Hint from mainboard if channels are half populated.
 */
void mem_populate_channel_data(FSPM_UPD *memupd, const struct soc_mem_cfg *soc_mem_cfg,
				const struct mem_spd *spd_info,
				bool half_populated,
				struct mem_channel_data *data);

/*
 * Given a channel number and the maximum number of supported channels, this
 * function returns if a channel is populated. This is useful for populating
 * DQ/DQS UPDs by the SoC code.
 */
static inline bool channel_is_populated(size_t curr_ch, size_t max_ch,
					enum channel_population flags)
{
	if ((curr_ch * 2) < max_ch)
		return !!(flags & BOTTOM_HALF_POPULATED);

	return !!(flags & TOP_HALF_POPULATED);
}

#endif /* __SOC_INTEL_COMMON_BLOCK_MEMINIT_H__ */
