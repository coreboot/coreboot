/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_CPU_H__
#define __PICASSO_CPU_H__

int get_cpu_count(void);
void check_mca(void);

#define RAVEN1_B0_CPUID			0x00810f10
#define PICASSO_B0_CPUID		0x00810f80
#define PICASSO_B1_CPUID		0x00810f81
#define RAVEN2_A0_CPUID			0x00820f00
#define RAVEN2_A1_CPUID			0x00820f01

#define RAVEN1_VBIOS_VID_DID		0x100215dd
#define RAVEN1_VBIOS_REV		0x81
#define PICASSO_VBIOS_VID_DID		0x100215d8
#define PICASSO_VBIOS_REV		0xc1
#define RAVEN2_VBIOS_VID_DID		0x100215dd  /* VID/DID in RV2 VBIOS header */
#define RAVEN2_VBIOS_REV		0xc4

#endif /* __PICASSO_CPU_H__ */
