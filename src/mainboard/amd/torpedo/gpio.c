/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "SbPlatform.h"
#include "gpio.h"
#include "vendorcode/amd/cimx/sb900/AmdSbLib.h"


#ifndef SB_GPIO_REG01
#define SB_GPIO_REG01   1
#endif

#ifndef SB_GPIO_REG07
#define SB_GPIO_REG07   7
#endif

#ifndef SB_GPIO_REG25
#define SB_GPIO_REG25   25
#endif

#ifndef SB_GPIO_REG26
#define SB_GPIO_REG26   26
#endif

#ifndef SB_GPIO_REG27
#define SB_GPIO_REG27   27
#endif

void gpioEarlyInit(void) {
	u8 Flags;
	u8 Data8 = 0;
	u8 StripInfo = 0;
	u8 BoardType = 1;
	u8 RegIndex8 = 0;
	u8 boardRevC = 0x2;
	u16 Data16 = 0;
	u32 Index = 0;
	u32 AcpiMmioAddr = 0;
	u32 GpioMmioAddr = 0;
	u32 IoMuxMmioAddr = 0;
	u32 MiscMmioAddr = 0;
	u32 SmiMmioAddr = 0;
	u32 andMask32 = 0;

	// Enable HUDSON MMIO Base (AcpiMmioAddr)
	ReadPMIO (SB_PMIOA_REG24, AccWidthUint8, &Data8);
	Data8 |= BIT0;
	WritePMIO (SB_PMIOA_REG24, AccWidthUint8, &Data8);
	// Get HUDSON MMIO Base (AcpiMmioAddr)
	ReadPMIO (SB_PMIOA_REG24 + 3, AccWidthUint8, &Data8);
	Data16 = Data8 << 8;
	ReadPMIO (SB_PMIOA_REG24 + 2, AccWidthUint8, &Data8);
	Data16 |= Data8;
	AcpiMmioAddr = (u32)Data16 << 16;
	GpioMmioAddr = AcpiMmioAddr + GPIO_BASE;
	IoMuxMmioAddr = AcpiMmioAddr + IOMUX_BASE;
	MiscMmioAddr =  AcpiMmioAddr + MISC_BASE;
	Data8 = Mmio8_G (MiscMmioAddr, SB_MISC_REG80);
	if ((Data8 & BIT4) == 0) {
		BoardType = 0; // external clock board
	}
	Data8 = Mmio8_G (GpioMmioAddr, GPIO_30);
	StripInfo = (Data8 & BIT7) >> 7;
	Data8 = Mmio8_G (GpioMmioAddr, GPIO_31);
	StripInfo |= (Data8 & BIT7) >> 6;
	if (StripInfo < boardRevC) {		// for old board. Rev B
		Mmio8_And_Or (IoMuxMmioAddr, GPIO_111, 0x00, 3);		// function 3
		Mmio8_And_Or (IoMuxMmioAddr, GPIO_113, 0x00, 0);		// function 0
	}
	for (Index = 0; Index < MAX_GPIO_NO; Index++) {
		if (!(((Index >= GPIO_RSVD_ZONE0_S) && (Index <= GPIO_RSVD_ZONE0_E)) || ((Index >= GPIO_RSVD_ZONE1_S) && (Index <= GPIO_RSVD_ZONE1_E)))) {
			if ((StripInfo >= boardRevC) || ((Index != GPIO_111) && (Index != GPIO_113))) {
				// Configure multi-function
				Mmio8_And_Or (IoMuxMmioAddr, Index, 0x00, (gpio_table[Index].select & ~NonGpio));
			}
			// Configure GPIO
			if(!((gpio_table[Index].NonGpioGevent & NonGpio))) {
				Mmio8_And_Or (GpioMmioAddr, Index, 0xDF, gpio_table[Index].type);
				Mmio8_And_Or (GpioMmioAddr, Index, 0xA3, gpio_table[Index].value);
			}
			if (Index == GPIO_65) {
				if ( BoardType == 0 ) {
					Mmio8_And_Or (IoMuxMmioAddr, GPIO_65, 0x00, 3);		// function 3
				}
			}
		}
		// Configure GEVENT
		if ((Index >= GEVENT_00) && (Index <= GEVENT_23) && ((gevent_table[Index - GEVENT_00].EventEnable))) {
			SmiMmioAddr = AcpiMmioAddr + SMI_BASE;

			andMask32 = ~(1 << (Index - GEVENT_00));

			//EventEnable: 0-Disable, 1-Enable
			Mmio32_And_Or(SmiMmioAddr, SMIREG_EVENT_ENABLE, andMask32, (gevent_table[Index - GEVENT_00].EventEnable << (Index - GEVENT_00)));

			//SciTrig: 0-Falling Edge, 1-Rising Edge
			Mmio32_And_Or(SmiMmioAddr, SMIREG_SCITRIG, andMask32, (gevent_table[Index - GEVENT_00].SciTrig << (Index - GEVENT_00)));

			//SciLevl: 0-Edge trigger, 1-Level Trigger
			Mmio32_And_Or(SmiMmioAddr, SMIREG_SCILEVEL, andMask32, (gevent_table[Index - GEVENT_00].SciLevl << (Index - GEVENT_00)));

			//SmiSciEn: 0-Not send SMI, 1-Send SMI
			Mmio32_And_Or(SmiMmioAddr, SMIREG_SMISCIEN, andMask32, (gevent_table[Index - GEVENT_00].SmiSciEn << (Index - GEVENT_00)));

			//SciS0En: 0-Disable, 1-Enable
			Mmio32_And_Or(SmiMmioAddr, SMIREG_SCIS0EN, andMask32, (gevent_table[Index - GEVENT_00].SciS0En << (Index - GEVENT_00)));

			//SciMap: 00000b ~ 11111b
			RegIndex8 = (u8)((Index - GEVENT_00) >> 2);
			Data8 = (u8)(((Index - GEVENT_00) & 0x3) * 8);
			Mmio32_And_Or(SmiMmioAddr, SMIREG_SCIMAP0+RegIndex8, ~(GEVENT_SCIMASK << Data8), (gevent_table[Index - GEVENT_00].SciMap << Data8));

			//SmiTrig: 0-Active Low, 1-Active High
			Mmio32_And_Or(SmiMmioAddr, SMIREG_SMITRIG, ~(gevent_table[Index - GEVENT_00].SmiTrig << (Index - GEVENT_00)), (gevent_table[Index - GEVENT_00].SmiTrig << (Index - GEVENT_00)));

			//SmiControl: 0-Disable, 1-SMI, 2-NMI, 3-IRQ13
			RegIndex8 = (u8)((Index - GEVENT_00) >> 4);
			Data8 = (u8)(((Index - GEVENT_00) & 0xF) * 2);
			Mmio32_And_Or(SmiMmioAddr, SMIREG_SMICONTROL0+RegIndex8, ~(SMICONTROL_MASK << Data8), (gevent_table[Index - GEVENT_00].SmiControl << Data8));
		}
	}

	//
	// config MXM
	//	GPIO9: Input for MXM_PRESENT2#
	//	GPIO10: Input for MXM_PRESENT1#
	//	GPIO28: Input for MXM_PWRGD
	//	GPIO35: Output for MXM Reset
	//	GPIO45: Output for MXM Power Enable, active HIGH
	//	GPIO55: Output for MXM_PWR_EN, 1 - Enable, 0 - Disable
	//	GPIO32: Output for PCIE_SW, 1 - MXM, 0 - LASSO
	//
		// set INTE#/GPIO32 as GPO for PCIE_SW
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG32, AccWidthUint8, 00, 0x1);      // GPIO
		RWMEM (GpioMmioAddr + SB_GPIO_REG32, AccWidthUint8, 0x03, 0);       // GPO
		RWMEM (GpioMmioAddr + SB_GPIO_REG32, AccWidthUint8, 0x23, BIT3+BIT6);

		// set SATA_IS4#/FANOUT3/GPIO55 as GPO for MXM_PWR_EN
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG55, AccWidthUint8, 00, 0x2);      // GPIO
		RWMEM (GpioMmioAddr + SB_GPIO_REG55, AccWidthUint8, 0x03, 0);       // GPO

		// set AD9/GPIO9 as GPI for MXM_PRESENT2#
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG09, AccWidthUint8, 00, 0x1);      // GPIO
		RWMEM (GpioMmioAddr + SB_GPIO_REG09, AccWidthUint8, 0x03, BIT5);    // GPI

		// set AD10/GPIO10 as GPI for MXM_PRESENT1#
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG10, AccWidthUint8, 00, 0x1);      // GPIO
		RWMEM (GpioMmioAddr + SB_GPIO_REG10, AccWidthUint8, 0x03, BIT5);    // GPI

		// set GNT1#/GPIO44 as GPO for MXM Reset
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG44, AccWidthUint8, 00, 0x1);      // GPIO
		RWMEM (GpioMmioAddr + SB_GPIO_REG44, AccWidthUint8, 0x03, 0);       // GPO

		// set GNT2#/SD_LED/GPO45 as GPO for MXM Power Enable
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG45, AccWidthUint8, 00, 0x2);      // GPIO
		RWMEM (GpioMmioAddr + SB_GPIO_REG45, AccWidthUint8, 0x03, 0);       // GPO

		// set AD28/GPIO28 as GPI for MXM_PWRGD
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG28, AccWidthUint8, 00, 0x1);      // GPIO
		RWMEM (GpioMmioAddr + SB_GPIO_REG28, AccWidthUint8, 0x03, BIT5);    // GPI

		// set BIT3 = 1 (PULLUP disable), BIT4 = 0 (PULLDOWN Disable), BIT6 = 0 (Output LOW)
		RWMEM (GpioMmioAddr + SB_GPIO_REG55, AccWidthUint8, 0x23, BIT3);
		RWMEM (GpioMmioAddr + SB_GPIO_REG09, AccWidthUint8, 0x23, BIT3);
		RWMEM (GpioMmioAddr + SB_GPIO_REG10, AccWidthUint8, 0x23, BIT3);
		RWMEM (GpioMmioAddr + SB_GPIO_REG44, AccWidthUint8, 0x23, BIT3);
		RWMEM (GpioMmioAddr + SB_GPIO_REG45, AccWidthUint8, 0x23, BIT3);
		RWMEM (GpioMmioAddr + SB_GPIO_REG28, AccWidthUint8, 0x23, BIT3);

		//
		// [GPIO] STRP_DATA: 1->RS880M VCC_NB 1.0V. 0->RS880M VCC_NB 1.1V (Default).
		//
		//Fusion_Llano BLWriteNBMISC_Dword (ATI_MISC_REG42, (BLReadNBMISC_Dword (ATI_MISC_REG42) | BIT20));
		//Fusion_Llano BLWriteNBMISC_Dword (ATI_MISC_REG40, (BLReadNBMISC_Dword (ATI_MISC_REG40) & (~BIT20)));

		// check if there any GFX card
		Flags = 0;
		// Value32 = MmPci32 (0, SB_ISA_BUS, SB_ISA_DEV, SB_ISA_FUNC, R_SB_ISA_GPIO_CONTROL);
		// Data8 = Mmio8 (GpioMmioAddr, SB_GPIO_REG09);
		ReadMEM (GpioMmioAddr + SB_GPIO_REG09, AccWidthUint8, &Data8);
		if (!(Data8 & BIT7))
		{
			//Data8 = Mmio8 (GpioMmioAddr, SB_GPIO_REG10);
			ReadMEM (GpioMmioAddr + SB_GPIO_REG10, AccWidthUint8, &Data8);
			if (!(Data8 & BIT7))
			{
				Flags = 1;
			}
		}
		if (Flags)
		{
			// [GPIO] GPIO44: PE_GPIO0 MXM Reset set to 0 for reset, ENH164467
			RWMEM (GpioMmioAddr + SB_GPIO_REG44, AccWidthUint8, 0xBF, 0);

			// [GPIO] GPIO45: PE_GPIO1 MXM_POWER_ENABLE, SET HIGH
			RWMEM (GpioMmioAddr + SB_GPIO_REG45, AccWidthUint8, 0xFF, BIT6);

			//PeiStall (PeiServices, NULL, 100); //delay 100 ms (should be >1ms)
			SbStall (10000);

			// Write the GPIO55(MXM_PWR_EN) to enable the integrated power module
			RWMEM (GpioMmioAddr + SB_GPIO_REG55, AccWidthUint8, 0xFF, BIT6);

			//PeiStall (PeiServices, NULL, 100); //delay 100 ms (should be >1ms)
			// WAIT POWER READY: GPIO28 (MXM_PWRGD)
			//while (!(Mmio8 (GpioMmioAddr, SB_GPIO_REG28) && BIT7)){}
			ReadMEM (GpioMmioAddr + SB_GPIO_REG28, AccWidthUint8, &Data8);
			while (!(Data8 & BIT7))
			{
				ReadMEM (GpioMmioAddr + SB_GPIO_REG28, AccWidthUint8, &Data8);
			}
			// [GPIO] GPIO44: PE_GPIO0 MXM Reset set to 1 for reset
			//RWMEM (GpioMmioAddr + SB_GPIO_REG44, AccWidthUint8, 0xBF, BIT6);
		}
		else
		{
			// Write the GPIO55(MXM_PWR_EN) to disable the integrated power module
			RWMEM (GpioMmioAddr + SB_GPIO_REG55, AccWidthUint8, 0xBF, 0);

			//PeiStall (PeiServices, NULL, 100); //delay 100 ms (should be >1ms)
			SbStall (10000);

			// [GPIO] GPIO45: PE_GPIO1 MXM_POWER_ENABLE down
			RWMEM (GpioMmioAddr + SB_GPIO_REG45, AccWidthUint8, 0xBF, 0);
		}

	//
	// APU GPP0: On board LAN
	//	GPIO25: PCIE_RST#_LAN, LOW active
	//	GPIO63: LAN_CLKREQ#
	//	GPIO197: LOM_POWER, HIGH Active
	//	Clock: GPP_CLK3
	//
		// Set EC_PWM0/EC_TIMER0/GPIO197 as GPO for LOM_POWER
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG197, AccWidthUint8, 00, 0x2);         // GPIO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG197, AccWidthUint8, 0x03, 0);          // GPO
		RWMEM (GpioMmioAddr + SB_GPIO_REG197, AccWidthUint8, 0x03, BIT6);       // output HIGH
		RWMEM (GpioMmioAddr + SB_GPIO_REG197, AccWidthUint8, 0x63, BIT3);       // pullup DISABLE

		// Setup AD25/GPIO25 as GPO for PCIE_RST#_LAN:
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG25, AccWidthUint8, 00, 0x1);          // GPIO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG25, AccWidthUint8, 0x03, 0);           // GPO
		RWMEM (GpioMmioAddr + SB_GPIO_REG25, AccWidthUint8, 0x03, BIT6);        // output HIGH
		RWMEM (GpioMmioAddr + SB_GPIO_REG25, AccWidthUint8, 0x63, BIT3);        // pullup DISABLE


		// set CLK_REQ3#/SATA_IS1#/GPIO63 as CLK_REQ for LAN_CLKREQ#
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG63, AccWidthUint8, 00, 0x0);          // CLK_REQ3#
		RWMEM (MiscMmioAddr + SB_MISC_REG00+1, AccWidthUint8, 0x0F, 0xF0);       // Enable GPP_CLK3

	//
	// APU GPP1: WUSB
	//	GPIO1: MPCIE_RST2#, LOW active
	//	GPIO13: WU_DISABLE#, LOW active
	//	GPIO177: MPICE_PD2, 1 - DISABLE, 0 - ENABLE (Default)
	//
		// Setup VIN2/SATA1_1/GPIO177 as GPO for MPCIE_PD2#: wireless disable
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG177, AccWidthUint8, 00, 0x2);     // GPIO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG177, AccWidthUint8, 0x03, 0);      // GPO
		RWMEM (GpioMmioAddr + SB_GPIO_REG177, AccWidthUint8, 0x03, 0);      // output LOW
		RWMEM (GpioMmioAddr + SB_GPIO_REG177, AccWidthUint8, 0x63, BIT3);   // pullup DISABLE

		// Setup AD01/GPIO01 as GPO for MPCIE_RST2#
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG01, AccWidthUint8, 00, 0x1);      // GPIO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG01, AccWidthUint8, 0x03, 0);       // GPO
		RWMEM (GpioMmioAddr + SB_GPIO_REG01, AccWidthUint8, 0x03, BIT6);    // output LOW
		RWMEM (GpioMmioAddr + SB_GPIO_REG01, AccWidthUint8, 0x63, BIT3);    // pullup DISABLE

		// Setup AD13/GPIO13 as GPO for WU_DISABLE#: disable WUSB
//		RWMEM (IoMuxMmioAddr + SB_GPIO_REG13, AccWidthUint8, 00, 0x1);      // GPIO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG13, AccWidthUint8, 0x03, 0);       // GPO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG13, AccWidthUint8, 0x03, BIT6);    // output HIGH
//		RWMEM (GpioMmioAddr + SB_GPIO_REG13, AccWidthUint8, 0x63, BIT3);    // pullup DISABLE

	//
	// APU GPP2: WWAN
	//	GPIO0: MPCIE_RST1#, LOW active
	//	GPIO14: WP_DISABLE#, LOW active
	//	GPIO176: MPICE_PD1, 1 - DISABLE, 0 - ENABLE (Default)
	//
		// Set VIN1/GPIO176 as GPO for MPCIE_PD1# for wireless disable
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG176, AccWidthUint8, 00, 0x1);     // GPIO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG176, AccWidthUint8, 0x03, 0);      // GPO
		RWMEM (GpioMmioAddr + SB_GPIO_REG176, AccWidthUint8, 0x03, 0);      // output LOW
		RWMEM (GpioMmioAddr + SB_GPIO_REG176, AccWidthUint8, 0x63, BIT3);   // pullup DISABLE

		// Set AD00/GPIO00 as GPO for MPCIE_RST1#
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG00, AccWidthUint8, 00, 0x1);      // GPIO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG00, AccWidthUint8, 0x03, 0);       // GPO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG00, AccWidthUint8, 0x03, BIT6);    // output LOW
		RWMEM (GpioMmioAddr + SB_GPIO_REG00, AccWidthUint8, 0x63, BIT3);    // pullup DISABLE

		// Set AD14/GPIO14 as GPO for WP_DISABLE#: disable WWAN
//		RWMEM (IoMuxMmioAddr + SB_GPIO_REG14, AccWidthUint8, 00, 0x1);      // GPIO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG14, AccWidthUint8, 0x03, 0);       // GPO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG14, AccWidthUint8, 0x03, BIT6);
//		RWMEM (GpioMmioAddr + SB_GPIO_REG14, AccWidthUint8, 0x63, BIT3);

	//
	// APU GPP3: 1394
	//	GPIO59: Power control, HIGH active
	//	GPIO27: PCIE_RST#_1394, LOW active
	//	GPIO41: CLKREQ#
	//	Clock: GPP_CLK8
	//
		// Setup SATA_IS5#/FANIN3/GPIO59 as GPO for 1394_ON:
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG59, AccWidthUint8, 00, 0x2);         // GPIO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG59, AccWidthUint8, 0x03, 0);          // GPO
		RWMEM (GpioMmioAddr + SB_GPIO_REG59, AccWidthUint8, 0x03, BIT6);       // output HIGH
		RWMEM (GpioMmioAddr + SB_GPIO_REG59, AccWidthUint8, 0x63, BIT3);       // pullup DISABLE

		// Setup AD27/GPIO27 as GPO for MPCIE_RST#_1394
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG27, AccWidthUint8, 00, 0x1);         // GPIO
//		RWMEM (GpioMmioAddr + SB_GPIO_REG27, AccWidthUint8, 0x03, 0);          // GPO
		RWMEM (GpioMmioAddr + SB_GPIO_REG27, AccWidthUint8, 0x03, BIT6);       // output HIGH
		RWMEM (GpioMmioAddr + SB_GPIO_REG27, AccWidthUint8, 0x63, BIT3);       // pullup DISABLE

		// set REQ2#/CLK_REQ2#/GPIO41 as CLK_REQ#
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG41, AccWidthUint8, 00, 0x1);         // CLK_REQ2#

		// set AZ_SDIN3/GPIO170 as GPO for GPIO_GATE_C
		RWMEM (IoMuxMmioAddr + SB_GPIO_REG170, AccWidthUint8, 00, 0x1);        // GPIO
		RWMEM (GpioMmioAddr + SB_GPIO_REG170, AccWidthUint8, 0x03, 0);         // GPO
		RWMEM (GpioMmioAddr + SB_GPIO_REG170, AccWidthUint8, 0x03, BIT6);       // output HIGH
		RWMEM (GpioMmioAddr + SB_GPIO_REG170, AccWidthUint8, 0x63, BIT3);       // pullup DISABLE
	//  To fix glitch issue
		RWMEM (GpioMmioAddr + SB_GPIO_REG170, AccWidthUint8, 0xBF, 0);      // set GPIO_GATE_C to LOW
	//
	// Enable/Disable OnBoard LAN
	//
		if (!CONFIG_ONBOARD_LAN)
		{ // 1 - DISABLED
			RWMEM (GpioMmioAddr + SB_GPIO_REG197, AccWidthUint8, 0xBF, 0);      // LOM_POWER off
			RWMEM (GpioMmioAddr + SB_GPIO_REG25, AccWidthUint8, 0xBF, 0);
			RWMEM (GpioMmioAddr + SB_GPIO_REG63, AccWidthUint8, 0xFF, BIT3);    // PULL UP - DISABLED
			RWMEM (MiscMmioAddr + SB_MISC_REG00+1, AccWidthUint8, 0x0F, 0);     // Disable GPP_CLK3
		}
//		else
//		{ // 0 - AUTO
//			// set BIT3 = 1 (PULLUP disable), BIT4 = 0 (PULLDOWN Disable)
//			RWMEM (GpioMmioAddr + SB_GPIO_REG197, AccWidthUint8, 0x23, BIT3);
//			RWMEM (GpioMmioAddr + SB_GPIO_REG25, AccWidthUint8, 0x23, BIT3);
//		}


	//
	// Enable/Disable 1394
	//
		if (!CONFIG_ONBOARD_1394)
		{ // 1 - DISABLED
//			RWMEM (GpioMmioAddr + SB_GPIO_REG170, AccWidthUint8, 0xBF, 0);      // set GPIO_GATE_C to LOW
			RWMEM (GpioMmioAddr + SB_GPIO_REG59, AccWidthUint8, 0xBF, 0);       // 1394 power off
			RWMEM (GpioMmioAddr + SB_GPIO_REG27, AccWidthUint8, 0xBF, 0);
			RWMEM (GpioMmioAddr + SB_GPIO_REG41, AccWidthUint8, 0xFF, BIT3);    // pullup DISABLE
			RWMEM (MiscMmioAddr + SB_MISC_REG04, AccWidthUint8, 0xF0, 0);       // DISABLE GPP_CLK8
//			RWMEM (GpioMmioAddr + SB_GPIO_REG170, AccWidthUint8, 0xBF, BIT6);   // set GPIO_GATE_C to HIGH
		}
//		else
//		{ // 0 - AUTO
//			// set BIT3 = 1 (PULLUP disable), BIT4 = 0 (PULLDOWN Disable), BIT6 = 1 (output HIGH)
//			RWMEM (GpioMmioAddr + SB_GPIO_REG27, AccWidthUint8, 0x03, BIT6);
//			RWMEM (GpioMmioAddr + SB_GPIO_REG27, AccWidthUint8, 0x63, BIT3);
//
//			RWMEM (GpioMmioAddr + SB_GPIO_REG59, AccWidthUint8, 0x03, BIT6);
//			RWMEM (GpioMmioAddr + SB_GPIO_REG59, AccWidthUint8, 0x63, BIT3);
//		}

//
// external USB 3.0 control:
//    amdExternalUSBController: CMOS, 0 - AUTO, 1 - DISABLE
//                      GPIO26: PCIE_RST#_USB3.0
//                      GPIO46: PCIE_USB30_CLKREQ#
//                     GPIO200: NEC_USB30_PWR_EN, 0 - OFF, 1 - ON
//                       Clock: GPP_CLK7
//                     GPIO172 used as FCH_USB3.0PORT_EN# 0:ENABLE; 1:DISABLE
//	if ((Amd_SystemConfiguration.XhciSwitch == 1) || (SystemConfiguration.amdExternalUSBController == 1)) {
// disable Onboard NEC USB3.0 controller
		if (!CONFIG_ONBOARD_USB30) {
			RWMEM (GpioMmioAddr + SB_GPIO_REG200, AccWidthUint8, 0xBF, 0);
			RWMEM (GpioMmioAddr + SB_GPIO_REG26, AccWidthUint8, 0xBF, 0);
			RWMEM (GpioMmioAddr + SB_GPIO_REG46, AccWidthUint8, 0xFF, BIT3);   // PULL_UP DISABLE
			RWMEM (MiscMmioAddr + SB_MISC_REG00+3, AccWidthUint8, 0x0F, 0);    // DISABLE GPP_CLK7
			RWMEM (GpioMmioAddr + SB_GPIO_REG172, AccWidthUint8, 0xBF, 0);  // FCH_USB3.0PORT_EN# 0:ENABLE; 1:DISABLE
		}
//	}

//
// BlueTooth control: BT_ON
//    amdBlueTooth: CMOS, 0 - AUTO, 1 - DISABLE
//          GPIO07: BT_ON, 0 - OFF, 1 - ON
//
	if (!CONFIG_ONBOARD_BLUETOOTH) {
	//-	if (SystemConfiguration.amdBlueTooth == 1) {
		RWMEM (GpioMmioAddr + SB_GPIO_REG07, AccWidthUint8, 0xBF, 0);
	//-	}
	}

//
// WebCam control:
//    amdWebCam: CMOS, 0 - AUTO, 1 - DISABLE
//       GPIO34: WEBCAM_ON#, 0 - ON, 1 - OFF
//
	if (!CONFIG_ONBOARD_WEBCAM) {
	//-	if (SystemConfiguration.amdWebCam == 1) {
		RWMEM (GpioMmioAddr + SB_GPIO_REG34, AccWidthUint8, 0xBF, BIT6);
	//-	}
	}

//
// Travis enable:
//    amdTravisCtrl: CMOS, 0 - DISABLE, 1 - ENABLE
//           GPIO66: TRAVIS_EN#, 0 - ENABLE, 1 - DISABLE
//
	if (!CONFIG_ONBOARD_TRAVIS) {
	//-	if (SystemConfiguration.amdTravisCtrl == 0) {
		RWMEM (GpioMmioAddr + SB_GPIO_REG66, AccWidthUint8, 0xBF, BIT6);
	//-	}
	}

//
// Disable Light Sensor if needed
//
	if (CONFIG_ONBOARD_LIGHTSENSOR) {
	//-    if (SystemConfiguration.amdLightSensor == 1) {
		RWMEM (IoMuxMmioAddr + SB_GEVENT_REG12, AccWidthUint8, 0x00, 0x1);
	//-    }
	}

}
