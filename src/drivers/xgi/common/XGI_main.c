/*
 * This file is part of the coreboot project.
 *
 * Code taken from the Linux xgifb driver (v3.18.5)
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

/*
 * Select functions taken from the Linux xgifb driver file XGI_main_26.c
 *
 * Original file header:
 * XG20, XG21, XG40, XG42 frame buffer device
 * for Linux kernels  2.5.x, 2.6.x
 * Base on TW's sis fbdev code.
 */


#define Index_CR_GPIO_Reg1 0x48
#define Index_CR_GPIO_Reg3 0x4a

#define GPIOG_EN    (1<<6)
#define GPIOG_READ  (1<<1)

// static char *mode;
static int vesa = -1;
static unsigned int refresh_rate;

/* ---------------- Chip generation dependent routines ---------------- */

/* for XGI 315/550/650/740/330 */

static int XGIfb_get_dram_size(struct xgifb_video_info *xgifb_info)
{

	u8 ChannelNum, tmp;
	u8 reg = 0;

	/* xorg driver sets 32MB * 1 channel */
	if (xgifb_info->chip == XG27)
		xgifb_reg_set(XGISR, IND_SIS_DRAM_SIZE, 0x51);

	reg = xgifb_reg_get(XGISR, IND_SIS_DRAM_SIZE);
	if (!reg)
		return -1;

	switch ((reg & XGI_DRAM_SIZE_MASK) >> 4) {
	case XGI_DRAM_SIZE_1MB:
		xgifb_info->video_size = 0x100000;
		break;
	case XGI_DRAM_SIZE_2MB:
		xgifb_info->video_size = 0x200000;
		break;
	case XGI_DRAM_SIZE_4MB:
		xgifb_info->video_size = 0x400000;
		break;
	case XGI_DRAM_SIZE_8MB:
		xgifb_info->video_size = 0x800000;
		break;
	case XGI_DRAM_SIZE_16MB:
		xgifb_info->video_size = 0x1000000;
		break;
	case XGI_DRAM_SIZE_32MB:
		xgifb_info->video_size = 0x2000000;
		break;
	case XGI_DRAM_SIZE_64MB:
		xgifb_info->video_size = 0x4000000;
		break;
	case XGI_DRAM_SIZE_128MB:
		xgifb_info->video_size = 0x8000000;
		break;
	case XGI_DRAM_SIZE_256MB:
		xgifb_info->video_size = 0x10000000;
		break;
	default:
		return -1;
	}

	tmp = (reg & 0x0c) >> 2;
	switch (xgifb_info->chip) {
	case XG20:
	case XG21:
	case XG27:
		ChannelNum = 1;
		break;

	case XG42:
		if (reg & 0x04)
			ChannelNum = 2;
		else
			ChannelNum = 1;
		break;

	case XG40:
	default:
		if (tmp == 2)
			ChannelNum = 2;
		else if (tmp == 3)
			ChannelNum = 3;
		else
			ChannelNum = 1;
		break;
	}

	xgifb_info->video_size = xgifb_info->video_size * ChannelNum;

	pr_info("SR14=%x DramSize %x ChannelNum %x\n",
	       reg,
	       xgifb_info->video_size, ChannelNum);
	return 0;

}

void XGIRegInit(struct vb_device_info *XGI_Pr, unsigned long BaseAddr)
{
	XGI_Pr->P3c4 = BaseAddr + 0x14;
	XGI_Pr->P3d4 = BaseAddr + 0x24;
	XGI_Pr->P3c0 = BaseAddr + 0x10;
	XGI_Pr->P3ce = BaseAddr + 0x1e;
	XGI_Pr->P3c2 = BaseAddr + 0x12;
	XGI_Pr->P3cc = BaseAddr + 0x1c;
	XGI_Pr->P3ca = BaseAddr + 0x1a;
	XGI_Pr->P3c6 = BaseAddr + 0x16;
	XGI_Pr->P3c7 = BaseAddr + 0x17;
	XGI_Pr->P3c8 = BaseAddr + 0x18;
	XGI_Pr->P3c9 = BaseAddr + 0x19;
	XGI_Pr->P3da = BaseAddr + 0x2A;
	XGI_Pr->Part0Port = BaseAddr + XGI_CRT2_PORT_00;
	/* Digital video interface registers (LCD) */
	XGI_Pr->Part1Port = BaseAddr + SIS_CRT2_PORT_04;
	/* 301 TV Encoder registers */
	XGI_Pr->Part2Port = BaseAddr + SIS_CRT2_PORT_10;
	/* 301 Macrovision registers */
	XGI_Pr->Part3Port = BaseAddr + SIS_CRT2_PORT_12;
	/* 301 VGA2 (and LCD) registers */
	XGI_Pr->Part4Port = BaseAddr + SIS_CRT2_PORT_14;
	/* 301 palette address port registers */
	XGI_Pr->Part5Port = BaseAddr + SIS_CRT2_PORT_14 + 2;

}

/* ------------------ Internal helper routines ----------------- */

static int XGIfb_GetXG21DefaultLVDSModeIdx(struct xgifb_video_info *xgifb_info)
{
	int i = 0;

	while ((XGIbios_mode[i].mode_no != 0)
	       && (XGIbios_mode[i].xres <= xgifb_info->lvds_data.LVDSHDE)) {
		if ((XGIbios_mode[i].xres == xgifb_info->lvds_data.LVDSHDE)
		    && (XGIbios_mode[i].yres == xgifb_info->lvds_data.LVDSVDE)
		    && (XGIbios_mode[i].bpp == 8)) {
			return i;
		}
		i++;
	}

	return -1;
}

static u8 XGIfb_search_refresh_rate(struct xgifb_video_info *xgifb_info,
				    unsigned int rate)
{
	u16 xres, yres;
	int i = 0;

	xres = XGIbios_mode[xgifb_info->mode_idx].xres;
	yres = XGIbios_mode[xgifb_info->mode_idx].yres;

	xgifb_info->rate_idx = 0;
	while ((XGIfb_vrate[i].idx != 0) && (XGIfb_vrate[i].xres <= xres)) {
		if ((XGIfb_vrate[i].xres == xres) &&
		    (XGIfb_vrate[i].yres == yres)) {
			if (XGIfb_vrate[i].refresh == rate) {
				xgifb_info->rate_idx = XGIfb_vrate[i].idx;
				break;
			} else if (XGIfb_vrate[i].refresh > rate) {
				if ((XGIfb_vrate[i].refresh - rate) <= 3) {
					pr_debug("Adjusting rate from %d up to %d\n",
						 rate, XGIfb_vrate[i].refresh);
					xgifb_info->rate_idx =
						XGIfb_vrate[i].idx;
					xgifb_info->refresh_rate =
						XGIfb_vrate[i].refresh;
				} else if (((rate - XGIfb_vrate[i - 1].refresh)
						<= 2) && (XGIfb_vrate[i].idx
						!= 1)) {
					pr_debug("Adjusting rate from %d down to %d\n",
						 rate,
						 XGIfb_vrate[i-1].refresh);
					xgifb_info->rate_idx =
						XGIfb_vrate[i - 1].idx;
					xgifb_info->refresh_rate =
						XGIfb_vrate[i - 1].refresh;
				}
				break;
			} else if ((rate - XGIfb_vrate[i].refresh) <= 2) {
				pr_debug("Adjusting rate from %d down to %d\n",
					 rate, XGIfb_vrate[i].refresh);
				xgifb_info->rate_idx = XGIfb_vrate[i].idx;
				break;
			}
		}
		i++;
	}
	if (xgifb_info->rate_idx > 0)
		return xgifb_info->rate_idx;
	pr_info("Unsupported rate %d for %dx%d\n",
		rate, xres, yres);
	return 0;
}

static void XGIfb_detect_VB(struct xgifb_video_info *xgifb_info)
{
	u8 cr32, temp = 0;

	xgifb_info->TV_plug = xgifb_info->TV_type = 0;

	cr32 = xgifb_reg_get(XGICR, IND_XGI_SCRATCH_REG_CR32);

	if ((cr32 & SIS_CRT1) && !XGIfb_crt1off)
		XGIfb_crt1off = 0;
	else {
		if (cr32 & 0x5F)
			XGIfb_crt1off = 1;
		else
			XGIfb_crt1off = 0;
	}

	if (!xgifb_info->display2_force) {
		if (cr32 & SIS_VB_TV)
			xgifb_info->display2 = XGIFB_DISP_TV;
		else if (cr32 & SIS_VB_LCD)
			xgifb_info->display2 = XGIFB_DISP_LCD;
		else if (cr32 & SIS_VB_CRT2)
			xgifb_info->display2 = XGIFB_DISP_CRT;
		else
			xgifb_info->display2 = XGIFB_DISP_NONE;
	}

	if (XGIfb_tvplug != -1)
		/* Override with option */
		xgifb_info->TV_plug = XGIfb_tvplug;
	else if (cr32 & SIS_VB_HIVISION) {
		xgifb_info->TV_type = TVMODE_HIVISION;
		xgifb_info->TV_plug = TVPLUG_SVIDEO;
	} else if (cr32 & SIS_VB_SVIDEO)
		xgifb_info->TV_plug = TVPLUG_SVIDEO;
	else if (cr32 & SIS_VB_COMPOSITE)
		xgifb_info->TV_plug = TVPLUG_COMPOSITE;
	else if (cr32 & SIS_VB_SCART)
		xgifb_info->TV_plug = TVPLUG_SCART;

	if (xgifb_info->TV_type == 0) {
		temp = xgifb_reg_get(XGICR, 0x38);
		if (temp & 0x10)
			xgifb_info->TV_type = TVMODE_PAL;
		else
			xgifb_info->TV_type = TVMODE_NTSC;
	}

	/* Copy forceCRT1 option to CRT1off if option is given */
	if (XGIfb_forcecrt1 != -1) {
		if (XGIfb_forcecrt1)
			XGIfb_crt1off = 0;
		else
			XGIfb_crt1off = 1;
	}
}

static int XGIfb_has_VB(struct xgifb_video_info *xgifb_info)
{
	u8 vb_chipid;

	vb_chipid = xgifb_reg_get(XGIPART4, 0x00);
	switch (vb_chipid) {
	case 0x01:
		xgifb_info->hasVB = HASVB_301;
		break;
	case 0x02:
		xgifb_info->hasVB = HASVB_302;
		break;
	default:
		xgifb_info->hasVB = HASVB_NONE;
		return 0;
	}
	return 1;
}

static void XGIfb_get_VB_type(struct xgifb_video_info *xgifb_info)
{
	u8 reg;

	if (!XGIfb_has_VB(xgifb_info)) {
		reg = xgifb_reg_get(XGICR, IND_XGI_SCRATCH_REG_CR37);
		switch ((reg & SIS_EXTERNAL_CHIP_MASK) >> 1) {
		case SIS_EXTERNAL_CHIP_LVDS:
			xgifb_info->hasVB = HASVB_LVDS;
			break;
		case SIS_EXTERNAL_CHIP_LVDS_CHRONTEL:
			xgifb_info->hasVB = HASVB_LVDS_CHRONTEL;
			break;
		default:
			break;
		}
	}
}

#if 0
static void XGIfb_search_mode(struct xgifb_video_info *xgifb_info,
			      const char *name)
{
	unsigned int xres;
	unsigned int yres;
	unsigned int bpp;
	int i;

	if (sscanf(name, "%ux%ux%u", &xres, &yres, &bpp) != 3)
		goto invalid_mode;

	if (bpp == 24)
		bpp = 32; /* That's for people who mix up color and fb depth. */

	for (i = 0; XGIbios_mode[i].mode_no != 0; i++)
		if (XGIbios_mode[i].xres == xres &&
		    XGIbios_mode[i].yres == yres &&
		    XGIbios_mode[i].bpp == bpp) {
			xgifb_info->mode_idx = i;
			return;
		}
invalid_mode:
	pr_info("Invalid mode '%s'\n", name);
}
#endif

static void XGIfb_search_vesamode(struct xgifb_video_info *xgifb_info,
				  unsigned int vesamode)
{
	int i = 0;

	if (vesamode == 0)
		goto invalid;

	vesamode &= 0x1dff; /* Clean VESA mode number from other flags */

	while (XGIbios_mode[i].mode_no != 0) {
		if ((XGIbios_mode[i].vesa_mode_no_1 == vesamode) ||
		    (XGIbios_mode[i].vesa_mode_no_2 == vesamode)) {
			xgifb_info->mode_idx = i;
			return;
		}
		i++;
	}

invalid:
	pr_info("Invalid VESA mode 0x%x'\n", vesamode);
}

static int XGIfb_validate_mode(struct xgifb_video_info *xgifb_info, int myindex)
{
	u16 xres, yres;
	struct xgi_hw_device_info *hw_info = &xgifb_info->hw_info;
	unsigned long required_mem;

	if (xgifb_info->chip == XG21) {
		if (xgifb_info->display2 == XGIFB_DISP_LCD) {
			xres = xgifb_info->lvds_data.LVDSHDE;
			yres = xgifb_info->lvds_data.LVDSVDE;
			if (XGIbios_mode[myindex].xres > xres)
				return -1;
			if (XGIbios_mode[myindex].yres > yres)
				return -1;
			if ((XGIbios_mode[myindex].xres < xres) &&
			    (XGIbios_mode[myindex].yres < yres)) {
				if (XGIbios_mode[myindex].bpp > 8)
					return -1;
			}

		}
		goto check_memory;

	}

	/* FIXME: for now, all is valid on XG27 */
	if (xgifb_info->chip == XG27)
		goto check_memory;

	if (!(XGIbios_mode[myindex].chipset & MD_XGI315))
		return -1;

	switch (xgifb_info->display2) {
	case XGIFB_DISP_LCD:
		switch (hw_info->ulCRT2LCDType) {
		case LCD_640x480:
			xres = 640;
			yres = 480;
			break;
		case LCD_800x600:
			xres = 800;
			yres = 600;
			break;
		case LCD_1024x600:
			xres = 1024;
			yres = 600;
			break;
		case LCD_1024x768:
			xres = 1024;
			yres = 768;
			break;
		case LCD_1152x768:
			xres = 1152;
			yres = 768;
			break;
		case LCD_1280x960:
			xres = 1280;
			yres = 960;
			break;
		case LCD_1280x768:
			xres = 1280;
			yres = 768;
			break;
		case LCD_1280x1024:
			xres = 1280;
			yres = 1024;
			break;
		case LCD_1400x1050:
			xres = 1400;
			yres = 1050;
			break;
		case LCD_1600x1200:
			xres = 1600;
			yres = 1200;
			break;
		default:
			xres = 0;
			yres = 0;
			break;
		}
		if (XGIbios_mode[myindex].xres > xres)
			return -1;
		if (XGIbios_mode[myindex].yres > yres)
			return -1;
		if ((hw_info->ulExternalChip == 0x01) || /* LVDS */
		    (hw_info->ulExternalChip == 0x05)) { /* LVDS+Chrontel */
			switch (XGIbios_mode[myindex].xres) {
			case 512:
				if (XGIbios_mode[myindex].yres != 512)
					return -1;
				if (hw_info->ulCRT2LCDType == LCD_1024x600)
					return -1;
				break;
			case 640:
				if ((XGIbios_mode[myindex].yres != 400)
						&& (XGIbios_mode[myindex].yres
								!= 480))
					return -1;
				break;
			case 800:
				if (XGIbios_mode[myindex].yres != 600)
					return -1;
				break;
			case 1024:
				if ((XGIbios_mode[myindex].yres != 600) &&
				    (XGIbios_mode[myindex].yres != 768))
					return -1;
				if ((XGIbios_mode[myindex].yres == 600) &&
				    (hw_info->ulCRT2LCDType != LCD_1024x600))
					return -1;
				break;
			case 1152:
				if ((XGIbios_mode[myindex].yres) != 768)
					return -1;
				if (hw_info->ulCRT2LCDType != LCD_1152x768)
					return -1;
				break;
			case 1280:
				if ((XGIbios_mode[myindex].yres != 768) &&
				    (XGIbios_mode[myindex].yres != 1024))
					return -1;
				if ((XGIbios_mode[myindex].yres == 768) &&
				    (hw_info->ulCRT2LCDType != LCD_1280x768))
					return -1;
				break;
			case 1400:
				if (XGIbios_mode[myindex].yres != 1050)
					return -1;
				break;
			case 1600:
				if (XGIbios_mode[myindex].yres != 1200)
					return -1;
				break;
			default:
				return -1;
			}
		} else {
			switch (XGIbios_mode[myindex].xres) {
			case 512:
				if (XGIbios_mode[myindex].yres != 512)
					return -1;
				break;
			case 640:
				if ((XGIbios_mode[myindex].yres != 400) &&
				    (XGIbios_mode[myindex].yres != 480))
					return -1;
				break;
			case 800:
				if (XGIbios_mode[myindex].yres != 600)
					return -1;
				break;
			case 1024:
				if (XGIbios_mode[myindex].yres != 768)
					return -1;
				break;
			case 1280:
				if ((XGIbios_mode[myindex].yres != 960) &&
				    (XGIbios_mode[myindex].yres != 1024))
					return -1;
				if (XGIbios_mode[myindex].yres == 960) {
					if (hw_info->ulCRT2LCDType ==
					    LCD_1400x1050)
						return -1;
				}
				break;
			case 1400:
				if (XGIbios_mode[myindex].yres != 1050)
					return -1;
				break;
			case 1600:
				if (XGIbios_mode[myindex].yres != 1200)
					return -1;
				break;
			default:
				return -1;
			}
		}
		break;
	case XGIFB_DISP_TV:
		switch (XGIbios_mode[myindex].xres) {
		case 512:
		case 640:
		case 800:
			break;
		case 720:
			if (xgifb_info->TV_type == TVMODE_NTSC) {
				if (XGIbios_mode[myindex].yres != 480)
					return -1;
			} else if (xgifb_info->TV_type == TVMODE_PAL) {
				if (XGIbios_mode[myindex].yres != 576)
					return -1;
			}
			/* LVDS/CHRONTEL does not support 720 */
			if (xgifb_info->hasVB == HASVB_LVDS_CHRONTEL ||
			    xgifb_info->hasVB == HASVB_CHRONTEL) {
				return -1;
			}
			break;
		case 1024:
			if (xgifb_info->TV_type == TVMODE_NTSC) {
				if (XGIbios_mode[myindex].bpp == 32)
					return -1;
			}
			break;
		default:
			return -1;
		}
		break;
	case XGIFB_DISP_CRT:
		if (XGIbios_mode[myindex].xres > 1280)
			return -1;
		break;
	case XGIFB_DISP_NONE:
		break;
	}

check_memory:
	required_mem = XGIbios_mode[myindex].xres * XGIbios_mode[myindex].yres *
		       XGIbios_mode[myindex].bpp / 8;
	if (required_mem > xgifb_info->video_size)
		return -1;
	return myindex;

}

/* --------------------- SetMode routines ------------------------- */

#if IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT)

static void XGIfb_pre_setmode(struct xgifb_video_info *xgifb_info)
{
	u8 cr30 = 0, cr31 = 0;

	cr31 = xgifb_reg_get(XGICR, 0x31);
	cr31 &= ~0x60;

	switch (xgifb_info->display2) {
	case XGIFB_DISP_CRT:
		cr30 = (SIS_VB_OUTPUT_CRT2 | SIS_SIMULTANEOUS_VIEW_ENABLE);
		cr31 |= SIS_DRIVER_MODE;
		break;
	case XGIFB_DISP_LCD:
		cr30 = (SIS_VB_OUTPUT_LCD | SIS_SIMULTANEOUS_VIEW_ENABLE);
		cr31 |= SIS_DRIVER_MODE;
		break;
	case XGIFB_DISP_TV:
		if (xgifb_info->TV_type == TVMODE_HIVISION)
			cr30 = (SIS_VB_OUTPUT_HIVISION
					| SIS_SIMULTANEOUS_VIEW_ENABLE);
		else if (xgifb_info->TV_plug == TVPLUG_SVIDEO)
			cr30 = (SIS_VB_OUTPUT_SVIDEO
					| SIS_SIMULTANEOUS_VIEW_ENABLE);
		else if (xgifb_info->TV_plug == TVPLUG_COMPOSITE)
			cr30 = (SIS_VB_OUTPUT_COMPOSITE
					| SIS_SIMULTANEOUS_VIEW_ENABLE);
		else if (xgifb_info->TV_plug == TVPLUG_SCART)
			cr30 = (SIS_VB_OUTPUT_SCART
					| SIS_SIMULTANEOUS_VIEW_ENABLE);
		cr31 |= SIS_DRIVER_MODE;

		if (XGIfb_tvmode == 1 || xgifb_info->TV_type == TVMODE_PAL)
			cr31 |= 0x01;
		else
			cr31 &= ~0x01;
		break;
	default: /* disable CRT2 */
		cr30 = 0x00;
		cr31 |= (SIS_DRIVER_MODE | SIS_VB_OUTPUT_DISABLE);
	}

	xgifb_reg_set(XGICR, IND_XGI_SCRATCH_REG_CR30, cr30);
	xgifb_reg_set(XGICR, IND_XGI_SCRATCH_REG_CR31, cr31);
	xgifb_reg_set(XGICR, IND_XGI_SCRATCH_REG_CR33,
						(xgifb_info->rate_idx & 0x0F));
}

static void XGIfb_post_setmode(struct xgifb_video_info *xgifb_info)
{
	u8 reg;
	unsigned char doit = 1;

	if (xgifb_info->video_bpp == 8) {
		/*
		 * We can't switch off CRT1 on LVDS/Chrontel
		 * in 8bpp Modes
		 */
		if ((xgifb_info->hasVB == HASVB_LVDS) ||
		    (xgifb_info->hasVB == HASVB_LVDS_CHRONTEL)) {
			doit = 0;
		}
		/*
		 * We can't switch off CRT1 on 301B-DH
		 * in 8bpp Modes if using LCD
		 */
		if (xgifb_info->display2 == XGIFB_DISP_LCD)
			doit = 0;
	}

	/* We can't switch off CRT1 if bridge is in slave mode */
	if (xgifb_info->hasVB != HASVB_NONE) {
		reg = xgifb_reg_get(XGIPART1, 0x00);

		if ((reg & 0x50) == 0x10)
			doit = 0;

	} else {
		XGIfb_crt1off = 0;
	}

	reg = xgifb_reg_get(XGICR, 0x17);
	if ((XGIfb_crt1off) && (doit))
		reg &= ~0x80;
	else
		reg |= 0x80;
	xgifb_reg_set(XGICR, 0x17, reg);

	xgifb_reg_and(XGISR, IND_SIS_RAMDAC_CONTROL, ~0x04);

	if (xgifb_info->display2 == XGIFB_DISP_TV &&
	    xgifb_info->hasVB == HASVB_301) {

		reg = xgifb_reg_get(XGIPART4, 0x01);

		if (reg < 0xB0) { /* Set filter for XGI301 */
			int filter_tb;

			switch (xgifb_info->video_width) {
			case 320:
				filter_tb = (xgifb_info->TV_type ==
					     TVMODE_NTSC) ? 4 : 12;
				break;
			case 640:
				filter_tb = (xgifb_info->TV_type ==
					     TVMODE_NTSC) ? 5 : 13;
				break;
			case 720:
				filter_tb = (xgifb_info->TV_type ==
					     TVMODE_NTSC) ? 6 : 14;
				break;
			case 800:
				filter_tb = (xgifb_info->TV_type ==
					     TVMODE_NTSC) ? 7 : 15;
				break;
			default:
				filter_tb = 0;
				filter = -1;
				break;
			}
			xgifb_reg_or(XGIPART1,
				     SIS_CRT2_WENABLE_315,
				     0x01);

			if (xgifb_info->TV_type == TVMODE_NTSC) {

				xgifb_reg_and(XGIPART2, 0x3a, 0x1f);

				if (xgifb_info->TV_plug == TVPLUG_SVIDEO) {

					xgifb_reg_and(XGIPART2, 0x30, 0xdf);

				} else if (xgifb_info->TV_plug
						== TVPLUG_COMPOSITE) {

					xgifb_reg_or(XGIPART2, 0x30, 0x20);

					switch (xgifb_info->video_width) {
					case 640:
						xgifb_reg_set(XGIPART2,
							      0x35,
							      0xEB);
						xgifb_reg_set(XGIPART2,
							      0x36,
							      0x04);
						xgifb_reg_set(XGIPART2,
							      0x37,
							      0x25);
						xgifb_reg_set(XGIPART2,
							      0x38,
							      0x18);
						break;
					case 720:
						xgifb_reg_set(XGIPART2,
							      0x35,
							      0xEE);
						xgifb_reg_set(XGIPART2,
							      0x36,
							      0x0C);
						xgifb_reg_set(XGIPART2,
							      0x37,
							      0x22);
						xgifb_reg_set(XGIPART2,
							      0x38,
							      0x08);
						break;
					case 800:
						xgifb_reg_set(XGIPART2,
							      0x35,
							      0xEB);
						xgifb_reg_set(XGIPART2,
							      0x36,
							      0x15);
						xgifb_reg_set(XGIPART2,
							      0x37,
							      0x25);
						xgifb_reg_set(XGIPART2,
							      0x38,
							      0xF6);
						break;
					}
				}

			} else if (xgifb_info->TV_type == TVMODE_PAL) {

				xgifb_reg_and(XGIPART2, 0x3A, 0x1F);

				if (xgifb_info->TV_plug == TVPLUG_SVIDEO) {

					xgifb_reg_and(XGIPART2, 0x30, 0xDF);

				} else if (xgifb_info->TV_plug
						== TVPLUG_COMPOSITE) {

					xgifb_reg_or(XGIPART2, 0x30, 0x20);

					switch (xgifb_info->video_width) {
					case 640:
						xgifb_reg_set(XGIPART2,
							      0x35,
							      0xF1);
						xgifb_reg_set(XGIPART2,
							      0x36,
							      0xF7);
						xgifb_reg_set(XGIPART2,
							      0x37,
							      0x1F);
						xgifb_reg_set(XGIPART2,
							      0x38,
							      0x32);
						break;
					case 720:
						xgifb_reg_set(XGIPART2,
							      0x35,
							      0xF3);
						xgifb_reg_set(XGIPART2,
							      0x36,
							      0x00);
						xgifb_reg_set(XGIPART2,
							      0x37,
							      0x1D);
						xgifb_reg_set(XGIPART2,
							      0x38,
							      0x20);
						break;
					case 800:
						xgifb_reg_set(XGIPART2,
							      0x35,
							      0xFC);
						xgifb_reg_set(XGIPART2,
							      0x36,
							      0xFB);
						xgifb_reg_set(XGIPART2,
							      0x37,
							      0x14);
						xgifb_reg_set(XGIPART2,
							      0x38,
							      0x2A);
						break;
					}
				}
			}

			if ((filter >= 0) && (filter <= 7)) {
				pr_debug("FilterTable[%d]-%d: %*ph\n",
					 filter_tb, filter,
					 4, XGI_TV_filter[filter_tb].
						   filter[filter]);
				xgifb_reg_set(
					XGIPART2,
					0x35,
					(XGI_TV_filter[filter_tb].
						filter[filter][0]));
				xgifb_reg_set(
					XGIPART2,
					0x36,
					(XGI_TV_filter[filter_tb].
						filter[filter][1]));
				xgifb_reg_set(
					XGIPART2,
					0x37,
					(XGI_TV_filter[filter_tb].
						filter[filter][2]));
				xgifb_reg_set(
					XGIPART2,
					0x38,
					(XGI_TV_filter[filter_tb].
						filter[filter][3]));
			}
		}
	}
}

#endif