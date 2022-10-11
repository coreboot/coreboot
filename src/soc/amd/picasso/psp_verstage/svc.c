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

uint32_t svc_map_user_stack(void *start_addr, void *end_addr, void *stack_va)
{
	uint32_t retval = 0;
	SVC_CALL3(SVC_MAP_USER_STACK, (uint32_t)start_addr,
			(uint32_t)end_addr, stack_va, retval);
	return retval;
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

uint32_t svc_wait_10ns_multiple(uint32_t multiple)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_WAIT_10NS_MULTIPLE, multiple, retval);
	return retval;
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
		uint32_t *bios_dir_offset, enum dir_offset_operation operation)
{
	uint32_t retval = 0;
	assert(operation < DIR_OFFSET_OPERATION_MAX);
	SVC_CALL3(SVC_UPDATE_PSP_BIOS_DIR, (uint32_t)psp_dir_offset,
			(uint32_t)bios_dir_offset, operation, retval);
	return retval;
}

uint32_t svc_save_uapp_data(enum uapp_copybuf type, void *address,
		uint32_t size)
{
	uint32_t retval = 0;
	assert(type < UAPP_COPYBUF_MAX);
	SVC_CALL3(SVC_COPY_DATA_FROM_UAPP, type, (uint32_t)address, size, retval);
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

uint32_t svc_write_postcode(uint32_t postcode)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_WRITE_POSTCODE, postcode, retval);
	return retval;
}

uint32_t svc_get_max_workbuf_size(uint32_t *size)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_GET_MAX_WORKBUF_SIZE, size, retval);
	return retval;
}

uint32_t svc_crypto_sha(struct sha_generic_data *sha_op, enum sha_operation_mode sha_mode)
{
	uint32_t retval = 0;
	SVC_CALL2(SVC_SHA, sha_op, sha_mode, retval);
	return retval;
}

uint32_t svc_rsa_pkcs_verify(const struct rsapkcs_verify_params *rsa_params)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_RSAPKCS_VERIFY, rsa_params, retval);
	return retval;
}

uint32_t svc_modexp(struct mod_exp_params *mod_exp_param)
{
	uint32_t retval = 0;
	SVC_CALL1(SVC_MODEXP, mod_exp_param, retval);
	return retval;
}
