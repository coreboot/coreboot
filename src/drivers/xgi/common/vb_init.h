/*
 * This file is part of the coreboot project.
 *
 * File taken verbatim from the Linux xgifb driver (v3.18.5)
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
 * Foundation, Inc.
 */

#ifndef _VBINIT_
#define _VBINIT_
extern unsigned char XGIInitNew(struct pci_dev *pdev);
extern void XGIRegInit(struct vb_device_info *, unsigned long);
#endif
