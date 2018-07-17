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
 */

#ifndef _ASPEED_COREBOOT_
#define _ASPEED_COREBOOT_

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

/* coreboot <--> kernel code interface */
#define __iomem
typedef u64 phys_addr_t;
#define pci_dev device

#define SZ_16M 0x01000000

#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1 : __min2; })

#define dev_info(dev, format, arg...) printk(BIOS_INFO, "%s: " format, __func__, ##arg)
#define dev_dbg(dev, format, arg...) printk(BIOS_DEBUG, "%s: " format, __func__, ##arg)
#define dev_err(dev, format, arg...) printk(BIOS_ERR, "%s: " format, __func__, ##arg)

#define pr_info(format, arg...) printk(BIOS_INFO, "%s: " format, __func__, ##arg)
#define pr_debug(format, arg...) printk(BIOS_INFO, "%s: " format, __func__, ##arg)
#define pr_err(format, arg...) printk(BIOS_ERR, "%s: " format, __func__, ##arg)

#define DRM_INFO pr_info

#define GFP_KERNEL 0
#define GFP_ATOMIC 1
#define kfree(address) free(address)

#define EIO 5
#define ENOMEM 12

struct firmware {
	size_t size;
	const u8 *data;
	struct page **pages;

	/* firmware loader private fields */
	void *priv;
};

struct drm_device {
	struct pci_dev *pdev;
	void *dev_private;
};

static inline void *kzalloc(size_t size, int flags) {
	void *ptr = malloc(size);
	memset(ptr, 0, size);
	return ptr;
}

static inline int pci_read_config_dword(struct pci_dev *dev, int where,
	u32 *val)
{
	*val = pci_read_config32(dev, where);
	return 0;
}

static inline int pci_write_config_dword(struct pci_dev *dev, int where,
	u32 val)
{
	pci_write_config32(dev, where, val);
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

static inline unsigned int ioread32(void __iomem *p) {
	return read32(p);
}

static inline void iowrite32(u32 val, void __iomem *p) {
	write32(p, val);
}

static inline unsigned int ioread16(void __iomem *p) {
	return read16(p);
}

static inline void iowrite16(u16 val, void __iomem *p) {
	write16(p, val);
}

static inline unsigned int ioread8(void __iomem *p) {
	return read8(p);
}

static inline void iowrite8(u8 val, void __iomem *p) {
	write8(p, val);
}

static inline unsigned int ioread_cbio32(void __iomem *p) {
	return inl((uint16_t)((intptr_t)p));
}

static inline void iowrite_cbio32(u32 val, void __iomem *p) {
	outl(val, (uint16_t)((intptr_t)p));
}

static inline unsigned int ioread_cbio16(void __iomem *p) {
	return inw((uint16_t)((intptr_t)p));
}

static inline void iowrite_cbio16(u16 val, void __iomem *p) {
	outw(val, (uint16_t)((intptr_t)p));
}

static inline unsigned int ioread_cbio8(void __iomem *p) {
	return inb((uint16_t)((intptr_t)p));
}

static inline void iowrite_cbio8(u8 val, void __iomem *p) {
	outb(val, (uint16_t)((intptr_t)p));
}

static inline void msleep(unsigned int msecs) {
	udelay(msecs * 1000);
}

#endif
