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
#define SUPPORT_LP4_DDR800_ACTIM 1
#define SUPPORT_LP4_DDR400_ACTIM 0

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

#if (__LP5_COMBO__)
/* Used to keep track the total number of LP5 ACTimings */
typedef enum
{
#if SUPPORT_LP5_DDR6400_ACTIM
#if ENABLE_READ_DBI
        AC_TIME_LP5_BYTE_DDR6400_RDBI_ON = 0,
        AC_TIME_LP5_NORM_DDR6400_RDBI_ON,
#else //(ENABLE_READ_DBI == 0)
        AC_TIME_LP5_BYTE_DDR6400_RDBI_OFF,
        AC_TIME_LP5_NORM_DDR6400_RDBI_OFF,
#endif //ENABLE_READ_DBI
#endif

#if SUPPORT_LP5_DDR5500_ACTIM
#if ((ENABLE_READ_DBI) || (LP5_DDR4266_RDBI_WORKAROUND))
    AC_TIME_LP5_BYTE_DDR5500_RDBI_ON,
    AC_TIME_LP5_NORM_DDR5500_RDBI_ON,
#else
    AC_TIME_LP5_BYTE_DDR5500_RDBI_OFF,
    AC_TIME_LP5_NORM_DDR5500_RDBI_OFF,
#endif
#endif

#if SUPPORT_LP5_DDR4266_ACTIM
#if ((ENABLE_READ_DBI) || (LP5_DDR4266_RDBI_WORKAROUND))
        AC_TIME_LP5_BYTE_DDR4266_RDBI_ON,
        AC_TIME_LP5_NORM_DDR4266_RDBI_ON,
#else //(ENABLE_READ_DBI == 0)
        AC_TIME_LP5_BYTE_DDR4266_RDBI_OFF,
        AC_TIME_LP5_NORM_DDR4266_RDBI_OFF,
#endif //ENABLE_READ_DBI
#endif

#if SUPPORT_LP5_DDR3200_ACTIM
    AC_TIME_LP5_BYTE_DDR3200_RDBI_OFF,
    AC_TIME_LP5_NORM_DDR3200_RDBI_OFF,
#endif
    AC_TIMING_NUMBER_LP5
} AC_TIMING_LP5_COUNT_TYPE_T;
#else
#define AC_TIMING_NUMBER_LP5    0
#endif

/* ACTiming struct declaration (declared here due Fld_wid for each register type)
 * Should include all fields from ACTiming excel file (And update the correct values in UpdateACTimingReg()
 * Note: DQSINCTL, DATLAT aren't in ACTiming excel file (internal delay parameters)
 */
typedef struct _ACTime_T_LP4
{
    U8 dramType, cbtMode, readDBI;
    U8 DivMode;
    U16 freq;
    U16 readLat, writeLat;
    U16 dqsinctl, datlat; //DQSINCTL, DATLAT aren't in ACTiming excel file

    U16 tras;
    U16 trp;
    U16 trpab;
    U16 trc;
    U16 trfc;
    U16 trfcpb;
    U16 txp;
    U16 trtp;
    U16 trcd;
    U16 twr;
    U16 twtr;
    U16 tpbr2pbr;
    U16 tpbr2act;
    U16 tr2mrw;
    U16 tw2mrw;
    U16 tmrr2mrw;
    U16 tmrw;
    U16 tmrd;
    U16 tmrwckel;
    U16 tpde;
    U16 tpdx;
    U16 tmrri;
    U16 trrd;
    U16 trrd_4266;
    U16 tfaw;
    U16 tfaw_4266;
    U16 trtw_odt_off;
    U16 trtw_odt_on;
    U16 txrefcnt;
    U16 tzqcs;
    U16 xrtw2w_new_mode;
    U16 xrtw2w_old_mode;
    U16 xrtw2r_odt_on;
    U16 xrtw2r_odt_off;
    U16 xrtr2w_odt_on;
    U16 xrtr2w_odt_off;
    U16 xrtr2r_new_mode;
    U16 xrtr2r_old_mode;
    U16 tr2mrr;
    U16 vrcgdis_prdcnt;
    U16 hwset_mr2_op;
    U16 hwset_mr13_op;
    U16 hwset_vrcg_op;
    U16 trcd_derate;
    U16 trc_derate;
    U16 tras_derate;
    U16 trpab_derate;
    U16 trp_derate;
    U16 trrd_derate;
    U16 trtpd;
    U16 twtpd;
    U16 tmrr2w_odt_off;
    U16 tmrr2w_odt_on;
    U16 ckeprd;
    U16 ckelckcnt;
    U16 zqlat2;

    //DRAMC_REG_SHU_AC_TIME_05T ===================================
    U16 tras_05T;
    U16 trp_05T;
    U16 trpab_05T;
    U16 trc_05T;
    U16 trfc_05T;
    U16 trfcpb_05T;
    U16 txp_05T;
    U16 trtp_05T;
    U16 trcd_05T;
    U16 twr_05T;
    U16 twtr_05T;
    U16 tpbr2pbr_05T;
    U16 tpbr2act_05T;
    U16 tr2mrw_05T;
    U16 tw2mrw_05T;
    U16 tmrr2mrw_05T;
    U16 tmrw_05T;
    U16 tmrd_05T;
    U16 tmrwckel_05T;
    U16 tpde_05T;
    U16 tpdx_05T;
    U16 tmrri_05T;
    U16 trrd_05T;
    U16 trrd_4266_05T;
    U16 tfaw_05T;
    U16 tfaw_4266_05T;
    U16 trtw_odt_off_05T;
    U16 trtw_odt_on_05T;
    U16 trcd_derate_05T;
    U16 trc_derate_05T;
    U16 tras_derate_05T;
    U16 trpab_derate_05T;
    U16 trp_derate_05T;
    U16 trrd_derate_05T;
    U16 trtpd_05T;
    U16 twtpd_05T;
} ACTime_T_LP4;

typedef struct _ACTime_T_LP5
{
    U8 dramType, cbtMode, readDBI;
    U8 DivMode;
    U16 freq;
    U16 readLat, writeLat;
    U16 dqsinctl, datlat; //DQSINCTL, DATLAT aren't in ACTiming excel file

    U16 tras;
    U16 trp;
    U16 trpab;
    U16 trc;
    U16 trfc;
    U16 trfcpb;
    U16 txp;
    U16 trtp;
    U16 trcd;
    U16 twr;
	U16 twtr;
    U16 twtr_l;
    U16 tpbr2pbr;
    U16 tpbr2act;
    U16 tr2mrw;
    U16 tw2mrw;
    U16 tmrr2mrw;
    U16 tmrw;
    U16 tmrd;
    U16 tmrwckel;
	U16 tpde;
	U16 tpdx;
    U16 tmrri;
    U16 trrd;
    U16 tfaw;
    U16 tr2w_odt_off;
    U16 tr2w_odt_on;
    U16 txrefcnt;
	U16 wckrdoff;
	U16 wckwroff;
    U16 tzqcs;
    U16 xrtw2w_odt_off;
    U16 xrtw2w_odt_on;
    U16	xrtw2r_odt_off_otf_off;
    U16 xrtw2r_odt_on_otf_off;
    U16 xrtw2r_odt_off_otf_on;
    U16 xrtw2r_odt_on_otf_on;
    U16 xrtr2w_odt_on;
    U16 xrtr2w_odt_off;
    U16 xrtr2r_odt_off;
    U16 xrtr2r_odt_on;
	U16 xrtw2w_odt_off_wck;
	U16 xrtw2w_odt_on_wck;
	U16 xrtw2r_odt_off_wck;
	U16 xrtw2r_odt_on_wck;
	U16 xrtr2w_odt_off_wck;
	U16 xrtr2w_odt_on_wck;
	U16 xrtr2r_wck;
    U16 tr2mrr;
    U16 hwset_mr2_op;
    U16 hwset_mr13_op;
    U16 hwset_vrcg_op;
    U16 vrcgdis_prdcnt;
    U16 lp5_cmd1to2en;
	U16 trtpd;
    U16 twtpd;
    U16 tmrr2w;
    U16 ckeprd;
    U16 ckelckcnt;
    U16 tcsh_cscal;
    U16 tcacsh;
	U16 tcsh;
	U16 trcd_derate;
	U16 trc_derate;
	U16 tras_derate;
	U16 trpab_derate;
	U16 trp_derate;
	U16 trrd_derate;
    U16 zqlat2;

    //DRAMC_REG_SHU_AC_TIME_05T ===================================
    U16 tras_05T;
    U16 trp_05T;
    U16 trpab_05T;
    U16 trc_05T;
    U16 trfc_05T;
    U16 trfcpb_05T;
    U16 txp_05T;
    U16 trtp_05T;
    U16 trcd_05T;
    U16 twr_05T;
	U16 twtr_05T;
    U16 twtr_l_05T;
    U16 tr2mrw_05T;
    U16 tw2mrw_05T;
    U16 tmrr2mrw_05T;
    U16 tmrw_05T;
    U16 tmrd_05T;
    U16 tmrwckel_05T;
	U16 tpde_05T;
	U16 tpdx_05T;
    U16 tmrri_05T;
    U16 trrd_05T;
    U16 tfaw_05T;
    U16 tr2w_odt_off_05T;
    U16 tr2w_odt_on_05T;
	U16 wckrdoff_05T;
	U16 wckwroff_05T;
	U16 trtpd_05T;
    U16 twtpd_05T;
	U16 tpbr2pbr_05T;
	U16 tpbr2act_05T;
	U16 trcd_derate_05T;
	U16 trc_derate_05T;
	U16 tras_derate_05T;
	U16 trpab_derate_05T;
	U16 trp_derate_05T;
	U16 trrd_derate_05T;
} ACTime_T_LP5;

//ACTimingTbl[] forward declaration
extern const ACTime_T_LP4 ACTimingTbl_LP4[AC_TIMING_NUMBER_LP4];
extern const ACTime_T_LP5 ACTimingTbl_LP5[AC_TIMING_NUMBER_LP5];

extern U8 vDramcACTimingGetDatLat(DRAMC_CTX_T *p);
extern DRAM_STATUS_T DdrUpdateACTiming(DRAMC_CTX_T *p);
extern void vDramcACTimingOptimize(DRAMC_CTX_T *p);
extern DRAM_CBT_MODE_T vGet_Dram_CBT_Mode(DRAMC_CTX_T *p);
#endif
