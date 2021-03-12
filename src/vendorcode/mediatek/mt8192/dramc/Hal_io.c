/* SPDX-License-Identifier: BSD-3-Clause */

#include "x_hal_io.h"
#include "dramc_common.h"
#include "dramc_int_global.h"

#if __ETT__
#include <barriers.h>
#endif

#ifdef DUMP_INIT_RG_LOG_TO_DE
	U8	gDUMP_INIT_RG_LOG_TO_DE_RG_log_flag = 0;
#endif

static U32 u4RegBaseAddrTraslate(DRAM_DFS_REG_SHU_T eShu, DRAM_RANK_T eRank, U32 u4reg_addr)
{
	U32 u4Offset = u4reg_addr & 0xffff;
	U32 u4RegType = ((u4reg_addr - Channel_A_DRAMC_NAO_BASE_VIRTUAL) >> POS_BANK_NUM) & 0xf;
	U32 u4BaseAddr = 0;

	if (u4reg_addr < Channel_A_DRAMC_NAO_BASE_VIRTUAL ||
		u4reg_addr >= MAX_BASE_VIRTUAL)
	{
		return u4reg_addr;
	}

	if (u4RegType >= 2 && u4RegType <= 3)// ChA/B Dramc AO Register
	{
		if (u4Offset < DRAMC_REG_AO_SHUFFLE0_BASE_ADDR || u4Offset > DRAMC_REG_AO_SHUFFLE0_END_ADDR)
			eShu = 0;
	}
	else if (u4RegType >= 6 && u4RegType <= 7)// ChA/B Dramc AO Register
	{
		if (u4Offset < DDRPHY_AO_SHUFFLE0_BASE_ADDR || u4Offset > DDRPHY_AO_SHUFFLE0_END_ADDR)
			eShu = 0;
	}

	if (eRank == RANK_1)
	{
		if (u4RegType >= 2 && u4RegType <= 3)// ChA/B Dramc AO Register
		{
			if (u4Offset >= DRAMC_REG_AO_RANK0_WO_SHUFFLE_BASE_ADDR &&
				u4Offset <= DRAMC_REG_AO_RANK0_WO_SHUFFLE_END_ADDR)
			{
				u4Offset += DRAMC_REG_AO_RANK_OFFSET;
			}
			else if (u4Offset >= DRAMC_REG_AO_RANK0_W_SHUFFLE0_BASE_ADDR &&
					 u4Offset <= DRAMC_REG_AO_RANK0_W_SHUFFLE0_END_ADDR)
			{
				u4Offset += DRAMC_REG_AO_RANK_OFFSET;
			}
		}
		else if (u4RegType >= 6 && u4RegType <= 7)// PhyA/B AO Register
		{
			// 0x60~0xE0
			if (u4Offset >= DDRPHY_AO_RANK0_B0_NON_SHU_BASE_ADDR &&
				u4Offset <= DDRPHY_AO_RANK0_B0_NON_SHU_END_ADDR)
			{
				u4Offset += DDRPHY_AO_RANK_OFFSET;
			}
			// 0x1E0~0x260
			else if (u4Offset >= DDRPHY_AO_RANK0_B1_NON_SHU_BASE_ADDR &&
					 u4Offset <= DDRPHY_AO_RANK0_B1_NON_SHU_END_ADDR)
			{
				u4Offset += DDRPHY_AO_RANK_OFFSET;
			}
			// 0x360~0x3E0
			else if (u4Offset >= DDRPHY_AO_RANK0_CA_NON_SHU_BASE_ADDR &&
					 u4Offset <= DDRPHY_AO_RANK0_CA_NON_SHU_END_ADDR)
			{
				u4Offset += DDRPHY_AO_RANK_OFFSET;
			}
			// 0x760~0x7E0
			else if (u4Offset >= DDRPHY_AO_RANK0_B0_SHU0_BASE_ADDR &&
					 u4Offset <= DDRPHY_AO_RANK0_B0_SHU0_END_ADDR)
			{
				u4Offset += DDRPHY_AO_RANK_OFFSET;
			}
			// 0x8E0~0x960
			else if (u4Offset >= DDRPHY_AO_RANK0_B1_SHU0_BASE_ADDR &&
					 u4Offset <= DDRPHY_AO_RANK0_B1_SHU0_END_ADDR)
			{
				u4Offset += DDRPHY_AO_RANK_OFFSET;
			}
			// 0xA60~0xAE0
			else if (u4Offset >= DDRPHY_AO_RANK0_CA_SHU0_BASE_ADDR &&
					 u4Offset <= DDRPHY_AO_RANK0_CA_SHU0_END_ADDR)
			{
				u4Offset += DDRPHY_AO_RANK_OFFSET;
			}
			// 0xBE0~0xC60
			else if (u4Offset >= DDRPHY_AO_RANK0_MISC_SHU0_BASE_ADDR &&
					 u4Offset <= DDRPHY_AO_RANK0_MISC_SHU0_END_ADDR)
			{
				u4Offset += DDRPHY_AO_RANK_OFFSET;
			}
		}
		else if (u4RegType <= 1)// ChA/B Dramc NAO Register
		{
			if (u4Offset >= (DRAMC_REG_RK0_DQSOSC_STATUS - DRAMC_NAO_BASE_ADDRESS) &&
				u4Offset < (DRAMC_REG_RK1_DQSOSC_STATUS - DRAMC_NAO_BASE_ADDRESS))
			{
				u4Offset += 0x100;
			}
			else if (u4Offset >= DRAMC_REG_NAO_RANK0_ROW_OFFSET_BASE_ADDR &&
					 u4Offset <= DRAMC_REG_NAO_RANK0_ROW_OFFSET_END_ADDR)
			{
				u4Offset += DRAMC_REG_NAO_RANK_OFFSET;
			}
		}
		else if (u4RegType >= 4 && u4RegType <= 5) // PhyA/B NAO Register
		{
			// PhyA/B NAO Register
			if (u4Offset >= DDRPHY_NAO_RANK0_B0_DQSIEN_AUTOK_STATUS_START &&
				u4Offset < DDRPHY_NAO_RANK0_B0_DQSIEN_AUTOK_STATUS_END)
			{
				u4Offset += DDRPHY_NAO_DQSIEN_AUTOK_STATUS_RK_OFFSET;
			}
			else if (u4Offset >= DDRPHY_NAO_RANK0_B1_DQSIEN_AUTOK_STATUS_START &&
				u4Offset < DDRPHY_NAO_RANK0_B1_DQSIEN_AUTOK_STATUS_END)
			{
				u4Offset += DDRPHY_NAO_DQSIEN_AUTOK_STATUS_RK_OFFSET;
			}
			else if (u4Offset >= DDRPHY_NAO_RANK0_CA_DQSIEN_AUTOK_STATUS_START &&
				u4Offset < DDRPHY_NAO_RANK0_CA_DQSIEN_AUTOK_STATUS_END)
			{
				u4Offset += DDRPHY_NAO_DQSIEN_AUTOK_STATUS_RK_OFFSET;
			}
			else if (u4Offset >= DDRPHY_NAO_RANK0_GATING_STATUS_START &&
				u4Offset < DDRPHY_NAO_RANK0_GATING_STATUS_END)
			{
				u4Offset += DDRPHY_NAO_GATING_STATUS_RK_OFFSET;
			}
		}
	}

	switch (u4RegType)
	{
		case 0:
			 u4BaseAddr = Channel_A_DRAMC_NAO_BASE_ADDRESS;
			 break;
		 case 1:
			 u4BaseAddr = Channel_B_DRAMC_NAO_BASE_ADDRESS;
			 break;
		 case 2:
			 u4BaseAddr = Channel_A_DRAMC_AO_BASE_ADDRESS + (eShu * DRAMC_REG_AO_SHU_OFFSET);
			 break;
		 case 3:
			 u4BaseAddr = Channel_B_DRAMC_AO_BASE_ADDRESS + (eShu * DRAMC_REG_AO_SHU_OFFSET);
			 break;
		 case 4:
			   u4BaseAddr = Channel_A_DDRPHY_NAO_BASE_ADDRESS;
			   break;
		 case 5:
			   u4BaseAddr = Channel_B_DDRPHY_NAO_BASE_ADDRESS;
			   break;
		 case 6:
			   u4BaseAddr = Channel_A_DDRPHY_AO_BASE_ADDRESS + (eShu * DDRPHY_AO_SHU_OFFSET);
			   break;
		 case 7:
			   u4BaseAddr = Channel_B_DDRPHY_AO_BASE_ADDRESS + (eShu * DDRPHY_AO_SHU_OFFSET);
			   break;
		 case 8:
			   u4BaseAddr = Channel_A_DDRPHY_DPM_BASE_ADDRESS;
			   break;
	}

	return (u4BaseAddr + u4Offset);
}

inline U32 _u4Dram_Register_Read(U64 u4reg_addr)
{
	U32 u4reg_value;
#if (!__ETT__) && (FOR_DV_SIMULATION_USED == 0)
	dsb();
#endif

#if QT_GUI_Tool
	ucDramRegRead_1(u4reg_addr, &u4reg_value);
#elif (FOR_DV_SIMULATION_USED == 1)   //DV
	u4reg_value = register_read_c(u4reg_addr);
#else // real chip
	u4reg_value = *((volatile unsigned int *)u4reg_addr);
#endif

	return u4reg_value;
}

//-------------------------------------------------------------------------
/** ucDram_Register_Read
 *	DRAM register read (32-bit).
 *	@param	u4reg_addr	  register address in 32-bit.
 *	@param	pu4reg_value  Pointer of register read value.
 *	@retval 0: OK, 1: FAIL
 */
//-------------------------------------------------------------------------
// This function need to be porting by BU requirement
U32 u4Dram_Register_Read(DRAMC_CTX_T *p, U32 u4reg_addr)
{
	u4reg_addr = u4RegBaseAddrTraslate(p->ShuRGAccessIdx, p->rank, u4reg_addr);

	return _u4Dram_Register_Read(u4reg_addr);
}


//-------------------------------------------------------------------------
/** ucDram_Register_Write
 *	DRAM register write (32-bit).
 *	@param	u4reg_addr	  register address in 32-bit.
 *	@param	u4reg_value   register write value.
 *	@retval 0: OK, 1: FAIL
 */
//-------------------------------------------------------------------------

#if REG_ACCESS_NAO_DGB
#if (fcFOR_CHIP_ID == fcCervino)
U8 Check_RG_Not_AO(U32 u4reg_addr)
{
	U8 RegNotAO = 0;
	if ((u4reg_addr >= DRAMC_AO_BASE_ADDRESS) && (u4reg_addr <= DRAMC_REG_SHU4_DQSG_RETRY))
	{
	}
	else if ((u4reg_addr >= DRAMC_AO_BASE_ADDRESS + SHIFT_TO_CHB_ADDR) && (u4reg_addr <= DRAMC_REG_SHU4_DQSG_RETRY + SHIFT_TO_CHB_ADDR))
	{
	}
	else if ((u4reg_addr >= DDRPHY_AO_BASE_ADDR) && (u4reg_addr <= DDRPHY_RFU_0X1FCC))
	{
	}
	else if ((u4reg_addr >= DDRPHY_AO_BASE_ADDR + SHIFT_TO_CHB_ADDR) && (u4reg_addr <= DDRPHY_RFU_0X1FCC + SHIFT_TO_CHB_ADDR))
	{
	}
	else
	{
		RegNotAO = 1;
	}
	return RegNotAO;
}
#endif
#endif

inline void _ucDram_Register_Write(U64 u4reg_addr, U32 u4reg_value)
{
#if QT_GUI_Tool
	 ucDramRegWrite_1(u4reg_addr, u4reg_value);
#elif (FOR_DV_SIMULATION_USED == 1) //DV
	register_write_c(u4reg_addr, u4reg_value);
#else // real chip
	(*(volatile unsigned int *)u4reg_addr) = u4reg_value;//real chip
	#if !defined(__MD32__)
	dsb();
	#endif
#endif

#ifdef DUMP_INIT_RG_LOG_TO_DE
	if (gDUMP_INIT_RG_LOG_TO_DE_RG_log_flag == 1)
	{
		mcSHOW_DUMP_INIT_RG_MSG("*(UINT32P)(0x%x) = 0x%x;\n",u4reg_addr,u4reg_value);
		gDUMP_INIT_RG_LOG_TO_DE_RG_log_flag = 0;
		mcDELAY_MS(1); // to receive log for log
		gDUMP_INIT_RG_LOG_TO_DE_RG_log_flag = 1;
	}
#endif

#if REG_ACCESS_PORTING_DGB
	if (RegLogEnable)
	{
		msg("\n[REG_ACCESS_PORTING_DBG]   ucDramC_Register_Write Reg(0x%X) = 0x%X\n", u4reg_addr, u4reg_value);
	}
#endif
}

//This function need to be porting by BU requirement
void ucDram_Register_Write(DRAMC_CTX_T *p, U32 u4reg_addr, U32 u4reg_value)
{
#if __ETT__
	//CheckDramcWBR(u4reg_addr);
#endif

	//msg("\n[REG_ACCESS_PORTING_DBG]	ucDramC_Register_Write Reg(0x%X) = 0x%X\n", u4reg_addr, u4reg_value);
	u4reg_addr = u4RegBaseAddrTraslate(p->ShuRGAccessIdx, p->rank, u4reg_addr);

	_ucDram_Register_Write(u4reg_addr, u4reg_value);
}

void vIO32Write4BMsk2(DRAMC_CTX_T *p, U32 reg32, U32 val32, U32 msk32)
{
	U32 u4Val;

	reg32 = u4RegBaseAddrTraslate(p->ShuRGAccessIdx, p->rank, reg32);

	val32 &= msk32;

	u4Val = _u4Dram_Register_Read(reg32);
	u4Val = ((u4Val & ~msk32) | val32);
	_ucDram_Register_Write(reg32, u4Val);
}


void vIO32Write4B_All2(DRAMC_CTX_T *p, U32 reg32, U32 val32)
{
U8 ii, u1AllCount;
U32 u4RegType = (reg32 & (0x1f << POS_BANK_NUM));

#if __ETT__
	if (GetDramcBroadcast()==DRAMC_BROADCAST_ON)
	{
		err("Error! virtual address 0x%x don't have to use write_all when Dramc WBR is on\n", reg32);
		while (1);
	}
#endif

	reg32 &= 0xffff;	 // remove channel information

	u1AllCount = CHANNEL_NUM; // for all dramC and PHY

	if (u4RegType >= Channel_A_DDRPHY_DPM_BASE_VIRTUAL)//DPM
	{
		reg32 += Channel_A_DDRPHY_DPM_BASE_VIRTUAL;
		if (u1AllCount > 1)
			u1AllCount >>= 1;
	}
	else if (u4RegType >= Channel_A_DDRPHY_AO_BASE_VIRTUAL)// PHY AO
	{
		reg32 += Channel_A_DDRPHY_AO_BASE_VIRTUAL;
	}
	else if (u4RegType >= Channel_A_DDRPHY_NAO_BASE_VIRTUAL)// PHY NAO
	{
		reg32 += Channel_A_DDRPHY_NAO_BASE_VIRTUAL;
	}
	else if (u4RegType >= Channel_A_DRAMC_AO_BASE_VIRTUAL)// DramC AO
	{
		reg32 += Channel_A_DRAMC_AO_BASE_VIRTUAL;
	}
	else // DramC NAO
	{
		reg32 += Channel_A_DRAMC_NAO_BASE_VIRTUAL;
	}

	for (ii = 0; ii < u1AllCount; ii++)
	{
		vIO32Write4B(reg32 + ((U32)ii << POS_BANK_NUM), val32);
	}
}

void vIO32Write4BMsk_All2(DRAMC_CTX_T *p, U32 reg32, U32 val32, U32 msk32)
{
	U32 u4Val, u4RegTmp;
	U8 ii, u1AllCount;
	U32 u4RegType = (reg32 & (0x1f << POS_BANK_NUM));

#if __ETT__
	if (GetDramcBroadcast()==DRAMC_BROADCAST_ON)
	{
		err("Error! virtual address 0x%x don't have to use write_all when Dramc WBR is on\n", reg32);
		while (1);
	}
#endif

	reg32 &= 0xffff;	 // remove channel information

	u1AllCount = CHANNEL_NUM; // for all dramC and PHY

	if (u4RegType >= Channel_A_DDRPHY_DPM_BASE_VIRTUAL)//DPM
	{
		reg32 += Channel_A_DDRPHY_DPM_BASE_VIRTUAL;
		if (u1AllCount > 1)
			u1AllCount >>= 1;
	}
	else if (u4RegType >= Channel_A_DDRPHY_AO_BASE_VIRTUAL)// PHY AO
	{
		reg32 += Channel_A_DDRPHY_AO_BASE_VIRTUAL;
	}
	else if (u4RegType >= Channel_A_DDRPHY_NAO_BASE_VIRTUAL)// PHY NAO
	{
		reg32 += Channel_A_DDRPHY_NAO_BASE_VIRTUAL;
	}
	else if (u4RegType >= Channel_A_DRAMC_AO_BASE_VIRTUAL)// DramC AO
	{
		reg32 += Channel_A_DRAMC_AO_BASE_VIRTUAL;
	}
	else // DramC NAO
	{
		reg32 += Channel_A_DRAMC_NAO_BASE_VIRTUAL;
	}

	for (ii = 0; ii < u1AllCount; ii++)
	{
		u4RegTmp = u4RegBaseAddrTraslate(p->ShuRGAccessIdx, p->rank, reg32 + ((U32)ii << POS_BANK_NUM));

		u4Val = _u4Dram_Register_Read(u4RegTmp);
		u4Val = ((u4Val & ~msk32) | val32);
		_ucDram_Register_Write(u4RegTmp, u4Val);
	}
}
