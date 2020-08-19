/* SPDX-License-Identifier: MIT */
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
	AST2500,
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
#define AST_DRAM_8Gx16   8

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
	enum {
		ast_use_p2a,
		ast_use_dt,
		ast_use_defaults
	} config_mode;

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

#define	EINVAL		22	/* Invalid argument */

#define AST_DEFAULT_HWC_NUM 2
/* define for signature structure */
#define AST_HWC_SIGNATURE_CHECKSUM  0x00
#define AST_HWC_SIGNATURE_SizeX     0x04
#define AST_HWC_SIGNATURE_SizeY     0x08
#define AST_HWC_SIGNATURE_X         0x0C
#define AST_HWC_SIGNATURE_Y         0x10
#define AST_HWC_SIGNATURE_HOTSPOTX  0x14
#define AST_HWC_SIGNATURE_HOTSPOTY  0x18

/* ast_mode.c stuff */
struct ast_vbios_stdtable {
	u8 misc;
	u8 seq[4];
	u8 crtc[25];
	u8 ar[20];
	u8 gr[9];
};

struct ast_vbios_enhtable {
	u32 ht;
	u32 hde;
	u32 hfp;
	u32 hsync;
	u32 vt;
	u32 vde;
	u32 vfp;
	u32 vsync;
	u32 dclk_index;
	u32 flags;
	u32 refresh_rate;
	u32 refresh_rate_index;
	u32 mode_id;
};

struct ast_vbios_dclk_info {
	u8 param1;
	u8 param2;
	u8 param3;
};

struct ast_vbios_mode_info {
	const struct ast_vbios_stdtable *std_table;
	const struct ast_vbios_enhtable *enh_table;
};

#define DRM_MODE_FLAG_NVSYNC 1
#define DRM_MODE_FLAG_PVSYNC 2
#define DRM_MODE_FLAG_NHSYNC 4
#define DRM_MODE_FLAG_PHSYNC 8

struct drm_display_mode {
	/* Proposed mode values */
	u16 vrefresh;	/* in Hz */
	u32 clock;
	u16 hdisplay;
	u16 vdisplay;
	u32 flags;

	/* Actual mode we give to hw */
	u16 crtc_hdisplay;
	u16 crtc_htotal;
	u16 crtc_hblank_start;
	u16 crtc_hblank_end;
	u16 crtc_hsync_start;
	u16 crtc_hsync_end;
	u16 crtc_vtotal;
	u16 crtc_vsync_start;
	u16 crtc_vsync_end;
	u16 crtc_vdisplay;
	u16 crtc_vblank_start;
	u16 crtc_vblank_end;
};

struct drm_format {
	u32 cpp[1]; /* Colors per pixel */
};

struct drm_framebuffer {
	u32 pitches[1];
	struct drm_format *format;
	u32 mmio_addr;
};

struct drm_primary {
	struct drm_framebuffer *fb;
};

struct drm_crtc {
	struct drm_device *dev;
	struct drm_primary *primary;
	struct drm_display_mode mode;
};

struct drm_connector {
	struct drm_device *dev;
};

enum drm_mode_status {
	MODE_NOMODE,
	MODE_OK
};

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
void ast_set_dp501_video_output(struct drm_device *dev, u8 mode);
bool ast_backup_fw(struct drm_device *dev, u8 *addr, u32 size);
bool ast_dp501_read_edid(struct drm_device *dev, u8 *ediddata);
u8 ast_get_dp501_max_clk(struct drm_device *dev);
void ast_init_3rdtx(struct drm_device *dev);
void ast_release_firmware(struct drm_device *dev);

/* ast mode */
int ast_crtc_mode_set(struct drm_crtc *crtc,
		      struct drm_display_mode *mode,
		      struct drm_display_mode *adjusted_mode);
enum drm_mode_status ast_mode_valid(struct drm_connector *connector,
				    const unsigned int hdisplay,
				    const unsigned int vdisplay);
void ast_hide_cursor(struct drm_crtc *crtc);
void ast_set_offset_reg(struct drm_crtc *crtc);
void ast_set_start_address_crt1(struct ast_private *ast, u32 offset);

/* ast_mode_corebootfb */
int ast_driver_framebuffer_init(struct drm_device *dev, int flags);
int ast_crtc_do_set_base(struct drm_crtc *crtc);

/* ast i2c */
int ast_software_i2c_read(struct ast_private *ast_priv, uint8_t edid[128]);

#endif
