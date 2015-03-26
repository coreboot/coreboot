/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/* Portions marked below taken from XGI/SiS Linux kernel drivers */

#ifndef _XGI_COREBOOT_
#define _XGI_COREBOOT_

#include <delay.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arch/io.h>

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include "initdef.h"

/* Begin code taken from Linux kernel 3.18.5 */

/* For 315/Xabre series */
#define COMMAND_QUEUE_AREA_SIZE	(512 * 1024)	/* 512K */
#define COMMAND_QUEUE_AREA_SIZE_Z7 (128 * 1024)	/* 128k for XGI Z7 */
#define HW_CURSOR_AREA_SIZE_315	16384		/* 16K */
#define COMMAND_QUEUE_THRESHOLD	0x1F

#define SIS_OH_ALLOC_SIZE	4000
#define SENTINEL		0x7fffffff

#define SEQ_ADR			0x14
#define SEQ_DATA		0x15
#define DAC_ADR			0x18
#define DAC_DATA		0x19
#define CRTC_ADR		0x24
#define CRTC_DATA		0x25
#define DAC2_ADR		(0x16-0x30)
#define DAC2_DATA		(0x17-0x30)
#define VB_PART1_ADR		(0x04-0x30)
#define VB_PART1_DATA		(0x05-0x30)
#define VB_PART2_ADR		(0x10-0x30)
#define VB_PART2_DATA		(0x11-0x30)
#define VB_PART3_ADR		(0x12-0x30)
#define VB_PART3_DATA		(0x13-0x30)
#define VB_PART4_ADR		(0x14-0x30)
#define VB_PART4_DATA		(0x15-0x30)

#define SISSR			ivideo->SiS_Pr.SiS_P3c4
#define SISCR			ivideo->SiS_Pr.SiS_P3d4
#define SISDACA			ivideo->SiS_Pr.SiS_P3c8
#define SISDACD			ivideo->SiS_Pr.SiS_P3c9
#define SISPART1		ivideo->SiS_Pr.SiS_Part1Port
#define SISPART2		ivideo->SiS_Pr.SiS_Part2Port
#define SISPART3		ivideo->SiS_Pr.SiS_Part3Port
#define SISPART4		ivideo->SiS_Pr.SiS_Part4Port
#define SISPART5		ivideo->SiS_Pr.SiS_Part5Port
#define SISDAC2A		SISPART5
#define SISDAC2D		(SISPART5 + 1)
#define SISMISCR		(ivideo->SiS_Pr.RelIO + 0x1c)
#define SISMISCW		ivideo->SiS_Pr.SiS_P3c2
#define SISINPSTAT		(ivideo->SiS_Pr.RelIO + 0x2a)
#define SISPEL			ivideo->SiS_Pr.SiS_P3c6
#define SISVGAENABLE		(ivideo->SiS_Pr.RelIO + 0x13)
#define SISVID			(ivideo->SiS_Pr.RelIO + 0x02 - 0x30)
#define SISCAP			(ivideo->SiS_Pr.RelIO + 0x00 - 0x30)

#define IND_SIS_PASSWORD		0x05  /* SRs */
#define IND_SIS_COLOR_MODE		0x06
#define IND_SIS_RAMDAC_CONTROL		0x07
#define IND_SIS_DRAM_SIZE		0x14
#define IND_SIS_MODULE_ENABLE		0x1E
#define IND_SIS_PCI_ADDRESS_SET		0x20
#define IND_SIS_TURBOQUEUE_ADR		0x26
#define IND_SIS_TURBOQUEUE_SET		0x27
#define IND_SIS_POWER_ON_TRAP		0x38
#define IND_SIS_POWER_ON_TRAP2		0x39
#define IND_SIS_CMDQUEUE_SET		0x26
#define IND_SIS_CMDQUEUE_THRESHOLD	0x27

#define IND_SIS_AGP_IO_PAD	0x48

#define SIS_CRT2_WENABLE_300	0x24  /* Part1 */
#define SIS_CRT2_WENABLE_315	0x2F

#define SIS_PASSWORD		0x86  /* SR05 */

#define SIS_INTERLACED_MODE	0x20  /* SR06 */
#define SIS_8BPP_COLOR_MODE	0x0
#define SIS_15BPP_COLOR_MODE	0x1
#define SIS_16BPP_COLOR_MODE	0x2
#define SIS_32BPP_COLOR_MODE	0x4

#define SIS_ENABLE_2D		0x40  /* SR1E */

#define SIS_MEM_MAP_IO_ENABLE	0x01  /* SR20 */
#define SIS_PCI_ADDR_ENABLE	0x80

#define SIS_AGP_CMDQUEUE_ENABLE		0x80  /* 315/330/340 series SR26 */
#define SIS_VRAM_CMDQUEUE_ENABLE	0x40
#define SIS_MMIO_CMD_ENABLE		0x20
#define SIS_CMD_QUEUE_SIZE_512k		0x00
#define SIS_CMD_QUEUE_SIZE_1M		0x04
#define SIS_CMD_QUEUE_SIZE_2M		0x08
#define SIS_CMD_QUEUE_SIZE_4M		0x0C
#define SIS_CMD_QUEUE_RESET		0x01
#define SIS_CMD_AUTO_CORR		0x02

#define SIS_CMD_QUEUE_SIZE_Z7_64k	0x00 /* XGI Z7 */
#define SIS_CMD_QUEUE_SIZE_Z7_128k	0x04

#define SIS_SIMULTANEOUS_VIEW_ENABLE	0x01  /* CR30 */
#define SIS_MODE_SELECT_CRT2		0x02
#define SIS_VB_OUTPUT_COMPOSITE		0x04
#define SIS_VB_OUTPUT_SVIDEO		0x08
#define SIS_VB_OUTPUT_SCART		0x10
#define SIS_VB_OUTPUT_LCD		0x20
#define SIS_VB_OUTPUT_CRT2		0x40
#define SIS_VB_OUTPUT_HIVISION		0x80

#define SIS_VB_OUTPUT_DISABLE	0x20  /* CR31 */
#define SIS_DRIVER_MODE		0x40

#define SIS_VB_COMPOSITE	0x01  /* CR32 */
#define SIS_VB_SVIDEO		0x02
#define SIS_VB_SCART		0x04
#define SIS_VB_LCD		0x08
#define SIS_VB_CRT2		0x10
#define SIS_CRT1		0x20
#define SIS_VB_HIVISION		0x40
#define SIS_VB_YPBPR		0x80
#define SIS_VB_TV		(SIS_VB_COMPOSITE | SIS_VB_SVIDEO | \
				SIS_VB_SCART | SIS_VB_HIVISION | SIS_VB_YPBPR)

#define SIS_EXTERNAL_CHIP_MASK			0x0E  /* CR37 (< SiS 660) */
#define SIS_EXTERNAL_CHIP_SIS301		0x01  /* in CR37 << 1 ! */
#define SIS_EXTERNAL_CHIP_LVDS			0x02
#define SIS_EXTERNAL_CHIP_TRUMPION		0x03
#define SIS_EXTERNAL_CHIP_LVDS_CHRONTEL		0x04
#define SIS_EXTERNAL_CHIP_CHRONTEL		0x05
#define SIS310_EXTERNAL_CHIP_LVDS		0x02
#define SIS310_EXTERNAL_CHIP_LVDS_CHRONTEL	0x03

#define SIS_AGP_2X		0x20  /* CR48 */

/* vbflags, private entries (others in sisfb.h) */
#define VB_CONEXANT		0x00000800	/* 661 series only */
#define VB_TRUMPION		VB_CONEXANT	/* 300 series only */
#define VB_302ELV		0x00004000
#define VB_301			0x00100000	/* Video bridge type */
#define VB_301B			0x00200000
#define VB_302B			0x00400000
#define VB_30xBDH		0x00800000	/* 30xB DH version (w/o LCD support) */
#define VB_LVDS			0x01000000
#define VB_CHRONTEL		0x02000000
#define VB_301LV		0x04000000
#define VB_302LV		0x08000000
#define VB_301C			0x10000000

#define VB_SISBRIDGE		(VB_301|VB_301B|VB_301C|VB_302B|VB_301LV|VB_302LV|VB_302ELV)
#define VB_VIDEOBRIDGE		(VB_SISBRIDGE | VB_LVDS | VB_CHRONTEL | VB_CONEXANT)

enum _SIS_LCD_TYPE {
    LCD_INVALID = 0,
    LCD_800x600,
    LCD_1024x768,
    LCD_1280x1024,
    LCD_1280x960,
    LCD_640x480,
    LCD_1600x1200,
    LCD_1920x1440,
    LCD_2048x1536,
    LCD_320x240,	/* FSTN */
    LCD_1400x1050,
    LCD_1152x864,
    LCD_1152x768,
    LCD_1280x768,
    LCD_1024x600,
    LCD_320x240_2,	/* DSTN */
    LCD_320x240_3,	/* DSTN */
    LCD_848x480,
    LCD_1280x800,
    LCD_1680x1050,
    LCD_1280x720,
    LCD_1280x854,
    LCD_CUSTOM,
    LCD_UNKNOWN
};

/* End code taken from Linux kernel 3.18.5 */

#define DEFAULT_TEXT_MODE         16 /* index for 800x600x8 */

/* coreboot <--> kernel code interface */
#define __iomem
#define SISIOMEMTYPE
typedef unsigned long SISIOADDRESS;
typedef u64 phys_addr_t;
#define pci_dev device

#define SZ_16M 0x01000000

#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1 : __min2; })

#define dev_info(dev, format, arg...) printk(BIOS_INFO, "XGI VGA: " format, ##arg)
#define dev_dbg(dev, format, arg...) printk(BIOS_DEBUG, "XGI VGA: " format, ##arg)
#define dev_err(dev, format, arg...) printk(BIOS_ERR, "XGI VGA: " format, ##arg)

#define pr_info(format, arg...) printk(BIOS_INFO, "XGI VGA: " format, ##arg)
#define pr_debug(format, arg...) printk(BIOS_INFO, "XGI VGA: " format, ##arg)
#define pr_err(format, arg...) printk(BIOS_ERR, "XGI VGA: " format, ##arg)

static inline void writel(u32 val, volatile void *addr) {
	*(u32*)addr = val;
}

static inline u32 readl(const volatile void *addr) {
	return *(u32*)addr;
}

static inline int pci_read_config_dword(struct pci_dev *dev, int where,
	u32 *val)
{
	*val = pci_read_config32(dev, where);
	return 0;
}

static inline int pci_read_config_byte(struct pci_dev *dev, int where,
	u8 *val)
{
	*val = pci_read_config8(dev, where);
	return 0;
}

static inline struct resource* resource_at_bar(struct pci_dev *dev, u8 bar) {
	struct resource *res = dev->resource_list;
	int i;
	for (i = 0; i < bar; i++) {
		res = res->next;
		if (res == NULL)
			return NULL;
	}

	return res;
}

static inline resource_t pci_resource_len(struct pci_dev *dev, u8 bar) {
	struct resource *res = resource_at_bar(dev, bar);
	if (res)
		return res->size;
	else
		return 0;
}

static inline resource_t pci_resource_start(struct pci_dev *dev, u8 bar) {
	struct resource *res = resource_at_bar(dev, bar);
	if (res)
		return res->base;
	else
		return 0;
}

struct xgifb_video_info *pci_get_drvdata(struct pci_dev *pdev);
void pci_set_drvdata(struct pci_dev *pdev, struct xgifb_video_info *data);

int xgifb_probe(struct pci_dev *pdev, struct xgifb_video_info *xgifb_info);
int xgifb_modeset(struct pci_dev *pdev, struct xgifb_video_info *xgifb_info);

#endif