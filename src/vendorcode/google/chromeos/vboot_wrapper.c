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
#include <cpu/x86/tsc.h>
#include <rmodule.h>
#include <stdlib.h>
#include <string.h>
#include "vboot_context.h"
#include "vboot_handoff.h"

static void vboot_wrapper(struct vboot_context *context);

DEFINE_RMODULE_HEADER(vboot_wrapper_header, vboot_wrapper, RMODULE_TYPE_VBOOT);

/* Keep a global context pointer around for the callbacks to use. */
static struct vboot_context *gcontext;

/* The FW areas consist of multiple components. At the beginning of
 * each area is the number of total compoments as well as the size and
 * offset for each component. One needs to caculate the total size of the
 * signed firmware region based off of the embedded metadata. */
#define MAX_NUM_COMPONENTS 20

struct component_entry {
	uint32_t offset;
	uint32_t size;
} __attribute__((packed));

struct components {
	uint32_t num_components;
	struct component_entry entries[0];
} __attribute__((packed));


static void parse_component(const struct components *components, int num,
                            struct firmware_component *fw)
{
	const char *base;

	if (num >= components->num_components)
		return;

	/* Offsets are relative to the stat of the book keeping structure. */
	base = (void *)components;

	fw->address = (uint32_t)&base[components->entries[num].offset];
	fw->size = (uint32_t)components->entries[num].size;
}

static void vboot_wrapper(struct vboot_context *context)
{
	int i;
	VbError_t res;
	const struct components *components;

	gcontext = context;

	VbExDebug("Calling VbInit()\n");
	res = VbInit(context->cparams, &context->handoff->init_params);
	VbExDebug("VbInit() returned 0x%08x\n", res);

	if (res != VBERROR_SUCCESS)
		return;

	VbExDebug("Calling VbSelectFirmware()\n");
	res = VbSelectFirmware(context->cparams, context->fparams);
	VbExDebug("VbSelectFirmware() returned 0x%08x\n", res);

	if (res != VBERROR_SUCCESS)
		return;

	/* Fix up the handoff structure. */
	context->handoff->selected_firmware =
		context->fparams->selected_firmware;

	/* Parse out the components for downstream consumption. */
	if (context->handoff->selected_firmware == VB_SELECT_FIRMWARE_A)
		components = (void *)context->fw_a;
	else if  (context->handoff->selected_firmware == VB_SELECT_FIRMWARE_B)
		components = (void *)context->fw_b;
	else
		return;

	for (i = 0; i < MAX_PARSED_FW_COMPONENTS; i++) {
		parse_component(components, i,
		                &context->handoff->components[i]);
	}
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
	return rdtscll();
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

VbError_t VbExHashFirmwareBody(VbCommonParams *cparams, uint32_t firmware_index)
{
	uint8_t *data;
	uint32_t size;
	uint32_t data_size;
	struct components *components;
	uint32_t i;

	switch (firmware_index) {
	case VB_SELECT_FIRMWARE_A:
		data = gcontext->fw_a;
		size = gcontext->fw_a_size;
		break;
	case VB_SELECT_FIRMWARE_B:
		data = gcontext->fw_b;
		size = gcontext->fw_b_size;
		break;
	default:
		return VBERROR_UNKNOWN;
	}

	components = (void *)data;
	data_size = sizeof(struct components);

	if (components->num_components > MAX_NUM_COMPONENTS)
		return VBERROR_UNKNOWN;

	data_size +=
		components->num_components * sizeof(struct component_entry);

	for (i = 0; i < components->num_components; i++)
		data_size += ALIGN(components->entries[i].size, 4);

	if (size < data_size)
			return VBERROR_UNKNOWN;

	VbUpdateFirmwareBodyHash(cparams, data, data_size);

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
	if (gcontext->tis_sendrecv(request, request_length,
	                           response, response_length))
		return VBERROR_UNKNOWN;
	return VBERROR_SUCCESS;
}

