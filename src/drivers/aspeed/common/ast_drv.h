/*
 * Copyright 2012 Red Hat Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 */
/*
 * Authors: Dave Airlie <airlied@redhat.com>
 */
#ifndef __AST_DRV_H__
#define __AST_DRV_H__

#include "aspeed_coreboot.h"

#define PCI_CHIP_AST2000 0x2000
#define PCI_CHIP_AST2100 0x2010
#define PCI_CHIP_AST1180 0x1180


enum ast_chip {
	AST2000,
	AST2100,
	AST1100,
	AST2200,
	AST2150,
	AST2300,
	AST2400,
	AST1180,
};

enum ast_tx_chip {
	AST_TX_NONE,
	AST_TX_SIL164,
	AST_TX_ITE66121,
	AST_TX_DP501,
};

#define AST_DRAM_512Mx16 0
#define AST_DRAM_1Gx16   1
#define AST_DRAM_512Mx32 2
#define AST_DRAM_1Gx32   3
#define AST_DRAM_2Gx16   6
#define AST_DRAM_4Gx16   7

struct ast_fbdev;

struct ast_private {
	struct drm_device *dev;

	void __iomem *regs;
	void __iomem *ioregs;
	bool io_space_uses_mmap;

	enum ast_chip chip;
	bool vga2_clone;
	uint32_t dram_bus_width;
	uint32_t dram_type;
	uint32_t mclk;
	uint32_t vram_size;

	struct ast_fbdev *fbdev;

	int fb_mtrr;

	struct drm_gem_object *cursor_cache;
	uint64_t cursor_cache_gpu_addr;

	int next_cursor;
	bool support_wide_screen;

	enum ast_tx_chip tx_chip_type;
	u8 dp501_maxclk;
	u8 *dp501_fw_addr;
	const struct firmware *dp501_fw;	/* dp501 fw */
};

int ast_driver_load(struct drm_device *dev, unsigned long flags);
int ast_driver_unload(struct drm_device *dev);

#define AST_IO_AR_PORT_WRITE		(0x40)
#define AST_IO_MISC_PORT_WRITE		(0x42)
#define AST_IO_VGA_ENABLE_PORT		(0x43)
#define AST_IO_SEQ_PORT			(0x44)
#define AST_IO_DAC_INDEX_READ		(0x47)
#define AST_IO_DAC_INDEX_WRITE		(0x48)
#define AST_IO_DAC_DATA		        (0x49)
#define AST_IO_GR_PORT			(0x4E)
#define AST_IO_CRTC_PORT		(0x54)
#define AST_IO_INPUT_STATUS1_READ	(0x5A)
#define AST_IO_MISC_PORT_READ		(0x4C)

#define AST_IO_MM_OFFSET		(0x380)

#define __ast_read(x) \
static inline u##x ast_read##x(struct ast_private *ast, u32 reg) { \
u##x val = 0;\
val = ioread##x(ast->regs + reg); \
return val;\
}

__ast_read(8);
__ast_read(16);
__ast_read(32)

#define __ast_io_read(x) \
static inline u##x ast_io_read##x(struct ast_private *ast, u32 reg) { \
u##x val = 0;\
if (ast->io_space_uses_mmap) \
val = ioread##x(ast->regs + reg); \
else \
val = ioread_cbio##x(ast->ioregs + reg); \
return val;\
}

__ast_io_read(8);
__ast_io_read(16);
__ast_io_read(32);

#define __ast_write(x) \
static inline void ast_write##x(struct ast_private *ast, u32 reg, u##x val) {\
	iowrite##x(val, ast->regs + reg);\
	}

__ast_write(8);
__ast_write(16);
__ast_write(32);

#define __ast_io_write(x) \
static inline void ast_io_write##x(struct ast_private *ast, u32 reg, u##x val) {\
	if (ast->io_space_uses_mmap) \
	iowrite##x(val, ast->regs + reg);\
	else \
	iowrite_cbio##x(val, ast->ioregs + reg);\
	}

__ast_io_write(8);
__ast_io_write(16);
#undef __ast_io_write

static inline void ast_set_index_reg(struct ast_private *ast,
				     uint32_t base, uint8_t index,
				     uint8_t val)
{
	ast_io_write16(ast, base, ((u16)val << 8) | index);
}

void ast_set_index_reg_mask(struct ast_private *ast,
			    uint32_t base, uint8_t index,
			    uint8_t mask, uint8_t val);
uint8_t ast_get_index_reg(struct ast_private *ast,
			  uint32_t base, uint8_t index);
uint8_t ast_get_index_reg_mask(struct ast_private *ast,
			       uint32_t base, uint8_t index, uint8_t mask);

static inline void ast_open_key(struct ast_private *ast)
{
	ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x80, 0xA8);
}

#define AST_VIDMEM_SIZE_8M    0x00800000
#define AST_VIDMEM_SIZE_16M   0x01000000
#define AST_VIDMEM_SIZE_32M   0x02000000
#define AST_VIDMEM_SIZE_64M   0x04000000
#define AST_VIDMEM_SIZE_128M  0x08000000

#define AST_VIDMEM_DEFAULT_SIZE AST_VIDMEM_SIZE_8M

#define AST_MAX_HWC_WIDTH 64
#define AST_MAX_HWC_HEIGHT 64

#define AST_HWC_SIZE                (AST_MAX_HWC_WIDTH*AST_MAX_HWC_HEIGHT*2)
#define AST_HWC_SIGNATURE_SIZE      32

#define AST_DEFAULT_HWC_NUM 2
/* define for signature structure */
#define AST_HWC_SIGNATURE_CHECKSUM  0x00
#define AST_HWC_SIGNATURE_SizeX     0x04
#define AST_HWC_SIGNATURE_SizeY     0x08
#define AST_HWC_SIGNATURE_X         0x0C
#define AST_HWC_SIGNATURE_Y         0x10
#define AST_HWC_SIGNATURE_HOTSPOTX  0x14
#define AST_HWC_SIGNATURE_HOTSPOTY  0x18

#define AST_MM_ALIGN_SHIFT 4
#define AST_MM_ALIGN_MASK ((1 << AST_MM_ALIGN_SHIFT) - 1)

#define DRM_FILE_PAGE_OFFSET (0x100000000ULL >> PAGE_SHIFT)

/* ast post */
void ast_enable_vga(struct drm_device *dev);
void ast_enable_mmio(struct drm_device *dev);
bool ast_is_vga_enabled(struct drm_device *dev);
void ast_post_gpu(struct drm_device *dev);
u32 ast_mindwm(struct ast_private *ast, u32 r);
void ast_moutdwm(struct ast_private *ast, u32 r, u32 v);
/* ast dp501 */
int ast_load_dp501_microcode(struct drm_device *dev);
void ast_set_dp501_video_output(struct drm_device *dev, u8 mode);
bool ast_launch_m68k(struct drm_device *dev);
bool ast_backup_fw(struct drm_device *dev, u8 *addr, u32 size);
bool ast_dp501_read_edid(struct drm_device *dev, u8 *ediddata);
u8 ast_get_dp501_max_clk(struct drm_device *dev);
void ast_init_3rdtx(struct drm_device *dev);
#endif
