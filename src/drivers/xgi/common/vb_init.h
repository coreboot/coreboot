/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* File taken from the Linux xgifb driver (v3.18.5) */

#ifndef _VBINIT_
#define _VBINIT_
extern unsigned char XGIInitNew(struct pci_dev *pdev);
extern void XGIRegInit(struct vb_device_info *, unsigned long);
#endif
