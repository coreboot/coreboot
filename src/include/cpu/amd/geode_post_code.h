/*
* This file is part of the LinuxBIOS project.
*
* Copyright (C) 2007 Advanced Micro Devices
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/
/* standard AMD post definitions -- might as well use them. */
#define POST_Output_Port				(0x080)	/*	port to write post codes to*/

#define POST_preSioInit					(0x000)
#define POST_clockInit					(0x001)
#define POST_CPURegInit					(0x002)
#define POST_UNREAL						(0x003)
#define POST_CPUMemRegInit				(0x004)
#define POST_CPUTest					(0x005)
#define POST_memSetup					(0x006)
#define POST_memSetUpStack				(0x007)
#define POST_memTest					(0x008)
#define POST_shadowRom					(0x009)
#define POST_memRAMoptimize				(0x00A)
#define POST_cacheInit					(0x00B)
#define POST_northBridgeInit			(0x00C)
#define POST_chipsetInit				(0x00D)
#define POST_sioTest					(0x00E)
#define POST_pcATjunk					(0x00F)


#define POST_intTable					(0x010)
#define POST_memInfo					(0x011)
#define POST_romCopy					(0x012)
#define POST_PLLCheck					(0x013)
#define POST_keyboardInit				(0x014)
#define POST_cpuCacheOff				(0x015)
#define POST_BDAInit					(0x016)
#define POST_pciScan					(0x017)
#define POST_optionRomInit				(0x018)
#define POST_ResetLimits				(0x019)
#define POST_summary_screen				(0x01A)
#define POST_Boot						(0x01B)
#define POST_SystemPreInit				(0x01C)
#define POST_ClearRebootFlag			(0x01D)
#define POST_GLIUInit					(0x01E)
#define POST_BootFailed					(0x01F)


#define POST_CPU_ID						(0x020)
#define POST_COUNTERBROKEN				(0x021)
#define POST_DIFF_DIMMS					(0x022)
#define POST_WIGGLE_MEM_LINES			(0x023)
#define POST_NO_GLIU_DESC				(0x024)
#define POST_CPU_LCD_CHECK				(0x025)
#define POST_CPU_LCD_PASS				(0x026)
#define POST_CPU_LCD_FAIL				(0x027)
#define POST_CPU_STEPPING				(0x028)
#define POST_CPU_DM_BIST_FAILURE		(0x029)
#define POST_CPU_FLAGS					(0x02A)
#define POST_CHIPSET_ID					(0x02b)
#define POST_CHIPSET_ID_PASS			(0x02c)
#define POST_CHIPSET_ID_FAIL			(0x02d)
#define POST_CPU_ID_GOOD				(0x02E)
#define POST_CPU_ID_FAIL				(0x02F)



/*	PCI config*/
#define P80_PCICFG					(0x030)


/*	PCI io*/
#define P80_PCIIO					(0x040)


/*	PCI memory*/
#define P80_PCIMEM					(0x050)


/*	SIO*/
#define P80_SIO						(0x060)

/*	Memory Setp*/
#define P80_MEM_SETUP				(0x070)
#define POST_MEM_SETUP				(0x070)
#define ERROR_32BIT_DIMMS			(0x071)
#define POST_MEM_SETUP2				(0x072)
#define POST_MEM_SETUP3				(0x073)
#define POST_MEM_SETUP4				(0x074)
#define POST_MEM_SETUP5				(0x075)
#define POST_MEM_ENABLE				(0x076)
#define ERROR_NO_DIMMS				(0x077)
#define ERROR_DIFF_DIMMS			(0x078)
#define ERROR_BAD_LATENCY			(0x079)
#define ERROR_SET_PAGE				(0x07a)
#define ERROR_DENSITY_DIMM			(0x07b)
#define ERROR_UNSUPPORTED_DIMM		(0x07c)
#define ERROR_BANK_SET				(0x07d)
#define POST_MEM_SETUP_GOOD			(0x07E)
#define POST_MEM_SETUP_FAIL			(0x07F)


#define POST_UserPreInit				(0x080)
#define POST_UserPostInit				(0x081)
#define POST_Equipment_check			(0x082)
#define POST_InitNVRAMBX				(0x083)
#define POST_NoPIRTable					(0x084)
#define POST_ChipsetFingerPrintPass		(0x085)
#define POST_ChipsetFingerPrintFail		(0x086)
#define POST_CPU_IM_TAG_BIST_FAILURE	(0x087)
#define POST_CPU_IM_DATA_BIST_FAILURE	(0x088)
#define POST_CPU_FPU_BIST_FAILURE		(0x089)
#define POST_CPU_BTB_BIST_FAILURE		(0x08a)
#define POST_CPU_EX_BIST_FAILURE		(0x08b)
#define POST_Chipset_PI_Test_Fail			(0x08c)
#define POST_Chipset_SMBus_SDA_Test_Fail	(0x08d)
#define POST_BIT_CLK_Fail				(0x08e)


#define POST_STACK_SETUP				(0x090)
#define POST_CPU_PF_BIST_FAILURE		(0x091)
#define POST_CPU_L2_BIST_FAILURE		(0x092)
#define POST_CPU_GLCP_BIST_FAILURE		(0x093)
#define POST_CPU_DF_BIST_FAILURE		(0x094)
#define POST_CPU_VG_BIST_FAILURE		(0x095)
#define POST_CPU_VIP_BIST_FAILURE		(0x096)
#define POST_STACK_SETUP_PASS			(0x09E)
#define POST_STACK_SETUP_FAIL			(0x09F)


#define POST_PLL_INIT					(0x0A0)
#define POST_PLL_MANUAL					(0x0A1)
#define POST_PLL_STRAP					(0x0A2)
#define POST_PLL_RESET_FAIL				(0x0A3)
#define POST_PLL_PCI_FAIL				(0x0A4)
#define POST_PLL_MEM_FAIL				(0x0A5)
#define POST_PLL_CPU_VER_FAIL			(0x0A6)


#define POST_MEM_TESTMEM			(0x0B0)
#define POST_MEM_TESTMEM1			(0x0B1)
#define POST_MEM_TESTMEM2			(0x0B2)
#define POST_MEM_TESTMEM3			(0x0B3)
#define POST_MEM_TESTMEM4			(0x0B4)
#define POST_MEM_TESTMEM_PASS		(0x0BE)
#define POST_MEM_TESTMEM_FAIL		(0x0BF)


#define POST_SECUROM_SECBOOT_START		(0x0C0)
#define POST_SECUROM_BOOTSRCSETUP		(0x0C1)
#define POST_SECUROM_REMAP_FAIL			(0x0C2)
#define POST_SECUROM_BOOTSRCSETUP_FAIL	(0x0C3)
#define POST_SECUROM_DCACHESETUP		(0x0C4)
#define POST_SECUROM_DCACHESETUP_FAIL	(0x0C5)
#define POST_SECUROM_ICACHESETUP		(0x0C6)
#define POST_SECUROM_DESCRIPTORSETUP	(0x0C7)
#define POST_SECUROM_DCACHESETUPBIOS	(0x0C8)
#define POST_SECUROM_PLATFORMSETUP		(0x0C9)
#define POST_SECUROM_SIGCHECKBIOS		(0x0CA)
#define POST_SECUROM_ICACHESETUPBIOS	(0x0CB)
#define POST_SECUROM_PASS				(0x0CC)
#define POST_SECUROM_FAIL				(0x0CD)

#define POST_RCONFInitError				(0x0CE)
#define POST_CacheInitError				(0x0CF)


#define POST_ROM_PREUNCOMPRESS			(0x0D0)
#define POST_ROM_UNCOMPRESS				(0x0D1)
#define POST_ROM_SMM_INIT				(0x0D2)
#define POST_ROM_VID_BIOS				(0x0D3)
#define POST_ROM_LCDINIT				(0x0D4)
#define POST_ROM_SPLASH					(0x0D5)
#define POST_ROM_HDDINIT				(0x0D6)
#define POST_ROM_SYS_INIT				(0x0D7)
#define POST_ROM_DMM_INIT				(0x0D8)
#define POST_ROM_TVINIT					(0x0D9)
#define POST_ROM_POSTUNCOMPRESS			(0x0DE)


#define P80_CHIPSET_INIT				(0x0E0)
#define POST_PreChipsetInit				(0x0E1)
#define POST_LateChipsetInit			(0x0E2)
#define POST_NORTHB_INIT				(0x0E8)


#define POST_INTR_SEG_JUMP				(0x0F0)
