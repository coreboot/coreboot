/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* File taken from the Linux xgifb driver (v3.18.5) */

#ifndef _VBUTIL_
#define _VBUTIL_
extern void xgifb_reg_set(unsigned long, u8, u8);
extern u8 xgifb_reg_get(unsigned long, u8);
extern void xgifb_reg_or(unsigned long, u8, unsigned int);
extern void xgifb_reg_and(unsigned long, u8, unsigned int);
extern void xgifb_reg_and_or(unsigned long, u8, unsigned int, unsigned int);
#endif
