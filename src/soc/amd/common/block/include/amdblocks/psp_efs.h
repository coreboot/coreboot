/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef AMD_COMMON_PSP_EFS_H
#define  AMD_COMMON_PSP_EFS_H

#include <types.h>

#define EFS_OFFSET (0xffffff - (0x80000 << CONFIG_AMD_FWM_POSITION_INDEX) + 0x20000 + 1)

#define EMBEDDED_FW_SIGNATURE			0x55aa55aa

#if CONFIG(SOC_AMD_STONEYRIDGE)
  #define SPI_MODE_FIELD spi_readmode_f15_mod_60_6f
  #define SPI_SPEED_FIELD fast_speed_new_f15_mod_60_6f
#elif CONFIG(SOC_AMD_PICASSO)
  #define SPI_MODE_FIELD spi_readmode_f17_mod_00_2f
  #define SPI_SPEED_FIELD  spi_fastspeed_f17_mod_00_2f
#elif CONFIG(SOC_AMD_CEZANNE) | CONFIG(SOC_AMD_MENDOCINO)
  #define SPI_MODE_FIELD spi_readmode_f17_mod_30_3f
  #define SPI_SPEED_FIELD spi_fastspeed_f17_mod_30_3f
#else
  #error <Error: Unknown Processor>
#endif


struct second_gen_efs { /* todo: expand for Server products */
	uint32_t gen:1; /* Client products only use bit 0 */
	uint32_t reserved:31;
} __attribute__((packed));

#define EFS_SECOND_GEN 0

/* Copied from coreboot/util/amdfwtool.h */
struct embedded_firmware {
	uint32_t signature; /* 0x55aa55aa */
	uint32_t imc_entry;
	uint32_t gec_entry;
	uint32_t xhci_entry;
	uint32_t psp_directory;
	uint32_t combo_psp_directory;
	uint32_t bios0_entry;
	uint32_t bios1_entry;
	uint32_t bios2_entry;
	struct second_gen_efs efs_gen;
	uint32_t bios3_entry;
	uint32_t reserved_2Ch;
	uint32_t promontory_fw_ptr;
	uint32_t lp_promontory_fw_ptr;
	uint32_t reserved_38h;
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
	uint32_t reserved_4Ch;
} __attribute__((packed, aligned(16)));

bool efs_is_valid(void);
bool read_efs_spi_settings(uint8_t *mode, uint8_t *speed);

#endif  /* AMD_COMMON_PSP_EFS_H */
