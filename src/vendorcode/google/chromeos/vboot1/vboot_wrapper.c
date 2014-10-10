/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <console/vtxprintf.h>
#if CONFIG_ARCH_X86
#include <cpu/x86/tsc.h>
#else
#include <timer.h>
#endif
#include <rmodule.h>
#include <stdlib.h>
#include <string.h>
#include "../vboot_context.h"
#include "../vboot_handoff.h"

/* Keep a global context pointer around for the callbacks to use. */
static struct vboot_context *gcontext;

static void vboot_wrapper(void *arg)
{
	VbError_t res;
	struct vboot_context *context;

	context = arg;
	gcontext = context;

	VbExDebug("Calling VbInit()\n");
	res = VbInit(context->cparams, &context->handoff->init_params);
	VbExDebug("VbInit() returned 0x%08x\n", res);

	if (res != VBERROR_SUCCESS) {
		if(res == VBERROR_TPM_REBOOT_REQUIRED) {
			VbExDebug("TPM Reboot Required. Proceeding reboot.\n");
			gcontext->reset();
		}
		return;
	}

	VbExDebug("Calling VbSelectFirmware()\n");
	res = VbSelectFirmware(context->cparams, context->fparams);
	VbExDebug("VbSelectFirmware() returned 0x%08x\n", res);

	if (res != VBERROR_SUCCESS)
		return;
}

void VbExError(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	gcontext->log_msg(format, args);
	va_end(args);

	gcontext->fatal_error();
}

void VbExDebug(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	gcontext->log_msg(format, args);
	va_end(args);
}

uint64_t VbExGetTimer(void)
{
#if CONFIG_ARCH_X86
	return rdtscll();
#else
	struct mono_time mt;
	timer_monotonic_get(&mt);
	return mt.microseconds;
#endif
}

VbError_t VbExNvStorageRead(uint8_t *buf)
{
	gcontext->read_vbnv(buf);
	return VBERROR_SUCCESS;
}

VbError_t VbExNvStorageWrite(const uint8_t *buf)
{
	gcontext->save_vbnv(buf);
	return VBERROR_SUCCESS;
}

extern char _heap[];
extern char _eheap[];
static char *heap_current;
static int heap_size;

void *VbExMalloc(size_t size)
{
	void *ptr;

	if (heap_current == NULL) {
		heap_current = &_heap[0];
		heap_size = &_eheap[0] - &_heap[0];
		VbExDebug("vboot heap: %p 0x%08x bytes\n",
		           heap_current, heap_size);
	}

	if (heap_size < size) {
		VbExError("vboot heap request cannot be fulfilled. "
		           "0x%08x available, 0x%08x requested\n",
		           heap_size, size);
	}

	ptr = heap_current;
	heap_size -= size;
	heap_current += size;

	return ptr;
}

void VbExFree(void *ptr)
{
	/* Leak all memory. */
}

/* vboot doesn't expose these through the vboot_api.h, but they are needed.
 * coreboot requires declarations so provide them to avoid compiler errors. */
int Memcmp(const void *src1, const void *src2, size_t n);
void *Memcpy(void *dest, const void *src, uint64_t n);
void *Memset(void *dest, const uint8_t c, uint64_t n);

int Memcmp(const void *src1, const void *src2, size_t n)
{
	return memcmp(src1, src2, n);
}

void *Memcpy(void *dest, const void *src, uint64_t n)
{
	return memcpy(dest, src, n);
}

void *Memset(void *dest, const uint8_t c, uint64_t n)
{
	return memset(dest, c, n);
}

static inline size_t get_hash_block_size(size_t requested_size)
{
	if (!IS_ENABLED(CONFIG_SPI_FLASH_MEMORY_MAPPED)) {
		const size_t block_size = 64 * 1024;
		if (requested_size > block_size)
			return block_size;
	}
	return requested_size;
}

VbError_t VbExHashFirmwareBody(VbCommonParams *cparams, uint32_t firmware_index)
{
	uint8_t *data;
	struct vboot_region *region;
	struct vboot_context *ctx;
	size_t data_size;
	uintptr_t offset_addr;

	ctx = cparams->caller_context;

	switch (firmware_index) {
	case VB_SELECT_FIRMWARE_A:
		region = &ctx->fw_a;
		break;
	case VB_SELECT_FIRMWARE_B:
		region = &ctx->fw_b;
		break;
	default:
		return VBERROR_UNKNOWN;
	}

	data_size = region->size;
	offset_addr = region->offset_addr;
	while (data_size) {
		size_t block_size;

		block_size = get_hash_block_size(data_size);
		data = ctx->get_region(offset_addr, block_size, NULL);
		if (data == NULL)
			return VBERROR_UNKNOWN;
		VbUpdateFirmwareBodyHash(cparams, data, block_size);

		data_size -= block_size;
		offset_addr += block_size;
	}

	return VBERROR_SUCCESS;
}

VbError_t VbExTpmInit(void)
{
	if (gcontext->tis_init())
		return VBERROR_UNKNOWN;
	return VbExTpmOpen();
}

VbError_t VbExTpmClose(void)
{
	if (gcontext->tis_close())
		return VBERROR_UNKNOWN;
	return VBERROR_SUCCESS;
}

VbError_t VbExTpmOpen(void)
{
	if (gcontext->tis_open())
		return VBERROR_UNKNOWN;
	return VBERROR_SUCCESS;
}

VbError_t VbExTpmSendReceive(const uint8_t *request, uint32_t request_length,
                             uint8_t *response, uint32_t *response_length)
{
	size_t len = *response_length;
	if (gcontext->tis_sendrecv(request, request_length, response, &len))
		return VBERROR_UNKNOWN;
	/* check 64->32bit overflow and (re)check response buffer overflow */
	if (len > *response_length)
		return VBERROR_UNKNOWN;
	*response_length = len;
	return VBERROR_SUCCESS;
}

#if !CONFIG_SPI_FLASH_MEMORY_MAPPED
VbError_t VbExRegionRead(VbCommonParams *cparams,
                         enum vb_firmware_region region, uint32_t offset,
                         uint32_t size, void *buf)
{
	struct vboot_context *ctx;
	VbExDebug("VbExRegionRead: offset=%x size=%x, buf=%p\n",
	          offset, size, buf);
	ctx = cparams->caller_context;

	if (region == VB_REGION_GBB) {
		if (offset + size > cparams->gbb_size)
			return VBERROR_REGION_READ_INVALID;
		offset += ctx->gbb.offset_addr;
		if (ctx->get_region(offset, size, buf) == NULL)
			return VBERROR_REGION_READ_INVALID;
		return VBERROR_SUCCESS;
	}

	return VBERROR_UNSUPPORTED_REGION;
}
#endif /* CONFIG_SPI_FLASH_MEMORY_MAPPED */

RMODULE_ENTRY(vboot_wrapper);
