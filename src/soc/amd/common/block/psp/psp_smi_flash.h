/* SPDX-License-Identifier: GPL-2.0-only */

#include <spi_flash.h>
#include <types.h>
#include "psp_def.h"

enum psp_spi_id_type {
	SMI_TARGET_NVRAM		= 0,
	SMI_TARGET_RPMC_NVRAM		= 5,
};

struct psp_spi_info_request {
#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_GEN2)
	uint64_t target_nv_id;
#endif
	uint64_t lba;
	uint64_t block_size;
	uint64_t num_blocks;
} __packed;

struct mbox_psp_cmd_spi_info {
	struct mbox_buffer_header header;
	struct psp_spi_info_request req;
} __packed;

struct psp_spi_read_write_request {
#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_GEN2)
	uint64_t target_nv_id;
#endif
	uint64_t lba;
	uint64_t offset;
	uint64_t num_bytes;
	uint8_t buffer[];
} __packed;

struct mbox_psp_cmd_spi_read_write {
	struct mbox_buffer_header header;
	struct psp_spi_read_write_request req;
} __packed;

struct psp_spi_erase_request {
#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_GEN2)
	uint64_t target_nv_id;
#endif
	uint64_t lba;
	uint64_t num_blocks;
} __packed;

struct mbox_psp_cmd_spi_erase {
	struct mbox_buffer_header header;
	struct psp_spi_erase_request req;
} __packed;

struct psp_spi_rpmc_inc_mc {
	uint32_t counter_address;
	uint32_t counter_data;
	uint8_t signature[SPI_RPMC_SIG_LEN];
} __packed;

struct mbox_psp_cmd_spi_rpmc_inc_mc {
	struct mbox_buffer_header header;
	struct psp_spi_rpmc_inc_mc req;
} __packed;

struct psp_smi_rpmc_req_mc {
	uint32_t counter_address;
	uint8_t tag[SPI_RPMC_TAG_LEN];
	uint8_t signature[SPI_RPMC_SIG_LEN];
	uint32_t output_counter_data;
	uint8_t output_signature[SPI_RPMC_SIG_LEN];
} __packed;

struct mbox_psp_cmd_spi_rpmc_req_mc {
	struct mbox_buffer_header header;
	struct psp_smi_rpmc_req_mc req;
} __packed;

bool is_valid_psp_spi_info(struct mbox_psp_cmd_spi_info *cmd_buf);
bool is_valid_psp_spi_read_write(struct mbox_psp_cmd_spi_read_write *cmd_buf);
bool is_valid_psp_spi_erase(struct mbox_psp_cmd_spi_erase *cmd_buf);
uint64_t get_psp_spi_info_id(struct mbox_psp_cmd_spi_info *cmd_buf);
void set_psp_spi_info(struct mbox_psp_cmd_spi_info *cmd_buf, uint64_t lba, uint64_t block_size,
		      uint64_t num_blocks);
void get_psp_spi_read_write(struct mbox_psp_cmd_spi_read_write *cmd_buf,
			    uint64_t *target_nv_id, uint64_t *lba, uint64_t *offset,
			    uint64_t *num_bytes, uint8_t **data);
void get_psp_spi_erase(struct mbox_psp_cmd_spi_erase *cmd_buf, uint64_t *target_nv_id,
		       uint64_t *lba, uint64_t *num_blocks);

enum mbox_p2c_status psp_smi_spi_get_info(struct mbox_default_buffer *buffer);
enum mbox_p2c_status psp_smi_spi_read(struct mbox_default_buffer *buffer);
enum mbox_p2c_status psp_smi_spi_write(struct mbox_default_buffer *buffer);
enum mbox_p2c_status psp_smi_spi_erase(struct mbox_default_buffer *buffer);
enum mbox_p2c_status psp_smi_spi_rpmc_inc_mc(struct mbox_default_buffer *buffer);
enum mbox_p2c_status psp_smi_spi_rpmc_req_mc(struct mbox_default_buffer *buffer);
