/*
 * This file is part of the coreboot project.
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#if (CONFIG_DIMM_SUPPORT & 0x000F)!=0x0005 /* not needed for AMD_FAM10_DDR3 */
static  void print_tx(const char *strval, u32 val)
{
#if CONFIG_DEBUG_RAM_SETUP
	printk(BIOS_DEBUG, "%s%08x\n", strval, val);
#endif
}
#endif

static  void print_t(const char *strval)
{
#if CONFIG_DEBUG_RAM_SETUP
	printk(BIOS_DEBUG, "%s", strval);
#endif
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


u32 mctGetLogicalCPUID(u32 Node)
{
	/* FIXME: Move this to a more generic place. Maybe to the CPU code */
	/* Converts the CPUID to a logical ID MASK that is used to check
	 CPU version support versions */
	u32 dev;
	u32 val, valx;
	u32 family, model, stepping;
	u32 ret;

	if (Node == 0xFF) { /* current node */
		val = cpuid_eax(0x80000001);
	} else {
		dev = PA_NBMISC(Node);
		val = Get_NB32(dev, 0xfc);
	}

	family = ((val >> 8) & 0x0f) + ((val >> 20) & 0xff);
	model = ((val >> 4) & 0x0f) | ((val >> (16-4)) & 0xf0);
	stepping = val & 0x0f;

	valx = (family << 12) | (model << 4) | (stepping);

	switch (valx) {
	case 0x10000:
		ret = AMD_DR_A0A;
		break;
	case 0x10001:
		ret = AMD_DR_A1B;
		break;
	case 0x10002:
		ret = AMD_DR_A2;
		break;
	case 0x10020:
		ret = AMD_DR_B0;
		break;
	case 0x10021:
		ret = AMD_DR_B1;
		break;
	case 0x10022:
		ret = AMD_DR_B2;
		break;
	case 0x10023:
		ret = AMD_DR_B3;
		break;
	case 0x10042:
		ret = AMD_RB_C2;
		break;
	case 0x10043:
		ret = AMD_RB_C3;
		break;
	case 0x10062:
		ret = AMD_DA_C2;
		break;
	case 0x10063:
		ret = AMD_DA_C3;
		break;
	case 0x10080:
		ret = AMD_HY_D0;
		break;
	case 0x10081:
		ret = AMD_HY_D1;
		break;
	case 0x100a0:
		ret = AMD_PH_E0;
		break;
	default:
		/* FIXME: mabe we should die() here. */
		print_err("FIXME! CPU Version unknown or not supported! \n");
		ret = 0;
	}

	return ret;
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

	print_debug("raminit_amdmct begin:\n");

	mctAutoInitMCT_D(pMCTstat, pDCTstatA);

	print_debug("raminit_amdmct end:\n");
}
