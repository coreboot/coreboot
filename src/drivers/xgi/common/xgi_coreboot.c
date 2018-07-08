/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * xgifb_probe taken from the Linux xgifb driver (v3.18.5) and adapted for coreboot
 * xgifb_modeset cobbled together from other portions of the same driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>
#include <vbe.h>

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <pc80/vga.h>

#include "xgi_coreboot.h"
#include "vstruct.h"

#include "XGIfb.h"
#include "XGI_main.h"
#include "vb_init.h"
#include "vb_util.h"
#include "vb_setmode.h"

#include "XGI_main.c"

static int xgi_vbe_valid;
static struct lb_framebuffer xgi_fb;

int xgifb_probe(struct pci_dev *pdev, struct xgifb_video_info *xgifb_info)
{
	u8 reg, reg1;
	u8 CR48, CR38;
	int ret;
	struct xgi_hw_device_info *hw_info;
	unsigned long video_size_max;

	hw_info = &xgifb_info->hw_info;
	xgifb_info->chip_id = pdev->device;
	pci_read_config_byte(pdev,
			     PCI_REVISION_ID,
			     &xgifb_info->revision_id);
	hw_info->jChipRevision = xgifb_info->revision_id;

	xgifb_info->subsysvendor = pdev->subsystem_vendor;
	xgifb_info->subsysdevice = pdev->subsystem_device;

	video_size_max = pci_resource_len(pdev, 0);
	xgifb_info->video_base = pci_resource_start(pdev, 0);
	xgifb_info->mmio_base = pci_resource_start(pdev, 1);
	xgifb_info->mmio_size = pci_resource_len(pdev, 1);
	xgifb_info->vga_base = pci_resource_start(pdev, 2) + 0x30;
	dev_info(&pdev->dev, "Relocate IO address: %Lx [%08lx]\n",
		 (u64) pci_resource_start(pdev, 2),
		 xgifb_info->vga_base);

	if (XGIfb_crt2type != -1) {
		xgifb_info->display2 = XGIfb_crt2type;
		xgifb_info->display2_force = true;
	}

	XGIRegInit(&xgifb_info->dev_info, xgifb_info->vga_base);

	xgifb_reg_set(XGISR, IND_SIS_PASSWORD, SIS_PASSWORD);
	reg1 = xgifb_reg_get(XGISR, IND_SIS_PASSWORD);

	if (reg1 != 0xa1) {
		dev_err(&pdev->dev, "I/O error\n");
		ret = -5;
		goto error_disable;
	}

	switch (xgifb_info->chip_id) {
	case PCI_DEVICE_ID_XGI_20:
		xgifb_reg_or(XGICR, Index_CR_GPIO_Reg3, GPIOG_EN);
		CR48 = xgifb_reg_get(XGICR, Index_CR_GPIO_Reg1);
		if (CR48&GPIOG_READ)
			xgifb_info->chip = XG21;
		else
			xgifb_info->chip = XG20;
		break;
	case PCI_DEVICE_ID_XGI_40:
		xgifb_info->chip = XG40;
		break;
	case PCI_DEVICE_ID_XGI_42:
		xgifb_info->chip = XG42;
		break;
	case PCI_DEVICE_ID_XGI_27:
		xgifb_info->chip = XG27;
		break;
	default:
		ret = -19;
		goto error_disable;
	}

	dev_info(&pdev->dev, "chipid = %x\n", xgifb_info->chip);
	hw_info->jChipType = xgifb_info->chip;

	if (XGIfb_get_dram_size(xgifb_info)) {
		xgifb_info->video_size = min_t(unsigned long, video_size_max,
						SZ_16M);
	} else if (xgifb_info->video_size > video_size_max) {
		xgifb_info->video_size = video_size_max;
	}

	if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER)) {
		/* Enable PCI_LINEAR_ADDRESSING and MMIO_ENABLE  */
		xgifb_reg_or(XGISR,
			     IND_SIS_PCI_ADDRESS_SET,
			     (SIS_PCI_ADDR_ENABLE | SIS_MEM_MAP_IO_ENABLE));
		/* Enable 2D accelerator engine */
		xgifb_reg_or(XGISR, IND_SIS_MODULE_ENABLE, SIS_ENABLE_2D);
	}

	hw_info->ulVideoMemorySize = xgifb_info->video_size;

	xgifb_info->video_vbase = hw_info->pjVideoMemoryAddress =
				  (void *)(intptr_t)xgifb_info->video_base;
	xgifb_info->mmio_vbase = (void *)(intptr_t)xgifb_info->mmio_base;

	dev_info(&pdev->dev,
		 "Framebuffer at 0x%Lx, mapped to 0x%p, size %dk\n",
		 (u64) xgifb_info->video_base,
		 xgifb_info->video_vbase,
		 xgifb_info->video_size / 1024);

	dev_info(&pdev->dev,
		 "MMIO at 0x%Lx, mapped to 0x%p, size %ldk\n",
		 (u64) xgifb_info->mmio_base, xgifb_info->mmio_vbase,
		 xgifb_info->mmio_size / 1024);

	pci_set_drvdata(pdev, xgifb_info);
	if (!XGIInitNew(pdev))
		dev_err(&pdev->dev, "XGIInitNew() failed!\n");

	xgifb_info->mtrr = -1;

	xgifb_info->hasVB = HASVB_NONE;
	if ((xgifb_info->chip == XG20) ||
	    (xgifb_info->chip == XG27)) {
		xgifb_info->hasVB = HASVB_NONE;
	} else if (xgifb_info->chip == XG21) {
		CR38 = xgifb_reg_get(XGICR, 0x38);
		if ((CR38 & 0xE0) == 0xC0)
			xgifb_info->display2 = XGIFB_DISP_LCD;
		else if ((CR38 & 0xE0) == 0x60)
			xgifb_info->hasVB = HASVB_CHRONTEL;
		else
			xgifb_info->hasVB = HASVB_NONE;
	} else {
		XGIfb_get_VB_type(xgifb_info);
	}

	hw_info->ujVBChipID = VB_CHIP_UNKNOWN;

	hw_info->ulExternalChip = 0;

	switch (xgifb_info->hasVB) {
	case HASVB_301:
		reg = xgifb_reg_get(XGIPART4, 0x01);
		if (reg >= 0xE0) {
			hw_info->ujVBChipID = VB_CHIP_302LV;
			dev_info(&pdev->dev,
				 "XGI302LV bridge detected (revision 0x%02x)\n",
				 reg);
		} else if (reg >= 0xD0) {
			hw_info->ujVBChipID = VB_CHIP_301LV;
			dev_info(&pdev->dev,
				 "XGI301LV bridge detected (revision 0x%02x)\n",
				 reg);
		} else {
			hw_info->ujVBChipID = VB_CHIP_301;
			dev_info(&pdev->dev, "XGI301 bridge detected\n");
		}
		break;
	case HASVB_302:
		reg = xgifb_reg_get(XGIPART4, 0x01);
		if (reg >= 0xE0) {
			hw_info->ujVBChipID = VB_CHIP_302LV;
			dev_info(&pdev->dev,
				 "XGI302LV bridge detected (revision 0x%02x)\n",
				 reg);
		} else if (reg >= 0xD0) {
			hw_info->ujVBChipID = VB_CHIP_301LV;
			dev_info(&pdev->dev,
				 "XGI302LV bridge detected (revision 0x%02x)\n",
				 reg);
		} else if (reg >= 0xB0) {
			reg1 = xgifb_reg_get(XGIPART4, 0x23);

			hw_info->ujVBChipID = VB_CHIP_302B;

		} else {
			hw_info->ujVBChipID = VB_CHIP_302;
			dev_info(&pdev->dev, "XGI302 bridge detected\n");
		}
		break;
	case HASVB_LVDS:
		hw_info->ulExternalChip = 0x1;
		dev_info(&pdev->dev, "LVDS transmitter detected\n");
		break;
	case HASVB_TRUMPION:
		hw_info->ulExternalChip = 0x2;
		dev_info(&pdev->dev, "Trumpion Zurac LVDS scaler detected\n");
		break;
	case HASVB_CHRONTEL:
		hw_info->ulExternalChip = 0x4;
		dev_info(&pdev->dev, "Chrontel TV encoder detected\n");
		break;
	case HASVB_LVDS_CHRONTEL:
		hw_info->ulExternalChip = 0x5;
		dev_info(&pdev->dev,
			 "LVDS transmitter and Chrontel TV encoder detected\n");
		break;
	default:
		dev_info(&pdev->dev, "No or unknown bridge type detected\n");
		break;
	}

	if (xgifb_info->hasVB != HASVB_NONE)
		XGIfb_detect_VB(xgifb_info);
	else if (xgifb_info->chip != XG21)
		xgifb_info->display2 = XGIFB_DISP_NONE;

	if (xgifb_info->display2 == XGIFB_DISP_LCD) {
		if (!enable_dstn) {
			reg = xgifb_reg_get(XGICR, IND_XGI_LCD_PANEL);
			reg &= 0x0f;
			hw_info->ulCRT2LCDType = XGI310paneltype[reg];
		}
	}

	xgifb_info->mode_idx = -1;

	/* FIXME coreboot does not provide sscanf, needed by XGIfb_search_mode */
	/* if (mode)
		XGIfb_search_mode(xgifb_info, mode);
	else */if (vesa != -1)
		XGIfb_search_vesamode(xgifb_info, vesa);

	if (xgifb_info->mode_idx >= 0)
		xgifb_info->mode_idx =
			XGIfb_validate_mode(xgifb_info, xgifb_info->mode_idx);

	if (xgifb_info->mode_idx < 0) {
		if (xgifb_info->display2 == XGIFB_DISP_LCD &&
		    xgifb_info->chip == XG21)
			xgifb_info->mode_idx =
				XGIfb_GetXG21DefaultLVDSModeIdx(xgifb_info);
		else
			if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER))
				xgifb_info->mode_idx = DEFAULT_MODE;
			else
				xgifb_info->mode_idx = DEFAULT_TEXT_MODE;
	}

	if (xgifb_info->mode_idx < 0) {
		dev_err(&pdev->dev, "No supported video mode found\n");
		ret = -22;
		goto error_1;
	}

	/* set default refresh rate */
	xgifb_info->refresh_rate = refresh_rate;
	if (xgifb_info->refresh_rate == 0)
		xgifb_info->refresh_rate = 60;
	if (XGIfb_search_refresh_rate(xgifb_info,
			xgifb_info->refresh_rate) == 0) {
		xgifb_info->rate_idx = 1;
		xgifb_info->refresh_rate = 60;
	}

	xgifb_info->video_bpp = XGIbios_mode[xgifb_info->mode_idx].bpp;
	xgifb_info->video_vwidth =
		xgifb_info->video_width =
			XGIbios_mode[xgifb_info->mode_idx].xres;
	xgifb_info->video_vheight =
		xgifb_info->video_height =
			XGIbios_mode[xgifb_info->mode_idx].yres;
	xgifb_info->org_x = xgifb_info->org_y = 0;
	xgifb_info->video_linelength =
		xgifb_info->video_width *
		(xgifb_info->video_bpp >> 3);
	switch (xgifb_info->video_bpp) {
	case 8:
		xgifb_info->DstColor = 0x0000;
		xgifb_info->XGI310_AccelDepth = 0x00000000;
		xgifb_info->video_cmap_len = 256;
		break;
	case 16:
		xgifb_info->DstColor = 0x8000;
		xgifb_info->XGI310_AccelDepth = 0x00010000;
		xgifb_info->video_cmap_len = 16;
		break;
	case 32:
		xgifb_info->DstColor = 0xC000;
		xgifb_info->XGI310_AccelDepth = 0x00020000;
		xgifb_info->video_cmap_len = 16;
		break;
	default:
		xgifb_info->video_cmap_len = 16;
		pr_info("Unsupported depth %d\n",
		       xgifb_info->video_bpp);
		break;
	}

	pr_info("Default mode is %dx%dx%d (%dHz)\n",
	       xgifb_info->video_width,
	       xgifb_info->video_height,
	       xgifb_info->video_bpp,
	       xgifb_info->refresh_rate);

	return 0;

error_1:
error_disable:
	return ret;
}

int xgifb_modeset(struct pci_dev *pdev, struct xgifb_video_info *xgifb_info)
{
	struct xgi_hw_device_info *hw_info;

	hw_info = &xgifb_info->hw_info;

	if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER)) {
		/* Set mode */
		XGIfb_pre_setmode(xgifb_info);
		if (XGISetModeNew(xgifb_info, hw_info,
				XGIbios_mode[xgifb_info->mode_idx].mode_no)
					== 0) {
			pr_err("Setting mode[0x%x] failed\n",
			XGIbios_mode[xgifb_info->mode_idx].mode_no);
			return -22;
		}
		xgifb_info->video_linelength =
				xgifb_info->video_width *
				(xgifb_info->video_bpp >> 3);

		xgifb_reg_set(XGISR, IND_SIS_PASSWORD, SIS_PASSWORD);

		xgifb_reg_set(XGICR, 0x13,
			(xgifb_info->video_linelength & 0x00ff));
		xgifb_reg_set(XGISR, 0x0e,
			(xgifb_info->video_linelength & 0xff00) >> 8);

		XGIfb_post_setmode(xgifb_info);

		pr_debug("Set new mode: %dx%dx%d-%d\n",
			XGIbios_mode[xgifb_info->mode_idx].xres,
			XGIbios_mode[xgifb_info->mode_idx].yres,
			XGIbios_mode[xgifb_info->mode_idx].bpp,
			xgifb_info->refresh_rate);

		/* Set LinuxBIOS framebuffer information */
		xgi_vbe_valid = 1;
		xgi_fb.physical_address = xgifb_info->video_base;
		xgi_fb.x_resolution = xgifb_info->video_width;
		xgi_fb.y_resolution = xgifb_info->video_height;
		xgi_fb.bytes_per_line =
			xgifb_info->video_width * xgifb_info->video_bpp;
		xgi_fb.bits_per_pixel = xgifb_info->video_bpp;

		xgi_fb.reserved_mask_pos = 0;
		xgi_fb.reserved_mask_size = 0;
		switch(xgifb_info->video_bpp){
		case 32:
		case 24:
			/* packed into 4-byte words */
			xgi_fb.reserved_mask_pos = 24;
			xgi_fb.reserved_mask_size = 8;
			xgi_fb.red_mask_pos = 16;
			xgi_fb.red_mask_size = 8;
			xgi_fb.green_mask_pos = 8;
			xgi_fb.green_mask_size = 8;
			xgi_fb.blue_mask_pos = 0;
			xgi_fb.blue_mask_size = 8;
			break;
		case 16:
			/* packed into 2-byte words */
			xgi_fb.red_mask_pos = 11;
			xgi_fb.red_mask_size = 5;
			xgi_fb.green_mask_pos = 5;
			xgi_fb.green_mask_size = 6;
			xgi_fb.blue_mask_pos = 0;
			xgi_fb.blue_mask_size = 5;
			break;
		default:
			printk(BIOS_SPEW, "%s: unsupported BPP %d\n", __func__,
			       xgifb_info->video_bpp);
			xgi_vbe_valid = 0;
		}
	} else {
		/*
		 * FIXME
		 * Text mode is slightly unstable/jittery
		 * (bad/incomplete DDR init?)
		 */

		/* Initialize standard VGA text mode */
		vga_io_init();
		vga_textmode_init();
		printk(BIOS_INFO, "XGI VGA text mode initialized\n");

		/* if we don't have console, at least print something... */
		vga_line_write(0, "XGI VGA text mode initialized");
	}

	return 0;
}

static int vbe_mode_info_valid(void)
{
	return xgi_vbe_valid;
}

int fill_lb_framebuffer(struct lb_framebuffer *framebuffer)
{
	if (!vbe_mode_info_valid())
		return -1;

	*framebuffer = xgi_fb;

	return 0;
}

struct xgifb_video_info *xgifb_video_info_ptr;

struct xgifb_video_info *pci_get_drvdata(struct pci_dev *pdev) {
	return xgifb_video_info_ptr;
}

void pci_set_drvdata(struct pci_dev *pdev, struct xgifb_video_info *data) {
	xgifb_video_info_ptr = data;
}
