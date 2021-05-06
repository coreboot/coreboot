/* SPDX-License-Identifier: GPL-2.0-only */

#include "svc.h"

#include <assert.h>
#include <bl_uapp/bl_syscall_public.h>
#include <psp_verstage.h>
#include <stddef.h>

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
	uint32_t i;
	for (i = 0; i < delay * 1000; i++)
		asm volatile ("nop");
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
