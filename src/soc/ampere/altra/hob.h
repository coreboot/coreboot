/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_AMPERE_ALTRA_HOB_H
#define SOC_AMPERE_ALTRA_HOB_H

#include <stdbool.h>
#include <stdint.h>

#define AC01_MAX_SOCKETS		2
#define AC01_DRAM_MAX_REGIONS		16
#define AC01_DIMM_MAX_SLOTS		32

/*
 * Platform information TF-A passes to BL33 as a GUID HOB, mirroring
 * PLATFORM_INFO_HOB of edk2-platforms (AmpereAltraPkg
 * Include/Guid/PlatformInfoHob.h), which has the same layout as long as the
 * types and their natural alignment match. The HOB is required to extend to
 * ahbc_id, the fields after it are only used if the HOB contains them. The
 * fields after the ECID are not needed and left out.
 */
struct ac01_dram_info {
	uint32_t num_region;
	uint64_t total_size;
	uint64_t base[AC01_DRAM_MAX_REGIONS];
	uint64_t size[AC01_DRAM_MAX_REGIONS];
	uint64_t node[AC01_DRAM_MAX_REGIONS];
	uint64_t socket[AC01_DRAM_MAX_REGIONS];
	uint32_t max_speed;
	uint32_t mcu_mask[AC01_MAX_SOCKETS];
	uint32_t nvd_region[AC01_DRAM_MAX_REGIONS];
	uint32_t nvdimm_mode[AC01_MAX_SOCKETS];
};

struct ac01_dimm_info {
	char part_number[32];
	uint64_t dimm_size;
	uint16_t dimm_mfc_id;
	uint16_t reserved;
	uint8_t dimm_nr_rank;
	uint8_t dimm_type;
	uint8_t dimm_status;
	uint8_t dimm_dev_type;
};

struct ac01_dimm {
	struct ac01_dimm_info info;
	uint8_t spd_data[512];
	uint32_t node_id;
};

struct ac01_platform_info {
	uint8_t major_number;
	uint8_t minor_number;
	uint64_t pcp_clk;
	uint64_t cpu_clk;
	uint64_t soc_clk;
	uint64_t ahb_clk;
	uint64_t sys_clk;
	uint8_t cpu_info[128];
	uint8_t cpu_ver[32];
	uint8_t smpmpro_ver[32];
	uint8_t smpmpro_build[32];
	struct ac01_dram_info dram_info;
	struct {
		uint32_t board_dimm_slots;
		struct ac01_dimm dimm[AC01_DIMM_MAX_SLOTS];
	} dimm_list;
	struct {
		uint32_t enable_mask[4];
	} cluster_en[AC01_MAX_SOCKETS];
	uint32_t failsafe_status;
	uint32_t rc_disable_mask[AC01_MAX_SOCKETS];
	uint8_t reset_status;
	uint16_t core_voltage[AC01_MAX_SOCKETS];
	uint16_t soc_voltage[AC01_MAX_SOCKETS];
	uint16_t dimm1_voltage[AC01_MAX_SOCKETS];
	uint16_t dimm2_voltage[AC01_MAX_SOCKETS];
	uint32_t scu_product_id[AC01_MAX_SOCKETS];
	uint8_t max_num_of_core[AC01_MAX_SOCKETS];
	uint8_t warranty[AC01_MAX_SOCKETS];
	uint8_t subnuma_mode[AC01_MAX_SOCKETS];
	uint8_t avs_enable[AC01_MAX_SOCKETS];
	uint32_t avs_voltage_mv[AC01_MAX_SOCKETS];
	uint8_t turbo_capability[AC01_MAX_SOCKETS];
	uint32_t turbo_frequency[AC01_MAX_SOCKETS];
	uint8_t sku_max_turbo[AC01_MAX_SOCKETS];
	uint8_t sku_max_core[AC01_MAX_SOCKETS];
	uint32_t ahbc_id[AC01_MAX_SOCKETS];
	/* PLATFORM_TPM2_INFO, a packed structure */
	uint8_t tpm2_info[152];
	uint8_t link_2p_speed[AC01_MAX_SOCKETS];
	uint8_t link_2p_width[AC01_MAX_SOCKETS];
	uint32_t ecid[AC01_MAX_SOCKETS][4];
};

/* Returns NULL if TF-A did not pass a (plausible) platform information HOB. */
const struct ac01_platform_info *ac01_platform_info(void);

/* ECID (unique chip ID) of socket 0, NULL if the HOB is too old to contain it */
const uint32_t *ac01_ecid(void);

/* Ampere Altra (AC01) rather than Ampere Altra Max (AC02) */
bool ac01_is_altra(void);

unsigned int ac01_active_sockets(void);

#endif
