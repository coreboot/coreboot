/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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


#if (CONFIG_DIMM_SUPPORT & 0x000F)!=0x0005 /* not needed for AMD_FAM10_DDR3 */
static  void print_tx(const char *strval, u32 val)
{
#if CONFIG_DEBUG_RAM_SETUP
	printk(BIOS_DEBUG, "%s%08x\n", strval, val);
#endif
}

static  void print_t(const char *strval)
{
#if CONFIG_DEBUG_RAM_SETUP
	printk(BIOS_DEBUG, "%s", strval);
#endif
}
#endif

static  void print_tf(const char *func, const char *strval)
{
#if CONFIG_DEBUG_RAM_SETUP
	printk(BIOS_DEBUG, "%s: %s", func, strval);
#endif
}

static uint16_t mct_MaxLoadFreq(uint8_t count, uint8_t registered, uint16_t freq)
{
	/* Return limited maximum RAM frequency */
	if (IS_ENABLED(CONFIG_DIMM_DDR2)) {
		if (IS_ENABLED(CONFIG_DIMM_REGISTERED) && registered) {
			/* K10 BKDG Rev. 3.62 Table 53 */
			if (count > 2) {
				/* Limit to DDR2-533 */
				if (freq > 266) {
					freq = 266;
					print_tf(__func__, ": More than 2 registered DIMMs on channel; limiting to DDR2-533\n");
				}
			}
		} else {
			/* K10 BKDG Rev. 3.62 Table 52 */
			if (count > 1) {
				/* Limit to DDR2-800 */
				if (freq > 400) {
					freq = 400;
					print_tf(__func__, ": More than 1 unbuffered DIMM on channel; limiting to DDR2-800\n");
				}
			}
		}
	} else if (IS_ENABLED(CONFIG_DIMM_DDR3)) {
		if (IS_ENABLED(CONFIG_DIMM_REGISTERED) && registered) {
			/* K10 BKDG Rev. 3.62 Table 34 */
			if (count > 2) {
				/* Limit to DDR3-800 */
				if (freq > 400) {
					freq = 400;
					print_tf(__func__, ": More than 2 registered DIMMs on channel; limiting to DDR3-800\n");
				}
			} else if (count == 2) {
				/* Limit to DDR3-1066 */
				if (freq > 533) {
					freq = 533;
					print_tf(__func__, ": 2 registered DIMMs on channel; limiting to DDR3-1066\n");
				}
			} else {
				/* Limit to DDR3-1333 */
				if (freq > 666) {
					freq = 666;
					print_tf(__func__, ": 1 registered DIMM on channel; limiting to DDR3-1333\n");
				}
			}
		} else {
			/* K10 BKDG Rev. 3.62 Table 33 */
			/* Limit to DDR3-1333 */
			if (freq > 666) {
				freq = 666;
				print_tf(__func__, ": unbuffered DIMMs on channel; limiting to DDR3-1333\n");
			}
		}
	}

	return freq;
}

#if (CONFIG_DIMM_SUPPORT & 0x000F)==0x0005 /* AMD_FAM10_DDR3 */
#include "amdfam10.h"
#include "../amdmct/wrappers/mcti.h"
#include "../amdmct/amddefs.h"
#include "../amdmct/mct_ddr3/mwlc_d.h"
#include "../amdmct/mct_ddr3/mct_d.h"
#include "../amdmct/mct_ddr3/mct_d_gcc.h"

#include "../amdmct/wrappers/mcti_d.c"
#include "../amdmct/mct_ddr3/mct_d.c"

#include "../amdmct/mct_ddr3/mctmtr_d.c"
#include "../amdmct/mct_ddr3/mctcsi_d.c"
#include "../amdmct/mct_ddr3/mctecc_d.c"
#include "../amdmct/mct_ddr3/mctdqs_d.c"
#include "../amdmct/mct_ddr3/mctsrc.c"
#include "../amdmct/mct_ddr3/mctsdi.c"
#include "../amdmct/mct_ddr3/mctproc.c"
#include "../amdmct/mct_ddr3/mctprob.c"
#include "../amdmct/mct_ddr3/mcthwl.c"
#include "../amdmct/mct_ddr3/mctwl.c"
#include "../amdmct/mct_ddr3/mport_d.c"
#include "../amdmct/mct_ddr3/mutilc_d.c"
#include "../amdmct/mct_ddr3/modtrdim.c"
#include "../amdmct/mct_ddr3/mhwlc_d.c"
#include "../amdmct/mct_ddr3/mctrci.c"
#include "../amdmct/mct_ddr3/mctsrc1p.c"
#include "../amdmct/mct_ddr3/mcttmrl.c"
#include "../amdmct/mct_ddr3/mcthdi.c"
#include "../amdmct/mct_ddr3/mctndi_d.c"
#include "../amdmct/mct_ddr3/mctchi_d.c"
#include "../amdmct/mct_ddr3/modtrd.c"

#if CONFIG_CPU_SOCKET_TYPE == 0x10
//TODO: S1G1?
#elif CONFIG_CPU_SOCKET_TYPE == 0x11
//AM3
#include "../amdmct/mct_ddr3/mctardk5.c"
#elif CONFIG_CPU_SOCKET_TYPE == 0x12
//F (1207), Fr2, G (1207)
#include "../amdmct/mct_ddr3/mctardk6.c"
#elif CONFIG_CPU_SOCKET_TYPE == 0x13
//ASB2
#include "../amdmct/mct_ddr3/mctardk5.c"
//C32
#elif CONFIG_CPU_SOCKET_TYPE == 0x14
#include "../amdmct/mct_ddr3/mctardk5.c"
//G34
#elif CONFIG_CPU_SOCKET_TYPE == 0x15
#include "../amdmct/mct_ddr3/mctardk5.c"
#endif

#else  /* DDR2 */

#include "amdfam10.h"
#include "../amdmct/wrappers/mcti.h"
#include "../amdmct/amddefs.h"
#include "../amdmct/mct/mct_d.h"
#include "../amdmct/mct/mct_d_gcc.h"

#include "../amdmct/wrappers/mcti_d.c"
#include "../amdmct/mct/mct_d.c"


#include "../amdmct/mct/mctmtr_d.c"
#include "../amdmct/mct/mctcsi_d.c"
#include "../amdmct/mct/mctecc_d.c"
#include "../amdmct/mct/mctpro_d.c"
#include "../amdmct/mct/mctdqs_d.c"
#include "../amdmct/mct/mctsrc.c"
#include "../amdmct/mct/mctsrc1p.c"
#include "../amdmct/mct/mcttmrl.c"
#include "../amdmct/mct/mcthdi.c"
#include "../amdmct/mct/mctndi_d.c"
#include "../amdmct/mct/mctchi_d.c"

#if CONFIG_CPU_SOCKET_TYPE == 0x10
//L1
#include "../amdmct/mct/mctardk3.c"
#elif CONFIG_CPU_SOCKET_TYPE == 0x11
//AM2
#include "../amdmct/mct/mctardk4.c"
//#elif SYSTEM_TYPE == MOBILE
//s1g1
//#include "../amdmct/mct/mctardk5.c"
#endif

#endif	/* DDR2 */

#include <arch/early_variables.h>
struct sys_info sysinfo_car CAR_GLOBAL;

int mctRead_SPD(u32 smaddr, u32 reg)
{
	return spd_read_byte(smaddr, reg);
}


void mctSMBhub_Init(u32 node)
{
	struct sys_info *sysinfo = &sysinfo_car;
	struct mem_controller *ctrl = &( sysinfo->ctrl[node] );
	activate_spd_rom(ctrl);
}


void mctGet_DIMMAddr(struct DCTStatStruc *pDCTstat, u32 node)
{
	int j;
	struct sys_info *sysinfo = &sysinfo_car;
	struct mem_controller *ctrl = &( sysinfo->ctrl[node] );

	for(j=0;j<DIMM_SOCKETS;j++) {
		pDCTstat->DIMMAddr[j*2] = ctrl->spd_addr[j] & 0xff;
		pDCTstat->DIMMAddr[j*2+1] = ctrl->spd_addr[DIMM_SOCKETS + j] & 0xff;
	}

}

static u8 mctGetProcessorPackageType(void) {
	/* FIXME: I guess this belongs wherever mctGetLogicalCPUID ends up ? */
     u32 BrandId = cpuid_ebx(0x80000001);
     return (u8)((BrandId >> 28) & 0x0F);
}

static void raminit_amdmct(struct sys_info *sysinfo)
{
	struct MCTStatStruc *pMCTstat = &(sysinfo->MCTstat);
	struct DCTStatStruc *pDCTstatA = sysinfo->DCTstatA;

	printk(BIOS_DEBUG, "raminit_amdmct begin:\n");

	mctAutoInitMCT_D(pMCTstat, pDCTstatA);

	printk(BIOS_DEBUG, "raminit_amdmct end:\n");
}

static void amdmct_cbmem_store_info(struct sys_info *sysinfo)
{
	if (!sysinfo)
		return;

	/* Save memory info structures for use in ramstage */
	size_t i;
	struct DCTStatStruc *pDCTstatA = NULL;

	/* Allocate memory */
	struct amdmct_memory_info* mem_info;
	mem_info = cbmem_add(CBMEM_ID_AMDMCT_MEMINFO, sizeof(struct amdmct_memory_info));
	if (!mem_info)
		return;

	printk(BIOS_DEBUG, "%s: Storing AMDMCT configuration in CBMEM\n", __func__);

	/* Initialize memory */
	memset(mem_info, 0,  sizeof(struct amdmct_memory_info));

	/* Copy data */
	memcpy(&mem_info->mct_stat, &(sysinfo->MCTstat), sizeof(struct MCTStatStruc));
	for (i = 0; i < MAX_NODES_SUPPORTED; i++) {
		pDCTstatA = sysinfo->DCTstatA + i;
		memcpy(&mem_info->dct_stat[i], pDCTstatA, sizeof(struct DCTStatStruc));
	}
	mem_info->ecc_enabled = mctGet_NVbits(NV_ECC_CAP);
	mem_info->ecc_scrub_rate = mctGet_NVbits(NV_DramBKScrub);

	/* Zero out invalid/unused pointers */
#if IS_ENABLED(CONFIG_DIMM_DDR3)
	for (i = 0; i < MAX_NODES_SUPPORTED; i++) {
		mem_info->dct_stat[i].C_MCTPtr = NULL;
		mem_info->dct_stat[i].C_DCTPtr[0] = NULL;
		mem_info->dct_stat[i].C_DCTPtr[1] = NULL;
	}
#endif
}
