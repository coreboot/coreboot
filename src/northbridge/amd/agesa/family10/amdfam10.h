/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AMDFAM10_H
#define AMDFAM10_H

#include <cpu/x86/msr.h>

#define HWCR_MSR                        0xC0010015
#define NB_CFG_MSR                      0xC001001f
#define LS_CFG_MSR                      0xC0011020
#define IC_CFG_MSR                      0xC0011021
#define DC_CFG_MSR                      0xC0011022
#define BU_CFG_MSR                      0xC0011023
#define BU_CFG2_MSR                     0xC001102A

#define CPU_ID_FEATURES_MSR		0xC0011004
#define CPU_ID_EXT_FEATURES_MSR		0xC0011005

/* Definitions of various FAM10 registers */
/* Function 0 */
#define HT_TRANSACTION_CONTROL 0x68
#define  HTTC_DIS_RD_B_P		(1 << 0)
#define  HTTC_DIS_RD_DW_P		(1 << 1)
#define  HTTC_DIS_WR_B_P		(1 << 2)
#define  HTTC_DIS_WR_DW_P		(1 << 3)
#define  HTTC_DIS_MTS			(1 << 4)
#define  HTTC_CPU1_EN			(1 << 5)
#define  HTTC_CPU_REQ_PASS_PW		(1 << 6)
#define  HTTC_CPU_RD_RSP_PASS_PW	(1 << 7)
#define  HTTC_DIS_P_MEM_C		(1 << 8)
#define  HTTC_DIS_RMT_MEM_C		(1 << 9)
#define  HTTC_DIS_FILL_P		(1 << 10)
#define  HTTC_RSP_PASS_PW		(1 << 11)
#define  HTTC_BUF_REL_PRI_SHIFT	13
#define  HTTC_BUF_REL_PRI_MASK		3
#define   HTTC_BUF_REL_PRI_64		0
#define   HTTC_BUF_REL_PRI_16		1
#define   HTTC_BUF_REL_PRI_8		2
#define   HTTC_BUF_REL_PRI_2		3
#define  HTTC_LIMIT_CLDT_CFG		(1 << 15)
#define  HTTC_LINT_EN			(1 << 16)
#define  HTTC_APIC_EXT_BRD_CST		(1 << 17)
#define  HTTC_APIC_EXT_ID		(1 << 18)
#define  HTTC_APIC_EXT_SPUR		(1 << 19)
#define  HTTC_SEQ_ID_SRC_NODE_EN	(1 << 20)
#define  HTTC_DS_NP_REQ_LIMIT_SHIFT	21
#define  HTTC_DS_NP_REQ_LIMIT_MASK	3
#define   HTTC_DS_NP_REQ_LIMIT_NONE	0
#define   HTTC_DS_NP_REQ_LIMIT_1	1
#define   HTTC_DS_NP_REQ_LIMIT_4	2
#define   HTTC_DS_NP_REQ_LIMIT_8	3

/* Function 1 */

/* Function 2 */

/* Function 3 */


/* Function 5 for FBDIMM */
#define LinkConnected		(1 << 0)
#define InitComplete		(1 << 1)
#define NonCoherent		(1 << 2)
#define ConnectionPending	(1 << 4)

#if CONFIG_MAX_PHYSICAL_CPUS > 8
	#if CONFIG_MAX_PHYSICAL_CPUS > 32
		#define NODE_NUMS 64
	#else
		#define NODE_NUMS 32
	#endif
#else
	#define NODE_NUMS 8
#endif

#ifdef __PRE_RAM__
#if NODE_NUMS==64
	 #define NODE_PCI(x, fn) ((x<32)?(PCI_DEV(CONFIG_CBB,(CONFIG_CDB+x),fn)):(PCI_DEV((CONFIG_CBB-1),(CONFIG_CDB+x-32),fn)))
#else
	 #define NODE_PCI(x, fn) PCI_DEV(CONFIG_CBB,(CONFIG_CDB+x),fn)
#endif
#endif

#endif /* AMDFAM10_H */
