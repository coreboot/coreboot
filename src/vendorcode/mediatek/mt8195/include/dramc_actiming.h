/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _ACTIMING_H
#define _ACTIMING_H

/***********************************************************************/
/*              Includes                                               */
/***********************************************************************/
#include "dramc_register.h"


//Definitions to enable specific freq's LP4 ACTiming support (To save code size)
#define SUPPORT_LP5_DDR6400_ACTIM 0
#define SUPPORT_LP5_DDR5500_ACTIM 0
#define SUPPORT_LP5_DDR4266_ACTIM 0
#define SUPPORT_LP5_DDR3200_ACTIM 0
#define SUPPORT_LP4_DDR4266_ACTIM 1
#define SUPPORT_LP4_DDR3733_ACTIM 1
#define SUPPORT_LP4_DDR3200_ACTIM 1
#define SUPPORT_LP4_DDR2667_ACTIM 0
#define SUPPORT_LP4_DDR2400_ACTIM 1
#define SUPPORT_LP4_DDR1866_ACTIM 1
#define SUPPORT_LP4_DDR1600_ACTIM 1
#define SUPPORT_LP4_DDR1333_ACTIM 0
#define SUPPORT_LP4_DDR1200_ACTIM 1
#define SUPPORT_LP4_DDR800_ACTIM  1
#if ENABLE_DDR400_OPEN_LOOP_MODE_OPTION
#define SUPPORT_LP4_DDR400_ACTIM  1
#else
#define SUPPORT_LP4_DDR400_ACTIM  0
#endif
/* Used to keep track the total number of LP4 ACTimings */
/* Since READ_DBI is enable/disabled using preprocessor C define
 * -> Save code size by excluding unneeded ACTimingTable entries
 * Note 1: READ_DBI on/off is for (LP4 data rate >= DDR2667 (FSP1))
 * Must make sure DDR3733 is the 1st entry (DMCATRAIN_INTV is used)
 */
typedef enum
{
#if SUPPORT_LP4_DDR4266_ACTIM
#if ENABLE_READ_DBI
    AC_TIME_LP4_BYTE_DDR4266_RDBI_ON = 0,
    AC_TIME_LP4_NORM_DDR4266_RDBI_ON,
#else //(ENABLE_READ_DBI == 0)
    AC_TIME_LP4_BYTE_DDR4266_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR4266_RDBI_OFF,
#endif //ENABLE_READ_DBI
#endif

#if SUPPORT_LP4_DDR3733_ACTIM
#if ENABLE_READ_DBI
    AC_TIME_LP4_BYTE_DDR3733_RDBI_ON,
    AC_TIME_LP4_NORM_DDR3733_RDBI_ON,
#else //(ENABLE_READ_DBI == 0)
    AC_TIME_LP4_BYTE_DDR3733_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR3733_RDBI_OFF,
#endif //ENABLE_READ_DBI
#endif

#if SUPPORT_LP4_DDR3200_ACTIM
#if ENABLE_READ_DBI
    AC_TIME_LP4_BYTE_DDR3200_RDBI_ON,
    AC_TIME_LP4_NORM_DDR3200_RDBI_ON,
#else //(ENABLE_READ_DBI == 0)
    AC_TIME_LP4_BYTE_DDR3200_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR3200_RDBI_OFF,
#endif //ENABLE_READ_DBI
#endif

#if SUPPORT_LP4_DDR2667_ACTIM
#if ENABLE_READ_DBI
    AC_TIME_LP4_BYTE_DDR2667_RDBI_ON,
    AC_TIME_LP4_NORM_DDR2667_RDBI_ON,
#else //(ENABLE_READ_DBI == 0)
    AC_TIME_LP4_BYTE_DDR2667_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR2667_RDBI_OFF,
#endif //ENABLE_READ_DBI
#endif

#if SUPPORT_LP4_DDR2400_ACTIM
    AC_TIME_LP4_BYTE_DDR2400_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR2400_RDBI_OFF,
#endif

#if SUPPORT_LP4_DDR1866_ACTIM
    AC_TIME_LP4_BYTE_DDR1866_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR1866_RDBI_OFF,
#endif

#if SUPPORT_LP4_DDR1600_ACTIM
    AC_TIME_LP4_BYTE_DDR1600_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR1600_RDBI_OFF,
    AC_TIME_LP4_BYTE_DDR1600_DIV4_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR1600_DIV4_RDBI_OFF,
#endif

#if SUPPORT_LP4_DDR1333_ACTIM
    AC_TIME_LP4_BYTE_DDR1333_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR1333_RDBI_OFF,
#endif


#if SUPPORT_LP4_DDR1200_ACTIM
    AC_TIME_LP4_BYTE_DDR1200_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR1200_RDBI_OFF,
    AC_TIME_LP4_BYTE_DDR1200_DIV4_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR1200_DIV4_RDBI_OFF,
#endif

#if SUPPORT_LP4_DDR800_ACTIM
    AC_TIME_LP4_BYTE_DDR800_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR800_RDBI_OFF,
    AC_TIME_LP4_BYTE_DDR800_DIV4_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR800_DIV4_RDBI_OFF,
#endif

#if SUPPORT_LP4_DDR400_ACTIM
    AC_TIME_LP4_BYTE_DDR400_RDBI_OFF,
    AC_TIME_LP4_NORM_DDR400_RDBI_OFF,
#endif

    AC_TIMING_NUMBER_LP4
} AC_TIMING_LP4_COUNT_TYPE_T;

#define AC_TIMING_NUMBER_LP5    0

/* ACTiming struct declaration (declared here due Fld_wid for each register type)
 * Should include all fields from ACTiming excel file (And update the correct values in UpdateACTimingReg()
 * Note: DQSINCTL, DATLAT aren't in ACTiming excel file (internal delay parameters)
 */
typedef struct _ACTime_T_LP4
{
    U8 dramType, cbtMode, readDBI;
    U8 DivMode;
    U16 freq;
    U8 readLat, writeLat;
    U8 dqsinctl, datlat; //DQSINCTL, DATLAT aren't in ACTiming excel file

    U8 tras;
    U8 trp;
    U8 trpab;
    U8 trc;
    U8 trfc;
    U8 trfcpb;
    U8 txp;
    U8 trtp;
    U8 trcd;
    U8 twr;
    U8 twtr;
    U8 tpbr2pbr;
    U8 tpbr2act;
    U8 tr2mrw;
    U8 tw2mrw;
    U8 tmrr2mrw;
    U8 tmrw;
    U8 tmrd;
    U8 tmrwckel;
    U8 tpde;
    U8 tpdx;
    U8 tmrri;
    U8 trrd;
    U8 trrd_4266;
    U8 tfaw;
    U8 tfaw_4266;
    U8 trtw_odt_off;
    U8 trtw_odt_on;
    U16 txrefcnt;
    U8 tzqcs;
    U8 xrtw2w_new_mode;
    U8 xrtw2w_old_mode;
    U8 xrtw2r_odt_on;
    U8 xrtw2r_odt_off;
    U8 xrtr2w_odt_on;
    U8 xrtr2w_odt_off;
    U8 xrtr2r_new_mode;
    U8 xrtr2r_old_mode;
    U8 tr2mrr;
    U8 vrcgdis_prdcnt;
    U8 hwset_mr2_op;
    U8 hwset_mr13_op;
    U8 hwset_vrcg_op;
    U8 trcd_derate;
    U8 trc_derate;
    U8 tras_derate;
    U8 trpab_derate;
    U8 trp_derate;
    U8 trrd_derate;
    U8 trtpd;
    U8 twtpd;
    U8 tmrr2w_odt_off;
    U8 tmrr2w_odt_on;
    U8 ckeprd;
    U8 ckelckcnt;
    U8 zqlat2;

    //DRAMC_REG_SHU_AC_TIME_05T ===================================
    U8 tras_05T;
    U8 trp_05T;
    U8 trpab_05T;
    U8 trc_05T;
    U8 trfc_05T;
    U8 trfcpb_05T;
    U8 txp_05T;
    U8 trtp_05T;
    U8 trcd_05T;
    U8 twr_05T;
    U8 twtr_05T;
    U8 tpbr2pbr_05T;
    U8 tpbr2act_05T;
    U8 tr2mrw_05T;
    U8 tw2mrw_05T;
    U8 tmrr2mrw_05T;
    U8 tmrw_05T;
    U8 tmrd_05T;
    U8 tmrwckel_05T;
    U8 tpde_05T;
    U8 tpdx_05T;
    U8 tmrri_05T;
    U8 trrd_05T;
    U8 trrd_4266_05T;
    U8 tfaw_05T;
    U8 tfaw_4266_05T;
    U8 trtw_odt_off_05T;
    U8 trtw_odt_on_05T;
    U8 trcd_derate_05T;
    U8 trc_derate_05T;
    U8 tras_derate_05T;
    U8 trpab_derate_05T;
    U8 trp_derate_05T;
    U8 trrd_derate_05T;
    U8 trtpd_05T;
    U8 twtpd_05T;
} ACTime_T_LP4;

typedef struct _ACTime_T_LP5
{
    U8 dramType, cbtMode, readDBI;
    U8 DivMode;
    U16 freq;
    U8 readLat, writeLat;
    U8 dqsinctl, datlat; //DQSINCTL, DATLAT aren't in ACTiming excel file

    U8 tras;
    U8 trp;
    U8 trpab;
    U8 trc;
    U8 trfc;
    U8 trfcpb;
    U8 txp;
    U8 trtp;
    U8 trcd;
    U8 twr;
    U8 twtr;
    U8 twtr_l;
    U8 tpbr2pbr;
    U8 tpbr2act;
    U8 tr2mrw;
    U8 tw2mrw;
    U8 tmrr2mrw;
    U8 tmrw;
    U8 tmrd;
    U8 tmrwckel;
    U8 tpde;
    U8 tpdx;
    U8 tmrri;
    U8 trrd;
    U8 tfaw;
    U8 tr2w_odt_off;
    U8 tr2w_odt_on;
    U16 txrefcnt;
    U8 wckrdoff;
    U8 wckwroff;
    U8 tzqcs;
    U8 xrtw2w_odt_off;
    U8 xrtw2w_odt_on;
    U8 xrtw2r_odt_off_otf_off;
    U8 xrtw2r_odt_on_otf_off;
    U8 xrtw2r_odt_off_otf_on;
    U8 xrtw2r_odt_on_otf_on;
    U8 xrtr2w_odt_on;
    U8 xrtr2w_odt_off;
    U8 xrtr2r_odt_off;
    U8 xrtr2r_odt_on;
    U8 xrtw2w_odt_off_wck;
    U8 xrtw2w_odt_on_wck;
    U8 xrtw2r_odt_off_wck;
    U8 xrtw2r_odt_on_wck;
    U8 xrtr2w_odt_off_wck;
    U8 xrtr2w_odt_on_wck;
    U8 xrtr2r_wck;
    U8 tr2mrr;
    U8 hwset_mr2_op;
    U8 hwset_mr13_op;
    U8 hwset_vrcg_op;
    U8 vrcgdis_prdcnt;
    U8 lp5_cmd1to2en;
    U8 trtpd;
    U8 twtpd;
    U8 tmrr2w;
    U8 ckeprd;
    U8 ckelckcnt;
    U8 tcsh_cscal;
    U8 tcacsh;
    U8 tcsh;
    U8 trcd_derate;
    U8 trc_derate;
    U8 tras_derate;
    U8 trpab_derate;
    U8 trp_derate;
    U8 trrd_derate;
    U8 zqlat2;

    //DRAMC_REG_SHU_AC_TIME_05T ===================================
    U8 tras_05T;
    U8 trp_05T;
    U8 trpab_05T;
    U8 trc_05T;
    U8 trfc_05T;
    U8 trfcpb_05T;
    U8 txp_05T;
    U8 trtp_05T;
    U8 trcd_05T;
    U8 twr_05T;
    U8 twtr_05T;
    U8 twtr_l_05T;
    U8 tr2mrw_05T;
    U8 tw2mrw_05T;
    U8 tmrr2mrw_05T;
    U8 tmrw_05T;
    U8 tmrd_05T;
    U8 tmrwckel_05T;
    U8 tpde_05T;
    U8 tpdx_05T;
    U8 tmrri_05T;
    U8 trrd_05T;
    U8 tfaw_05T;
    U8 tr2w_odt_off_05T;
    U8 tr2w_odt_on_05T;
    U8 wckrdoff_05T;
    U8 wckwroff_05T;
    U8 trtpd_05T;
    U8 twtpd_05T;
    U8 tpbr2pbr_05T;
    U8 tpbr2act_05T;
    U8 trcd_derate_05T;
    U8 trc_derate_05T;
    U8 tras_derate_05T;
    U8 trpab_derate_05T;
    U8 trp_derate_05T;
    U8 trrd_derate_05T;
} ACTime_T_LP5;

//ACTimingTbl[] forward declaration

extern U8 vDramcACTimingGetDatLat(DRAMC_CTX_T *p);
extern DRAM_STATUS_T DdrUpdateACTiming(DRAMC_CTX_T *p);
#endif
