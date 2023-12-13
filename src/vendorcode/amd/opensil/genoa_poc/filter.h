/* SPDX-License-Identifier: GPL-2.0-only */

/* Keep this in sync with openSIL SilCommon.h file */
#define DEBUG_FILTER_APOB       0x00000001UL
#define DEBUG_FILTER_NBIO       0x00000002UL
#define DEBUG_FILTER_CCX        0x00000004UL
#define DEBUG_FILTER_SMU        0x00000008UL
#define DEBUG_FILTER_DF         0x00000010UL
#define DEBUG_FILTER_MEM        0x00000040UL
#define DEBUG_FILTER_FCH        0x00000080UL
#define DEBUG_FILTER_RAS        0x00000100UL

#define SIL_DEBUG(topic) (CONFIG(OPENSIL_DEBUG_##topic) ? DEBUG_FILTER_##topic : 0)

#define SIL_DEBUG_MODULE_FILTER ( \
	  SIL_DEBUG(APOB) | \
	  SIL_DEBUG(NBIO) | \
	  SIL_DEBUG(CCX) | \
	  SIL_DEBUG(SMU) | \
	  SIL_DEBUG(DF) | \
	  SIL_DEBUG(MEM) | \
	  SIL_DEBUG(FCH) | \
	  SIL_DEBUG(RAS) )
