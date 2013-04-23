/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * Copyright (c) 2009 Pattrick Hueper <phueper@hueper.net>
 * All rights reserved.
 * This program and the accompanying materials
 * are made available under the terms of the BSD License
 * which accompanies this distribution, and is available at
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/
#ifndef VBE_H
#define VBE_H

#include <boot/coreboot_tables.h>
// these structs are for input from and output to OF
typedef struct {
	u8 display_type;	// 0=NONE, 1= analog, 2=digital
	u16 screen_width;
	u16 screen_height;
	u16 screen_linebytes;	// bytes per line in framebuffer, may be more than screen_width
	u8 color_depth;	// color depth in bpp
	u32 framebuffer_address;
	u8 edid_block_zero[128];
} __attribute__ ((__packed__)) screen_info_t;

typedef struct {
	u8 signature[4];
	u16 size_reserved;
	u8 monitor_number;
	u16 max_screen_width;
	u8 color_depth;
} __attribute__ ((__packed__)) screen_info_input_t;

// these structs only store a subset of the VBE defined fields
// only those needed.
typedef struct {
	char signature[4];
	u16 version;
	u8 *oem_string_ptr;
	u32 capabilities;
	u16 video_mode_list[256];	// lets hope we never have more than 256 video modes...
	u16 total_memory;
} vbe_info_t;

typedef struct {
	u16 mode_attributes; // 00
	u8 win_a_attributes; // 02
	u8 win_b_attributes; // 03
	u16 win_granularity; // 04
	u16 win_size;        // 06
	u16 win_a_segment;   // 08
	u16 win_b_segment;   // 0a
	u32 win_func_ptr;    // 0c
	u16 bytes_per_scanline; // 10
	u16 x_resolution;    // 12
	u16 y_resolution;    // 14
	u8 x_charsize;       // 16
	u8 y_charsize;       // 17
	u8 number_of_planes; // 18
	u8 bits_per_pixel;   // 19
	u8 number_of_banks;  // 20
	u8 memory_model;     // 21
	u8 bank_size;        // 22
	u8 number_of_image_pages; // 23
	u8 reserved_page;
	u8 red_mask_size;
	u8 red_mask_pos;
	u8 green_mask_size;
	u8 green_mask_pos;
	u8 blue_mask_size;
	u8 blue_mask_pos;
	u8 reserved_mask_size;
	u8 reserved_mask_pos;
	u8 direct_color_mode_info;
	u32 phys_base_ptr;
	u32 offscreen_mem_offset;
	u16 offscreen_mem_size;
	u8 reserved[206];
} __attribute__ ((__packed__)) vesa_mode_info_t;

typedef struct {
	u16 video_mode;
	union {
		vesa_mode_info_t vesa;
		u8 mode_info_block[256];
	};
	// our crap
	//u16 attributes;
	//u16 linebytes;
	//u16 x_resolution;
	//u16 y_resolution;
	//u8 x_charsize;
	//u8 y_charsize;
	//u8 bits_per_pixel;
	//u8 memory_model;
	//u32 framebuffer_address;
} vbe_mode_info_t;

typedef struct {
	u8 port_number;	// i.e. monitor number
	u8 edid_transfer_time;
	u8 ddc_level;
	u8 edid_block_zero[128];
} vbe_ddc_info_t;

#define VESA_GET_INFO		0x4f00
#define VESA_GET_MODE_INFO	0x4f01
#define VESA_SET_MODE		0x4f02

int vbe_mode_info_valid(void);
void fill_lb_framebuffer(struct lb_framebuffer *framebuffer);
void vbe_set_graphics(void);
void vbe_textmode_console(void);

#endif // VBE_H
