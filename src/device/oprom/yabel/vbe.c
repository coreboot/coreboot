/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * Copyright (c) 2009 Pattrick Hueper <phueper@hueper.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#include <string.h>
#include <types.h>
#if IS_ENABLED(CONFIG_FRAMEBUFFER_SET_VESA_MODE)
#include <boot/coreboot_tables.h>
#endif

#include <endian.h>

#include "debug.h"

#include <x86emu/x86emu.h>
#include <x86emu/regs.h>
#include "../x86emu/prim_ops.h"

#include "biosemu.h"
#include "io.h"
#include "mem.h"
#include "interrupt.h"
#include "device.h"

#include <cbfs.h>

#include <delay.h>
#include "../../src/lib/jpeg.h"

#include <vbe.h>

// pointer to VBEInfoBuffer, set by vbe_prepare
u8 *vbe_info_buffer = 0;

// virtual BIOS Memory
u8 *biosmem;
u32 biosmem_size;

#if IS_ENABLED(CONFIG_FRAMEBUFFER_SET_VESA_MODE)
static inline u8
vbe_prepare(void)
{
	vbe_info_buffer = biosmem + (VBE_SEGMENT << 4);	// segment:offset off VBE Data Area
	//clear buffer
	memset(vbe_info_buffer, 0, 512);
	//set VbeSignature to "VBE2" to indicate VBE 2.0+ request
	vbe_info_buffer[0] = 'V';
	vbe_info_buffer[1] = 'B';
	vbe_info_buffer[2] = 'E';
	vbe_info_buffer[3] = '2';
	// ES:DI store pointer to buffer in virtual mem see vbe_info_buffer above...
	M.x86.R_EDI = 0x0;
	M.x86.R_ES = VBE_SEGMENT;

	return 0;		// successful init
}

// VBE Function 00h
static u8
vbe_info(vbe_info_t * info)
{
	vbe_prepare();
	// call VBE function 00h (Info Function)
	M.x86.R_EAX = 0x4f00;

	// enable trace
	CHECK_DBG(DEBUG_TRACE_X86EMU) {
		X86EMU_trace_on();
	}
	// run VESA Interrupt
	runInt10();

	if (M.x86.R_AL != 0x4f) {
		DEBUG_PRINTF_VBE("%s: VBE Info Function NOT supported! AL=%x\n",
				 __func__, M.x86.R_AL);
		return -1;
	}

	if (M.x86.R_AH != 0x0) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Info Function Return Code NOT OK! AH=%x\n",
		     __func__, M.x86.R_AH);
		return M.x86.R_AH;
	}
	//printf("VBE Info Dump:");
	//dump(vbe_info_buffer, 64);

	//offset 0: signature
	info->signature[0] = vbe_info_buffer[0];
	info->signature[1] = vbe_info_buffer[1];
	info->signature[2] = vbe_info_buffer[2];
	info->signature[3] = vbe_info_buffer[3];

	// offset 4: 16bit le containing VbeVersion
	info->version = in16le(vbe_info_buffer + 4);

	// offset 6: 32bit le containing segment:offset of OEM String in virtual Mem.
	info->oem_string_ptr =
	    biosmem + ((in16le(vbe_info_buffer + 8) << 4) +
		       in16le(vbe_info_buffer + 6));

	// offset 10: 32bit le capabilities
	info->capabilities = in32le(vbe_info_buffer + 10);

	// offset 14: 32 bit le containing segment:offset of supported video mode table
	u16 *video_mode_ptr;
	video_mode_ptr =
	    (u16 *) (biosmem +
			  ((in16le(vbe_info_buffer + 16) << 4) +
			   in16le(vbe_info_buffer + 14)));
	u32 i = 0;
	do {
		info->video_mode_list[i] = in16le(video_mode_ptr + i);
		i++;
	}
	while ((i <
		(sizeof(info->video_mode_list) /
		 sizeof(info->video_mode_list[0])))
	       && (info->video_mode_list[i - 1] != 0xFFFF));

	//offset 18: 16bit le total memory in 64KB blocks
	info->total_memory = in16le(vbe_info_buffer + 18);

	return 0;
}

static int mode_info_valid;

static int vbe_mode_info_valid(void)
{
	return mode_info_valid;
}

// VBE Function 01h
static u8
vbe_get_mode_info(vbe_mode_info_t * mode_info)
{
	vbe_prepare();
	// call VBE function 01h (Return VBE Mode Info Function)
	M.x86.R_EAX = 0x4f01;
	M.x86.R_CX = mode_info->video_mode;

	// enable trace
	CHECK_DBG(DEBUG_TRACE_X86EMU) {
		X86EMU_trace_on();
	}
	// run VESA Interrupt
	runInt10();

	if (M.x86.R_AL != 0x4f) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Return Mode Info Function NOT supported! AL=%x\n",
		     __func__, M.x86.R_AL);
		return -1;
	}

	if (M.x86.R_AH != 0x0) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Return Mode Info (mode: %04x) Function Return Code NOT OK! AH=%02x\n",
		     __func__, mode_info->video_mode, M.x86.R_AH);
		return M.x86.R_AH;
	}

	//pointer to mode_info_block is in ES:DI
	memcpy(mode_info->mode_info_block,
	       biosmem + ((M.x86.R_ES << 4) + M.x86.R_DI),
	       sizeof(mode_info->mode_info_block));
	mode_info_valid = 1;

	//printf("Mode Info Dump:");
	//dump(mode_info_block, 64);

	return 0;
}

// VBE Function 02h
static u8
vbe_set_mode(vbe_mode_info_t * mode_info)
{
	vbe_prepare();
	// call VBE function 02h (Set VBE Mode Function)
	M.x86.R_EAX = 0x4f02;
	M.x86.R_BX = mode_info->video_mode;
	M.x86.R_BX |= 0x4000;	// set bit 14 to request linear framebuffer mode
	M.x86.R_BX &= 0x7FFF;	// clear bit 15 to request clearing of framebuffer

	DEBUG_PRINTF_VBE("%s: setting mode: 0x%04x\n", __func__,
			 M.x86.R_BX);

	// enable trace
	CHECK_DBG(DEBUG_TRACE_X86EMU) {
		X86EMU_trace_on();
	}
	// run VESA Interrupt
	runInt10();

	if (M.x86.R_AL != 0x4f) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Set Mode Function NOT supported! AL=%x\n",
		     __func__, M.x86.R_AL);
		return -1;
	}

	if (M.x86.R_AH != 0x0) {
		DEBUG_PRINTF_VBE
		    ("%s: mode: %x VBE Set Mode Function Return Code NOT OK! AH=%x\n",
		     __func__, mode_info->video_mode, M.x86.R_AH);
		return M.x86.R_AH;
	}
	return 0;
}

#if 0
//VBE Function 08h
static u8
vbe_set_palette_format(u8 format)
{
	vbe_prepare();
	// call VBE function 09h (Set/Get Palette Data Function)
	M.x86.R_EAX = 0x4f08;
	M.x86.R_BL = 0x00;	// set format
	M.x86.R_BH = format;

	DEBUG_PRINTF_VBE("%s: setting palette format: %d\n", __func__,
			 format);

	// enable trace
	CHECK_DBG(DEBUG_TRACE_X86EMU) {
		X86EMU_trace_on();
	}
	// run VESA Interrupt
	runInt10();

	if (M.x86.R_AL != 0x4f) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Set Palette Format Function NOT supported! AL=%x\n",
		     __func__, M.x86.R_AL);
		return -1;
	}

	if (M.x86.R_AH != 0x0) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Set Palette Format Function Return Code NOT OK! AH=%x\n",
		     __func__, M.x86.R_AH);
		return M.x86.R_AH;
	}
	return 0;
}

// VBE Function 09h
static u8
vbe_set_color(u16 color_number, u32 color_value)
{
	vbe_prepare();
	// call VBE function 09h (Set/Get Palette Data Function)
	M.x86.R_EAX = 0x4f09;
	M.x86.R_BL = 0x00;	// set color
	M.x86.R_CX = 0x01;	// set only one entry
	M.x86.R_DX = color_number;
	// ES:DI is address where color_value is stored, we store it at 2000:0000
	M.x86.R_ES = 0x2000;
	M.x86.R_DI = 0x0;

	// store color value at ES:DI
	out32le(biosmem + (M.x86.R_ES << 4) + M.x86.R_DI, color_value);

	DEBUG_PRINTF_VBE("%s: setting color #%x: 0x%04x\n", __func__,
			 color_number, color_value);

	// enable trace
	CHECK_DBG(DEBUG_TRACE_X86EMU) {
		X86EMU_trace_on();
	}
	// run VESA Interrupt
	runInt10();

	if (M.x86.R_AL != 0x4f) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Set Palette Function NOT supported! AL=%x\n",
		     __func__, M.x86.R_AL);
		return -1;
	}

	if (M.x86.R_AH != 0x0) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Set Palette Function Return Code NOT OK! AH=%x\n",
		     __func__, M.x86.R_AH);
		return M.x86.R_AH;
	}
	return 0;
}

static u8
vbe_get_color(u16 color_number, u32 *color_value)
{
	vbe_prepare();
	// call VBE function 09h (Set/Get Palette Data Function)
	M.x86.R_EAX = 0x4f09;
	M.x86.R_BL = 0x00;	// get color
	M.x86.R_CX = 0x01;	// get only one entry
	M.x86.R_DX = color_number;
	// ES:DI is address where color_value is stored, we store it at 2000:0000
	M.x86.R_ES = 0x2000;
	M.x86.R_DI = 0x0;

	// enable trace
	CHECK_DBG(DEBUG_TRACE_X86EMU) {
		X86EMU_trace_on();
	}
	// run VESA Interrupt
	runInt10();

	if (M.x86.R_AL != 0x4f) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Set Palette Function NOT supported! AL=%x\n",
		     __func__, M.x86.R_AL);
		return -1;
	}

	if (M.x86.R_AH != 0x0) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Set Palette Function Return Code NOT OK! AH=%x\n",
		     __func__, M.x86.R_AH);
		return M.x86.R_AH;
	}
	// read color value from ES:DI
	*color_value = in32le(biosmem + (M.x86.R_ES << 4) + M.x86.R_DI);

	DEBUG_PRINTF_VBE("%s: getting color #%x --> 0x%04x\n", __func__,
			 color_number, *color_value);

	return 0;
}

// VBE Function 15h
static u8
vbe_get_ddc_info(vbe_ddc_info_t * ddc_info)
{
	vbe_prepare();
	// call VBE function 15h (DDC Info Function)
	M.x86.R_EAX = 0x4f15;
	M.x86.R_BL = 0x00;	// get DDC Info
	M.x86.R_CX = ddc_info->port_number;
	M.x86.R_ES = 0x0;
	M.x86.R_DI = 0x0;

	// enable trace
	CHECK_DBG(DEBUG_TRACE_X86EMU) {
		X86EMU_trace_on();
	}
	// run VESA Interrupt
	runInt10();

	if (M.x86.R_AL != 0x4f) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Get DDC Info Function NOT supported! AL=%x\n",
		     __func__, M.x86.R_AL);
		return -1;
	}

	if (M.x86.R_AH != 0x0) {
		DEBUG_PRINTF_VBE
		    ("%s: port: %x VBE Get DDC Info Function Return Code NOT OK! AH=%x\n",
		     __func__, ddc_info->port_number, M.x86.R_AH);
		return M.x86.R_AH;
	}
	// BH = approx. time in seconds to transfer one EDID block
	ddc_info->edid_transfer_time = M.x86.R_BH;
	// BL = DDC Level
	ddc_info->ddc_level = M.x86.R_BL;

	vbe_prepare();
	// call VBE function 15h (DDC Info Function)
	M.x86.R_EAX = 0x4f15;
	M.x86.R_BL = 0x01;	// read EDID
	M.x86.R_CX = ddc_info->port_number;
	M.x86.R_DX = 0x0;	// block number
	// ES:DI is address where EDID is stored, we store it at 2000:0000
	M.x86.R_ES = 0x2000;
	M.x86.R_DI = 0x0;

	// enable trace
	CHECK_DBG(DEBUG_TRACE_X86EMU) {
		X86EMU_trace_on();
	}
	// run VESA Interrupt
	runInt10();

	if (M.x86.R_AL != 0x4f) {
		DEBUG_PRINTF_VBE
		    ("%s: VBE Read EDID Function NOT supported! AL=%x\n",
		     __func__, M.x86.R_AL);
		return -1;
	}

	if (M.x86.R_AH != 0x0) {
		DEBUG_PRINTF_VBE
		    ("%s: port: %x VBE Read EDID Function Return Code NOT OK! AH=%x\n",
		     __func__, ddc_info->port_number, M.x86.R_AH);
		return M.x86.R_AH;
	}

	memcpy(ddc_info->edid_block_zero,
	       biosmem + (M.x86.R_ES << 4) + M.x86.R_DI,
	       sizeof(ddc_info->edid_block_zero));

	return 0;
}

static u32
vbe_get_info(void)
{
	u8 rval;
	int i;

	// XXX FIXME these need to be filled with sane values

	// get a copy of input struct...
	screen_info_input_t input;
	// output is pointer to the address passed as argv[4]
	screen_info_t local_output;
	screen_info_t *output = &local_output;
	// zero input
	memset(&input, 0, sizeof(screen_info_input_t));
	// zero output
	memset(&output, 0, sizeof(screen_info_t));

	vbe_info_t info;
	rval = vbe_info(&info);
	if (rval != 0)
		return rval;

	DEBUG_PRINTF_VBE("VbeSignature: %s\n", info.signature);
	DEBUG_PRINTF_VBE("VbeVersion: 0x%04x\n", info.version);
	DEBUG_PRINTF_VBE("OemString: %s\n", info.oem_string_ptr);
	DEBUG_PRINTF_VBE("Capabilities:\n");
	DEBUG_PRINTF_VBE("\tDAC: %s\n",
			 (info.capabilities & 0x1) ==
			 0 ? "fixed 6bit" : "switchable 6/8bit");
	DEBUG_PRINTF_VBE("\tVGA: %s\n",
			 (info.capabilities & 0x2) ==
			 0 ? "compatible" : "not compatible");
	DEBUG_PRINTF_VBE("\tRAMDAC: %s\n",
			 (info.capabilities & 0x4) ==
			 0 ? "normal" : "use blank bit in Function 09h");

	// argv[4] may be a pointer with enough space to return screen_info_t
	// as input, it must contain a screen_info_input_t with the following content:
	// byte[0:3] = "DDC\0" (zero-terminated signature header)
	// byte[4:5] = reserved space for the return struct... just in case we ever change
	//             the struct and don't have reserved enough memory (and let's hope the struct
	//             never gets larger than 64KB)
	// byte[6] = monitor port number for DDC requests ("only" one byte... so lets hope we never have more than 255 monitors...
	// byte[7:8] = max. screen width (OF may want to limit this)
	// byte[9] = required color depth in bpp
	if (strncmp((char *) input.signature, "DDC", 4) != 0) {
		printf
		    ("%s: Invalid input signature! expected: %s, is: %s\n",
		     __func__, "DDC", input.signature);
		return -1;
	}
	if (input.size_reserved != sizeof(screen_info_t)) {
		printf
		    ("%s: Size of return struct is wrong, required: %d, available: %d\n",
		     __func__, (int) sizeof(screen_info_t),
		     input.size_reserved);
		return -1;
	}

	vbe_ddc_info_t ddc_info;
	ddc_info.port_number = input.monitor_number;
	vbe_get_ddc_info(&ddc_info);

#if 0
	DEBUG_PRINTF_VBE("DDC: edid_tranfer_time: %d\n",
			 ddc_info.edid_transfer_time);
	DEBUG_PRINTF_VBE("DDC: ddc_level: %x\n", ddc_info.ddc_level);
	DEBUG_PRINTF_VBE("DDC: EDID:\n");
	CHECK_DBG(DEBUG_VBE) {
		dump(ddc_info.edid_block_zero,
		     sizeof(ddc_info.edid_block_zero));
	}
#endif
/* This could fail because of alignment issues, so use a longer form.
	*((u64 *) ddc_info.edid_block_zero) != (u64) 0x00FFFFFFFFFFFF00ULL
*/
	if (ddc_info.edid_block_zero[0] != 0x00 ||
	    ddc_info.edid_block_zero[1] != 0xFF ||
	    ddc_info.edid_block_zero[2] != 0xFF ||
	    ddc_info.edid_block_zero[3] != 0xFF ||
	    ddc_info.edid_block_zero[4] != 0xFF ||
	    ddc_info.edid_block_zero[5] != 0xFF ||
	    ddc_info.edid_block_zero[6] != 0xFF ||
	    ddc_info.edid_block_zero[7] != 0x00 ) {
		// invalid EDID signature... probably no monitor

		output->display_type = 0x0;
		return 0;
	} else if ((ddc_info.edid_block_zero[20] & 0x80) != 0) {
		// digital display
		output->display_type = 2;
	} else {
		// analog
		output->display_type = 1;
	}
	DEBUG_PRINTF_VBE("DDC: found display type %d\n", output->display_type);
	memcpy(output->edid_block_zero, ddc_info.edid_block_zero,
	       sizeof(ddc_info.edid_block_zero));
	i = 0;
	vbe_mode_info_t mode_info;
	vbe_mode_info_t best_mode_info;
	// initialize best_mode to 0
	memset(&best_mode_info, 0, sizeof(best_mode_info));
	while ((mode_info.video_mode = info.video_mode_list[i]) != 0xFFFF) {
		//DEBUG_PRINTF_VBE("%x: Mode: %04x\n", i, mode_info.video_mode);
		vbe_get_mode_info(&mode_info);

		// FIXME all these values are little endian!

		DEBUG_PRINTF_VBE("Video Mode 0x%04x available, %s\n",
				 mode_info.video_mode,
				 (le16_to_cpu(mode_info.vesa.mode_attributes) & 0x1) ==
				 0 ? "not supported" : "supported");
		DEBUG_PRINTF_VBE("\tTTY: %s\n",
				 (le16_to_cpu(mode_info.vesa.mode_attributes) & 0x4) ==
				 0 ? "no" : "yes");
		DEBUG_PRINTF_VBE("\tMode: %s %s\n",
				 (le16_to_cpu(mode_info.vesa.mode_attributes) & 0x8) ==
				 0 ? "monochrome" : "color",
				 (le16_to_cpu(mode_info.vesa.mode_attributes) & 0x10) ==
				 0 ? "text" : "graphics");
		DEBUG_PRINTF_VBE("\tVGA: %s\n",
				 (le16_to_cpu(mode_info.vesa.mode_attributes) & 0x20) ==
				 0 ? "compatible" : "not compatible");
		DEBUG_PRINTF_VBE("\tWindowed Mode: %s\n",
				 (le16_to_cpu(mode_info.vesa.mode_attributes) & 0x40) ==
				 0 ? "yes" : "no");
		DEBUG_PRINTF_VBE("\tFramebuffer: %s\n",
				 (le16_to_cpu(mode_info.vesa.mode_attributes) & 0x80) ==
				 0 ? "no" : "yes");
		DEBUG_PRINTF_VBE("\tResolution: %dx%d\n",
				 le16_to_cpu(mode_info.vesa.x_resolution),
				 le16_to_cpu(mode_info.vesa.y_resolution));
		DEBUG_PRINTF_VBE("\tChar Size: %dx%d\n",
				 mode_info.vesa.x_charsize, mode_info.vesa.y_charsize);
		DEBUG_PRINTF_VBE("\tColor Depth: %dbpp\n",
				 mode_info.vesa.bits_per_pixel);
		DEBUG_PRINTF_VBE("\tMemory Model: 0x%x\n",
				 mode_info.vesa.memory_model);
		DEBUG_PRINTF_VBE("\tFramebuffer Offset: %08x\n",
				 le32_to_cpu(mode_info.vesa.phys_base_ptr));

		if ((mode_info.vesa.bits_per_pixel == input.color_depth)
		    && (le16_to_cpu(mode_info.vesa.x_resolution) <= input.max_screen_width)
		    && ((le16_to_cpu(mode_info.vesa.mode_attributes) & 0x80) != 0)	// framebuffer mode
		    && ((le16_to_cpu(mode_info.vesa.mode_attributes) & 0x10) != 0)	// graphics
		    && ((le16_to_cpu(mode_info.vesa.mode_attributes) & 0x8) != 0)	// color
		    && (le16_to_cpu(mode_info.vesa.x_resolution) > le16_to_cpu(best_mode_info.vesa.x_resolution)))	// better than previous best_mode
		{
			// yiiiihaah... we found a new best mode
			memcpy(&best_mode_info, &mode_info, sizeof(mode_info));
		}
		i++;
	}

	if (best_mode_info.video_mode != 0) {
		DEBUG_PRINTF_VBE
		    ("Best Video Mode found: 0x%x, %dx%d, %dbpp, framebuffer_address: 0x%x\n",
		     best_mode_info.video_mode,
		     best_mode_info.vesa.x_resolution,
		     best_mode_info.vesa.y_resolution,
		     best_mode_info.vesa.bits_per_pixel,
		     le32_to_cpu(best_mode_info.vesa.phys_base_ptr));

		//printf("Mode Info Dump:");
		//dump(best_mode_info.mode_info_block, 64);

		// set the video mode
		vbe_set_mode(&best_mode_info);

		if ((info.capabilities & 0x1) != 0) {
			// switch to 8 bit palette format
			vbe_set_palette_format(8);
		}
		// setup a palette:
		// - first 216 colors are mixed colors for each component in 6 steps
		//   (6*6*6=216)
		// - then 10 shades of the three primary colors
		// - then 10 shades of grey
		// -------
		// = 256 colors
		//
		// - finally black is color 0 and white color FF (because SLOF expects it
		//   this way...)
		// this resembles the palette that the kernel/X Server seems to expect...

		u8 mixed_color_values[6] =
		    { 0xFF, 0xDA, 0xB3, 0x87, 0x54, 0x00 };
		u8 primary_color_values[10] =
		    { 0xF3, 0xE7, 0xCD, 0xC0, 0xA5, 0x96, 0x77, 0x66, 0x3F,
			0x27
		};
		u8 mc_size = sizeof(mixed_color_values);
		u8 prim_size = sizeof(primary_color_values);

		u8 curr_color_index;
		u32 curr_color;

		u8 r, g, b;
		// 216 mixed colors
		for (r = 0; r < mc_size; r++) {
			for (g = 0; g < mc_size; g++) {
				for (b = 0; b < mc_size; b++) {
					curr_color_index =
					    (r * mc_size * mc_size) +
					    (g * mc_size) + b;
					curr_color = 0;
					curr_color |= ((u32) mixed_color_values[r]) << 16;	//red value
					curr_color |= ((u32) mixed_color_values[g]) << 8;	//green value
					curr_color |= (u32) mixed_color_values[b];	//blue value
					vbe_set_color(curr_color_index,
						      curr_color);
				}
			}
		}

		// 10 shades of each primary color
		// red
		for (r = 0; r < prim_size; r++) {
			curr_color_index = mc_size * mc_size * mc_size + r;
			curr_color = ((u32) primary_color_values[r]) << 16;
			vbe_set_color(curr_color_index, curr_color);
		}
		//green
		for (g = 0; g < prim_size; g++) {
			curr_color_index =
			    mc_size * mc_size * mc_size + prim_size + g;
			curr_color = ((u32) primary_color_values[g]) << 8;
			vbe_set_color(curr_color_index, curr_color);
		}
		//blue
		for (b = 0; b < prim_size; b++) {
			curr_color_index =
			    mc_size * mc_size * mc_size + prim_size * 2 + b;
			curr_color = (u32) primary_color_values[b];
			vbe_set_color(curr_color_index, curr_color);
		}
		// 10 shades of grey
		for (i = 0; i < prim_size; i++) {
			curr_color_index =
			    mc_size * mc_size * mc_size + prim_size * 3 + i;
			curr_color = 0;
			curr_color |= ((u32) primary_color_values[i]) << 16;	//red
			curr_color |= ((u32) primary_color_values[i]) << 8;	//green
			curr_color |= ((u32) primary_color_values[i]);	//blue
			vbe_set_color(curr_color_index, curr_color);
		}

		// SLOF is using color 0x0 (black) and 0xFF (white) to draw to the screen...
		vbe_set_color(0x00, 0x00000000);
		vbe_set_color(0xFF, 0x00FFFFFF);

		output->screen_width = le16_to_cpu(best_mode_info.vesa.x_resolution);
		output->screen_height = le16_to_cpu(best_mode_info.vesa.y_resolution);
		output->screen_linebytes = le16_to_cpu(best_mode_info.vesa.bytes_per_scanline);
		output->color_depth = best_mode_info.vesa.bits_per_pixel;
		output->framebuffer_address =
		    le32_to_cpu(best_mode_info.vesa.phys_base_ptr);
	} else {
		printf("%s: No suitable video mode found!\n", __func__);
		//unset display_type...
		output->display_type = 0;
	}
	return 0;
}
#endif

vbe_mode_info_t mode_info;

void vbe_set_graphics(void)
{
	u8 rval;

	vbe_info_t info;
	rval = vbe_info(&info);
	if (rval != 0)
		return;

	DEBUG_PRINTF_VBE("VbeSignature: %s\n", info.signature);
	DEBUG_PRINTF_VBE("VbeVersion: 0x%04x\n", info.version);
	DEBUG_PRINTF_VBE("OemString: %s\n", info.oem_string_ptr);
	DEBUG_PRINTF_VBE("Capabilities:\n");
	DEBUG_PRINTF_VBE("\tDAC: %s\n",
			 (info.capabilities & 0x1) ==
			 0 ? "fixed 6bit" : "switchable 6/8bit");
	DEBUG_PRINTF_VBE("\tVGA: %s\n",
			 (info.capabilities & 0x2) ==
			 0 ? "compatible" : "not compatible");
	DEBUG_PRINTF_VBE("\tRAMDAC: %s\n",
			 (info.capabilities & 0x4) ==
			 0 ? "normal" : "use blank bit in Function 09h");

	mode_info.video_mode = (1 << 14) | CONFIG_FRAMEBUFFER_VESA_MODE;
	vbe_get_mode_info(&mode_info);
	vbe_set_mode(&mode_info);

#if IS_ENABLED(CONFIG_BOOTSPLASH)
	unsigned char *framebuffer =
		(unsigned char *) le32_to_cpu(mode_info.vesa.phys_base_ptr);
	DEBUG_PRINTF_VBE("FRAMEBUFFER: 0x%p\n", framebuffer);

	struct jpeg_decdata *decdata;

	/* Switching Intel IGD to 1MB video memory will break this. Who
	 * cares. */
	// int imagesize = 1024*768*2;

	unsigned char *jpeg = cbfs_boot_map_with_leak("bootsplash.jpg",
							CBFS_TYPE_BOOTSPLASH,
							NULL);
	if (!jpeg) {
		DEBUG_PRINTF_VBE("Could not find bootsplash.jpg\n");
		return;
	}
	DEBUG_PRINTF_VBE("Splash at %p ...\n", jpeg);
	dump(jpeg, 64);

	decdata = malloc(sizeof(*decdata));
	int ret = 0;
	DEBUG_PRINTF_VBE("Decompressing boot splash screen...\n");
	ret = jpeg_decode(jpeg, framebuffer, 1024, 768, 16, decdata);
	DEBUG_PRINTF_VBE("returns %x\n", ret);
#endif
}

int fill_lb_framebuffer(struct lb_framebuffer *framebuffer)
{
	if (!vbe_mode_info_valid())
		return -1;

	framebuffer->physical_address = le32_to_cpu(mode_info.vesa.phys_base_ptr);

	framebuffer->x_resolution = le16_to_cpu(mode_info.vesa.x_resolution);
	framebuffer->y_resolution = le16_to_cpu(mode_info.vesa.y_resolution);
	framebuffer->bytes_per_line = le16_to_cpu(mode_info.vesa.bytes_per_scanline);
	framebuffer->bits_per_pixel = mode_info.vesa.bits_per_pixel;

	framebuffer->red_mask_pos = mode_info.vesa.red_mask_pos;
	framebuffer->red_mask_size = mode_info.vesa.red_mask_size;

	framebuffer->green_mask_pos = mode_info.vesa.green_mask_pos;
	framebuffer->green_mask_size = mode_info.vesa.green_mask_size;

	framebuffer->blue_mask_pos = mode_info.vesa.blue_mask_pos;
	framebuffer->blue_mask_size = mode_info.vesa.blue_mask_size;

	framebuffer->reserved_mask_pos = mode_info.vesa.reserved_mask_pos;
	framebuffer->reserved_mask_size = mode_info.vesa.reserved_mask_size;

	return 0;
}

void vbe_textmode_console(void)
{
	/* Wait, just a little bit more, pleeeease ;-) */
	delay(2);

	M.x86.R_EAX = 0x0003;
	runInt10();
}

#endif
