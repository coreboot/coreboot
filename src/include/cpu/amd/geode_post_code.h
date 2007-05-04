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

#define POST_preSioInit					(0x000)	/* geode.asm*/
#define POST_clockInit					(0x001)	/* geode.asm*/
#define POST_CPURegInit					(0x002)	/* geode.asm*/
#define POST_UNREAL						(0x003)	/* geode.asm*/
#define POST_CPUMemRegInit				(0x004)	/* geode.asm*/
#define POST_CPUTest					(0x005)	/* geode.asm*/
#define POST_memSetup					(0x006)	/* geode.asm*/
#define POST_memSetUpStack				(0x007)	/* geode.asm*/
#define POST_memTest					(0x008)	/* geode.asm*/
#define POST_shadowRom					(0x009)	/* geode.asm*/
#define POST_memRAMoptimize				(0x00A)	/* geode.asm*/
#define POST_cacheInit					(0x00B)	/* geode.asm*/
#define POST_northBridgeInit			(0x00C)	/* geode.asm*/
#define POST_chipsetInit				(0x00D)	/* geode.asm*/
#define POST_sioTest					(0x00E)	/* geode.asm*/
#define POST_pcATjunk					(0x00F)	/* geode.asm*/


#define POST_intTable					(0x010)	/* geode.asm*/
#define POST_memInfo					(0x011)	/* geode.asm*/
#define POST_romCopy					(0x012)	/* geode.asm*/
#define POST_PLLCheck					(0x013)	/* geode.asm*/
#define POST_keyboardInit				(0x014)	/* geode.asm*/
#define POST_cpuCacheOff				(0x015)	/* geode.asm*/
#define POST_BDAInit					(0x016)	/* geode.asm*/
#define POST_pciScan					(0x017)	/* geode.asm*/
#define POST_optionRomInit				(0x018)	/* geode.asm*/
#define POST_ResetLimits				(0x019)	/* geode.asm*/
#define POST_summary_screen				(0x01A)	/* geode.asm*/
#define POST_Boot						(0x01B)	/* geode.asm*/
#define POST_SystemPreInit				(0x01C)	/* geode.asm*/
#define POST_ClearRebootFlag			(0x01D)	/* geode.asm*/
#define POST_GLIUInit					(0x01E)	/* geode.asm*/
#define POST_BootFailed					(0x01F)	/* geode.asm*/


#define POST_CPU_ID						(0x020)	/* cpucpuid.asm*/
#define POST_COUNTERBROKEN				(0x021)	/* pllinit.asm*/
#define POST_DIFF_DIMMS					(0x022)	/* pllinit.asm*/
#define POST_WIGGLE_MEM_LINES			(0x023)	/* pllinit.asm*/
#define POST_NO_GLIU_DESC				(0x024)	/* pllinit.asm*/
#define POST_CPU_LCD_CHECK				(0x025)	/* pllinit.asm*/
#define POST_CPU_LCD_PASS				(0x026)	/* pllinit.asm*/
#define POST_CPU_LCD_FAIL				(0x027)	/* pllinit.asm*/
#define POST_CPU_STEPPING				(0x028)	/* cpucpuid.asm*/
#define POST_CPU_DM_BIST_FAILURE		(0x029)	/* gx2reg.asm*/
#define POST_CPU_FLAGS					(0x02A)	/* cpucpuid.asm*/
#define POST_CHIPSET_ID					(0x02b)	/* chipset.asm*/
#define POST_CHIPSET_ID_PASS			(0x02c)	/* chipset.asm*/
#define POST_CHIPSET_ID_FAIL			(0x02d)	/* chipset.asm*/
#define POST_CPU_ID_GOOD				(0x02E)	/* cpucpuid.asm*/
#define POST_CPU_ID_FAIL				(0x02F)	/* cpucpuid.asm*/



/*	PCI config*/
#define P80_PCICFG					(0x030)	/*	pcispace.asm*/


/*	PCI io*/
#define P80_PCIIO					(0x040)	/*	pcispace.asm*/


/*	PCI memory*/
#define P80_PCIMEM					(0x050)	/*	pcispace.asm*/


/*	SIO*/
#define P80_SIO						(0x060)		/*	*sio.asm*/

/*	Memory Setp*/
#define P80_MEM_SETUP				(0x070)	/* docboot meminit*/
#define POST_MEM_SETUP				(0x070)	/* memsize.asm*/
#define ERROR_32BIT_DIMMS			(0x071)	/* memsize.asm*/
#define POST_MEM_SETUP2				(0x072)	/* memsize.asm*/
#define POST_MEM_SETUP3				(0x073)	/* memsize.asm*/
#define POST_MEM_SETUP4				(0x074)	/* memsize.asm*/
#define POST_MEM_SETUP5				(0x075)	/* memsize.asm*/
#define POST_MEM_ENABLE				(0x076)	/* memsize.asm*/
#define ERROR_NO_DIMMS				(0x077)	/* memsize.asm*/
#define ERROR_DIFF_DIMMS			(0x078)	/* memsize.asm*/
#define ERROR_BAD_LATENCY			(0x079)	/* memsize.asm*/
#define ERROR_SET_PAGE				(0x07a)	/* memsize.asm*/
#define ERROR_DENSITY_DIMM			(0x07b)	/* memsize.asm*/
#define ERROR_UNSUPPORTED_DIMM		(0x07c)	/* memsize.asm*/
#define ERROR_BANK_SET				(0x07d)	/* memsize.asm*/
#define POST_MEM_SETUP_GOOD			(0x07E)	/* memsize.asm*/
#define POST_MEM_SETUP_FAIL			(0x07F)	/* memsize.asm*/


#define POST_UserPreInit				(0x080)	/* geode.asm*/
#define POST_UserPostInit				(0x081)	/* geode.asm*/
#define POST_Equipment_check			(0x082)	/* geode.asm*/
#define POST_InitNVRAMBX				(0x083)	/* geode.asm*/
#define POST_NoPIRTable					(0x084)	/* pci.asm*/
#define POST_ChipsetFingerPrintPass		(0x085)	/*	prechipsetinit*/
#define POST_ChipsetFingerPrintFail		(0x086)	/*	prechipsetinit*/
#define POST_CPU_IM_TAG_BIST_FAILURE	(0x087)	/*	gx2reg.asm*/
#define POST_CPU_IM_DATA_BIST_FAILURE	(0x088)	/*	gx2reg.asm*/
#define POST_CPU_FPU_BIST_FAILURE		(0x089)	/*	gx2reg.asm*/
#define POST_CPU_BTB_BIST_FAILURE		(0x08a)	/*	gx2reg.asm*/
#define POST_CPU_EX_BIST_FAILURE		(0x08b)	/*	gx2reg.asm*/
#define POST_Chipset_PI_Test_Fail			(0x08c)	/*	prechipsetinit*/
#define POST_Chipset_SMBus_SDA_Test_Fail	(0x08d)	/*	prechipsetinit*/
#define POST_BIT_CLK_Fail				(0x08e)	/*	Hawk geode.asm override*/


#define POST_STACK_SETUP				(0x090)	/* memstack.asm*/
#define POST_CPU_PF_BIST_FAILURE		(0x091)	/*	gx2reg.asm*/
#define POST_CPU_L2_BIST_FAILURE		(0x092)	/*	gx2reg.asm*/
#define POST_CPU_GLCP_BIST_FAILURE		(0x093)	/*	gx2reg.asm*/
#define POST_CPU_DF_BIST_FAILURE		(0x094)	/*	gx2reg.asm*/
#define POST_CPU_VG_BIST_FAILURE		(0x095)	/*	gx2reg.asm*/
#define POST_CPU_VIP_BIST_FAILURE		(0x096)	/*	gx2reg.asm*/
#define POST_STACK_SETUP_PASS			(0x09E)	/* memstack.asm*/
#define POST_STACK_SETUP_FAIL			(0x09F)	/* memstack.asm*/


#define POST_PLL_INIT					(0x0A0)	/* pllinit.asm*/
#define POST_PLL_MANUAL					(0x0A1)	/* pllinit.asm*/
#define POST_PLL_STRAP					(0x0A2)	/* pllinit.asm*/
#define POST_PLL_RESET_FAIL				(0x0A3)	/* pllinit.asm*/
#define POST_PLL_PCI_FAIL				(0x0A4)	/* pllinit.asm*/
#define POST_PLL_MEM_FAIL				(0x0A5)	/* pllinit.asm*/
#define POST_PLL_CPU_VER_FAIL			(0x0A6)	/* pllinit.asm*/


#define POST_MEM_TESTMEM			(0x0B0)	/* memtest.asm*/
#define POST_MEM_TESTMEM1			(0x0B1)	/* memtest.asm*/
#define POST_MEM_TESTMEM2			(0x0B2)	/* memtest.asm*/
#define POST_MEM_TESTMEM3			(0x0B3)	/* memtest.asm*/
#define POST_MEM_TESTMEM4			(0x0B4)	/* memtest.asm*/
#define POST_MEM_TESTMEM_PASS		(0x0BE)	/* memtest.asm*/
#define POST_MEM_TESTMEM_FAIL		(0x0BF)	/* memtest.asm*/


#define POST_SECUROM_SECBOOT_START		(0x0C0)	/* secstart.asm*/
#define POST_SECUROM_BOOTSRCSETUP		(0x0C1)	/* secstart.asm*/
#define POST_SECUROM_REMAP_FAIL			(0x0C2)	/* secstart.asm*/
#define POST_SECUROM_BOOTSRCSETUP_FAIL	(0x0C3)	/* secstart.asm*/
#define POST_SECUROM_DCACHESETUP		(0x0C4)	/* secstart.asm*/
#define POST_SECUROM_DCACHESETUP_FAIL	(0x0C5)	/* secstart.asm*/
#define POST_SECUROM_ICACHESETUP		(0x0C6)	/* secstart.asm*/
#define POST_SECUROM_DESCRIPTORSETUP	(0x0C7)	/* secstart.asm*/
#define POST_SECUROM_DCACHESETUPBIOS	(0x0C8)	/* secstart.asm*/
#define POST_SECUROM_PLATFORMSETUP		(0x0C9)	/* secstart.asm*/
#define POST_SECUROM_SIGCHECKBIOS		(0x0CA)	/* secstart.asm*/
#define POST_SECUROM_ICACHESETUPBIOS	(0x0CB)	/* secstart.asm*/
#define POST_SECUROM_PASS				(0x0CC)	/* secstart.asm*/
#define POST_SECUROM_FAIL				(0x0CD)	/* secstart.asm*/

#define POST_RCONFInitError				(0x0CE)	/* cache.asm*/
#define POST_CacheInitError				(0x0CF)	/* cache.asm*/


#define POST_ROM_PREUNCOMPRESS			(0x0D0)	/* rominit.asm*/
#define POST_ROM_UNCOMPRESS				(0x0D1)	/* rominit.asm*/
#define POST_ROM_SMM_INIT				(0x0D2)	/* rominit.asm*/
#define POST_ROM_VID_BIOS				(0x0D3)	/* rominit.asm*/
#define POST_ROM_LCDINIT				(0x0D4)	/* rominit.asm*/
#define POST_ROM_SPLASH					(0x0D5)	/* rominit.asm*/
#define POST_ROM_HDDINIT				(0x0D6)	/* rominit.asm*/
#define POST_ROM_SYS_INIT				(0x0D7)	/* rominit.asm*/
#define POST_ROM_DMM_INIT				(0x0D8)	/* rominit.asm*/
#define POST_ROM_TVINIT					(0x0D9)	/* rominit.asm*/
#define POST_ROM_POSTUNCOMPRESS			(0x0DE)


#define P80_CHIPSET_INIT				(0x0E0)	/* chipset.asm*/
#define POST_PreChipsetInit				(0x0E1)	/* geode.asm*/
#define POST_LateChipsetInit			(0x0E2)	/* geode.asm*/
#define POST_NORTHB_INIT				(0x0E8)	/* northb.asm*/


#define POST_INTR_SEG_JUMP				(0x0F0)	/* vector.asm*/
