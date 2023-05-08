/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _DRAMC_COMMON_H_
#define _DRAMC_COMMON_H_


#include <soc/dramc_soc.h>

#include <delay.h>
#include <device/mmio.h>
#include <stdint.h>
#include <types.h>
#include <reg.h>
#include <soc/dramc_common.h>
#include <timer.h>
#include <print.h>

#include <string.h>
#include "dramc_register.h"
#include "dramc_pi_api.h"
#include "dramc_int_slt.h"
#include "print.h"
#include "reg.h"

#if FOR_DV_SIMULATION_USED == 1
#include "dramc_dv_init.h"
#else
#ifdef __DPM__
#include "dramc_dpm.h"
#elif __ETT__
#include "common.h"
#endif
#endif

#if __ETT__
#include "dramc_version.h"
#else
#define _VERSION_ "ETT Version 0.0.0.1"
#define _COMMIT_VERSION_ "01234567"
#define _COMMIT_DATE_ "2020-10-10/10:10:10"
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
// choose a proper mcDELAY
#if defined(__DPM__)
#define mcDELAY_US(x)		timer_busy_wait_us(TIMER3, x)
#define mcDELAY_XUS(x)		timer_busy_wait_us(TIMER3, x)
#define mcDELAY_XNS(x)		timer_busy_wait_us(TIMER3, 1)
#define mcDELAY_MS(x)		timer_busy_wait_ms(TIMER3, x)
#endif

/**********************************************/
/* Priority of debug log                      */
/*--------------------------------------------*/
/* mcSHOW_DBG_MSG: High                       */
/* mcSHOW_DBG_MSG2: Medium High               */
/* mcSHOW_DBG_MSG3: Medium Low                */
/* mcSHOW_DBG_MSG4: Low                       */
/**********************************************/

#if __FLASH_TOOL_DA__
  #define printf DBG_MSG
  #define print DBG_MSG
#elif defined(RELEASE)
#if !__ETT__
  #undef printf
  #define printf
  #undef print
  #define print
#endif
#endif

#if FOR_DV_SIMULATION_USED
    #define mcSHOW_DBG_MSG(_x_)   {printf _x_;}
    #define mcSHOW_DBG_MSG2(_x_)  {printf _x_;}
    #define mcSHOW_DBG_MSG3(_x_)  {printf _x_;}
    #define mcSHOW_DBG_MSG4(_x_)  {printf _x_;}
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)  {printf _x_;}
    #define mcSHOW_EYESCAN_MSG(_x_)
    #define mcSHOW_TIME_MSG(_x_)
    #define mcSHOW_ERR_MSG(_x_)   {printf _x_;}
    #define mcSHOW_DUMP_INIT_RG_MSG(_x_)  {printf _x_;}
#elif __ETT__
    #if QT_GUI_Tool
        #if MRW_CHECK_ONLY
        #define mcSHOW_DBG_MSG_tmp(...) {printf (__VA_ARGS__);  if(fp_A60868){fprintf (fp_A60868,__VA_ARGS__);}}
        #define mcSHOW_DBG_MSG_Dump(...)
        #define mcSHOW_DBG_MSG(_x_)
        #define mcSHOW_DBG_MSG2(_x_)
        #define mcSHOW_DBG_MSG3(_x_)
        #define mcSHOW_DBG_MSG4(_x_)
        #define mcSHOW_DBG_MSG5(_x_)
        #define mcSHOW_DBG_MSG6(_x_)
        #define mcSHOW_EYESCAN_MSG(_x_)
        #define mcSHOW_TIME_MSG(_x_)
        #define mcSHOW_ERR_MSG(_x_)
        #define mcSHOW_DUMP_INIT_RG_MSG(_x_)
        #define mcSHOW_MRW_MSG(_x_)    {mcSHOW_DBG_MSG_tmp _x_;}
        #else
        #define mcSHOW_DBG_MSG_tmp(...)   {printf (__VA_ARGS__);  if(fp_A60868){fprintf (fp_A60868,__VA_ARGS__);}}
        #define mcSHOW_DBG_MSG_Dump(...)  {if(fp_A60868_RGDump){fprintf (fp_A60868_RGDump,__VA_ARGS__);}}
        #define mcSHOW_DBG_MSG(_x_)   {mcSHOW_DBG_MSG_tmp _x_;}
        #define mcSHOW_DBG_MSG2(_x_)  {mcSHOW_DBG_MSG_tmp _x_;}
        #define mcSHOW_DBG_MSG3(_x_)  {mcSHOW_DBG_MSG_tmp _x_;}
        #define mcSHOW_DBG_MSG4(_x_)  {mcSHOW_DBG_MSG_tmp _x_;}
        #define mcSHOW_DBG_MSG5(_x_)
        #define mcSHOW_DBG_MSG6(_x_)
        #define mcSHOW_EYESCAN_MSG(_x_) {mcSHOW_DBG_MSG_tmp _x_;}
        #define mcSHOW_TIME_MSG(_x_)
        #define mcSHOW_ERR_MSG(_x_)   {mcSHOW_DBG_MSG_tmp _x_;}
        #define mcSHOW_DUMP_INIT_RG_MSG(_x_)
        #endif
    #elif (defined(DDR_INIT_TIME_PROFILING))
    #define mcSHOW_DBG_MSG(_x_)
    #define mcSHOW_DBG_MSG2(_x_)
    #define mcSHOW_DBG_MSG3(_x_)
    #define mcSHOW_DBG_MSG4(_x_)
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)
    #define mcSHOW_JV_LOG_MSG(_x_)
    #define mcSHOW_EYESCAN_MSG(_x_)
    #define mcSHOW_TIME_MSG(_x_)   {opt_print _x_;}
    #define mcSHOW_ERR_MSG(_x_)
    #elif DUMP_ALLSUH_RG
    #define mcSHOW_DBG_MSG(_x_)     {mcDELAY_US(50);opt_print _x_;}
    #define mcSHOW_DBG_MSG2(_x_)    {mcDELAY_US(50); opt_print _x_;}
    #define mcSHOW_DBG_MSG3(_x_)
    #define mcSHOW_DBG_MSG4(_x_)
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)
    #define mcSHOW_JV_LOG_MSG(_x_)
    #define mcSHOW_EYESCAN_MSG(_x_)
    #define mcSHOW_TIME_MSG(_x_)
    #define mcSHOW_ERR_MSG(_x_) {mcDELAY_US(50);opt_print _x_;}
    #elif defined(RELEASE)
    #define mcSHOW_DBG_MSG(_x_)  //{opt_print _x_;}
    #define mcSHOW_DBG_MSG2(_x_)
    #define mcSHOW_DBG_MSG3(_x_)
    #define mcSHOW_DBG_MSG4(_x_)
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)
    #define mcSHOW_JV_LOG_MSG(_x_) {opt_print _x_;}
    #define mcSHOW_EYESCAN_MSG(_x_) {opt_print _x_;}   //mcSHOW_JV_LOG_MSG(_x_) is for vendor JV
    #define mcSHOW_TIME_MSG(_x_)
    #define mcSHOW_ERR_MSG(_x_)  {opt_print _x_;}
    #elif VENDER_JV_LOG
    #define mcSHOW_DBG_MSG(_x_)
    #define mcSHOW_DBG_MSG2(_x_)
    #define mcSHOW_DBG_MSG3(_x_)
    #define mcSHOW_DBG_MSG4(_x_)
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)
    #define mcSHOW_JV_LOG_MSG(_x_)    {opt_print _x_;}   //mcSHOW_JV_LOG_MSG(_x_) is for vendor JV
    #define mcSHOW_EYESCAN_MSG(_x_) {opt_print _x_;}   //mcSHOW_JV_LOG_MSG(_x_) is for vendor JV
    #define mcSHOW_TIME_MSG(_x_)
    #define mcSHOW_ERR_MSG(_x_)
    #elif SW_CHANGE_FOR_SIMULATION
    #define mcSHOW_DBG_MSG(_x_)
    #define mcSHOW_DBG_MSG2(_x_)
    #define mcSHOW_DBG_MSG3(_x_)
    #define mcSHOW_DBG_MSG4(_x_)
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)
    #define mcSHOW_JV_LOG_MSG(_x_)
    #define mcSHOW_EYESCAN_MSG(_x_)
    #define mcSHOW_TIME_MSG(_x_)
    #define mcSHOW_ERR_MSG(_x_)
    #elif defined(DUMP_INIT_RG_LOG_TO_DE)
    #define mcSHOW_DBG_MSG(_x_)
    #define mcSHOW_DBG_MSG2(_x_)
    #define mcSHOW_DBG_MSG3(_x_)
    #define mcSHOW_DBG_MSG4(_x_)
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)
    #define mcSHOW_JV_LOG_MSG(_x_)
    #define mcSHOW_DUMP_INIT_RG_MSG(_x_) {gpt_busy_wait_us(50); print _x_;}
    #define mcSHOW_EYESCAN_MSG(_x_)
    #define mcSHOW_TIME_MSG(_x_)
    #define mcSHOW_ERR_MSG(_x_)
    #elif MRW_CHECK_ONLY
    #define mcSHOW_DBG_MSG(_x_)
    #define mcSHOW_DBG_MSG2(_x_)
    #define mcSHOW_DBG_MSG3(_x_)
    #define mcSHOW_DBG_MSG4(_x_)
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)
    #define mcSHOW_JV_LOG_MSG(_x_)
    #define mcSHOW_MRW_MSG(_x_)    {printf _x_;}
    #define mcSHOW_EYESCAN_MSG(_x_)
    #define mcSHOW_TIME_MSG(_x_)
    #define mcSHOW_ERR_MSG(_x_)
    #else   // ETT real chip
    #define mcSHOW_DBG_MSG(_x_)   {mcDELAY_US(10); print _x_;}
    #define mcSHOW_DBG_MSG2(_x_)  {mcDELAY_US(10); print _x_;}
    #define mcSHOW_DBG_MSG3(_x_)  {mcDELAY_US(10); print _x_;}
    #define mcSHOW_DBG_MSG4(_x_)
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)
    #define mcSHOW_JV_LOG_MSG(_x_)
    #define mcSHOW_EYESCAN_MSG(_x_) {if (gEye_Scan_color_flag) {mcDELAY_US(200);}; print _x_;}
    #define mcSHOW_TIME_MSG(_x_)
    #define mcSHOW_ERR_MSG(_x_)   {print _x_;}
    #endif
#else  // preloader
    #if defined(DDR_INIT_TIME_PROFILING)
    #define mcSHOW_DBG_MSG(_x_)
    #define mcSHOW_DBG_MSG2(_x_)
    #define mcSHOW_DBG_MSG3(_x_)
    #define mcSHOW_DBG_MSG4(_x_)
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)
    #define mcSHOW_JV_LOG_MSG(_x_)
    #define mcSHOW_EYESCAN_MSG(_x_)
    #define mcSHOW_TIME_MSG(_x_)   {print _x_;}
    #define mcSHOW_ERR_MSG(_x_)
    #elif defined(TARGET_BUILD_VARIANT_ENG) //&& !defined(MTK_EFUSE_WRITER_SUPPORT) && !CFG_TEE_SUPPORT && !MTK_EMMC_SUPPORT
    #define mcSHOW_DBG_MSG(_x_)   {print _x_;}
    #define mcSHOW_DBG_MSG2(_x_)  {print _x_;}
    #define mcSHOW_DBG_MSG3(_x_)  {print _x_;}
    #define mcSHOW_DBG_MSG4(_x_)
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)  {print _x_;}
        #if (CFG_DRAM_LOG_TO_STORAGE)
        #define mcSHOW_EYESCAN_MSG(_x_)  {print _x_;}
        #define mcSHOW_JV_LOG_MSG(_x_)   {print _x_;}
        #else
        #define mcSHOW_EYESCAN_MSG(_x_)
        #define mcSHOW_JV_LOG_MSG(_x_)
        #endif
    #define mcSHOW_TIME_MSG(_x_)
    #define mcSHOW_ERR_MSG(_x_)   {print _x_;}
    #else
    #define mcSHOW_DBG_MSG(_x_) { if (0) { print _x_; } }
    #define mcSHOW_DBG_MSG2(_x_) { if (0) { print _x_; } }
    #define mcSHOW_DBG_MSG3(_x_) { if (0) { print _x_; } }
    #define mcSHOW_DBG_MSG4(_x_) { if (0) { print _x_; } }
    #define mcSHOW_DBG_MSG5(_x_)
    #define mcSHOW_DBG_MSG6(_x_)
    #define mcSHOW_JV_LOG_MSG(_x_)
    #define mcSHOW_EYESCAN_MSG(_x_)
    #define mcSHOW_TIME_MSG(_x_)
    #define mcSHOW_ERR_MSG(_x_)     {print _x_;}
    #endif
#endif



#if QT_GUI_Tool ==1
#define mcFPRINTF(_x_)     fprintf _x_;
#else
#define mcFPRINTF(_x_)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)    (sizeof (x) / sizeof (x[0]))
#endif

#define enter_function() \
	({mcSHOW_DBG_MSG(("enter %s\n", __FUNCTION__));})

#define exit_function() \
	({mcSHOW_DBG_MSG(("exit %s\n", __FUNCTION__));})

extern int dump_log;
#endif   // _DRAMC_COMMON_H_
