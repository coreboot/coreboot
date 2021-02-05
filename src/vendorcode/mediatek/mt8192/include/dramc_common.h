/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _DRAMC_COMMON_H_
#define _DRAMC_COMMON_H_

#define __ETT__ 0

//#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <stdint.h>
#include <types.h>
#include <reg.h>
#include <string.h>
#include <soc/dramc_common.h>
#include <timer.h>
#include <print.h>

#include "dramc_register.h"
#include "dramc_pi_api.h"
#include "dramc_int_slt.h"

#if FOR_DV_SIMULATION_USED == 1
#include "dramc_dv_init.h"
#endif

/***********************************************************************/
/*                  Public Types                                       */
/***********************************************************************/

/*------------------------------------------------------------*/
/*                  macros, defines, typedefs, enums          */
/*------------------------------------------------------------*/
/************************** Common Macro *********************/
#define dsb()		asm volatile("dsb sy" : : : "memory")

#define DRV_Reg32(x)		read32((const void *)((u64)(x)))
#define DRV_WriteReg32(x, y)	write32((void *)((u64)(x)), (y))

#define mcDELAY_US(x)       udelay(x)
#define mcDELAY_MS(x)       udelay(x*1000)
#define mcDELAY_XUS(x)      udelay(x)
#define mcDELAY_XNS(x)      udelay(1)

/**********************************************/
/* Priority of debug log                      */
/*--------------------------------------------*/
/* mcSHOW_DBG_MSG: High                       */
/* mcSHOW_DBG_MSG2: Medium High               */
/* mcSHOW_DBG_MSG3: Medium Low                */
/* mcSHOW_DBG_MSG4: Low                       */
/**********************************************/

#define CALIBRATION_LOG		1

#if CALIBRATION_LOG
#define mcSHOW_DBG_MSG(_x_)	{print _x_;}
#define mcSHOW_DBG_MSG2(_x_)	//{print _x_;}
#define mcSHOW_ERR_MSG(_x_) 	{print _x_;}
#else
#define mcSHOW_DBG_MSG(_x_)
#define mcSHOW_DBG_MSG2(_x_)
#define mcSHOW_ERR_MSG(_x_)
#endif

#define mcSHOW_DBG_MSG3(_x_) // {print _x_;}
#define mcSHOW_DBG_MSG4(_x_)
#define mcSHOW_DBG_MSG5(_x_)
#define mcSHOW_JV_LOG_MSG(_x_)
#if EYESCAN_LOG
#define mcSHOW_EYESCAN_MSG(_x_) {print _x_;}
#else
#define mcSHOW_EYESCAN_MSG(_x_) //{print _x_;}
#endif
#define mcSHOW_DBG_MSG5(_x_)
#define mcSHOW_TIME_MSG(_x_)
#define mcDUMP_REG_MSG(_x_)
#define mcFPRINTF(_x_)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)    (sizeof (x) / sizeof (x[0]))
#endif

#define enter_function() \
	({mcSHOW_DBG_MSG(("enter %s\n", __FUNCTION__));})

#define exit_function() \
	({mcSHOW_DBG_MSG(("exit %s\n", __FUNCTION__));})

extern int dump_log;
#endif   // _DRAMC_COMMON_H_
