/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PSP_VERSTAGE_H
#define PSP_VERSTAGE_H

#include <2crypto.h>
#include <amdblocks/psp_efs.h>
#include <bl_uapp/bl_syscall_public.h>
#include <stdint.h>
#include <soc/psp_transfer.h>

#define EMBEDDED_FW_SIGNATURE			0x55aa55aa
#define PSP_COOKIE				0x50535024	/* 'PSP$' */
#define BHD_COOKIE				0x44484224	/* 'DHB$ */

#define PSP_VBOOT_ERROR_SUBCODE			0x0D5D0000

#define POSTCODE_ENTERED_PSP_VERSTAGE		0x00
#define POSTCODE_CONSOLE_INIT			0x01
#define POSTCODE_EARLY_INIT			0x02
#define POSTCODE_LATE_INIT			0x03
#define POSTCODE_VERSTAGE_MAIN			0x04
#define POSTCODE_VERSTAGE_S0I3_RESUME		0x05

#define POSTCODE_SAVE_BUFFERS			0x0E
#define POSTCODE_UPDATE_BOOT_REGION		0x0F

#define POSTCODE_DEFAULT_BUFFER_SIZE_NOTICE	0xC0
#define POSTCODE_WORKBUF_RESIZE_WARNING		0xC1
#define POSTCODE_WORKBUF_SAVE_ERROR		0xC2
#define POSTCODE_WORKBUF_BUFFER_SIZE_ERROR	0xC3
#define POSTCODE_ROMSIG_MISMATCH_ERROR		0xC4
#define POSTCODE_PSP_COOKIE_MISMATCH_ERROR	0xC5
#define POSTCODE_BHD_COOKIE_MISMATCH_ERROR	0xC6
#define POSTCODE_UPDATE_PSP_BIOS_DIR_ERROR	0xC7
#define POSTCODE_FMAP_REGION_MISSING		0xC8
#define POSTCODE_AMD_FW_MISSING			0xC9
#define POSTCODE_CMOS_RECOVERY			0xCA
#define POSTCODE_EARLY_INIT_ERROR		0xCB
#define POSTCODE_INIT_TPM_FAILED		0xCC


#define POSTCODE_UNMAP_SPI_ROM			0xF0
#define POSTCODE_UNMAP_FCH_DEVICES		0xF1
#define POSTCODE_LEAVING_VERSTAGE		0xF2

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
uintptr_t *map_spi_rom(void);

uint32_t update_psp_bios_dir(uint32_t *psp_dir_offset, uint32_t *bios_dir_offset);
uint32_t save_uapp_data(void *address, uint32_t size);
uint32_t get_bios_dir_addr(struct embedded_firmware *ef_table);
int platform_set_sha_op(enum vb2_hash_algorithm hash_alg,
			struct sha_generic_data *sha_op);
void platform_report_mode(int developer_mode_enabled);

void update_psp_fw_hash_table(const char *fname);

void report_prev_boot_status_to_vboot(void);

void report_hsp_secure_state(void);

#endif /* PSP_VERSTAGE_H */
