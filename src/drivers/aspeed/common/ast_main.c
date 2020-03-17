/*
 * This file is part of the coreboot project.
 *
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

#include <delay.h>

#include "ast_drv.h"
#include "ast_dram_tables.h"

void ast_set_index_reg_mask(struct ast_private *ast,
			    uint32_t base, uint8_t index,
			    uint8_t mask, uint8_t val)
{
	u8 tmp;
	ast_io_write8(ast, base, index);
	tmp = (ast_io_read8(ast, base + 1) & mask) | val;
	ast_set_index_reg(ast, base, index, tmp);
}

uint8_t ast_get_index_reg(struct ast_private *ast,
			  uint32_t base, uint8_t index)
{
	uint8_t ret;
	ast_io_write8(ast, base, index);
	ret = ast_io_read8(ast, base + 1);
	return ret;
}

uint8_t ast_get_index_reg_mask(struct ast_private *ast,
			       uint32_t base, uint8_t index, uint8_t mask)
{
	uint8_t ret;
	ast_io_write8(ast, base, index);
	ret = ast_io_read8(ast, base + 1) & mask;
	return ret;
}


static int ast_detect_chip(struct drm_device *dev, bool *need_post)
{
	struct ast_private *ast = dev->dev_private;
	uint32_t data, jreg;
	ast_open_key(ast);

	if (dev->pdev->device == PCI_CHIP_AST1180) {
		ast->chip = AST1100;
		DRM_INFO("AST 1180 detected\n");
	} else {
		pci_read_config_dword(ast->dev->pdev, 0x08, &data);
		uint8_t revision = data & 0xff;
		if (revision >= 0x40) {
			ast->chip = AST2500;
			DRM_INFO("AST 2500 detected\n");
		} else if (revision >= 0x30) {
			ast->chip = AST2400;
			DRM_INFO("AST 2400 detected\n");
		} else if (revision >= 0x20) {
			ast->chip = AST2300;
			DRM_INFO("AST 2300 detected\n");
		} else if (revision >= 0x10) {
			ast_write32(ast, 0xf004, 0x1e6e0000);
			ast_write32(ast, 0xf000, 0x1);

			data = ast_read32(ast, 0x1207c);
			switch (data & 0x0300) {
			case 0x0200:
				ast->chip = AST1100;
				DRM_INFO("AST 1100 detected\n");
				break;
			case 0x0100:
				ast->chip = AST2200;
				DRM_INFO("AST 2200 detected\n");
				break;
			case 0x0000:
				ast->chip = AST2150;
				DRM_INFO("AST 2150 detected\n");
				break;
			default:
				ast->chip = AST2100;
				DRM_INFO("AST 2100 detected\n");
				break;
			}
			ast->vga2_clone = false;
		} else {
			ast->chip = AST2000;
			DRM_INFO("AST 2000 detected\n");
		}
	}

	/*
	 * If VGA isn't enabled, we need to enable now or subsequent
	 * access to the scratch registers will fail. We also inform
	 * our caller that it needs to POST the chip
	 * (Assumption: VGA not enabled -> need to POST)
	 */
	if (!ast_is_vga_enabled(dev)) {
		ast_enable_vga(dev);
		ast_enable_mmio(dev);
		DRM_INFO("VGA not enabled on entry, requesting chip POST\n");
		*need_post = true;
	} else
		*need_post = false;

	/* Check if we support wide screen */
	switch (ast->chip) {
	case AST1180:
		ast->support_wide_screen = true;
		break;
	case AST2000:
		ast->support_wide_screen = false;
		break;
	default:
		jreg = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xd0, 0xff);
		if (!(jreg & 0x80))
			ast->support_wide_screen = true;
		else if (jreg & 0x01)
			ast->support_wide_screen = true;
		else {
			ast->support_wide_screen = false;
			/* Read SCU7c (silicon revision register) */
			ast_write32(ast, 0xf004, 0x1e6e0000);
			ast_write32(ast, 0xf000, 0x1);
			data = ast_read32(ast, 0x1207c);
			data &= 0x300;
			if (ast->chip == AST2300 && data == 0x0) /* ast1300 */
				ast->support_wide_screen = true;
			if (ast->chip == AST2400 && data == 0x100) /* ast1400 */
				ast->support_wide_screen = true;
			if (ast->chip == AST2500 && data == 0x100) /* ast2510 */
				ast->support_wide_screen = true;
		}
		break;
	}

	/* Check 3rd Tx option (digital output afaik) */
	ast->tx_chip_type = AST_TX_NONE;

	/*
	 * VGACRA3 Enhanced Color Mode Register, check if DVO is already
	 * enabled, in that case, assume we have a SIL164 TMDS transmitter
	 *
	 * Don't make that assumption if we the chip wasn't enabled and
	 * is at power-on reset, otherwise we'll incorrectly "detect" a
	 * SIL164 when there is none.
	 */
	if (!*need_post) {
		jreg = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xa3, 0xff);
		if (jreg & 0x80)
			ast->tx_chip_type = AST_TX_SIL164;
	}

	if ((ast->chip == AST2300) || (ast->chip == AST2400)) {
		/*
		 * On AST2300 and 2400, look the configuration set by the SoC in
		 * the SOC scratch register #1 bits 11:8 (interestingly marked
		 * as "reserved" in the spec)
		 */
		jreg = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xd1, 0xff);
		switch (jreg) {
		case 0x04:
			ast->tx_chip_type = AST_TX_SIL164;
			break;
		case 0x08:
			ast->dp501_fw_addr = kzalloc(32*1024, GFP_KERNEL);
			if (ast->dp501_fw_addr) {
				/* backup firmware */
				if (ast_backup_fw(dev, ast->dp501_fw_addr, 32*1024)) {
					kfree(ast->dp501_fw_addr);
					ast->dp501_fw_addr = NULL;
				}
			}
			/* fallthrough */
		case 0x0c:
			ast->tx_chip_type = AST_TX_DP501;
		}
	}

	/* Print stuff for diagnostic purposes */
	switch (ast->tx_chip_type) {
	case AST_TX_SIL164:
		DRM_INFO("Using Sil164 TMDS transmitter\n");
		break;
	case AST_TX_DP501:
		DRM_INFO("Using DP501 DisplayPort transmitter\n");
		break;
	default:
		DRM_INFO("Analog VGA only\n");
	}
	return 0;
}

static int ast_get_dram_info(struct drm_device *dev)
{
	struct ast_private *ast = dev->dev_private;
	uint8_t i;
	uint32_t data, data2;
	uint32_t denum, num, div, ref_pll;

	ast_write32(ast, 0xf004, 0x1e6e0000);
	ast_write32(ast, 0xf000, 0x1);


	ast_write32(ast, 0x10000, 0xfc600309);

	/* Wait up to 2.5 seconds for device initialization / register unlock */
	for (i = 0; i < 250; i++) {
		if (ast_read32(ast, 0x10000) == 0x01)
			break;
		mdelay(10);
	}
	if (ast_read32(ast, 0x10000) != 0x01)
		dev_err(dev->pdev, "Unable to unlock SDRAM control registers\n");

	data = ast_read32(ast, 0x10004);

	if (data & 0x40)
		ast->dram_bus_width = 16;
	else
		ast->dram_bus_width = 32;

	if (ast->chip == AST2500) {
		switch (data & 0x03) {
		case 0:
			ast->dram_type = AST_DRAM_1Gx16;
			break;
		default:
		case 1:
			ast->dram_type = AST_DRAM_2Gx16;
			break;
		case 2:
			ast->dram_type = AST_DRAM_4Gx16;
			break;
		case 3:
			ast->dram_type = AST_DRAM_8Gx16;
			break;
		}
	} else if (ast->chip == AST2300 || ast->chip == AST2400) {
		switch (data & 0x03) {
		case 0:
			ast->dram_type = AST_DRAM_512Mx16;
			break;
		default:
		case 1:
			ast->dram_type = AST_DRAM_1Gx16;
			break;
		case 2:
			ast->dram_type = AST_DRAM_2Gx16;
			break;
		case 3:
			ast->dram_type = AST_DRAM_4Gx16;
			break;
		}
	} else {
		switch (data & 0x0c) {
		case 0:
		case 4:
			ast->dram_type = AST_DRAM_512Mx16;
			break;
		case 8:
			if (data & 0x40)
				ast->dram_type = AST_DRAM_1Gx16;
			else
				ast->dram_type = AST_DRAM_512Mx32;
			break;
		case 0xc:
			ast->dram_type = AST_DRAM_1Gx32;
			break;
		}
	}

	data = ast_read32(ast, 0x10120);
	data2 = ast_read32(ast, 0x10170);
	if (data2 & 0x2000)
		ref_pll = 14318;
	else
		ref_pll = 12000;

	denum = data & 0x1f;
	num = (data & 0x3fe0) >> 5;
	data = (data & 0xc000) >> 14;
	switch (data) {
	case 3:
		div = 0x4;
		break;
	case 2:
	case 1:
		div = 0x2;
		break;
	default:
		div = 0x1;
		break;
	}
	ast->mclk = ref_pll * (num + 2) / (denum + 2) * (div * 1000);
	return 0;
}

static u32 ast_get_vram_info(struct drm_device *dev)
{
	struct ast_private *ast = dev->dev_private;
	u8 jreg;
	u32 vram_size;
	ast_open_key(ast);

	vram_size = AST_VIDMEM_DEFAULT_SIZE;
	jreg = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xaa, 0xff);
	switch (jreg & 3) {
	case 0: vram_size = AST_VIDMEM_SIZE_8M; break;
	case 1: vram_size = AST_VIDMEM_SIZE_16M; break;
	case 2: vram_size = AST_VIDMEM_SIZE_32M; break;
	case 3: vram_size = AST_VIDMEM_SIZE_64M; break;
	}

	return vram_size;
}

int ast_driver_load(struct drm_device *dev, unsigned long flags)
{
	struct ast_private *ast;
	bool need_post;
	int ret = 0;
	struct resource *res;

	ast = kzalloc(sizeof(struct ast_private), GFP_KERNEL);
	if (!ast)
		return -ENOMEM;

	dev->dev_private = ast;
	ast->dev = dev;

	/* PCI BAR 1 */
	res = find_resource(dev->pdev, 0x14);
	if (!res) {
		dev_err(dev->pdev, "BAR1 resource not found.\n");
		ret = -EIO;
		goto out_free;
	}
	ast->regs = res2mmio(res, 0, 0);
	if (!ast->regs) {
		ret = -EIO;
		goto out_free;
	}

	/* PCI BAR 2 */
	ast->io_space_uses_mmap = false;
	res = find_resource(dev->pdev, 0x18);
	if (!res) {
		dev_err(dev->pdev, "BAR2 resource not found.\n");
		ret = -EIO;
		goto out_free;
	}

	/*
	 * If we don't have IO space at all, use MMIO now and
	 * assume the chip has MMIO enabled by default (rev 0x20
	 * and higher).
	 */
	if (!(res->flags & IORESOURCE_IO)) {
		DRM_INFO("platform has no IO space, trying MMIO\n");
		ast->ioregs = ast->regs + AST_IO_MM_OFFSET;
		ast->io_space_uses_mmap = true;
	}

	/* "map" IO regs if the above hasn't done so already */
	if (!ast->ioregs) {
		ast->ioregs = res2mmio(res, 0, 0);
		if (!ast->ioregs) {
			ret = -EIO;
			goto out_free;
		}
		/* Adjust the I/O space location to match expectations (the code expects offset 0x0 to be I/O location 0x380) */
		ast->ioregs = (void *)AST_IO_MM_OFFSET;
	}

	ast_detect_chip(dev, &need_post);

	if (ast->chip != AST1180) {
		ast_get_dram_info(dev);
		ast->vram_size = ast_get_vram_info(dev);
		DRM_INFO("dram %d %d %d %08x\n", ast->mclk, ast->dram_type, ast->dram_bus_width, ast->vram_size);
	}

	if (need_post)
		ast_post_gpu(dev);

	return 0;
out_free:
	kfree(ast);
	dev->dev_private = NULL;
	return ret;
}
