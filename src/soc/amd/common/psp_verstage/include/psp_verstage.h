/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PSP_VERSTAGE_H
#define PSP_VERSTAGE_H

#include <2crypto.h>
#include <amdblocks/psp_efs.h>
#include <bl_uapp/bl_syscall_public.h>
#include <stdint.h>
#include <soc/psp_transfer.h>
#include <psp_post_code.h>

#define EMBEDDED_FW_SIGNATURE			0x55aa55aa
#define PSP_COOKIE				0x50535024	/* 'PSP$' */
#define BHD_COOKIE				0x44484224	/* 'DHB$ */

#define PSP_VBOOT_ERROR_SUBCODE			0x0D5D0000

#define SPI_ADDR_MASK				0x00ffffff
#define MIN_TRANSFER_BUFFER_SIZE		(8 * KiB)
#define MIN_WORKBUF_TRANSFER_SIZE		(MIN_TRANSFER_BUFFER_SIZE - TRANSFER_INFO_SIZE)

void test_svc_calls(void);
uint32_t unmap_fch_devices(void);
uint32_t verstage_soc_early_init(void);
void verstage_mainboard_espi_init(void);
void verstage_mainboard_tpm_init(void);
void verstage_soc_aoac_init(void);
void verstage_soc_espi_init(void);
void verstage_soc_i2c_init(void);
void verstage_soc_spi_init(void);
void *map_spi_rom(void);

uint32_t update_psp_bios_dir(uint32_t *psp_dir_offset, uint32_t *bios_dir_offset);
uint32_t save_uapp_data(void *address, uint32_t size);
uint32_t get_bios_dir_addr(struct embedded_firmware *ef_table);
int platform_set_sha_op(enum vb2_hash_algorithm hash_alg,
			struct sha_generic_data *sha_op);
void platform_report_mode(int developer_mode_enabled);

void update_psp_fw_hash_tables(void);

void report_prev_boot_status_to_vboot(void);

void report_hsp_secure_state(void);

uint32_t boot_dev_get_active_map_count(void);

#endif /* PSP_VERSTAGE_H */
