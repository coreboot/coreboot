/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bl_uapp/bl_syscall_public.h>
#include <psp_verstage.h>
#include <stddef.h>
#include <svc_call.h>

void svc_exit(uint32_t status)
{
	uint32_t unused = 0;
	SVC_CALL0(SVC_EXIT, unused);
}

void svc_debug_print(const char *string)
{
	uint32_t unused = 0;
	SVC_CALL1(SVC_DEBUG_PRINT, (uint32_t)string, unused);
}

void svc_debug_print_ex(uint32_t dword0,
		uint32_t dword1, uint32_t dword2, uint32_t dword3)
{
	uint32_t unused = 0;
	SVC_CALL4(SVC_DEBUG_PRINT_EX, dword0, dword1, dword2, dword3, unused);
}

uint32_t svc_get_boot_mode(uint32_t *boot_mode)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_GET_BOOT_MODE, boot_mode, retval);
	return retval;
}

void svc_delay_in_usec(uint32_t delay)
{
	uint32_t unused = 0;
	SVC_CALL1(SVC_DELAY_IN_MICRO_SECONDS, delay, unused);
}

uint32_t svc_get_spi_rom_info(struct spirom_info *spi_rom_info)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_GET_SPI_INFO, (uint32_t)spi_rom_info, retval);
	return retval;
}

uint32_t svc_map_fch_dev(enum fch_io_device io_device,
		uint32_t arg1, uint32_t arg2, void **io_device_axi_addr)
{
	uint32_t retval = 0;
	assert(io_device < FCH_IO_DEVICE_END);
	SVC_CALL4(SVC_MAP_FCH_IO_DEVICE, io_device, arg1, arg2,
			(uint32_t)io_device_axi_addr, retval);
	return retval;
}

uint32_t svc_unmap_fch_dev(enum fch_io_device io_device, void *io_device_axi_addr)
{
	uint32_t retval = 0;
	assert(io_device < FCH_IO_DEVICE_END);
	SVC_CALL2(SVC_UNMAP_FCH_IO_DEVICE, (uint32_t)io_device,
			(uint32_t)io_device_axi_addr, retval);
	return retval;
}

uint32_t svc_map_spi_rom(void *spi_rom_addr,
		uint32_t size, void **spi_rom_axi_addr)
{
	uint32_t retval = 0;
	SVC_CALL3(SVC_MAP_SPIROM_DEVICE, spi_rom_addr, size,
			(uint32_t)spi_rom_axi_addr, retval);
	return retval;
}

uint32_t svc_unmap_spi_rom(void *spi_rom_addr)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_UNMAP_SPIROM_DEVICE, (uint32_t)spi_rom_addr, retval);
	return retval;
}

uint32_t svc_update_psp_bios_dir(uint32_t *psp_dir_offset,
		uint32_t *bios_dir_offset)
{
	uint32_t retval = 0;
	SVC_CALL2(SVC_UPDATE_PSP_BIOS_DIR, (uint32_t)psp_dir_offset,
			(uint32_t)bios_dir_offset, retval);
	return retval;
}

uint32_t svc_save_uapp_data(void *address, uint32_t size)
{
	uint32_t retval = 0;
	SVC_CALL2(SVC_COPY_DATA_FROM_UAPP, (uint32_t)address, size, retval);
	return retval;
}

uint32_t svc_read_timer_val(enum psp_timer_type type, uint64_t *counter_value)
{
	unsigned int retval = 0;
	assert(type < PSP_TIMER_TYPE_MAX);
	SVC_CALL2(SVC_READ_TIMER_VAL, type, counter_value, retval);
	return retval;
}

uint32_t svc_reset_system(enum reset_type reset_type)
{
	unsigned int retval = 0;
	assert(reset_type < RESET_TYPE_MAX);
	SVC_CALL1(SVC_RESET_SYSTEM, reset_type, retval);
	return retval;
}

uint32_t svc_crypto_sha(struct sha_generic_data *sha_op, enum sha_operation_mode sha_mode)
{
	uint32_t retval = 0;
	SVC_CALL2(SVC_SHA, sha_op, sha_mode, retval);
	return retval;
}

uint32_t svc_modexp(struct mod_exp_params *mod_exp_param)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_MODEXP, mod_exp_param, retval);
	return retval;
}

uint32_t svc_ccp_dma(uint32_t spi_rom_offset, void *dest, uint32_t size)
{
	uint32_t retval = 0;
	SVC_CALL3(SVC_CCP_DMA, spi_rom_offset, dest, size, retval);
	return retval;
}

uint32_t svc_set_platform_boot_mode(enum chrome_platform_boot_mode boot_mode)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_SET_PLATFORM_BOOT_MODE, (uint32_t)boot_mode, retval);
	return retval;
}

uint32_t svc_write_postcode(uint32_t postcode)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_WRITE_POSTCODE, postcode, retval);
	return retval;
}
