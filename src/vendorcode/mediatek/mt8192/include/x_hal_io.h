/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef X_HAL_IO_H
#define X_HAL_IO_H

#include "dramc_pi_api.h"
//===========================================================================
#define REG_ACCESS_NAO_DGB 0
#define REG_ACCESS_PORTING_DGB 0
#define REG_SHUFFLE_REG_CHECK 0

// field access macro-----------------------------------------------------------

/* field macros */
#define Fld(wid, shft)    (((U32)wid << 16) | (shft << 8))
#define Fld_wid(fld)    ((UINT8)((fld) >> 16))
#define Fld_shft(fld)   ((UINT8)((fld) >> 8))
#define Fld_ac(fld)     (UINT8)(fld)

/* access method*/
#define AC_FULLB0       1
#define AC_FULLB1       2
#define AC_FULLB2       3
#define AC_FULLB3       4
#define AC_FULLW10      5
#define AC_FULLW21      6
#define AC_FULLW32      7
#define AC_FULLDW       8
#define AC_MSKB0        11
#define AC_MSKB1        12
#define AC_MSKB2        13
#define AC_MSKB3        14
#define AC_MSKW10       15
#define AC_MSKW21       16
#define AC_MSKW32       17
#define AC_MSKDW        18

#define Fld2Msk32(fld)  /*lint -save -e504 */ (((U32)0xffffffff>>(32-Fld_wid(fld)))<<Fld_shft(fld)) /*lint -restore */
#define P_Fld(val, fld) ( upk > 0 ? Fld2Msk32(fld): (((UINT32)(val) & ((1 << Fld_wid(fld)) - 1)) << Fld_shft(fld)))

extern U32 u4Dram_Register_Read(DRAMC_CTX_T *p, U32 u4reg_addr);
extern void ucDram_Register_Write(DRAMC_CTX_T *p, U32 u4reg_addr, U32 u4reg_value);

extern void vIO32Write4BMsk2(DRAMC_CTX_T *p, U32 reg32, U32 val32, U32 msk32);
extern void vIO32Write4BMsk_All2(DRAMC_CTX_T *p, U32 reg32, U32 val32, U32 msk32);
extern void vIO32Write4B_All2(DRAMC_CTX_T *p, U32 reg32, U32 val32);

// =========================
// public Macro for general use.
//==========================
#define u4IO32Read4B(reg32) u4Dram_Register_Read(p, reg32)
#define vIO32Write4B(reg32, val32) ucDram_Register_Write(p, reg32, val32)
#define vIO32Write4B_All(reg32, val32) vIO32Write4B_All2(p, reg32, val32)
#define vIO32Write4BMsk(reg32, val32, msk32) vIO32Write4BMsk2(p, reg32, val32, msk32)
#define vIO32Write4BMsk_All(reg32, val32, msk32) vIO32Write4BMsk_All2(p, reg32, val32, msk32)

#define u4IO32ReadFldAlign(reg32, fld) /*lint -save -e506 -e504 -e514 -e62 -e737 -e572 -e961 -e648 -e701 -e732 -e571 */ \
	((u4IO32Read4B(reg32) & Fld2Msk32(fld)) >> Fld_shft(fld))

#define vIO32WriteFldAlign(reg32, val, fld) /*lint -save -e506 -e504 -e514 -e62 -e737 -e572 -e961 -e648 -e701 -e732 -e571 */ \
	(vIO32Write4BMsk((reg32), ((U32)(val) << Fld_shft(fld)), Fld2Msk32(fld)))

#define vIO32WriteFldMulti(reg32, list) /*lint -save -e506 -e504 -e514 -e62 -e737 -e572 -e961 -e648 -e701 -e732 -e571 */ \
{ \
	UINT16 upk = 1; \
	INT32 msk = (INT32)(list); \
	{ upk = 0; \
    ((U32)msk == 0xffffffff)? (vIO32Write4B(reg32, (list))): (((U32)msk)? vIO32Write4BMsk(reg32, (list), ((U32)msk)):(U32)0); \
	} \
}/*lint -restore */

//=========================
// Public Macro for write all-dramC or all-PHY registers
//=========================
#define vIO32WriteFldAlign_All(reg32, val, fld) /*lint -save -e506 -e504 -e514 -e62 -e737 -e572 -e961 -e648 -e701 -e732 -e571 */ \
	(vIO32Write4BMsk_All((reg32), ((U32)(val) << Fld_shft(fld)), Fld2Msk32(fld)))

#define vIO32WriteFldMulti_All(reg32, list) /*lint -save -e506 -e504 -e514 -e62 -e737 -e572 -e961 -e648 -e701 -e732 -e571 */ \
{ \
	UINT16 upk = 1; \
	INT32 msk = (INT32)(list); \
	{ upk = 0; \
    ((U32)msk == 0xffffffff)? (vIO32Write4B_All(reg32, (list))): (((U32)msk)? vIO32Write4BMsk_All(reg32, (list), ((U32)msk)): (void)0); \
	} \
}/*lint -restore */

#ifdef __MD32__
#include "x_hal_io_dpm.h"
#endif

#endif  // X_HAL_IO_H
