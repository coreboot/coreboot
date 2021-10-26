/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _INT_SLT_H
#define _INT_SLT_H


//======================== EMI LPBK TEST Definition =====================================

#if defined(SLT)
#define ENABLE_EMI_LPBK_TEST 1
#else
#define ENABLE_EMI_LPBK_TEST 0
#endif

#define EMI_LPBK_DRAM_USED  !ENABLE_EMI_LPBK_TEST

#define EMI_LPBK_USE_THROUGH_IO 0
#define EMI_INT_LPBK_WL_DQS_RINGCNT   0
#define EMI_LPBK_ADDRESS_DEFECT 0

#if ENABLE_EMI_LPBK_TEST
#define EMI_USE_TA2     0
#else
#define EMI_USE_TA2     0
#endif


#define EMI_LPBK_1W1R   0

#define EMI_LPBK_S1     0

#define FREQ_METER      1
#define DQSG_COUNTER    1


#define ADJUST_TXDLY_SCAN_RX_WIN   0

#define EMI_LPBK_K_TX     0
#define ENABLE_PRE_POSTAMBLE !EMI_USE_TA2


#define EMI_LPBK_DFS_32 0
#define EMI_LPBK_DFS_24 0
#define EMI_LPBK_DFS_16 0
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

#define LP4_4266_freq_meter  533
#define LP4_3733_freq_meter  464
#define LP4_3200_freq_meter  386
#define LP4_2400_freq_meter  299
#define LP4_1600_freq_meter  191
#define LP4_1200_freq_meter  299
#define LP4_800_freq_meter   199


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
