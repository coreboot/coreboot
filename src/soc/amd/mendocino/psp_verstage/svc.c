/* SPDX-License-Identifier: GPL-2.0-only */

#include "svc.h"

#include <assert.h>
#include <bl_uapp/bl_syscall_public.h>
#include <psp_verstage.h>
#include <stddef.h>
#include <string.h>

void svc_exit(uint32_t status)
{
	uint32_t unused = 0;
	SVC_CALL0(SVC_EXIT, unused);
}

void svc_debug_print(const char *string)
{
	uint32_t unused = 0;
	struct cmd_param_debug param = {
		.debug_buffer = (char *)string,
		.debug_buffer_len = strlen(string),
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_DEBUG_PRINT, (void *)&param, unused);
}

void svc_debug_print_ex(uint32_t dword0,
		uint32_t dword1, uint32_t dword2, uint32_t dword3)
{
	uint32_t unused = 0;
	struct cmd_param_debug_ex param = {
		.word0 = dword0,
		.word1 = dword1,
		.word2 = dword2,
		.word3 = dword3,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_DEBUG_PRINT_EX, (void *)&param, unused);
}

uint32_t svc_get_boot_mode(uint32_t *boot_mode)
{
	uint32_t retval = 0;
	struct cmd_param_get_boot_mode param = {
		.ptr_boot_mode = boot_mode,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_GET_BOOT_MODE, (void *)&param, retval);
	return retval;
}

void svc_delay_in_usec(uint32_t delay)
{
	uint32_t unused = 0;
	struct cmd_param_delay_in_micro_second param = {
		.delay = delay,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_DELAY_IN_MICRO_SECONDS, (void *)&param, unused);
}

uint32_t svc_get_spi_rom_info(struct spirom_info *spi_rom_info)
{
	uint32_t retval = 0;
	struct cmd_param_spirom_info param = {
		.ptr_spirom_info = spi_rom_info,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_GET_SPI_INFO, (void *)&param, retval);
	return retval;
}

uint32_t svc_map_fch_dev(enum fch_io_device io_device,
		uint32_t arg1, uint32_t arg2, void **io_device_axi_addr)
{
	uint32_t retval = 0;
	struct cmd_param_map_fch_io_device param = {
		.io_device = io_device,
		.arg1 = arg1,
		.arg2 = arg2,
		.pptr_io_device_addr_axi = io_device_axi_addr,
	};
	assert(io_device < FCH_IO_DEVICE_END);
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_MAP_FCH_IO_DEVICE, (void *)&param, retval);
	return retval;
}

uint32_t svc_unmap_fch_dev(enum fch_io_device io_device, void *io_device_axi_addr)
{
	uint32_t retval = 0;
	struct cmd_param_unmap_fch_io_device param = {
		.io_device = io_device,
		.ptr_io_device_addr_axi = io_device_axi_addr,
	};
	assert(io_device < FCH_IO_DEVICE_END);
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_UNMAP_FCH_IO_DEVICE, (void *)&param, retval);
	return retval;
}

uint32_t svc_map_spi_rom(void *spi_rom_addr,
		uint32_t size, void **spi_rom_axi_addr)
{
	uint32_t retval = 0;
	struct cmd_param_map_spirom param = {
		.spirom_addr = (uintptr_t)spi_rom_addr,
		.size = size,
		.ppspirom_addr_axi = spi_rom_axi_addr,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_MAP_SPIROM_DEVICE, (void *)&param, retval);
	return retval;
}

uint32_t svc_unmap_spi_rom(void *spi_rom_addr)
{
	uint32_t retval = 0;
	struct cmd_param_unmap_spirom param = {
		.ptr_spirom_addr_axi = spi_rom_addr,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_UNMAP_SPIROM_DEVICE, (void *)&param, retval);
	return retval;
}

uint32_t svc_update_psp_bios_dir(uint32_t *psp_dir_offset,
		uint32_t *bios_dir_offset)
{
	uint32_t retval = 0;
	struct cmd_param_psp_update param = {
		.ptr_psp_dir_addr = psp_dir_offset,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_UPDATE_PSP_BIOS_DIR, (void *)&param, retval);
	return retval;
}

uint32_t svc_save_uapp_data(void *address, uint32_t size)
{
	uint32_t retval = 0;
	struct cmd_param_copy_data_from_uapp param = {
		.address = (uintptr_t)address,
		.size = size,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_COPY_DATA_FROM_UAPP, (void *)&param, retval);
	return retval;
}

uint32_t svc_read_timer_val(enum psp_timer_type type, uint64_t *counter_value)
{
	unsigned int retval = 0;
	struct cmd_param_read_timer_val param = {
		.timer_type = type,
		.ptr_counter_value = counter_value,
	};
	assert(type < PSP_TIMER_TYPE_MAX);
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_READ_TIMER_VAL, (void *)&param, retval);
	return retval;
}

uint32_t svc_reset_system(enum reset_type reset_type)
{
	unsigned int retval = 0;
	struct cmd_param_reset_system param = {
		.reset_type = reset_type,
	};
	assert(reset_type < RESET_TYPE_MAX);
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_RESET_SYSTEM, (void *)&param, retval);
	return retval;
}

uint32_t svc_crypto_sha(struct sha_generic_data *sha_op, enum sha_operation_mode sha_mode)
{
	uint32_t retval = 0;
	struct cmd_param_sha param = {
		.ptr_sha_op = sha_op,
	};
	assert(sha_mode == SHA_GENERIC);
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_SHA, (void *)&param, retval);
	return retval;
}

uint32_t svc_modexp(struct mod_exp_params *mod_exp_param)
{
	uint32_t retval = 0;
	struct cmd_param_modexp param = {
		.ptr_modexp = mod_exp_param,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_MODEXP, (void *)&param, retval);
	return retval;

}

uint32_t svc_ccp_dma(uint32_t spi_rom_offset, void *dest, uint32_t size)
{
	uint32_t retval = 0;
	struct cmd_param_ccp_dma param = {
		.spi_offset = spi_rom_offset,
		.dst_addr = (uintptr_t)dest,
		.size = size,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_CCP_DMA, (void *)&param, retval);
	return retval;
}

uint32_t svc_set_platform_boot_mode(enum chrome_platform_boot_mode boot_mode)
{
	uint32_t retval = 0;
	struct cmd_param_set_platform_boot_mode param = {
		.boot_mode = boot_mode,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_SET_PLATFORM_BOOT_MODE, (void *)&param, retval);
	return retval;
}

uint32_t svc_set_fw_hash_table(struct psp_fw_hash_table *hash_table)
{
	uint32_t retval = 0;
	struct cmd_param_set_fw_hash_table param = {
		.ptr_psp_fw_hash_table = hash_table,
	};
	SVC_CALL2(SVC_VERSTAGE_CMD, CMD_SET_FW_HASH_TABLE, (void *)&param, retval);
	return retval;
}
