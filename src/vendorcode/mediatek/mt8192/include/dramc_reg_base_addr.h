/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __REG_BASE_ADDR__
#define __REG_BASE_ADDR__

#include "soc/addressmap.h"
#include "emi_hw.h"

#ifndef __ETT__
#define __ETT__ 0
#endif

//#define DRAM_BASE 0x40000000ULL
//#define DDR_BASE DRAM_BASE
#define CQ_DMA_BASE (IO_PHYS + 0x212000)
//#define CKSYS_BASE  IO_PHYS
//#define EMI_APB_BASE    0x10219000
//#define EMI_BASE EMI_APB_BASE
//#define EMI_MPU_BASE 0x10226000
#define CHN0_EMI_BASE  (IO_PHYS + 0x235000)
#define CHN1_EMI_BASE  (IO_PHYS + 0x245000)
#define INFRA_DRAMC_REG_CONFIG      (INFRACFG_AO_BASE + 0xB4)
//#define INFRACFG_AO_BASE		(IO_PHYS + 0x00001000)
//#define APMIXED_BASE		(IO_PHYS + 0x0000C000)

#define MPLL_CON0		(APMIXED_BASE + 0x390)
#define MPLL_CON1		(APMIXED_BASE + 0x394)
#define MPLL_CON3		(APMIXED_BASE + 0x39C)

#define PLLON_CON0		(APMIXED_BASE + 0x050)
#define PLLON_CON1		(APMIXED_BASE + 0x054)
#define PLLON_CON2		(APMIXED_BASE + 0x058)
#define PLLON_CON3		(APMIXED_BASE + 0x05C)

/* TOPCKGEN Register */
#define CLK_MISC_CFG_0          (CKSYS_BASE + 0x104)
#define CLK_MISC_CFG_1          (CKSYS_BASE + 0x108)
#define CLK_DBG_CFG             (CKSYS_BASE + 0x10C)
#define CLK26CALI_0             (CKSYS_BASE + 0x220)
#define CLK26CALI_1             (CKSYS_BASE + 0x224)

#endif //__REG_BASE_ADDR__
