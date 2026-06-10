/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _AMD_PSP_PSP_EFS_H_
#define  _AMD_PSP_PSP_EFS_H_

#include <stdint.h>

#define EMBEDDED_FW_SIGNATURE			0x55aa55aa

struct second_gen_efs {
	uint32_t gen:1; /* Client products only use bit 0 */
	uint32_t reserved:31;
} __packed;

#define EFS_SECOND_GEN 0
#define EFS_BEFORE_SECOND_GEN 1

/*
 * AMD Embedded Firmware Structure (EFS).
 * Document #55758.
 */
struct embedded_firmware {
	uint32_t signature; /* 0x55aa55aa */
	uint32_t imc_entry;
	uint32_t gec_entry;
	uint32_t xhci_entry;
	uint32_t psp_directory; /* also used as combo_psp_directory */
	uint32_t new_psp_directory;
	uint32_t bios0_entry;
	uint32_t bios1_entry;
	uint32_t bios2_entry;
	union {
		struct second_gen_efs efs_gen;	/* Client SoC */
		uint32_t multi_gen_efs;		/* Server SoC */
	};
	uint32_t bios3_entry;
	uint32_t psp_bak_directory;
	uint32_t promontory_fw_ptr;
	uint32_t lp_promontory_fw_ptr;
	uint32_t promontory19_fw_ptr;
	uint32_t reserved_3Ch;
	uint8_t spi_readmode_f15_mod_60_6f;
	uint8_t fast_speed_new_f15_mod_60_6f;
	uint8_t reserved_42h;
	uint8_t spi_readmode_f17_mod_00_2f;
	uint8_t spi_fastspeed_f17_mod_00_2f;
	uint8_t qpr_dummy_cycle_f17_mod_00_2f;
	uint8_t reserved_46h;
	uint8_t spi_readmode_f17_mod_30_3f;
	uint8_t spi_fastspeed_f17_mod_30_3f;
	uint8_t micron_detect_f17_mod_30_3f;
	uint8_t reserved_4Ah;
	uint8_t reserved_4Bh;
	uint16_t vendor_id;
	uint16_t board_id;
	uint8_t espi0_config0;
	uint8_t espi1_config0;
	uint8_t espi0_config1;
	uint8_t espi1_config1;
	uint32_t ubu_table;
	uint8_t bios_size;
} __packed __aligned(16);

#endif  /* _AMD_PSP_PSP_EFS_H_ */
