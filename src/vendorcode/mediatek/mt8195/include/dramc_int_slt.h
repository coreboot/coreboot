/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _INT_SLT_H
#define _INT_SLT_H


//======================== EMI LPBK TEST Definition =====================================

#if defined(SLT)
#define ENABLE_EMI_LPBK_TEST 1
#else
#define ENABLE_EMI_LPBK_TEST 0
#endif

#define EMI_LPBK_DRAM_USED  !ENABLE_EMI_LPBK_TEST   // 0: EMI LPBK test, 1: normal K, dram used

#define EMI_LPBK_USE_THROUGH_IO 0           //test through IO
#define EMI_INT_LPBK_WL_DQS_RINGCNT   0     //DQS Ring cnt: through io @ 800,1600,2400,3200, emi intlpbk wo rx/tx K window
#define EMI_LPBK_ADDRESS_DEFECT 0           //test address defect, MUST use CPU WRITE mode

#if ENABLE_EMI_LPBK_TEST
#define EMI_USE_TA2     0   // 0:CPU write, 1:TA2,  DVsim/Dsim use TA2, but 1:4 mode must use cpu write(because TA2 not support 1:4 mode)
#else
#define EMI_USE_TA2     0
#endif

/****************************
Summary:
1W1R: address offset : 0, 4, 8, c (1:8 mode only), no support 1:4 mode
8W1R: address offset 0x0 ~ 0xC (8W1R), 0x10 ~ 0x1C, (10W1R) (1:8 & 1:4 mode)
****************************/
#define EMI_LPBK_1W1R   0   //CPU mode 0:8W1R, 1:1W1R

#define EMI_LPBK_S1     0

#define FREQ_METER      1
#define DQSG_COUNTER    1


#define ADJUST_TXDLY_SCAN_RX_WIN   0

#define EMI_LPBK_K_TX     0
#define ENABLE_PRE_POSTAMBLE !EMI_USE_TA2  //0: no pre/post-amble for TA2, 1: need pre/post-amble for cpu write


#define EMI_LPBK_DFS_32 0   //DFS 32<->32<->32
#define EMI_LPBK_DFS_24 0   //DFS 24<->24<->24
#define EMI_LPBK_DFS_16 0   //DFS 16<->16<->16
#define EMI_LPBK_USE_LP3_PINMUX 0
#define EMI_LPBK_8W1R   1
#if EMI_LPBK_1W1R
#undef EMI_LPBK_8W1R
#define EMI_LPBK_8W1R   0
#endif

#if EMI_LPBK_USE_THROUGH_IO
#define EMI_LPBK_USE_DDR_800 1
#else
#define EMI_LPBK_USE_DDR_800 0
#endif
//#define K_TX_DQS_DLY 0

#define LP4_4266_freq_meter  533    //   //shu0 533
#define LP4_3733_freq_meter  464    //   //shu0 464
#define LP4_3200_freq_meter  386    //   //shu8 386  //shu9 386
#define LP4_2400_freq_meter  299          //shu6 299  shu5 299
#define LP4_1600_freq_meter  191    //199   //shu4 383  shu3 191
#define LP4_1200_freq_meter  299          //shu2 299  shu1 299
#define LP4_800_freq_meter   199          //shu7 199


#if ENABLE_EMI_LPBK_TEST    //EMI_LPBK_DRAM_USED==0
/*
#define SLT
#undef ENABLE_TMRRI_NEW_MODE
#define ENABLE_TMRRI_NEW_MODE 0
#undef ENABLE_DUTY_SCAN_V2
#define ENABLE_DUTY_SCAN_V2   0
#undef ENABLE_RODT_TRACKING
#define ENABLE_RODT_TRACKING 0
#undef TX_K_DQM_WITH_WDBI
#define TX_K_DQM_WITH_WDBI  0
#undef ENABLE_WRITE_DBI
#define ENABLE_WRITE_DBI 0
*/

#if EMI_INT_LPBK_WL_DQS_RINGCNT
#undef EMI_LPBK_USE_THROUGH_IO
#define EMI_LPBK_USE_THROUGH_IO 1
#undef EMI_LPBK_USE_DDR_800
#define EMI_LPBK_USE_DDR_800 0
#endif

#endif
//#if (EMI_LPBK_DRAM_USED)
//#undef ENABLE_MIOCK_JMETER
//#define ENABLE_MIOCK_JMETER   // for TX_PER_BIT_DELAY_CELL
//#endif


//=============================================================================




#endif //_INT_GLOBAL_H
