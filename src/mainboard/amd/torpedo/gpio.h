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

#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>
#include "cbtypes.h"

#define Mmio_Address( BaseAddr, Register ) \
	  ( (UINTN)BaseAddr + \
		(UINTN)(Register) \
	  )

#define Mmio32_Ptr( BaseAddr, Register ) \
	  ( (volatile u32 *)Mmio_Address( BaseAddr, Register ) )

#define Mmio32_G( BaseAddr, Register ) \
	  *Mmio32_Ptr( BaseAddr, Register )

#define Mmio32_And_Or( BaseAddr, Register, AndData, OrData ) \
	Mmio32_G( BaseAddr, Register ) = \
	(u32) ( \
	  ( Mmio32_G( BaseAddr, Register ) & \
	      (u32)(AndData) \
	  ) | \
	  (u32)(OrData) \
	)

#define Mmio8_Ptr( BaseAddr, Register ) \
	( (volatile u8 *)Mmio_Address( BaseAddr, Register ) )

#define Mmio8_G( BaseAddr, Register ) \
	*Mmio8_Ptr( BaseAddr, Register )

#define Mmio8_And_Or( BaseAddr, Register, AndData, OrData ) \
	Mmio8_G( BaseAddr, Register ) = \
	(u8) ( \
	  ( Mmio8_G( BaseAddr, Register ) & \
	      (u8)(AndData) \
	    ) | \
	  (u8)(OrData) \
	)

#define SMIREG_EVENT_ENABLE 0x04
#define SMIREG_SCITRIG      0x08
#define SMIREG_SCILEVEL     0x0C
#define SMIREG_SMISCIEN     0x14
#define SMIREG_SCIS0EN      0x20
#define SMIREG_SCIMAP0      0x40
#define SMIREG_SCIMAP1      0x44
#define SMIREG_SCIMAP2      0x48
#define SMIREG_SCIMAP3      0x4C
#define SMIREG_SCIMAP4      0x50
#define SMIREG_SCIMAP5      0x54
#define SMIREG_SCIMAP6      0x58
#define SMIREG_SCIMAP7      0x5C
#define SMIREG_SCIMAP8      0x60
#define SMIREG_SCIMAP9      0x64
#define SMIREG_SCIMAP10     0x68
#define SMIREG_SCIMAP11     0x6C
#define SMIREG_SCIMAP12     0x70
#define SMIREG_SCIMAP13     0x74
#define SMIREG_SCIMAP14     0x78
#define SMIREG_SCIMAP15     0x7C
#define SMIREG_SMITRIG      0x98
#define SMIREG_SMICONTROL0  0xA0
#define SMIREG_SMICONTROL1  0xA4

#define FUNCTION0           0
#define FUNCTION1           1
#define FUNCTION2           2
#define FUNCTION3           3
#define NonGpio             0x80				// BIT7

// S0-domain General Purpose I/O: GPIO 00~67
#define GPIO_00_SELECT      FUNCTION1+NonGpio   // MPCIE_RST1# for J3703, LOW ACTIVE, HIGH DEFAULT
#define GPIO_01_SELECT      FUNCTION1+NonGpio   // MPCIE_RST2# for J3711, LOW ACTIVE, HIGH DEFAULT
#define GPIO_02_SELECT      FUNCTION1           // MPCIE_RST0# for J3700, LOW ACTIVE, HIGH DEFAULT
#define GPIO_03_SELECT      FUNCTION1+NonGpio   // NOT USED
#define GPIO_04_SELECT      FUNCTION1+NonGpio   // x1 gpp reset, for J3701, low active, HIGH DEFAULT
#define GPIO_05_SELECT      FUNCTION1+NonGpio   // express card reset, for J2500,  low active, HIGH DEFAULT
#define GPIO_06_SELECT      FUNCTION0+NonGpio   //NOT USED
#define GPIO_07_SELECT      FUNCTION1           // BT_ON, 1: BT ON(DEFAULT); 0: BT OFF
#define GPIO_08_SELECT      FUNCTION1           // PEX_STD_SW#, 1:Low Level Mode(default); 0:Standard(desktop) Swing Level
#define GPIO_09_SELECT      FUNCTION1+NonGpio   // MXM_PRESENT2#, INPUT, LOW MEANS MXM IS INSTALLED
#define GPIO_10_SELECT      FUNCTION1+NonGpio   // MXM_PRESENT1#, INPUT, LOW MEANS MXM IS INSTALLED
#define GPIO_11_SELECT      FUNCTION0+NonGpio   // NOT USED
#define GPIO_12_SELECT      FUNCTION1           // WL_DISABLE#, DISABLE THE WALN IN J3702
#define GPIO_13_SELECT      FUNCTION1           // WU_DISABLE#, DISABLE THE WUSB IN J3711
#define GPIO_14_SELECT      FUNCTION1           // WP_DISABLE, DISABLE THE WWAN IN J3703
#define GPIO_15_SELECT      FUNCTION1+NonGpio   // NOT USED, //FUNCTION1, Reset_CEC# Low Active, High default
#define GPIO_16_SELECT      FUNCTION0+NonGpio   // NOT USED
#define GPIO_17_SELECT      FUNCTION0+NonGpio   // NOT USED
#define GPIO_18_SELECT      FUNCTION0+NonGpio   // NOT USED
#define GPIO_19_SELECT      FUNCTION1           // For LASSO_DET# detection when Gevent14# is asserted.
#define GPIO_20_SELECT      FUNCTION1           // PX_MUX for DOCKING card, PX MUX selection in mux mode. dGPU enable with high(option)
#define GPIO_21_SELECT      FUNCTION1           // DOCK_MUX for DCKING card, MUX selection output. Docking display enabled when high(option)
#define GPIO_22_SELECT      FUNCTION1           // SB_PWR_LV, INDICATE TO THE MXM THE SYSTEM IS IN LOW BATTERY MODE
                                                //      1:BATTERY IS FINE(DEFAULT)
                                                //      0:BATTERY IS LOW
#define GPIO_23_SELECT      FUNCTION1	        // CODEC_ON.1: CODEC ON (default)0: CODEC OFF
#define GPIO_24_SELECT      FUNCTION1           // Travis reset,Low active High default
#define GPIO_25_SELECT      FUNCTION1+NonGpio   // PCIE_RST# for LAN (AND gate with PCIE_RST#); default high
#define GPIO_26_SELECT      FUNCTION1+NonGpio   // PCIE_RST# for USB3.0 (AND gate with PCIE_RST#); default high
#define GPIO_27_SELECT      FUNCTION1+NonGpio   // PCIE_RST# for 1394 (AND gate with PCIE_RST#); default high
#define GPIO_28_SELECT      FUNCTION1           // MXM PWRGD INDICATOR, INPUT
#define GPIO_29_SELECT      FUNCTION1           // MEM HOT, LOW ACTIVE, OUTPUT
#define GPIO_30_SELECT      FUNCTION1           // INPUT, DEFINE THE BOARD REVISION 0
#define GPIO_31_SELECT      FUNCTION1           // INPUT, DEFINE THE BOARD REVISION 1
                                                //      00 - REVA
                                                //      01 - REVB
                                                //      10 - REVC
                                                //      11 - REVD
#define GPIO_32_SELECT      FUNCTION1+NonGpio   // PCIE_SW - HIGH:MXM; LOW:LASSO
#define GPIO_33_SELECT      FUNCTION1           // USB3.0 DETECT of Express Card:USB3.0_DET#, Low active.
                                                //      0:USB3.0 I/F in Express CARD
                                                //      1:PCIE I/F  in Express CARD detection
#define GPIO_34_SELECT      FUNCTION1           // WEBCAM_ON#. 0: ON (default) 1: OFF
#define GPIO_35_SELECT      FUNCTION1           // ODD_DA_INTH#
#define GPIO_36_SELECT      FUNCTION0+NonGpio   // PCICLK FOR KBC
#define GPIO_37_SELECT      FUNCTION0+NonGpio   // NOT USED
#define GPIO_38_SELECT      FUNCTION0+NonGpio   // NOT USED
#define GPIO_39_SELECT      FUNCTION0+NonGpio   // NOT USED
#define GPIO_40_SELECT      FUNCTION1           // For DOCK# detection when Gevent14# is asserted.
#define GPIO_41_SELECT      FUNCTION1+NonGpio   // 1394 CLK REQ#
#define GPIO_42_SELECT      FUNCTION1+NonGpio   // X4 GPP CLK REQ#
#define GPIO_43_SELECT      FUNCTION0+NonGpio   // SMBUS0, CLOCK
#define GPIO_44_SELECT      FUNCTION1+NonGpio   // PEGPIO0, RESET THE MXM MODULE
#define GPIO_45_SELECT      FUNCTION2+NonGpio   // PEGPIO1, 1:MXM IS POWER ON; 0:MXM IS OFF
#define GPIO_46_SELECT      FUNCTION1+NonGpio   // USB3.0_CLKREQ#
#define GPIO_47_SELECT      FUNCTION0+NonGpio   // SMBUS0, DATA
#define GPIO_48_SELECT      FUNCTION0+NonGpio   // SERIRQ
#define GPIO_49_SELECT      FUNCTION0+NonGpio   // LDRQ#1
#define GPIO_50_SELECT      FUNCTION2           // SMARTVOLTAGE TO CONTROL THE 5V - 1:5V; 0:4.56V
#define GPIO_51_SELECT      FUNCTION0+NonGpio   // back-up for SMARTVOLTAGE1
#define GPIO_52_SELECT      FUNCTION0+NonGpio   // CPU FAN OUT
#define GPIO_53_SELECT      FUNCTION1           // ODD POWER ENABLE, HIGH ACTIVE
#define GPIO_54_SELECT      FUNCTION0+NonGpio   // SB_PROCHOT, OUTPUT, LOW ACTIVE
#define GPIO_55_SELECT      FUNCTION2+NonGpio   // MXM POWER ENABLE(POWER ON MODULE)
                                                //      1:ENABLE; 0:DISABLE
                                                // DEFAULT VALUE DEPENDS ON GPIO 9 AND 10
#define GPIO_56_SELECT      FUNCTION0+NonGpio   //HDD2_POWER/HDD0_POWER/CPU FAN ;CPU FAN
#define GPIO_57_SELECT      FUNCTION1           // HDD0_POWER
#define GPIO_58_SELECT      FUNCTION1           // HDD2_POWER
#define GPIO_59_SELECT      FUNCTION2+NonGpio   // 1394 POWER, OUTPUT, HIGH ACTIVE
#define GPIO_60_SELECT      FUNCTION0+NonGpio   // EXPCARD_CLKREQ#
#define GPIO_61_SELECT      FUNCTION0+NonGpio   // PE0_CLKREQ#, FROM J3700
#define GPIO_62_SELECT      FUNCTION0+NonGpio   // PE2_CLKREQ#, FROM J3711
#define GPIO_63_SELECT      FUNCTION0+NonGpio   // LAN_CLKREQ#
#define GPIO_64_SELECT      FUNCTION0+NonGpio   // PE1_CLKREQ#, FROM J3703
#define GPIO_65_SELECT      FUNCTION0+NonGpio   // MXM CLK REQ#, FROM MXM
#define GPIO_66_SELECT      FUNCTION1           // USED AS TRAVIS_EN#; 0:ENABLE as default
#define GPIO_67_SELECT      FUNCTION0+NonGpio   // USED AS SATA_ACT#
#define GPIO_68_SELECT      FUNCTION0+NonGpio
#define GPIO_69_SELECT      FUNCTION0+NonGpio
#define GPIO_70_SELECT      FUNCTION0+NonGpio
#define GPIO_71_SELECT      FUNCTION0+NonGpio
#define GPIO_72_SELECT      FUNCTION0+NonGpio
#define GPIO_73_SELECT      FUNCTION0+NonGpio
#define GPIO_74_SELECT      FUNCTION0+NonGpio
#define GPIO_75_SELECT      FUNCTION0+NonGpio
#define GPIO_76_SELECT      FUNCTION0+NonGpio
#define GPIO_77_SELECT      FUNCTION0+NonGpio
#define GPIO_78_SELECT      FUNCTION0+NonGpio
#define GPIO_79_SELECT      FUNCTION0+NonGpio
#define GPIO_80_SELECT      FUNCTION0+NonGpio
#define GPIO_81_SELECT      FUNCTION0+NonGpio
#define GPIO_82_SELECT      FUNCTION0+NonGpio
#define GPIO_83_SELECT      FUNCTION0+NonGpio
#define GPIO_84_SELECT      FUNCTION0+NonGpio
#define GPIO_85_SELECT      FUNCTION0+NonGpio
#define GPIO_86_SELECT      FUNCTION0+NonGpio
#define GPIO_87_SELECT      FUNCTION0+NonGpio
#define GPIO_88_SELECT      FUNCTION0+NonGpio
#define GPIO_89_SELECT      FUNCTION0+NonGpio
#define GPIO_90_SELECT      FUNCTION0+NonGpio
#define GPIO_91_SELECT      FUNCTION0+NonGpio
#define GPIO_92_SELECT      FUNCTION0+NonGpio
#define GPIO_93_SELECT      FUNCTION0+NonGpio
#define GPIO_94_SELECT      FUNCTION0+NonGpio
#define GPIO_95_SELECT      FUNCTION0+NonGpio
// GEVENT 00~23 are mapped to GPIO 96~119
#define GPIO_96_SELECT      FUNCTION0           // GA20IN/GEVENT0#
#define GPIO_97_SELECT      FUNCTION0           // KBRST#/GEVENT1#
#define GPIO_98_SELECT      FUNCTION0           // THRMTRIP#/SMBALERT#/GEVENT2# -> APU_THERMTRIP
#define GPIO_99_SELECT      FUNCTION1           // LPC_PME#/GEVENT3# -> EC_SCI#
#define GPIO_100_SELECT     FUNCTION2           // PCIE_RST2#/PCI_PME#/GEVENT4# -> APU_MEMHOT#
#define GPIO_101_SELECT     FUNCTION1           // LPC_PD#/GEVENT5# -> hotplug of express card, low active
#define GPIO_102_SELECT     FUNCTION0+NonGpio   // USB_OC6#/IR_TX1/ GEVENT6# -> NOT USED,
                                                // there is a confliction to IR function when this pin is as a GEVENT.
#define GPIO_103_SELECT     FUNCTION0+NonGpio   // DDR3_RST#/GEVENT7#/VGA_PD -> VGA_PD,
                                                // special pin difination for SB900 VGA OUTPUT, high active,
                                                // VGA power for Hudson-M2 will be down when it was asserted.
#define GPIO_104_SELECT     FUNCTION0           // WAKE#/GEVENT8# -> WAKEUP, low active
#define GPIO_105_SELECT     FUNCTION2           // SPI_HOLD/GBE_LED1/GEVENT9# - WF_RADIO (wireless radio)
#define GPIO_106_SELECT     FUNCTION0           // GBE_LED2/GEVENT10# -> GBE_LED2
#define GPIO_107_SELECT     FUNCTION0+NonGpio   // GBE_STAT0/GEVENT11# -> GBE_STAT0
#define GPIO_108_SELECT     FUNCTION2           // USB_OC0#/TRST#/GEVENT12# -> SMBALERT# (Light Sensor), low active
                                                // [option for SPI_TPM_CS# in Hudson-M2 A12)]
#define GPIO_109_SELECT     FUNCTION0           // USB_OC1#/TDI/GEVENT13# - USB OC for 0, 1,2,3 & USB_OC expresscard (usb4) &
                                                //  USB3.0 PORT0,1:low active,disable all usb ports and new card power at a same time
#define GPIO_110_SELECT     FUNCTION2           // USB_OC2#/TCK/GEVENT14# -> Lasso detect or Dock detect,
                                                // plus judge GPIO40 and GPIO19 level,low is assert.
                                                //      LASSO_DET# :0 & GPIO19:0 -----> LASSO is present (default)
                                                //      DOCK#:0 & GPIO40:0 -----------> DOCK is present(option)
#define GPIO_111_SELECT     FUNCTION1+NonGpio	// USB_OC3#/AC_PRES/TDO/GEVENT15# -> AC_PRES, high active
#define GPIO_112_SELECT     FUNCTION2           // USB_OC4#/IR_RX0/GEVENT16# -> ODD_DA, ODD device attention,
                                                // low active, when it's low, BIOS will enbale ODD_PWR
#define GPIO_113_SELECT     FUNCTION2			// USB_OC5#/IR_TX0/GEVENT17# -> use TWARN mapping to trigger GEVENT17#
#define GPIO_114_SELECT     FUNCTION2           // BLINK/USB_OC7#/GEVENT18# -> BLINK
#define GPIO_115_SELECT     FUNCTION0           // SYS_RESET#/GEVENT19# -> SYS_RST#
#define GPIO_116_SELECT     FUNCTION0           // R_RX1/GEVENT20# -> IR INPUT
#define GPIO_117_SELECT     FUNCTION1+NonGpio   // SPI_CS3#/GBE_STAT1/GEVENT21# -> GBE_STAT1
#define GPIO_118_SELECT     FUNCTION1           // RI#/GEVENT22# -> LID_CLOSED#
#define GPIO_119_SELECT     FUNCTION0           // LPC_SMI#/GEVENT23# -> EC_SMI
#define GPIO_120_SELECT     FUNCTION0+NonGpio
#define GPIO_121_SELECT     FUNCTION0+NonGpio
#define GPIO_122_SELECT     FUNCTION0+NonGpio
#define GPIO_123_SELECT     FUNCTION0+NonGpio
#define GPIO_124_SELECT     FUNCTION0+NonGpio
#define GPIO_125_SELECT     FUNCTION0+NonGpio
#define GPIO_126_SELECT     FUNCTION0+NonGpio
#define GPIO_127_SELECT     FUNCTION0+NonGpio
#define GPIO_128_SELECT     FUNCTION0+NonGpio
#define GPIO_129_SELECT     FUNCTION0+NonGpio
#define GPIO_130_SELECT     FUNCTION0+NonGpio
#define GPIO_131_SELECT     FUNCTION0+NonGpio
#define GPIO_132_SELECT     FUNCTION0+NonGpio
#define GPIO_133_SELECT     FUNCTION0+NonGpio
#define GPIO_134_SELECT     FUNCTION0+NonGpio
#define GPIO_135_SELECT     FUNCTION0+NonGpio
#define GPIO_136_SELECT     FUNCTION0+NonGpio
#define GPIO_137_SELECT     FUNCTION0+NonGpio
#define GPIO_138_SELECT     FUNCTION0+NonGpio
#define GPIO_139_SELECT     FUNCTION0+NonGpio
#define GPIO_140_SELECT     FUNCTION0+NonGpio
#define GPIO_141_SELECT     FUNCTION0+NonGpio
#define GPIO_142_SELECT     FUNCTION0+NonGpio
#define GPIO_143_SELECT     FUNCTION0+NonGpio
#define GPIO_144_SELECT     FUNCTION0+NonGpio
#define GPIO_145_SELECT     FUNCTION0+NonGpio
#define GPIO_146_SELECT     FUNCTION0+NonGpio
#define GPIO_147_SELECT     FUNCTION0+NonGpio
#define GPIO_148_SELECT     FUNCTION0+NonGpio
#define GPIO_149_SELECT     FUNCTION0+NonGpio
#define GPIO_150_SELECT     FUNCTION0+NonGpio
#define GPIO_151_SELECT     FUNCTION0+NonGpio
#define GPIO_152_SELECT     FUNCTION0+NonGpio
#define GPIO_153_SELECT     FUNCTION0+NonGpio
#define GPIO_154_SELECT     FUNCTION0+NonGpio
#define GPIO_155_SELECT     FUNCTION0+NonGpio
#define GPIO_156_SELECT     FUNCTION0+NonGpio
#define GPIO_157_SELECT     FUNCTION0+NonGpio
#define GPIO_158_SELECT     FUNCTION0+NonGpio
#define GPIO_159_SELECT     FUNCTION0+NonGpio
#define GPIO_160_SELECT     FUNCTION0+NonGpio

// S5-domain General Purpose I/O
#define GPIO_161_SELECT     FUNCTION0+NonGpio   // ROM_RST#
#define GPIO_162_SELECT     FUNCTION0+NonGpio   // SPI ROM
#define GPIO_163_SELECT     FUNCTION0+NonGpio   // SPI ROM
#define GPIO_164_SELECT     FUNCTION0+NonGpio   // SPI ROM
#define GPIO_165_SELECT     FUNCTION0+NonGpio   // SPI ROM
#define GPIO_166_SELECT     FUNCTION1+NonGpio   // GBE_STAT2
#define GPIO_167_SELECT     FUNCTION0+NonGpio   // AZ_SDATA_IN0
#define GPIO_168_SELECT     FUNCTION0+NonGpio   // AZ_SDATA_IN1
#define GPIO_169_SELECT     FUNCTION0+NonGpio   // AZ_SDATA_IN2
#define GPIO_170_SELECT     FUNCTION1+NonGpio   // gating the power control signal for ODD, see BIOS requirements doc for detail.
#define GPIO_171_SELECT     FUNCTION0+NonGpio   // TEMPIN0,
#define GPIO_172_SELECT     FUNCTION1           // used as FCH_USB3.0PORT_EN# - 0:ENABLE; 1:DISABLE
#define GPIO_173_SELECT     FUNCTION0+NonGpio   // TEMPIN3
#define GPIO_174_SELECT     FUNCTION1+NonGpio   // USED AS TALERT#
#define GPIO_175_SELECT     FUNCTION1           // WLAN, WIRELESS DISABLE 1:DISABLE; 0:ENABLE
#define GPIO_176_SELECT     FUNCTION1+NonGpio   // WWAN, WIRELESS DISABLE 1:DISABLE; 0:ENABLE
#define GPIO_177_SELECT     FUNCTION2+NonGpio   // WUSB, WIRELESS DISABLE 1:DISABLE; 0:ENABLE
#define GPIO_178_SELECT     FUNCTION2           // MEM_1V5
#define GPIO_179_SELECT     FUNCTION2           // MEM_1V35
#define GPIO_180_SELECT     FUNCTION0+NonGpio   // Use as VIN VDDIO
#define GPIO_181_SELECT     FUNCTION0+NonGpio   // Use as VIN VDDR
#define GPIO_182_SELECT     FUNCTION1+NonGpio   // GBE_LED3
#define GPIO_183_SELECT     FUNCTION0+NonGpio   // GBE_LED0
#define GPIO_184_SELECT     FUNCTION1+NonGpio   // USED AS LLB#
#define GPIO_185_SELECT     FUNCTION0+NonGpio   // USED AS USB
#define GPIO_186_SELECT     FUNCTION0+NonGpio   // USED AS USB
#define GPIO_187_SELECT     FUNCTION2           // USED AS AC LED INDICATOR, LOW ACTIVE
#define GPIO_188_SELECT     FUNCTION2           // default used AS BATT LED INDICATOR, LOW ACTIVE
                                                // option for HDMI CEC signal OW ACTIVE
#define GPIO_189_SELECT     FUNCTION1           // USED AS AC_OK RECEIVER, INPUT, low active
#define GPIO_190_SELECT     FUNCTION1           // USED TO MONITER INTERRUPT FROM BATT CHARGER, INPUT
#define GPIO_191_SELECT     FUNCTION0+NonGpio   // TOUCH PAD, DATA
#define GPIO_192_SELECT     FUNCTION0+NonGpio   // TOUCH PAD, CLK
#define GPIO_193_SELECT     FUNCTION0+NonGpio   // SMBUS CLK,
#define GPIO_194_SELECT     FUNCTION0+NonGpio   // SMBUS, DATA
#define GPIO_195_SELECT     FUNCTION0+NonGpio   // SMBUS CLK,
#define GPIO_196_SELECT     FUNCTION0+NonGpio   // SMBUS, DATA
#define GPIO_197_SELECT     FUNCTION2+NonGpio   // Default GPIO for LOM_POWER, high active
                                                // RESERVED FOR LCD BACKLIGHT PWM
#define GPIO_198_SELECT     FUNCTION0+NonGpio   // IMC SCROLL LED CONTROL
#define GPIO_199_SELECT     FUNCTION3           // STRAP TO SELECT BOOT ROM - H:LPC ROM   L: SPI ROM
#define GPIO_200_SELECT     FUNCTION2           // NEC USB3.0 POWER CONTROL 1:ON(DEFAULT); 0:OFF
#define GPIO_201_SELECT     FUNCTION0+NonGpio   // KSI
#define GPIO_202_SELECT     FUNCTION0+NonGpio   // KSI
#define GPIO_203_SELECT     FUNCTION0+NonGpio   // KSI
#define GPIO_204_SELECT     FUNCTION0+NonGpio   // KSI
#define GPIO_205_SELECT     FUNCTION0+NonGpio   // KSI
#define GPIO_206_SELECT     FUNCTION0+NonGpio   // KSI
#define GPIO_207_SELECT     FUNCTION0+NonGpio   // KSI
#define GPIO_208_SELECT     FUNCTION0+NonGpio   // KSI
#define GPIO_209_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_210_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_211_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_212_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_213_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_214_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_215_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_216_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_217_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_218_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_219_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_220_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_221_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_222_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_223_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_224_SELECT     FUNCTION0+NonGpio   // KSO
#define GPIO_225_SELECT     FUNCTION2+NonGpio   // KSO
#define GPIO_226_SELECT     FUNCTION2+NonGpio   // KSO
#define GPIO_227_SELECT     FUNCTION0+NonGpio   // SMBUS CLK,
#define GPIO_228_SELECT     FUNCTION0+NonGpio   // SMBUS, DATA
#define GPIO_229_SELECT     FUNCTION0+NonGpio   // DP1_HPD

#define TYPE_GPI  (1 << 5)
#define TYPE_GPO  (0 << 5)

#define GPIO_00_TYPE        TYPE_GPO
#define GPIO_01_TYPE        TYPE_GPO
#define GPIO_02_TYPE        TYPE_GPO
#define GPIO_03_TYPE        TYPE_GPO
#define GPIO_04_TYPE        TYPE_GPO
#define GPIO_05_TYPE        TYPE_GPO
#define GPIO_06_TYPE        TYPE_GPO
#define GPIO_07_TYPE        TYPE_GPO
#define GPIO_08_TYPE        TYPE_GPO
#define GPIO_09_TYPE        TYPE_GPI
#define GPIO_10_TYPE        TYPE_GPI
#define GPIO_11_TYPE        TYPE_GPO
#define GPIO_12_TYPE        TYPE_GPO
#define GPIO_13_TYPE        TYPE_GPO
#define GPIO_14_TYPE        TYPE_GPO
#define GPIO_15_TYPE        TYPE_GPO
#define GPIO_16_TYPE        TYPE_GPO
#define GPIO_17_TYPE        TYPE_GPO
#define GPIO_18_TYPE        TYPE_GPO
#define GPIO_19_TYPE        TYPE_GPO
#define GPIO_20_TYPE        TYPE_GPO
#define GPIO_21_TYPE        TYPE_GPO
#define GPIO_22_TYPE        TYPE_GPO
#define GPIO_23_TYPE        TYPE_GPO
#define GPIO_24_TYPE        TYPE_GPO
#define GPIO_25_TYPE        TYPE_GPO
#define GPIO_26_TYPE        TYPE_GPO
#define GPIO_27_TYPE        TYPE_GPO
#define GPIO_28_TYPE        TYPE_GPI
#define GPIO_29_TYPE        TYPE_GPO
#define GPIO_30_TYPE        TYPE_GPI
#define GPIO_31_TYPE        TYPE_GPI
#define GPIO_32_TYPE        TYPE_GPO
#define GPIO_33_TYPE        TYPE_GPI
#define GPIO_34_TYPE        TYPE_GPO
#define GPIO_35_TYPE        TYPE_GPO
#define GPIO_36_TYPE        TYPE_GPO
#define GPIO_37_TYPE        TYPE_GPO
#define GPIO_38_TYPE        TYPE_GPO
#define GPIO_39_TYPE        TYPE_GPO
#define GPIO_40_TYPE        TYPE_GPO
#define GPIO_41_TYPE        TYPE_GPI
#define GPIO_42_TYPE        TYPE_GPI
#define GPIO_43_TYPE        TYPE_GPO
#define GPIO_44_TYPE        TYPE_GPO
#define GPIO_45_TYPE        TYPE_GPO
#define GPIO_46_TYPE        TYPE_GPI
#define GPIO_47_TYPE        TYPE_GPO
#define GPIO_48_TYPE        TYPE_GPO
#define GPIO_49_TYPE        TYPE_GPO
#define GPIO_50_TYPE        TYPE_GPO
#define GPIO_51_TYPE        TYPE_GPO
#define GPIO_52_TYPE        TYPE_GPO
#define GPIO_53_TYPE        TYPE_GPO
#define GPIO_54_TYPE        TYPE_GPO
#define GPIO_55_TYPE        TYPE_GPO
#define GPIO_56_TYPE        TYPE_GPI
#define GPIO_57_TYPE        TYPE_GPO
#define GPIO_58_TYPE        TYPE_GPO
#define GPIO_59_TYPE        TYPE_GPO
#define GPIO_60_TYPE        TYPE_GPI
#define GPIO_61_TYPE        TYPE_GPI
#define GPIO_62_TYPE        TYPE_GPI
#define GPIO_63_TYPE        TYPE_GPI
#define GPIO_64_TYPE        TYPE_GPI
#define GPIO_65_TYPE        TYPE_GPI
#define GPIO_66_TYPE        TYPE_GPO
#define GPIO_67_TYPE        TYPE_GPO
#define GPIO_68_TYPE        TYPE_GPO
#define GPIO_69_TYPE        TYPE_GPO
#define GPIO_70_TYPE        TYPE_GPO
#define GPIO_71_TYPE        TYPE_GPO
#define GPIO_72_TYPE        TYPE_GPO
#define GPIO_73_TYPE        TYPE_GPO
#define GPIO_74_TYPE        TYPE_GPO
#define GPIO_75_TYPE        TYPE_GPO
#define GPIO_76_TYPE        TYPE_GPO
#define GPIO_77_TYPE        TYPE_GPO
#define GPIO_78_TYPE        TYPE_GPO
#define GPIO_79_TYPE        TYPE_GPO
#define GPIO_80_TYPE        TYPE_GPO
#define GPIO_81_TYPE        TYPE_GPO
#define GPIO_82_TYPE        TYPE_GPO
#define GPIO_83_TYPE        TYPE_GPO
#define GPIO_84_TYPE        TYPE_GPO
#define GPIO_85_TYPE        TYPE_GPO
#define GPIO_86_TYPE        TYPE_GPO
#define GPIO_87_TYPE        TYPE_GPO
#define GPIO_88_TYPE        TYPE_GPO
#define GPIO_89_TYPE        TYPE_GPO
#define GPIO_90_TYPE        TYPE_GPO
#define GPIO_91_TYPE        TYPE_GPO
#define GPIO_92_TYPE        TYPE_GPO
#define GPIO_93_TYPE        TYPE_GPO
#define GPIO_94_TYPE        TYPE_GPO
#define GPIO_95_TYPE        TYPE_GPO

// GEVENT 00 ~ 23 are mapped to GPIO 96 ~ 119
#define GPIO_96_TYPE        TYPE_GPI
#define GPIO_97_TYPE        TYPE_GPI
#define GPIO_98_TYPE        TYPE_GPI
#define GPIO_99_TYPE        TYPE_GPI
#define GPIO_100_TYPE       TYPE_GPI
#define GPIO_101_TYPE       TYPE_GPI
#define GPIO_102_TYPE       TYPE_GPO
#define GPIO_103_TYPE       TYPE_GPO
#define GPIO_104_TYPE       TYPE_GPI
#define GPIO_105_TYPE       TYPE_GPI
#define GPIO_106_TYPE       TYPE_GPO
#define GPIO_107_TYPE       TYPE_GPI
#define GPIO_108_TYPE       TYPE_GPI
#define GPIO_109_TYPE       TYPE_GPI
#define GPIO_110_TYPE       TYPE_GPI
#define GPIO_111_TYPE       TYPE_GPI
#define GPIO_112_TYPE       TYPE_GPI
#define GPIO_113_TYPE       TYPE_GPI
#define GPIO_114_TYPE       TYPE_GPO
#define GPIO_115_TYPE       TYPE_GPI
#define GPIO_116_TYPE       TYPE_GPI
#define GPIO_117_TYPE       TYPE_GPI
#define GPIO_118_TYPE       TYPE_GPI
#define GPIO_119_TYPE       TYPE_GPI

#define GPIO_120_TYPE       TYPE_GPO
#define GPIO_121_TYPE       TYPE_GPO
#define GPIO_122_TYPE       TYPE_GPO
#define GPIO_123_TYPE       TYPE_GPO
#define GPIO_124_TYPE       TYPE_GPO
#define GPIO_125_TYPE       TYPE_GPO
#define GPIO_126_TYPE       TYPE_GPO
#define GPIO_127_TYPE       TYPE_GPO
#define GPIO_128_TYPE       TYPE_GPO
#define GPIO_129_TYPE       TYPE_GPO
#define GPIO_130_TYPE       TYPE_GPO
#define GPIO_131_TYPE       TYPE_GPO
#define GPIO_132_TYPE       TYPE_GPO
#define GPIO_133_TYPE       TYPE_GPO
#define GPIO_134_TYPE       TYPE_GPO
#define GPIO_135_TYPE       TYPE_GPO
#define GPIO_136_TYPE       TYPE_GPO
#define GPIO_137_TYPE       TYPE_GPO
#define GPIO_138_TYPE       TYPE_GPO
#define GPIO_139_TYPE       TYPE_GPO
#define GPIO_140_TYPE       TYPE_GPO
#define GPIO_141_TYPE       TYPE_GPO
#define GPIO_142_TYPE       TYPE_GPO
#define GPIO_143_TYPE       TYPE_GPO
#define GPIO_144_TYPE       TYPE_GPO
#define GPIO_145_TYPE       TYPE_GPO
#define GPIO_146_TYPE       TYPE_GPO
#define GPIO_147_TYPE       TYPE_GPO
#define GPIO_148_TYPE       TYPE_GPO
#define GPIO_149_TYPE       TYPE_GPO
#define GPIO_150_TYPE       TYPE_GPO
#define GPIO_151_TYPE       TYPE_GPO
#define GPIO_152_TYPE       TYPE_GPO
#define GPIO_153_TYPE       TYPE_GPO
#define GPIO_154_TYPE       TYPE_GPO
#define GPIO_155_TYPE       TYPE_GPO
#define GPIO_156_TYPE       TYPE_GPO
#define GPIO_157_TYPE       TYPE_GPO
#define GPIO_158_TYPE       TYPE_GPO
#define GPIO_159_TYPE       TYPE_GPO
#define GPIO_160_TYPE       TYPE_GPO
#define GPIO_161_TYPE       TYPE_GPO
#define GPIO_162_TYPE       TYPE_GPO
#define GPIO_163_TYPE       TYPE_GPO
#define GPIO_164_TYPE       TYPE_GPI
#define GPIO_165_TYPE       TYPE_GPO
#define GPIO_166_TYPE       TYPE_GPI
#define GPIO_167_TYPE       TYPE_GPI
#define GPIO_168_TYPE       TYPE_GPI
#define GPIO_169_TYPE       TYPE_GPI
#define GPIO_170_TYPE       TYPE_GPO
#define GPIO_171_TYPE       TYPE_GPI
#define GPIO_172_TYPE       TYPE_GPO
#define GPIO_173_TYPE       TYPE_GPI
#define GPIO_174_TYPE       TYPE_GPI
#define GPIO_175_TYPE       TYPE_GPO
#define GPIO_176_TYPE       TYPE_GPO
#define GPIO_177_TYPE       TYPE_GPO
#define GPIO_178_TYPE       TYPE_GPO
#define GPIO_179_TYPE       TYPE_GPO
#define GPIO_180_TYPE       TYPE_GPO
#define GPIO_181_TYPE       TYPE_GPO
#define GPIO_182_TYPE       TYPE_GPO
#define GPIO_183_TYPE       TYPE_GPO
#define GPIO_184_TYPE       TYPE_GPI
#define GPIO_185_TYPE       TYPE_GPO
#define GPIO_186_TYPE       TYPE_GPO
#define GPIO_187_TYPE       TYPE_GPO
#define GPIO_188_TYPE       TYPE_GPO
#define GPIO_189_TYPE       TYPE_GPI
#define GPIO_190_TYPE       TYPE_GPI
#define GPIO_191_TYPE       TYPE_GPO
#define GPIO_192_TYPE       TYPE_GPO
#define GPIO_193_TYPE       TYPE_GPO
#define GPIO_194_TYPE       TYPE_GPO
#define GPIO_195_TYPE       TYPE_GPO
#define GPIO_196_TYPE       TYPE_GPO
#define GPIO_197_TYPE       TYPE_GPO
#define GPIO_198_TYPE       TYPE_GPO
#define GPIO_199_TYPE       TYPE_GPI
#define GPIO_200_TYPE       TYPE_GPO
#define GPIO_201_TYPE       TYPE_GPI
#define GPIO_202_TYPE       TYPE_GPI
#define GPIO_203_TYPE       TYPE_GPI
#define GPIO_204_TYPE       TYPE_GPI
#define GPIO_205_TYPE       TYPE_GPI
#define GPIO_206_TYPE       TYPE_GPI
#define GPIO_207_TYPE       TYPE_GPI
#define GPIO_208_TYPE       TYPE_GPI
#define GPIO_209_TYPE       TYPE_GPO
#define GPIO_210_TYPE       TYPE_GPO
#define GPIO_211_TYPE       TYPE_GPO
#define GPIO_212_TYPE       TYPE_GPO
#define GPIO_213_TYPE       TYPE_GPO
#define GPIO_214_TYPE       TYPE_GPO
#define GPIO_215_TYPE       TYPE_GPO
#define GPIO_216_TYPE       TYPE_GPO
#define GPIO_217_TYPE       TYPE_GPO
#define GPIO_218_TYPE       TYPE_GPO
#define GPIO_219_TYPE       TYPE_GPO
#define GPIO_220_TYPE       TYPE_GPO
#define GPIO_221_TYPE       TYPE_GPO
#define GPIO_222_TYPE       TYPE_GPO
#define GPIO_223_TYPE       TYPE_GPO
#define GPIO_224_TYPE       TYPE_GPO
#define GPIO_225_TYPE       TYPE_GPO
#define GPIO_226_TYPE       TYPE_GPO
#define GPIO_227_TYPE       TYPE_GPO
#define GPIO_228_TYPE       TYPE_GPO
#define GPIO_229_TYPE       TYPE_GPO

#define GPO_LOW   (0 << 6)
#define GPO_HI    (1 << 6)

#define GPO_00_LEVEL        GPO_HI
#define GPO_01_LEVEL        GPO_HI
#define GPO_02_LEVEL        GPO_HI
#define GPO_03_LEVEL        GPO_HI
#define GPO_04_LEVEL        GPO_HI
#define GPO_05_LEVEL        GPO_HI
#define GPO_06_LEVEL        GPO_HI
#define GPO_07_LEVEL        GPO_HI
#define GPO_08_LEVEL        GPO_HI
#define GPO_09_LEVEL        GPO_LOW
#define GPO_10_LEVEL        GPO_LOW
#define GPO_11_LEVEL        GPO_HI
#define GPO_12_LEVEL        GPO_HI
#define GPO_13_LEVEL        GPO_HI
#define GPO_14_LEVEL        GPO_HI
#define GPO_15_LEVEL        GPO_HI
#define GPO_16_LEVEL        GPO_HI
#define GPO_17_LEVEL        GPO_HI
#define GPO_18_LEVEL        GPO_HI
#define GPO_19_LEVEL        GPO_LOW
#define GPO_20_LEVEL        GPO_LOW
#define GPO_21_LEVEL        GPO_LOW
#define GPO_22_LEVEL        GPO_HI
#define GPO_23_LEVEL        GPO_HI
#define GPO_24_LEVEL        GPO_HI
#define GPO_25_LEVEL        GPO_HI
#define GPO_26_LEVEL        GPO_HI
#define GPO_27_LEVEL        GPO_HI
#define GPO_28_LEVEL        GPO_LOW
#define GPO_29_LEVEL        GPO_HI
#define GPO_30_LEVEL        GPO_LOW
#define GPO_31_LEVEL        GPO_LOW
#define GPO_32_LEVEL        GPO_HI
#define GPO_33_LEVEL        GPO_LOW
#define GPO_34_LEVEL        GPO_LOW
#define GPO_35_LEVEL        GPO_LOW
#define GPO_36_LEVEL        GPO_LOW
#define GPO_37_LEVEL        GPO_HI
#define GPO_38_LEVEL        GPO_HI
#define GPO_39_LEVEL        GPO_HI
#define GPO_40_LEVEL        GPO_LOW
#define GPO_41_LEVEL        GPO_LOW
#define GPO_42_LEVEL        GPO_LOW
#define GPO_43_LEVEL        GPO_LOW
#define GPO_44_LEVEL        GPO_HI
#define GPO_45_LEVEL        GPO_HI
#define GPO_46_LEVEL        GPO_LOW
#define GPO_47_LEVEL        GPO_LOW
#define GPO_48_LEVEL        GPO_LOW
#define GPO_49_LEVEL        GPO_HI
#define GPO_50_LEVEL        GPO_HI
#define GPO_51_LEVEL        GPO_LOW
#define GPO_52_LEVEL        GPO_HI
#define GPO_53_LEVEL        GPO_HI
#define GPO_54_LEVEL        GPO_LOW
#define GPO_55_LEVEL        GPO_LOW
#define GPO_56_LEVEL        GPO_LOW
#define GPO_57_LEVEL        GPO_HI
#define GPO_58_LEVEL        GPO_HI
#define GPO_59_LEVEL        GPO_HI
#define GPO_60_LEVEL        GPO_LOW
#define GPO_61_LEVEL        GPO_LOW
#define GPO_62_LEVEL        GPO_LOW
#define GPO_63_LEVEL        GPO_LOW
#define GPO_64_LEVEL        GPO_LOW
#define GPO_65_LEVEL        GPO_LOW
#define GPO_66_LEVEL        GPO_LOW
#define GPO_67_LEVEL        GPO_LOW
#define GPO_68_LEVEL        GPO_LOW
#define GPO_69_LEVEL        GPO_LOW
#define GPO_70_LEVEL        GPO_LOW
#define GPO_71_LEVEL        GPO_LOW
#define GPO_72_LEVEL        GPO_LOW
#define GPO_73_LEVEL        GPO_LOW
#define GPO_74_LEVEL        GPO_LOW
#define GPO_75_LEVEL        GPO_LOW
#define GPO_76_LEVEL        GPO_LOW
#define GPO_77_LEVEL        GPO_LOW
#define GPO_78_LEVEL        GPO_LOW
#define GPO_79_LEVEL        GPO_LOW
#define GPO_80_LEVEL        GPO_LOW
#define GPO_81_LEVEL        GPO_LOW
#define GPO_82_LEVEL        GPO_LOW
#define GPO_83_LEVEL        GPO_LOW
#define GPO_84_LEVEL        GPO_LOW
#define GPO_85_LEVEL        GPO_LOW
#define GPO_86_LEVEL        GPO_LOW
#define GPO_87_LEVEL        GPO_LOW
#define GPO_88_LEVEL        GPO_LOW
#define GPO_89_LEVEL        GPO_LOW
#define GPO_90_LEVEL        GPO_LOW
#define GPO_91_LEVEL        GPO_LOW
#define GPO_92_LEVEL        GPO_LOW
#define GPO_93_LEVEL        GPO_LOW
#define GPO_94_LEVEL        GPO_LOW
#define GPO_95_LEVEL        GPO_LOW
#define GPO_96_LEVEL        GPO_LOW
#define GPO_97_LEVEL        GPO_LOW
#define GPO_98_LEVEL        GPO_LOW
#define GPO_99_LEVEL        GPO_LOW
#define GPO_100_LEVEL       GPO_LOW
#define GPO_101_LEVEL       GPO_LOW
#define GPO_102_LEVEL       GPO_LOW
#define GPO_103_LEVEL       GPO_LOW
#define GPO_104_LEVEL       GPO_LOW
#define GPO_105_LEVEL       GPO_LOW
#define GPO_106_LEVEL       GPO_LOW
#define GPO_107_LEVEL       GPO_LOW
#define GPO_108_LEVEL       GPO_HI
#define GPO_109_LEVEL       GPO_LOW
#define GPO_110_LEVEL       GPO_HI
#define GPO_111_LEVEL       GPO_HI
#define GPO_112_LEVEL       GPO_HI
#define GPO_113_LEVEL       GPO_LOW
#define GPO_114_LEVEL       GPO_LOW
#define GPO_115_LEVEL       GPO_LOW
#define GPO_116_LEVEL       GPO_LOW
#define GPO_117_LEVEL       GPO_LOW
#define GPO_118_LEVEL       GPO_LOW
#define GPO_119_LEVEL       GPO_LOW
#define GPO_120_LEVEL       GPO_LOW
#define GPO_121_LEVEL       GPO_LOW
#define GPO_122_LEVEL       GPO_LOW
#define GPO_123_LEVEL       GPO_LOW
#define GPO_124_LEVEL       GPO_LOW
#define GPO_125_LEVEL       GPO_LOW
#define GPO_126_LEVEL       GPO_LOW
#define GPO_127_LEVEL       GPO_LOW
#define GPO_128_LEVEL       GPO_LOW
#define GPO_129_LEVEL       GPO_LOW
#define GPO_130_LEVEL       GPO_LOW
#define GPO_131_LEVEL       GPO_LOW
#define GPO_132_LEVEL       GPO_LOW
#define GPO_133_LEVEL       GPO_LOW
#define GPO_134_LEVEL       GPO_LOW
#define GPO_135_LEVEL       GPO_LOW
#define GPO_136_LEVEL       GPO_LOW
#define GPO_137_LEVEL       GPO_LOW
#define GPO_138_LEVEL       GPO_LOW
#define GPO_139_LEVEL       GPO_LOW
#define GPO_140_LEVEL       GPO_LOW
#define GPO_141_LEVEL       GPO_LOW
#define GPO_142_LEVEL       GPO_LOW
#define GPO_143_LEVEL       GPO_LOW
#define GPO_144_LEVEL       GPO_LOW
#define GPO_145_LEVEL       GPO_LOW
#define GPO_146_LEVEL       GPO_LOW
#define GPO_147_LEVEL       GPO_LOW
#define GPO_148_LEVEL       GPO_LOW
#define GPO_149_LEVEL       GPO_LOW
#define GPO_150_LEVEL       GPO_LOW
#define GPO_151_LEVEL       GPO_LOW
#define GPO_152_LEVEL       GPO_LOW
#define GPO_153_LEVEL       GPO_LOW
#define GPO_154_LEVEL       GPO_LOW
#define GPO_155_LEVEL       GPO_LOW
#define GPO_156_LEVEL       GPO_LOW
#define GPO_157_LEVEL       GPO_LOW
#define GPO_158_LEVEL       GPO_LOW
#define GPO_159_LEVEL       GPO_LOW
#define GPO_160_LEVEL       GPO_LOW
#define GPO_161_LEVEL       GPO_LOW
#define GPO_162_LEVEL       GPO_LOW
#define GPO_163_LEVEL       GPO_LOW
#define GPO_164_LEVEL       GPO_LOW
#define GPO_165_LEVEL       GPO_LOW
#define GPO_166_LEVEL       GPO_LOW
#define GPO_167_LEVEL       GPO_LOW
#define GPO_168_LEVEL       GPO_LOW
#define GPO_169_LEVEL       GPO_LOW
#define GPO_170_LEVEL       GPO_HI
#define GPO_171_LEVEL       GPO_LOW
#define GPO_172_LEVEL       GPO_HI		// FCH_USB3.0PORT_EN# 0:ENABLE; 1:DISABLE
#define GPO_173_LEVEL       GPO_LOW
#define GPO_174_LEVEL       GPO_LOW
#define GPO_175_LEVEL       GPO_LOW
#define GPO_176_LEVEL       GPO_LOW
#define GPO_177_LEVEL       GPO_LOW
#define GPO_178_LEVEL       GPO_HI      // AMD.SR BU to set VDDIO level to 1.5V for Barb BU
#define GPO_179_LEVEL       GPO_HI
#define GPO_180_LEVEL       GPO_HI
#define GPO_181_LEVEL       GPO_LOW
#define GPO_182_LEVEL       GPO_HI
#define GPO_183_LEVEL       GPO_LOW
#define GPO_184_LEVEL       GPO_LOW
#define GPO_185_LEVEL       GPO_LOW
#define GPO_186_LEVEL       GPO_LOW
#define GPO_187_LEVEL       GPO_LOW
#define GPO_188_LEVEL       GPO_LOW
#define GPO_189_LEVEL       GPO_LOW
#define GPO_190_LEVEL       GPO_LOW
#define GPO_191_LEVEL       GPO_LOW
#define GPO_192_LEVEL       GPO_LOW
#define GPO_193_LEVEL       GPO_LOW
#define GPO_194_LEVEL       GPO_LOW
#define GPO_195_LEVEL       GPO_LOW
#define GPO_196_LEVEL       GPO_LOW
#define GPO_197_LEVEL       GPO_LOW
#define GPO_198_LEVEL       GPO_LOW
#define GPO_199_LEVEL       GPO_LOW
#define GPO_200_LEVEL       GPO_HI
#define GPO_201_LEVEL       GPO_LOW
#define GPO_202_LEVEL       GPO_LOW
#define GPO_203_LEVEL       GPO_LOW
#define GPO_204_LEVEL       GPO_LOW
#define GPO_205_LEVEL       GPO_LOW
#define GPO_206_LEVEL       GPO_LOW
#define GPO_207_LEVEL       GPO_LOW
#define GPO_208_LEVEL       GPO_LOW
#define GPO_209_LEVEL       GPO_LOW
#define GPO_210_LEVEL       GPO_LOW
#define GPO_211_LEVEL       GPO_LOW
#define GPO_212_LEVEL       GPO_LOW
#define GPO_213_LEVEL       GPO_LOW
#define GPO_214_LEVEL       GPO_LOW
#define GPO_215_LEVEL       GPO_LOW
#define GPO_216_LEVEL       GPO_LOW
#define GPO_217_LEVEL       GPO_LOW
#define GPO_218_LEVEL       GPO_LOW
#define GPO_219_LEVEL       GPO_LOW
#define GPO_220_LEVEL       GPO_LOW
#define GPO_221_LEVEL       GPO_LOW
#define GPO_222_LEVEL       GPO_LOW
#define GPO_223_LEVEL       GPO_LOW
#define GPO_224_LEVEL       GPO_LOW
#define GPO_225_LEVEL       GPO_LOW
#define GPO_226_LEVEL       GPO_LOW
#define GPO_227_LEVEL       GPO_LOW
#define GPO_228_LEVEL       GPO_LOW
#define GPO_229_LEVEL       GPO_LOW

#define GPIO_NONSTICKY   (0 << 2)
#define GPIO_STICKY      (1 << 2)

#define GPIO_00_STICKY      GPIO_NONSTICKY
#define GPIO_01_STICKY      GPIO_NONSTICKY
#define GPIO_02_STICKY      GPIO_NONSTICKY
#define GPIO_03_STICKY      GPIO_NONSTICKY
#define GPIO_04_STICKY      GPIO_NONSTICKY
#define GPIO_05_STICKY      GPIO_NONSTICKY
#define GPIO_06_STICKY      GPIO_NONSTICKY
#define GPIO_07_STICKY      GPIO_NONSTICKY
#define GPIO_08_STICKY      GPIO_NONSTICKY
#define GPIO_09_STICKY      GPIO_NONSTICKY
#define GPIO_10_STICKY      GPIO_NONSTICKY
#define GPIO_11_STICKY      GPIO_NONSTICKY
#define GPIO_12_STICKY      GPIO_NONSTICKY
#define GPIO_13_STICKY      GPIO_NONSTICKY
#define GPIO_14_STICKY      GPIO_NONSTICKY
#define GPIO_15_STICKY      GPIO_NONSTICKY
#define GPIO_16_STICKY      GPIO_NONSTICKY
#define GPIO_17_STICKY      GPIO_STICKY
#define GPIO_18_STICKY      GPIO_NONSTICKY
#define GPIO_19_STICKY      GPIO_NONSTICKY
#define GPIO_20_STICKY      GPIO_NONSTICKY
#define GPIO_21_STICKY      GPIO_NONSTICKY
#define GPIO_22_STICKY      GPIO_NONSTICKY
#define GPIO_23_STICKY      GPIO_NONSTICKY
#define GPIO_24_STICKY      GPIO_NONSTICKY
#define GPIO_25_STICKY      GPIO_NONSTICKY
#define GPIO_26_STICKY      GPIO_NONSTICKY
#define GPIO_27_STICKY      GPIO_NONSTICKY
#define GPIO_28_STICKY      GPIO_NONSTICKY
#define GPIO_29_STICKY      GPIO_NONSTICKY
#define GPIO_30_STICKY      GPIO_NONSTICKY
#define GPIO_31_STICKY      GPIO_NONSTICKY
#define GPIO_32_STICKY      GPIO_NONSTICKY
#define GPIO_33_STICKY      GPIO_NONSTICKY
#define GPIO_34_STICKY      GPIO_NONSTICKY
#define GPIO_35_STICKY      GPIO_NONSTICKY
#define GPIO_36_STICKY      GPIO_NONSTICKY
#define GPIO_37_STICKY      GPIO_NONSTICKY
#define GPIO_38_STICKY      GPIO_NONSTICKY
#define GPIO_39_STICKY      GPIO_NONSTICKY
#define GPIO_40_STICKY      GPIO_NONSTICKY
#define GPIO_41_STICKY      GPIO_NONSTICKY
#define GPIO_42_STICKY      GPIO_NONSTICKY
#define GPIO_43_STICKY      GPIO_NONSTICKY
#define GPIO_44_STICKY      GPIO_NONSTICKY
#define GPIO_45_STICKY      GPIO_NONSTICKY
#define GPIO_46_STICKY      GPIO_NONSTICKY
#define GPIO_47_STICKY      GPIO_NONSTICKY
#define GPIO_48_STICKY      GPIO_NONSTICKY
#define GPIO_49_STICKY      GPIO_NONSTICKY
#define GPIO_50_STICKY      GPIO_NONSTICKY
#define GPIO_51_STICKY      GPIO_NONSTICKY
#define GPIO_52_STICKY      GPIO_NONSTICKY
#define GPIO_53_STICKY      GPIO_NONSTICKY
#define GPIO_54_STICKY      GPIO_NONSTICKY
#define GPIO_55_STICKY      GPIO_NONSTICKY
#define GPIO_56_STICKY      GPIO_NONSTICKY
#define GPIO_57_STICKY      GPIO_NONSTICKY
#define GPIO_58_STICKY      GPIO_NONSTICKY
#define GPIO_59_STICKY      GPIO_NONSTICKY
#define GPIO_60_STICKY      GPIO_NONSTICKY
#define GPIO_61_STICKY      GPIO_NONSTICKY
#define GPIO_62_STICKY      GPIO_NONSTICKY
#define GPIO_63_STICKY      GPIO_NONSTICKY
#define GPIO_64_STICKY      GPIO_NONSTICKY
#define GPIO_65_STICKY      GPIO_NONSTICKY
#define GPIO_66_STICKY      GPIO_NONSTICKY
#define GPIO_67_STICKY      GPIO_NONSTICKY
#define GPIO_68_STICKY      GPIO_NONSTICKY
#define GPIO_69_STICKY      GPIO_NONSTICKY
#define GPIO_70_STICKY      GPIO_NONSTICKY
#define GPIO_71_STICKY      GPIO_NONSTICKY
#define GPIO_72_STICKY      GPIO_NONSTICKY
#define GPIO_73_STICKY      GPIO_NONSTICKY
#define GPIO_74_STICKY      GPIO_NONSTICKY
#define GPIO_75_STICKY      GPIO_NONSTICKY
#define GPIO_76_STICKY      GPIO_NONSTICKY
#define GPIO_77_STICKY      GPIO_NONSTICKY
#define GPIO_78_STICKY      GPIO_NONSTICKY
#define GPIO_79_STICKY      GPIO_NONSTICKY
#define GPIO_80_STICKY      GPIO_NONSTICKY
#define GPIO_81_STICKY      GPIO_NONSTICKY
#define GPIO_82_STICKY      GPIO_NONSTICKY
#define GPIO_83_STICKY      GPIO_NONSTICKY
#define GPIO_84_STICKY      GPIO_NONSTICKY
#define GPIO_85_STICKY      GPIO_NONSTICKY
#define GPIO_86_STICKY      GPIO_NONSTICKY
#define GPIO_87_STICKY      GPIO_NONSTICKY
#define GPIO_88_STICKY      GPIO_NONSTICKY
#define GPIO_89_STICKY      GPIO_NONSTICKY
#define GPIO_90_STICKY      GPIO_NONSTICKY
#define GPIO_91_STICKY      GPIO_NONSTICKY
#define GPIO_92_STICKY      GPIO_NONSTICKY
#define GPIO_93_STICKY      GPIO_NONSTICKY
#define GPIO_94_STICKY      GPIO_NONSTICKY
#define GPIO_95_STICKY      GPIO_NONSTICKY
#define GPIO_96_STICKY      GPIO_NONSTICKY
#define GPIO_97_STICKY      GPIO_NONSTICKY
#define GPIO_98_STICKY      GPIO_NONSTICKY
#define GPIO_99_STICKY      GPIO_NONSTICKY
#define GPIO_100_STICKY     GPIO_NONSTICKY
#define GPIO_101_STICKY     GPIO_NONSTICKY
#define GPIO_102_STICKY     GPIO_STICKY
#define GPIO_103_STICKY     GPIO_STICKY
#define GPIO_104_STICKY     GPIO_NONSTICKY
#define GPIO_105_STICKY     GPIO_NONSTICKY
#define GPIO_106_STICKY     GPIO_NONSTICKY
#define GPIO_107_STICKY     GPIO_NONSTICKY
#define GPIO_108_STICKY     GPIO_STICKY
#define GPIO_109_STICKY     GPIO_NONSTICKY
#define GPIO_110_STICKY     GPIO_NONSTICKY
#define GPIO_111_STICKY     GPIO_NONSTICKY
#define GPIO_112_STICKY     GPIO_NONSTICKY
#define GPIO_113_STICKY     GPIO_NONSTICKY
#define GPIO_114_STICKY     GPIO_NONSTICKY
#define GPIO_115_STICKY     GPIO_NONSTICKY
#define GPIO_116_STICKY     GPIO_NONSTICKY
#define GPIO_117_STICKY     GPIO_NONSTICKY
#define GPIO_118_STICKY     GPIO_NONSTICKY
#define GPIO_119_STICKY     GPIO_NONSTICKY
#define GPIO_120_STICKY     GPIO_NONSTICKY
#define GPIO_121_STICKY     GPIO_NONSTICKY
#define GPIO_122_STICKY     GPIO_NONSTICKY
#define GPIO_123_STICKY     GPIO_NONSTICKY
#define GPIO_124_STICKY     GPIO_NONSTICKY
#define GPIO_125_STICKY     GPIO_NONSTICKY
#define GPIO_126_STICKY     GPIO_NONSTICKY
#define GPIO_127_STICKY     GPIO_NONSTICKY
#define GPIO_128_STICKY     GPIO_NONSTICKY
#define GPIO_129_STICKY     GPIO_NONSTICKY
#define GPIO_130_STICKY     GPIO_NONSTICKY
#define GPIO_131_STICKY     GPIO_NONSTICKY
#define GPIO_132_STICKY     GPIO_NONSTICKY
#define GPIO_133_STICKY     GPIO_NONSTICKY
#define GPIO_134_STICKY     GPIO_NONSTICKY
#define GPIO_135_STICKY     GPIO_NONSTICKY
#define GPIO_136_STICKY     GPIO_NONSTICKY
#define GPIO_137_STICKY     GPIO_NONSTICKY
#define GPIO_138_STICKY     GPIO_NONSTICKY
#define GPIO_139_STICKY     GPIO_NONSTICKY
#define GPIO_140_STICKY     GPIO_NONSTICKY
#define GPIO_141_STICKY     GPIO_NONSTICKY
#define GPIO_142_STICKY     GPIO_NONSTICKY
#define GPIO_143_STICKY     GPIO_NONSTICKY
#define GPIO_144_STICKY     GPIO_NONSTICKY
#define GPIO_145_STICKY     GPIO_NONSTICKY
#define GPIO_146_STICKY     GPIO_NONSTICKY
#define GPIO_147_STICKY     GPIO_NONSTICKY
#define GPIO_148_STICKY     GPIO_NONSTICKY
#define GPIO_149_STICKY     GPIO_NONSTICKY
#define GPIO_150_STICKY     GPIO_NONSTICKY
#define GPIO_151_STICKY     GPIO_NONSTICKY
#define GPIO_152_STICKY     GPIO_NONSTICKY
#define GPIO_153_STICKY     GPIO_NONSTICKY
#define GPIO_154_STICKY     GPIO_NONSTICKY
#define GPIO_155_STICKY     GPIO_NONSTICKY
#define GPIO_156_STICKY     GPIO_NONSTICKY
#define GPIO_157_STICKY     GPIO_NONSTICKY
#define GPIO_158_STICKY     GPIO_NONSTICKY
#define GPIO_159_STICKY     GPIO_NONSTICKY
#define GPIO_160_STICKY     GPIO_NONSTICKY
#define GPIO_161_STICKY     GPIO_NONSTICKY
#define GPIO_162_STICKY     GPIO_NONSTICKY
#define GPIO_163_STICKY     GPIO_NONSTICKY
#define GPIO_164_STICKY     GPIO_NONSTICKY
#define GPIO_165_STICKY     GPIO_NONSTICKY
#define GPIO_166_STICKY     GPIO_NONSTICKY
#define GPIO_167_STICKY     GPIO_NONSTICKY
#define GPIO_168_STICKY     GPIO_NONSTICKY
#define GPIO_169_STICKY     GPIO_NONSTICKY
#define GPIO_170_STICKY     GPIO_STICKY
#define GPIO_171_STICKY     GPIO_NONSTICKY
#define GPIO_172_STICKY     GPIO_STICKY
#define GPIO_173_STICKY     GPIO_NONSTICKY
#define GPIO_174_STICKY     GPIO_NONSTICKY
#define GPIO_175_STICKY     GPIO_NONSTICKY
#define GPIO_176_STICKY     GPIO_NONSTICKY
#define GPIO_177_STICKY     GPIO_NONSTICKY
#define GPIO_178_STICKY     GPIO_NONSTICKY
#define GPIO_179_STICKY     GPIO_NONSTICKY
#define GPIO_180_STICKY     GPIO_NONSTICKY
#define GPIO_181_STICKY     GPIO_NONSTICKY
#define GPIO_182_STICKY     GPIO_NONSTICKY
#define GPIO_183_STICKY     GPIO_NONSTICKY
#define GPIO_184_STICKY     GPIO_NONSTICKY
#define GPIO_185_STICKY     GPIO_NONSTICKY
#define GPIO_186_STICKY     GPIO_NONSTICKY
#define GPIO_187_STICKY     GPIO_NONSTICKY
#define GPIO_188_STICKY     GPIO_NONSTICKY
#define GPIO_189_STICKY     GPIO_NONSTICKY
#define GPIO_190_STICKY     GPIO_NONSTICKY
#define GPIO_191_STICKY     GPIO_NONSTICKY
#define GPIO_192_STICKY     GPIO_NONSTICKY
#define GPIO_193_STICKY     GPIO_NONSTICKY
#define GPIO_194_STICKY     GPIO_NONSTICKY
#define GPIO_195_STICKY     GPIO_NONSTICKY
#define GPIO_196_STICKY     GPIO_NONSTICKY
#define GPIO_197_STICKY     GPIO_NONSTICKY
#define GPIO_198_STICKY     GPIO_NONSTICKY
#define GPIO_199_STICKY     GPIO_NONSTICKY
#define GPIO_200_STICKY     GPIO_NONSTICKY
#define GPIO_201_STICKY     GPIO_NONSTICKY
#define GPIO_202_STICKY     GPIO_NONSTICKY
#define GPIO_203_STICKY     GPIO_NONSTICKY
#define GPIO_204_STICKY     GPIO_NONSTICKY
#define GPIO_205_STICKY     GPIO_NONSTICKY
#define GPIO_206_STICKY     GPIO_NONSTICKY
#define GPIO_207_STICKY     GPIO_NONSTICKY
#define GPIO_208_STICKY     GPIO_NONSTICKY
#define GPIO_209_STICKY     GPIO_NONSTICKY
#define GPIO_210_STICKY     GPIO_NONSTICKY
#define GPIO_211_STICKY     GPIO_NONSTICKY
#define GPIO_212_STICKY     GPIO_NONSTICKY
#define GPIO_213_STICKY     GPIO_NONSTICKY
#define GPIO_214_STICKY     GPIO_NONSTICKY
#define GPIO_215_STICKY     GPIO_NONSTICKY
#define GPIO_216_STICKY     GPIO_NONSTICKY
#define GPIO_217_STICKY     GPIO_NONSTICKY
#define GPIO_218_STICKY     GPIO_NONSTICKY
#define GPIO_219_STICKY     GPIO_NONSTICKY
#define GPIO_220_STICKY     GPIO_NONSTICKY
#define GPIO_221_STICKY     GPIO_NONSTICKY
#define GPIO_222_STICKY     GPIO_NONSTICKY
#define GPIO_223_STICKY     GPIO_NONSTICKY
#define GPIO_224_STICKY     GPIO_NONSTICKY
#define GPIO_225_STICKY     GPIO_NONSTICKY
#define GPIO_226_STICKY     GPIO_NONSTICKY
#define GPIO_227_STICKY     GPIO_NONSTICKY
#define GPIO_228_STICKY     GPIO_NONSTICKY
#define GPIO_229_STICKY     GPIO_NONSTICKY

#define PULLUP_ENABLE      (0 << 3)
#define PULLUP_DISABLE     (1 << 3)

#define GPIO_00_PULLUP      PULLUP_DISABLE
#define GPIO_01_PULLUP      PULLUP_DISABLE
#define GPIO_02_PULLUP      PULLUP_DISABLE
#define GPIO_03_PULLUP      PULLUP_DISABLE
#define GPIO_04_PULLUP      PULLUP_DISABLE
#define GPIO_05_PULLUP      PULLUP_DISABLE
#define GPIO_06_PULLUP      PULLUP_DISABLE
#define GPIO_07_PULLUP      PULLUP_DISABLE
#define GPIO_08_PULLUP      PULLUP_DISABLE
#define GPIO_09_PULLUP      PULLUP_DISABLE
#define GPIO_10_PULLUP      PULLUP_DISABLE
#define GPIO_11_PULLUP      PULLUP_DISABLE
#define GPIO_12_PULLUP      PULLUP_DISABLE
#define GPIO_13_PULLUP      PULLUP_DISABLE
#define GPIO_14_PULLUP      PULLUP_DISABLE
#define GPIO_15_PULLUP      PULLUP_DISABLE
#define GPIO_16_PULLUP      PULLUP_DISABLE
#define GPIO_17_PULLUP      PULLUP_DISABLE
#define GPIO_18_PULLUP      PULLUP_DISABLE
#define GPIO_19_PULLUP      PULLUP_DISABLE
#define GPIO_20_PULLUP      PULLUP_DISABLE
#define GPIO_21_PULLUP      PULLUP_DISABLE
#define GPIO_22_PULLUP      PULLUP_DISABLE
#define GPIO_23_PULLUP      PULLUP_DISABLE
#define GPIO_24_PULLUP      PULLUP_DISABLE
#define GPIO_25_PULLUP      PULLUP_DISABLE
#define GPIO_26_PULLUP      PULLUP_DISABLE
#define GPIO_27_PULLUP      PULLUP_DISABLE
#define GPIO_28_PULLUP      PULLUP_DISABLE
#define GPIO_29_PULLUP      PULLUP_DISABLE
#define GPIO_30_PULLUP      PULLUP_DISABLE
#define GPIO_31_PULLUP      PULLUP_DISABLE
#define GPIO_32_PULLUP      PULLUP_DISABLE
#define GPIO_33_PULLUP      PULLUP_DISABLE
#define GPIO_34_PULLUP      PULLUP_DISABLE
#define GPIO_35_PULLUP      PULLUP_DISABLE
#define GPIO_36_PULLUP      PULLUP_DISABLE
#define GPIO_37_PULLUP      PULLUP_DISABLE
#define GPIO_38_PULLUP      PULLUP_DISABLE
#define GPIO_39_PULLUP      PULLUP_DISABLE
#define GPIO_40_PULLUP      PULLUP_DISABLE
#define GPIO_41_PULLUP      PULLUP_DISABLE
#define GPIO_42_PULLUP      PULLUP_DISABLE
#define GPIO_43_PULLUP      PULLUP_DISABLE
#define GPIO_44_PULLUP      PULLUP_DISABLE
#define GPIO_45_PULLUP      PULLUP_DISABLE
#define GPIO_46_PULLUP      PULLUP_DISABLE
#define GPIO_47_PULLUP      PULLUP_DISABLE
#define GPIO_48_PULLUP      PULLUP_DISABLE
#define GPIO_49_PULLUP      PULLUP_DISABLE
#define GPIO_50_PULLUP      PULLUP_DISABLE
#define GPIO_51_PULLUP      PULLUP_DISABLE
#define GPIO_52_PULLUP      PULLUP_DISABLE
#define GPIO_53_PULLUP      PULLUP_DISABLE
#define GPIO_54_PULLUP      PULLUP_DISABLE
#define GPIO_55_PULLUP      PULLUP_DISABLE
#define GPIO_56_PULLUP      PULLUP_DISABLE
#define GPIO_57_PULLUP      PULLUP_DISABLE
#define GPIO_58_PULLUP      PULLUP_DISABLE
#define GPIO_59_PULLUP      PULLUP_DISABLE
#define GPIO_60_PULLUP      PULLUP_DISABLE
#define GPIO_61_PULLUP      PULLUP_DISABLE
#define GPIO_62_PULLUP      PULLUP_DISABLE
#define GPIO_63_PULLUP      PULLUP_DISABLE
#define GPIO_64_PULLUP      PULLUP_DISABLE
#define GPIO_65_PULLUP      PULLUP_DISABLE
#define GPIO_66_PULLUP      PULLUP_DISABLE
#define GPIO_67_PULLUP      PULLUP_DISABLE
#define GPIO_68_PULLUP      PULLUP_DISABLE
#define GPIO_69_PULLUP      PULLUP_DISABLE
#define GPIO_70_PULLUP      PULLUP_DISABLE
#define GPIO_71_PULLUP      PULLUP_DISABLE
#define GPIO_72_PULLUP      PULLUP_DISABLE
#define GPIO_73_PULLUP      PULLUP_DISABLE
#define GPIO_74_PULLUP      PULLUP_DISABLE
#define GPIO_75_PULLUP      PULLUP_DISABLE
#define GPIO_76_PULLUP      PULLUP_DISABLE
#define GPIO_77_PULLUP      PULLUP_DISABLE
#define GPIO_78_PULLUP      PULLUP_DISABLE
#define GPIO_79_PULLUP      PULLUP_DISABLE
#define GPIO_80_PULLUP      PULLUP_DISABLE
#define GPIO_80_PULLUP      PULLUP_DISABLE
#define GPIO_81_PULLUP      PULLUP_DISABLE
#define GPIO_82_PULLUP      PULLUP_DISABLE
#define GPIO_83_PULLUP      PULLUP_DISABLE
#define GPIO_84_PULLUP      PULLUP_DISABLE
#define GPIO_85_PULLUP      PULLUP_DISABLE
#define GPIO_86_PULLUP      PULLUP_DISABLE
#define GPIO_87_PULLUP      PULLUP_DISABLE
#define GPIO_88_PULLUP      PULLUP_DISABLE
#define GPIO_89_PULLUP      PULLUP_DISABLE
#define GPIO_90_PULLUP      PULLUP_DISABLE
#define GPIO_91_PULLUP      PULLUP_DISABLE
#define GPIO_92_PULLUP      PULLUP_DISABLE
#define GPIO_93_PULLUP      PULLUP_DISABLE
#define GPIO_94_PULLUP      PULLUP_DISABLE
#define GPIO_95_PULLUP      PULLUP_DISABLE
#define GPIO_96_PULLUP      PULLUP_DISABLE
#define GPIO_97_PULLUP      PULLUP_DISABLE
#define GPIO_98_PULLUP      PULLUP_DISABLE
#define GPIO_99_PULLUP      PULLUP_DISABLE
#define GPIO_100_PULLUP     PULLUP_DISABLE
#define GPIO_101_PULLUP     PULLUP_DISABLE
#define GPIO_102_PULLUP     PULLUP_DISABLE
#define GPIO_103_PULLUP     PULLUP_DISABLE
#define GPIO_104_PULLUP     PULLUP_DISABLE
#define GPIO_105_PULLUP     PULLUP_DISABLE
#define GPIO_106_PULLUP     PULLUP_DISABLE
#define GPIO_107_PULLUP     PULLUP_DISABLE
#define GPIO_108_PULLUP     PULLUP_DISABLE
#define GPIO_109_PULLUP     PULLUP_DISABLE
#define GPIO_110_PULLUP     PULLUP_DISABLE
#define GPIO_111_PULLUP     PULLUP_DISABLE
#define GPIO_112_PULLUP     PULLUP_DISABLE
#define GPIO_113_PULLUP     PULLUP_DISABLE
#define GPIO_114_PULLUP     PULLUP_DISABLE
#define GPIO_115_PULLUP     PULLUP_DISABLE
#define GPIO_116_PULLUP     PULLUP_DISABLE
#define GPIO_117_PULLUP     PULLUP_DISABLE
#define GPIO_118_PULLUP     PULLUP_ENABLE
#define GPIO_119_PULLUP     PULLUP_DISABLE
#define GPIO_120_PULLUP     PULLUP_DISABLE
#define GPIO_121_PULLUP     PULLUP_DISABLE
#define GPIO_122_PULLUP     PULLUP_DISABLE
#define GPIO_123_PULLUP     PULLUP_DISABLE
#define GPIO_124_PULLUP     PULLUP_DISABLE
#define GPIO_125_PULLUP     PULLUP_DISABLE
#define GPIO_126_PULLUP     PULLUP_DISABLE
#define GPIO_127_PULLUP     PULLUP_DISABLE
#define GPIO_128_PULLUP     PULLUP_DISABLE
#define GPIO_129_PULLUP     PULLUP_DISABLE
#define GPIO_130_PULLUP     PULLUP_DISABLE
#define GPIO_131_PULLUP     PULLUP_DISABLE
#define GPIO_132_PULLUP     PULLUP_DISABLE
#define GPIO_133_PULLUP     PULLUP_DISABLE
#define GPIO_134_PULLUP     PULLUP_DISABLE
#define GPIO_135_PULLUP     PULLUP_DISABLE
#define GPIO_136_PULLUP     PULLUP_DISABLE
#define GPIO_137_PULLUP     PULLUP_DISABLE
#define GPIO_138_PULLUP     PULLUP_DISABLE
#define GPIO_139_PULLUP     PULLUP_DISABLE
#define GPIO_140_PULLUP     PULLUP_DISABLE
#define GPIO_141_PULLUP     PULLUP_DISABLE
#define GPIO_142_PULLUP     PULLUP_DISABLE
#define GPIO_143_PULLUP     PULLUP_DISABLE
#define GPIO_144_PULLUP     PULLUP_DISABLE
#define GPIO_145_PULLUP     PULLUP_DISABLE
#define GPIO_146_PULLUP     PULLUP_DISABLE
#define GPIO_147_PULLUP     PULLUP_DISABLE
#define GPIO_148_PULLUP     PULLUP_DISABLE
#define GPIO_149_PULLUP     PULLUP_DISABLE
#define GPIO_150_PULLUP     PULLUP_DISABLE
#define GPIO_151_PULLUP     PULLUP_DISABLE
#define GPIO_152_PULLUP     PULLUP_DISABLE
#define GPIO_153_PULLUP     PULLUP_DISABLE
#define GPIO_154_PULLUP     PULLUP_DISABLE
#define GPIO_155_PULLUP     PULLUP_DISABLE
#define GPIO_156_PULLUP     PULLUP_DISABLE
#define GPIO_157_PULLUP     PULLUP_DISABLE
#define GPIO_158_PULLUP     PULLUP_DISABLE
#define GPIO_159_PULLUP     PULLUP_DISABLE
#define GPIO_160_PULLUP     PULLUP_DISABLE
#define GPIO_161_PULLUP     PULLUP_DISABLE
#define GPIO_162_PULLUP     PULLUP_DISABLE
#define GPIO_163_PULLUP     PULLUP_DISABLE
#define GPIO_164_PULLUP     PULLUP_DISABLE
#define GPIO_165_PULLUP     PULLUP_DISABLE
#define GPIO_166_PULLUP     PULLUP_DISABLE
#define GPIO_167_PULLUP     PULLUP_DISABLE
#define GPIO_168_PULLUP     PULLUP_DISABLE
#define GPIO_169_PULLUP     PULLUP_DISABLE
#define GPIO_170_PULLUP     PULLUP_DISABLE
#define GPIO_171_PULLUP     PULLUP_DISABLE
#define GPIO_172_PULLUP     PULLUP_DISABLE
#define GPIO_173_PULLUP     PULLUP_DISABLE
#define GPIO_174_PULLUP     PULLUP_DISABLE
#define GPIO_175_PULLUP     PULLUP_DISABLE
#define GPIO_176_PULLUP     PULLUP_DISABLE
#define GPIO_177_PULLUP     PULLUP_DISABLE
#define GPIO_178_PULLUP     PULLUP_DISABLE
#define GPIO_179_PULLUP     PULLUP_DISABLE
#define GPIO_180_PULLUP     PULLUP_DISABLE
#define GPIO_180_PULLUP     PULLUP_DISABLE
#define GPIO_181_PULLUP     PULLUP_DISABLE
#define GPIO_182_PULLUP     PULLUP_DISABLE
#define GPIO_183_PULLUP     PULLUP_DISABLE
#define GPIO_184_PULLUP     PULLUP_DISABLE
#define GPIO_185_PULLUP     PULLUP_DISABLE
#define GPIO_186_PULLUP     PULLUP_DISABLE
#define GPIO_187_PULLUP     PULLUP_DISABLE
#define GPIO_188_PULLUP     PULLUP_DISABLE
#define GPIO_189_PULLUP     PULLUP_DISABLE
#define GPIO_190_PULLUP     PULLUP_DISABLE
#define GPIO_191_PULLUP     PULLUP_DISABLE
#define GPIO_192_PULLUP     PULLUP_DISABLE
#define GPIO_193_PULLUP     PULLUP_DISABLE
#define GPIO_194_PULLUP     PULLUP_DISABLE
#define GPIO_195_PULLUP     PULLUP_DISABLE
#define GPIO_196_PULLUP     PULLUP_DISABLE
#define GPIO_197_PULLUP     PULLUP_DISABLE
#define GPIO_198_PULLUP     PULLUP_DISABLE
#define GPIO_199_PULLUP     PULLUP_DISABLE
#define GPIO_200_PULLUP     PULLUP_DISABLE
#define GPIO_201_PULLUP     PULLUP_DISABLE
#define GPIO_202_PULLUP     PULLUP_DISABLE
#define GPIO_203_PULLUP     PULLUP_DISABLE
#define GPIO_204_PULLUP     PULLUP_DISABLE
#define GPIO_205_PULLUP     PULLUP_DISABLE
#define GPIO_206_PULLUP     PULLUP_DISABLE
#define GPIO_207_PULLUP     PULLUP_DISABLE
#define GPIO_208_PULLUP     PULLUP_DISABLE
#define GPIO_209_PULLUP     PULLUP_DISABLE
#define GPIO_210_PULLUP     PULLUP_DISABLE
#define GPIO_211_PULLUP     PULLUP_DISABLE
#define GPIO_212_PULLUP     PULLUP_DISABLE
#define GPIO_213_PULLUP     PULLUP_DISABLE
#define GPIO_214_PULLUP     PULLUP_DISABLE
#define GPIO_215_PULLUP     PULLUP_DISABLE
#define GPIO_216_PULLUP     PULLUP_DISABLE
#define GPIO_217_PULLUP     PULLUP_DISABLE
#define GPIO_218_PULLUP     PULLUP_DISABLE
#define GPIO_219_PULLUP     PULLUP_DISABLE
#define GPIO_220_PULLUP     PULLUP_DISABLE
#define GPIO_221_PULLUP     PULLUP_DISABLE
#define GPIO_222_PULLUP     PULLUP_DISABLE
#define GPIO_223_PULLUP     PULLUP_DISABLE
#define GPIO_224_PULLUP     PULLUP_DISABLE
#define GPIO_225_PULLUP     PULLUP_DISABLE
#define GPIO_226_PULLUP     PULLUP_DISABLE
#define GPIO_227_PULLUP     PULLUP_DISABLE
#define GPIO_228_PULLUP     PULLUP_DISABLE
#define GPIO_229_PULLUP     PULLUP_DISABLE

#define PULLDOWN_ENABLE       (1 << 4)
#define PULLDOWN_DISABLE      (0 << 4)

#define GPIO_00_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_01_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_02_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_03_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_04_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_05_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_06_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_07_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_08_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_09_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_10_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_11_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_12_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_13_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_14_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_15_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_16_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_17_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_18_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_19_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_20_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_21_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_22_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_23_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_24_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_25_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_26_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_27_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_28_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_29_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_30_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_31_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_32_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_33_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_34_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_35_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_36_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_37_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_38_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_39_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_40_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_41_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_42_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_43_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_44_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_45_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_46_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_47_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_48_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_49_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_50_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_51_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_52_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_53_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_54_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_55_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_56_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_57_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_58_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_59_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_60_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_61_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_62_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_63_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_64_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_65_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_66_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_67_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_68_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_69_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_70_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_71_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_72_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_73_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_74_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_75_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_76_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_77_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_78_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_79_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_80_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_80_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_81_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_82_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_83_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_84_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_85_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_86_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_87_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_88_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_89_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_90_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_91_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_92_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_93_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_94_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_95_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_96_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_97_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_98_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_99_PULLDOWN    PULLDOWN_DISABLE
#define GPIO_100_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_101_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_102_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_103_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_104_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_105_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_106_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_107_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_108_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_109_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_110_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_111_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_112_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_113_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_114_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_115_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_116_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_117_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_118_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_119_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_120_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_121_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_122_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_123_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_124_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_125_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_126_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_127_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_128_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_129_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_130_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_131_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_132_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_133_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_134_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_135_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_136_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_137_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_138_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_139_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_140_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_141_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_142_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_143_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_144_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_145_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_146_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_147_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_148_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_149_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_150_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_151_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_152_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_153_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_154_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_155_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_156_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_157_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_158_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_159_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_160_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_161_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_162_PULLDOWN   PULLDOWN_ENABLE
#define GPIO_163_PULLDOWN   PULLDOWN_ENABLE
#define GPIO_164_PULLDOWN   PULLDOWN_ENABLE
#define GPIO_165_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_166_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_167_PULLDOWN   PULLDOWN_ENABLE
#define GPIO_168_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_169_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_170_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_171_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_172_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_173_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_174_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_175_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_176_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_177_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_178_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_179_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_180_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_180_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_181_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_182_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_183_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_184_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_185_PULLDOWN   PULLDOWN_ENABLE
#define GPIO_186_PULLDOWN   PULLDOWN_ENABLE
#define GPIO_187_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_188_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_189_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_190_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_191_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_192_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_193_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_194_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_195_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_196_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_197_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_198_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_199_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_200_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_201_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_202_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_203_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_204_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_205_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_206_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_207_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_208_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_209_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_210_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_211_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_212_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_213_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_214_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_215_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_216_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_217_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_218_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_219_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_220_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_221_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_222_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_223_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_224_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_225_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_226_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_227_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_228_PULLDOWN   PULLDOWN_DISABLE
#define GPIO_229_PULLDOWN   PULLDOWN_DISABLE

#define EVENT_DISABLE           0
#define EVENT_ENABLE            1

#define GEVENT_00_EVENTENABLE   EVENT_DISABLE
#define GEVENT_01_EVENTENABLE   EVENT_DISABLE
#define GEVENT_02_EVENTENABLE   EVENT_ENABLE	// APU THERMTRIP#
#define GEVENT_03_EVENTENABLE   EVENT_ENABLE    // EC_SCI#
#define GEVENT_04_EVENTENABLE   EVENT_ENABLE    // APU_MEMHOT#
#define GEVENT_05_EVENTENABLE   EVENT_ENABLE    // PCIE_EXPCARD_PWREN#
#define GEVENT_06_EVENTENABLE   EVENT_DISABLE
#define GEVENT_07_EVENTENABLE   EVENT_DISABLE
#define GEVENT_08_EVENTENABLE   EVENT_DISABLE
#define GEVENT_09_EVENTENABLE   EVENT_ENABLE    // WF_RADIO
#define GEVENT_10_EVENTENABLE   EVENT_DISABLE
#define GEVENT_11_EVENTENABLE   EVENT_DISABLE
#define GEVENT_12_EVENTENABLE   EVENT_ENABLE    // SMBALERT#
#define GEVENT_13_EVENTENABLE   EVENT_DISABLE
#define GEVENT_14_EVENTENABLE   EVENT_ENABLE    // LASSO_DET#/DOCK#
#define GEVENT_15_EVENTENABLE   EVENT_ENABLE    // ODD_PLUGIN#
#define GEVENT_16_EVENTENABLE   EVENT_ENABLE    // ODD_DA
#define GEVENT_17_EVENTENABLE   EVENT_ENABLE    // TWARN
#define GEVENT_18_EVENTENABLE   EVENT_DISABLE
#define GEVENT_19_EVENTENABLE   EVENT_DISABLE
#define GEVENT_20_EVENTENABLE   EVENT_DISABLE
#define GEVENT_21_EVENTENABLE   EVENT_DISABLE
#define GEVENT_22_EVENTENABLE   EVENT_ENABLE    // LID_CLOSE#
#define GEVENT_23_EVENTENABLE   EVENT_DISABLE   // EC_SMI#

#define SCITRIG_LOW             0
#define SCITRIG_HI              1

#define GEVENT_00_SCITRIG       SCITRIG_LOW
#define GEVENT_01_SCITRIG       SCITRIG_LOW
#define GEVENT_02_SCITRIG       SCITRIG_LOW
#define GEVENT_03_SCITRIG       SCITRIG_LOW
#define GEVENT_04_SCITRIG       SCITRIG_LOW
#define GEVENT_05_SCITRIG       SCITRIG_LOW
#define GEVENT_06_SCITRIG       SCITRIG_LOW
#define GEVENT_07_SCITRIG       SCITRIG_LOW
#define GEVENT_08_SCITRIG       SCITRIG_LOW
#define GEVENT_09_SCITRIG       SCITRIG_LOW
#define GEVENT_10_SCITRIG       SCITRIG_LOW
#define GEVENT_11_SCITRIG       SCITRIG_LOW
#define GEVENT_12_SCITRIG       SCITRIG_LOW
#define GEVENT_13_SCITRIG       SCITRIG_LOW
#define GEVENT_14_SCITRIG       SCITRIG_LOW
#define GEVENT_15_SCITRIG       SCITRIG_LOW
#define GEVENT_16_SCITRIG       SCITRIG_LOW
#define GEVENT_17_SCITRIG       SCITRIG_HI
#define GEVENT_18_SCITRIG       SCITRIG_LOW
#define GEVENT_19_SCITRIG       SCITRIG_LOW
#define GEVENT_20_SCITRIG       SCITRIG_LOW
#define GEVENT_21_SCITRIG       SCITRIG_LOW
#define GEVENT_22_SCITRIG       SCITRIG_LOW
#define GEVENT_23_SCITRIG       SCITRIG_LOW

#define SCILEVEL_EDGE           0
#define SCILEVEL_LEVEL          1

#define GEVENT_00_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_01_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_02_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_03_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_04_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_05_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_06_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_07_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_08_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_09_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_10_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_11_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_12_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_13_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_14_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_15_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_16_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_17_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_18_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_19_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_20_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_21_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_22_SCILEVEL      SCILEVEL_EDGE
#define GEVENT_23_SCILEVEL      SCILEVEL_EDGE

#define SMISCI_DISABLE          0
#define SMISCI_ENABLE           1

#define GEVENT_00_SMISCIEN      SMISCI_DISABLE
#define GEVENT_01_SMISCIEN      SMISCI_DISABLE
#define GEVENT_02_SMISCIEN      SMISCI_DISABLE
#define GEVENT_03_SMISCIEN      SMISCI_DISABLE
#define GEVENT_04_SMISCIEN      SMISCI_DISABLE
#define GEVENT_05_SMISCIEN      SMISCI_DISABLE
#define GEVENT_06_SMISCIEN      SMISCI_DISABLE
#define GEVENT_07_SMISCIEN      SMISCI_DISABLE
#define GEVENT_08_SMISCIEN      SMISCI_DISABLE
#define GEVENT_09_SMISCIEN      SMISCI_DISABLE
#define GEVENT_10_SMISCIEN      SMISCI_DISABLE
#define GEVENT_11_SMISCIEN      SMISCI_DISABLE
#define GEVENT_12_SMISCIEN      SMISCI_DISABLE
#define GEVENT_13_SMISCIEN      SMISCI_DISABLE
#define GEVENT_14_SMISCIEN      SMISCI_DISABLE
#define GEVENT_15_SMISCIEN      SMISCI_DISABLE
#define GEVENT_16_SMISCIEN      SMISCI_DISABLE
#define GEVENT_17_SMISCIEN      SMISCI_DISABLE
#define GEVENT_18_SMISCIEN      SMISCI_DISABLE
#define GEVENT_19_SMISCIEN      SMISCI_DISABLE
#define GEVENT_20_SMISCIEN      SMISCI_DISABLE
#define GEVENT_21_SMISCIEN      SMISCI_DISABLE
#define GEVENT_22_SMISCIEN      SMISCI_DISABLE
#define GEVENT_23_SMISCIEN      SMISCI_DISABLE

#define SCIS0_DISABLE           0
#define SCIS0_ENABLE            1

#define GEVENT_00_SCIS0EN       SCIS0_DISABLE
#define GEVENT_01_SCIS0EN       SCIS0_DISABLE
#define GEVENT_02_SCIS0EN       SCIS0_DISABLE
#define GEVENT_03_SCIS0EN       SCIS0_DISABLE
#define GEVENT_04_SCIS0EN       SCIS0_DISABLE
#define GEVENT_05_SCIS0EN       SCIS0_DISABLE
#define GEVENT_06_SCIS0EN       SCIS0_DISABLE
#define GEVENT_07_SCIS0EN       SCIS0_DISABLE
#define GEVENT_08_SCIS0EN       SCIS0_DISABLE
#define GEVENT_09_SCIS0EN       SCIS0_DISABLE
#define GEVENT_10_SCIS0EN       SCIS0_DISABLE
#define GEVENT_11_SCIS0EN       SCIS0_DISABLE
#define GEVENT_12_SCIS0EN       SCIS0_DISABLE
#define GEVENT_13_SCIS0EN       SCIS0_DISABLE
#define GEVENT_14_SCIS0EN       SCIS0_DISABLE
#define GEVENT_15_SCIS0EN       SCIS0_DISABLE
#define GEVENT_16_SCIS0EN       SCIS0_DISABLE
#define GEVENT_17_SCIS0EN       SCIS0_DISABLE
#define GEVENT_18_SCIS0EN       SCIS0_DISABLE
#define GEVENT_19_SCIS0EN       SCIS0_DISABLE
#define GEVENT_20_SCIS0EN       SCIS0_DISABLE
#define GEVENT_21_SCIS0EN       SCIS0_DISABLE
#define GEVENT_22_SCIS0EN       SCIS0_DISABLE
#define GEVENT_23_SCIS0EN       SCIS0_DISABLE

#define GEVENT_SCIMASK          0x1F
#define GEVENT_00_SCIMAP        0
#define GEVENT_01_SCIMAP        1
#define GEVENT_02_SCIMAP        2
#define GEVENT_03_SCIMAP        3
#define GEVENT_04_SCIMAP        4
#define GEVENT_05_SCIMAP        5
#define GEVENT_06_SCIMAP        6
#define GEVENT_07_SCIMAP        7
#define GEVENT_08_SCIMAP        8
#define GEVENT_09_SCIMAP        9
#define GEVENT_10_SCIMAP        10
#define GEVENT_11_SCIMAP        11
#define GEVENT_12_SCIMAP        12
#define GEVENT_13_SCIMAP        13
#define GEVENT_14_SCIMAP        14
#define GEVENT_15_SCIMAP        15
#define GEVENT_16_SCIMAP        16
#define GEVENT_17_SCIMAP        17
#define GEVENT_18_SCIMAP        18
#define GEVENT_19_SCIMAP        19
#define GEVENT_20_SCIMAP        20
#define GEVENT_21_SCIMAP        21
#define GEVENT_22_SCIMAP        22
#define GEVENT_23_SCIMAP        23

#define SMITRIG_LOW             0
#define SMITRIG_HI              1

#define GEVENT_00_SMITRIG       SMITRIG_HI
#define GEVENT_01_SMITRIG       SMITRIG_HI
#define GEVENT_02_SMITRIG       SMITRIG_HI
#define GEVENT_03_SMITRIG       SMITRIG_HI
#define GEVENT_04_SMITRIG       SMITRIG_HI
#define GEVENT_05_SMITRIG       SMITRIG_HI
#define GEVENT_06_SMITRIG       SMITRIG_HI
#define GEVENT_07_SMITRIG       SMITRIG_HI
#define GEVENT_08_SMITRIG       SMITRIG_HI
#define GEVENT_09_SMITRIG       SMITRIG_HI
#define GEVENT_10_SMITRIG       SMITRIG_HI
#define GEVENT_11_SMITRIG       SMITRIG_HI
#define GEVENT_12_SMITRIG       SMITRIG_HI
#define GEVENT_13_SMITRIG       SMITRIG_HI
#define GEVENT_14_SMITRIG       SMITRIG_HI
#define GEVENT_15_SMITRIG       SMITRIG_HI
#define GEVENT_16_SMITRIG       SMITRIG_HI
#define GEVENT_17_SMITRIG       SMITRIG_HI
#define GEVENT_18_SMITRIG       SMITRIG_HI
#define GEVENT_19_SMITRIG       SMITRIG_HI
#define GEVENT_20_SMITRIG       SMITRIG_HI
#define GEVENT_21_SMITRIG       SMITRIG_HI
#define GEVENT_22_SMITRIG       SMITRIG_HI
#define GEVENT_23_SMITRIG       SMITRIG_HI

#define SMICONTROL_MASK         3
#define SMICONTROL_DISABLE      0
#define SMICONTROL_SMI          1
#define SMICONTROL_NMI          2
#define SMICONTROL_IRQ13        3

#define GEVENT_00_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_01_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_02_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_03_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_04_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_05_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_06_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_07_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_08_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_09_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_10_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_11_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_12_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_13_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_14_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_15_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_16_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_17_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_18_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_19_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_20_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_21_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_22_SMICONTROL    SMICONTROL_DISABLE
#define GEVENT_23_SMICONTROL    SMICONTROL_DISABLE

#define GPIO_RSVD_ZONE0_S   GPIO_81
#define GPIO_RSVD_ZONE0_E   GPIO_95
#define GPIO_RSVD_ZONE1_S   GPIO_120
#define GPIO_RSVD_ZONE1_E   GPIO_127

typedef enum _GPIO_COUNT
{
	GPIO_00 = 0,
	GPIO_01,
	GPIO_02,
	GPIO_03,
	GPIO_04,
	GPIO_05,
	GPIO_06,
	GPIO_07,
	GPIO_08,
	GPIO_09,
	GPIO_10,
	GPIO_11,
	GPIO_12,
	GPIO_13,
	GPIO_14,
	GPIO_15,
	GPIO_16,
	GPIO_17,
	GPIO_18,
	GPIO_19,
	GPIO_20,
	GPIO_21,
	GPIO_22,
	GPIO_23,
	GPIO_24,
	GPIO_25,
	GPIO_26,
	GPIO_27,
	GPIO_28,
	GPIO_29,
	GPIO_30,
	GPIO_31,
	GPIO_32,
	GPIO_33,
	GPIO_34,
	GPIO_35,
	GPIO_36,
	GPIO_37,
	GPIO_38,
	GPIO_39,
	GPIO_40,
	GPIO_41,
	GPIO_42,
	GPIO_43,
	GPIO_44,
	GPIO_45,
	GPIO_46,
	GPIO_47,
	GPIO_48,
	GPIO_49,
	GPIO_50,
	GPIO_51,
	GPIO_52,
	GPIO_53,
	GPIO_54,
	GPIO_55,
	GPIO_56,
	GPIO_57,
	GPIO_58,
	GPIO_59,
	GPIO_60,
	GPIO_61,
	GPIO_62,
	GPIO_63,
	GPIO_64,
	GPIO_65,
	GPIO_66,
	GPIO_67,
	GPIO_68,
	GPIO_69,
	GPIO_70,
	GPIO_71,
	GPIO_72,
	GPIO_73,
	GPIO_74,
	GPIO_75,
	GPIO_76,
	GPIO_77,
	GPIO_78,
	GPIO_79,
	GPIO_80,
	GPIO_81,
	GPIO_82,
	GPIO_83,
	GPIO_84,
	GPIO_85,
	GPIO_86,
	GPIO_87,
	GPIO_88,
	GPIO_89,
	GPIO_90,
	GPIO_91,
	GPIO_92,
	GPIO_93,
	GPIO_94,
	GPIO_95,
	GPIO_96,
	GPIO_97,
	GPIO_98,
	GPIO_99,
	GPIO_100,
	GPIO_101,
	GPIO_102,
	GPIO_103,
	GPIO_104,
	GPIO_105,
	GPIO_106,
	GPIO_107,
	GPIO_108,
	GPIO_109,
	GPIO_110,
	GPIO_111,
	GPIO_112,
	GPIO_113,
	GPIO_114,
	GPIO_115,
	GPIO_116,
	GPIO_117,
	GPIO_118,
	GPIO_119,
	GPIO_120,
	GPIO_121,
	GPIO_122,
	GPIO_123,
	GPIO_124,
	GPIO_125,
	GPIO_126,
	GPIO_127,
	GPIO_128,
	GPIO_129,
	GPIO_130,
	GPIO_131,
	GPIO_132,
	GPIO_133,
	GPIO_134,
	GPIO_135,
	GPIO_136,
	GPIO_137,
	GPIO_138,
	GPIO_139,
	GPIO_140,
	GPIO_141,
	GPIO_142,
	GPIO_143,
	GPIO_144,
	GPIO_145,
	GPIO_146,
	GPIO_147,
	GPIO_148,
	GPIO_149,
	GPIO_150,
	GPIO_151,
	GPIO_152,
	GPIO_153,
	GPIO_154,
	GPIO_155,
	GPIO_156,
	GPIO_157,
	GPIO_158,
	GPIO_159,
	GPIO_160,
	GPIO_161,
	GPIO_162,
	GPIO_163,
	GPIO_164,
	GPIO_165,
	GPIO_166,
	GPIO_167,
	GPIO_168,
	GPIO_169,
	GPIO_170,
	GPIO_171,
	GPIO_172,
	GPIO_173,
	GPIO_174,
	GPIO_175,
	GPIO_176,
	GPIO_177,
	GPIO_178,
	GPIO_179,
	GPIO_180,
	GPIO_181,
	GPIO_182,
	GPIO_183,
	GPIO_184,
	GPIO_185,
	GPIO_186,
	GPIO_187,
	GPIO_188,
	GPIO_189,
	GPIO_190,
	GPIO_191,
	GPIO_192,
	GPIO_193,
	GPIO_194,
	GPIO_195,
	GPIO_196,
	GPIO_197,
	GPIO_198,
	GPIO_199,
	GPIO_200,
	GPIO_201,
	GPIO_202,
	GPIO_203,
	GPIO_204,
	GPIO_205,
	GPIO_206,
	GPIO_207,
	GPIO_208,
	GPIO_209,
	GPIO_210,
	GPIO_211,
	GPIO_212,
	GPIO_213,
	GPIO_214,
	GPIO_215,
	GPIO_216,
	GPIO_217,
	GPIO_218,
	GPIO_219,
	GPIO_220,
	GPIO_221,
	GPIO_222,
	GPIO_223,
	GPIO_224,
	GPIO_225,
	GPIO_226,
	GPIO_227,
	GPIO_228,
	GPIO_229,
	MAX_GPIO_NO
} GPIO_COUNT;

typedef struct _GPIO_SETTINGS
{
	u8 select;
	u8 type;
	u8 value;
	u8 NonGpioGevent;
} GPIO_SETTINGS;

const GPIO_SETTINGS gpio_table[]=
{
	{GPIO_00_SELECT, GPIO_00_TYPE, GPO_00_LEVEL+GPIO_00_STICKY+GPIO_00_PULLUP+GPIO_00_PULLDOWN, GPIO_00_SELECT},
	{GPIO_01_SELECT, GPIO_01_TYPE, GPO_01_LEVEL+GPIO_01_STICKY+GPIO_01_PULLUP+GPIO_01_PULLDOWN, GPIO_01_SELECT},
	{GPIO_02_SELECT, GPIO_02_TYPE, GPO_02_LEVEL+GPIO_02_STICKY+GPIO_02_PULLUP+GPIO_02_PULLDOWN, GPIO_02_SELECT},
	{GPIO_03_SELECT, GPIO_03_TYPE, GPO_03_LEVEL+GPIO_03_STICKY+GPIO_03_PULLUP+GPIO_03_PULLDOWN, GPIO_03_SELECT},
	{GPIO_04_SELECT, GPIO_04_TYPE, GPO_04_LEVEL+GPIO_04_STICKY+GPIO_04_PULLUP+GPIO_04_PULLDOWN, GPIO_04_SELECT},
	{GPIO_05_SELECT, GPIO_05_TYPE, GPO_05_LEVEL+GPIO_05_STICKY+GPIO_05_PULLUP+GPIO_05_PULLDOWN, GPIO_05_SELECT},
	{GPIO_06_SELECT, GPIO_06_TYPE, GPO_06_LEVEL+GPIO_06_STICKY+GPIO_06_PULLUP+GPIO_06_PULLDOWN, GPIO_06_SELECT},
	{GPIO_07_SELECT, GPIO_07_TYPE, GPO_07_LEVEL+GPIO_07_STICKY+GPIO_07_PULLUP+GPIO_07_PULLDOWN, GPIO_07_SELECT},
	{GPIO_08_SELECT, GPIO_08_TYPE, GPO_08_LEVEL+GPIO_08_STICKY+GPIO_08_PULLUP+GPIO_08_PULLDOWN, GPIO_08_SELECT},
	{GPIO_09_SELECT, GPIO_09_TYPE, GPO_09_LEVEL+GPIO_09_STICKY+GPIO_09_PULLUP+GPIO_09_PULLDOWN, GPIO_09_SELECT},
	{GPIO_10_SELECT, GPIO_10_TYPE, GPO_10_LEVEL+GPIO_10_STICKY+GPIO_10_PULLUP+GPIO_10_PULLDOWN, GPIO_10_SELECT},
	{GPIO_11_SELECT, GPIO_11_TYPE, GPO_11_LEVEL+GPIO_11_STICKY+GPIO_11_PULLUP+GPIO_11_PULLDOWN, GPIO_11_SELECT},
	{GPIO_12_SELECT, GPIO_12_TYPE, GPO_12_LEVEL+GPIO_12_STICKY+GPIO_12_PULLUP+GPIO_12_PULLDOWN, GPIO_12_SELECT},
	{GPIO_13_SELECT, GPIO_13_TYPE, GPO_13_LEVEL+GPIO_13_STICKY+GPIO_13_PULLUP+GPIO_13_PULLDOWN, GPIO_13_SELECT},
	{GPIO_14_SELECT, GPIO_14_TYPE, GPO_14_LEVEL+GPIO_14_STICKY+GPIO_14_PULLUP+GPIO_14_PULLDOWN, GPIO_14_SELECT},
	{GPIO_15_SELECT, GPIO_15_TYPE, GPO_15_LEVEL+GPIO_15_STICKY+GPIO_15_PULLUP+GPIO_15_PULLDOWN, GPIO_15_SELECT},
	{GPIO_16_SELECT, GPIO_16_TYPE, GPO_16_LEVEL+GPIO_16_STICKY+GPIO_16_PULLUP+GPIO_16_PULLDOWN, GPIO_16_SELECT},
	{GPIO_17_SELECT, GPIO_17_TYPE, GPO_17_LEVEL+GPIO_17_STICKY+GPIO_17_PULLUP+GPIO_17_PULLDOWN, GPIO_17_SELECT},
	{GPIO_18_SELECT, GPIO_18_TYPE, GPO_18_LEVEL+GPIO_18_STICKY+GPIO_18_PULLUP+GPIO_18_PULLDOWN, GPIO_18_SELECT},
	{GPIO_19_SELECT, GPIO_19_TYPE, GPO_19_LEVEL+GPIO_19_STICKY+GPIO_19_PULLUP+GPIO_19_PULLDOWN, GPIO_19_SELECT},
	{GPIO_20_SELECT, GPIO_20_TYPE, GPO_20_LEVEL+GPIO_20_STICKY+GPIO_20_PULLUP+GPIO_20_PULLDOWN, GPIO_20_SELECT},
	{GPIO_21_SELECT, GPIO_21_TYPE, GPO_21_LEVEL+GPIO_21_STICKY+GPIO_21_PULLUP+GPIO_21_PULLDOWN, GPIO_21_SELECT},
	{GPIO_22_SELECT, GPIO_22_TYPE, GPO_22_LEVEL+GPIO_22_STICKY+GPIO_22_PULLUP+GPIO_22_PULLDOWN, GPIO_22_SELECT},
	{GPIO_23_SELECT, GPIO_23_TYPE, GPO_23_LEVEL+GPIO_23_STICKY+GPIO_23_PULLUP+GPIO_23_PULLDOWN, GPIO_23_SELECT},
	{GPIO_24_SELECT, GPIO_24_TYPE, GPO_24_LEVEL+GPIO_24_STICKY+GPIO_24_PULLUP+GPIO_24_PULLDOWN, GPIO_24_SELECT},
	{GPIO_25_SELECT, GPIO_25_TYPE, GPO_25_LEVEL+GPIO_25_STICKY+GPIO_25_PULLUP+GPIO_25_PULLDOWN, GPIO_25_SELECT},
	{GPIO_26_SELECT, GPIO_26_TYPE, GPO_26_LEVEL+GPIO_26_STICKY+GPIO_26_PULLUP+GPIO_26_PULLDOWN, GPIO_26_SELECT},
	{GPIO_27_SELECT, GPIO_27_TYPE, GPO_27_LEVEL+GPIO_27_STICKY+GPIO_27_PULLUP+GPIO_27_PULLDOWN, GPIO_27_SELECT},
	{GPIO_28_SELECT, GPIO_28_TYPE, GPO_28_LEVEL+GPIO_28_STICKY+GPIO_28_PULLUP+GPIO_28_PULLDOWN, GPIO_28_SELECT},
	{GPIO_29_SELECT, GPIO_29_TYPE, GPO_29_LEVEL+GPIO_29_STICKY+GPIO_29_PULLUP+GPIO_29_PULLDOWN, GPIO_29_SELECT},
	{GPIO_30_SELECT, GPIO_30_TYPE, GPO_30_LEVEL+GPIO_30_STICKY+GPIO_30_PULLUP+GPIO_30_PULLDOWN, GPIO_30_SELECT},
	{GPIO_31_SELECT, GPIO_31_TYPE, GPO_31_LEVEL+GPIO_31_STICKY+GPIO_31_PULLUP+GPIO_31_PULLDOWN, GPIO_31_SELECT},
	{GPIO_32_SELECT, GPIO_32_TYPE, GPO_32_LEVEL+GPIO_32_STICKY+GPIO_32_PULLUP+GPIO_32_PULLDOWN, GPIO_32_SELECT},
	{GPIO_33_SELECT, GPIO_33_TYPE, GPO_33_LEVEL+GPIO_33_STICKY+GPIO_33_PULLUP+GPIO_33_PULLDOWN, GPIO_33_SELECT},
	{GPIO_34_SELECT, GPIO_34_TYPE, GPO_34_LEVEL+GPIO_34_STICKY+GPIO_34_PULLUP+GPIO_34_PULLDOWN, GPIO_34_SELECT},
	{GPIO_35_SELECT, GPIO_35_TYPE, GPO_35_LEVEL+GPIO_35_STICKY+GPIO_35_PULLUP+GPIO_35_PULLDOWN, GPIO_35_SELECT},
	{GPIO_36_SELECT, GPIO_36_TYPE, GPO_36_LEVEL+GPIO_36_STICKY+GPIO_36_PULLUP+GPIO_36_PULLDOWN, GPIO_36_SELECT},
	{GPIO_37_SELECT, GPIO_37_TYPE, GPO_37_LEVEL+GPIO_37_STICKY+GPIO_37_PULLUP+GPIO_37_PULLDOWN, GPIO_37_SELECT},
	{GPIO_38_SELECT, GPIO_38_TYPE, GPO_38_LEVEL+GPIO_38_STICKY+GPIO_38_PULLUP+GPIO_38_PULLDOWN, GPIO_38_SELECT},
	{GPIO_39_SELECT, GPIO_39_TYPE, GPO_39_LEVEL+GPIO_39_STICKY+GPIO_39_PULLUP+GPIO_39_PULLDOWN, GPIO_39_SELECT},
	{GPIO_40_SELECT, GPIO_40_TYPE, GPO_40_LEVEL+GPIO_40_STICKY+GPIO_40_PULLUP+GPIO_40_PULLDOWN, GPIO_40_SELECT},
	{GPIO_41_SELECT, GPIO_41_TYPE, GPO_41_LEVEL+GPIO_41_STICKY+GPIO_41_PULLUP+GPIO_41_PULLDOWN, GPIO_41_SELECT},
	{GPIO_42_SELECT, GPIO_42_TYPE, GPO_42_LEVEL+GPIO_42_STICKY+GPIO_42_PULLUP+GPIO_42_PULLDOWN, GPIO_42_SELECT},
	{GPIO_43_SELECT, GPIO_43_TYPE, GPO_43_LEVEL+GPIO_43_STICKY+GPIO_43_PULLUP+GPIO_43_PULLDOWN, GPIO_43_SELECT},
	{GPIO_44_SELECT, GPIO_44_TYPE, GPO_44_LEVEL+GPIO_44_STICKY+GPIO_44_PULLUP+GPIO_44_PULLDOWN, GPIO_44_SELECT},
	{GPIO_45_SELECT, GPIO_45_TYPE, GPO_45_LEVEL+GPIO_45_STICKY+GPIO_45_PULLUP+GPIO_45_PULLDOWN, GPIO_45_SELECT},
	{GPIO_46_SELECT, GPIO_46_TYPE, GPO_46_LEVEL+GPIO_46_STICKY+GPIO_46_PULLUP+GPIO_46_PULLDOWN, GPIO_46_SELECT},
	{GPIO_47_SELECT, GPIO_47_TYPE, GPO_47_LEVEL+GPIO_47_STICKY+GPIO_47_PULLUP+GPIO_47_PULLDOWN, GPIO_47_SELECT},
	{GPIO_48_SELECT, GPIO_48_TYPE, GPO_48_LEVEL+GPIO_48_STICKY+GPIO_48_PULLUP+GPIO_48_PULLDOWN, GPIO_48_SELECT},
	{GPIO_49_SELECT, GPIO_49_TYPE, GPO_49_LEVEL+GPIO_49_STICKY+GPIO_49_PULLUP+GPIO_49_PULLDOWN, GPIO_49_SELECT},
	{GPIO_50_SELECT, GPIO_50_TYPE, GPO_50_LEVEL+GPIO_50_STICKY+GPIO_50_PULLUP+GPIO_50_PULLDOWN, GPIO_50_SELECT},
	{GPIO_51_SELECT, GPIO_51_TYPE, GPO_51_LEVEL+GPIO_51_STICKY+GPIO_51_PULLUP+GPIO_51_PULLDOWN, GPIO_51_SELECT},
	{GPIO_52_SELECT, GPIO_52_TYPE, GPO_52_LEVEL+GPIO_52_STICKY+GPIO_52_PULLUP+GPIO_52_PULLDOWN, GPIO_52_SELECT},
	{GPIO_53_SELECT, GPIO_53_TYPE, GPO_53_LEVEL+GPIO_53_STICKY+GPIO_53_PULLUP+GPIO_53_PULLDOWN, GPIO_53_SELECT},
	{GPIO_54_SELECT, GPIO_54_TYPE, GPO_54_LEVEL+GPIO_54_STICKY+GPIO_54_PULLUP+GPIO_54_PULLDOWN, GPIO_54_SELECT},
	{GPIO_55_SELECT, GPIO_55_TYPE, GPO_55_LEVEL+GPIO_55_STICKY+GPIO_55_PULLUP+GPIO_55_PULLDOWN, GPIO_55_SELECT},
	{GPIO_56_SELECT, GPIO_56_TYPE, GPO_56_LEVEL+GPIO_56_STICKY+GPIO_56_PULLUP+GPIO_56_PULLDOWN, GPIO_56_SELECT},
	{GPIO_57_SELECT, GPIO_57_TYPE, GPO_57_LEVEL+GPIO_57_STICKY+GPIO_57_PULLUP+GPIO_57_PULLDOWN, GPIO_57_SELECT},
	{GPIO_58_SELECT, GPIO_58_TYPE, GPO_58_LEVEL+GPIO_58_STICKY+GPIO_58_PULLUP+GPIO_58_PULLDOWN, GPIO_58_SELECT},
	{GPIO_59_SELECT, GPIO_59_TYPE, GPO_59_LEVEL+GPIO_59_STICKY+GPIO_59_PULLUP+GPIO_59_PULLDOWN, GPIO_59_SELECT},
	{GPIO_60_SELECT, GPIO_60_TYPE, GPO_60_LEVEL+GPIO_60_STICKY+GPIO_60_PULLUP+GPIO_60_PULLDOWN, GPIO_60_SELECT},
	{GPIO_61_SELECT, GPIO_61_TYPE, GPO_61_LEVEL+GPIO_61_STICKY+GPIO_61_PULLUP+GPIO_61_PULLDOWN, GPIO_61_SELECT},
	{GPIO_62_SELECT, GPIO_62_TYPE, GPO_62_LEVEL+GPIO_62_STICKY+GPIO_62_PULLUP+GPIO_62_PULLDOWN, GPIO_62_SELECT},
	{GPIO_63_SELECT, GPIO_63_TYPE, GPO_63_LEVEL+GPIO_63_STICKY+GPIO_63_PULLUP+GPIO_63_PULLDOWN, GPIO_63_SELECT},
	{GPIO_64_SELECT, GPIO_64_TYPE, GPO_64_LEVEL+GPIO_64_STICKY+GPIO_64_PULLUP+GPIO_64_PULLDOWN, GPIO_64_SELECT},
	{GPIO_65_SELECT, GPIO_65_TYPE, GPO_65_LEVEL+GPIO_65_STICKY+GPIO_65_PULLUP+GPIO_65_PULLDOWN, GPIO_65_SELECT},
	{GPIO_66_SELECT, GPIO_66_TYPE, GPO_66_LEVEL+GPIO_66_STICKY+GPIO_66_PULLUP+GPIO_66_PULLDOWN, GPIO_66_SELECT},
	{GPIO_67_SELECT, GPIO_67_TYPE, GPO_67_LEVEL+GPIO_67_STICKY+GPIO_67_PULLUP+GPIO_67_PULLDOWN, GPIO_67_SELECT},
	{GPIO_68_SELECT, GPIO_68_TYPE, GPO_68_LEVEL+GPIO_68_STICKY+GPIO_68_PULLUP+GPIO_68_PULLDOWN, GPIO_68_SELECT},
	{GPIO_69_SELECT, GPIO_69_TYPE, GPO_69_LEVEL+GPIO_69_STICKY+GPIO_69_PULLUP+GPIO_69_PULLDOWN, GPIO_69_SELECT},
	{GPIO_70_SELECT, GPIO_70_TYPE, GPO_70_LEVEL+GPIO_70_STICKY+GPIO_70_PULLUP+GPIO_70_PULLDOWN, GPIO_70_SELECT},
	{GPIO_71_SELECT, GPIO_71_TYPE, GPO_71_LEVEL+GPIO_71_STICKY+GPIO_71_PULLUP+GPIO_71_PULLDOWN, GPIO_71_SELECT},
	{GPIO_72_SELECT, GPIO_72_TYPE, GPO_72_LEVEL+GPIO_72_STICKY+GPIO_72_PULLUP+GPIO_72_PULLDOWN, GPIO_72_SELECT},
	{GPIO_73_SELECT, GPIO_73_TYPE, GPO_73_LEVEL+GPIO_73_STICKY+GPIO_73_PULLUP+GPIO_73_PULLDOWN, GPIO_73_SELECT},
	{GPIO_74_SELECT, GPIO_74_TYPE, GPO_74_LEVEL+GPIO_74_STICKY+GPIO_74_PULLUP+GPIO_74_PULLDOWN, GPIO_74_SELECT},
	{GPIO_75_SELECT, GPIO_75_TYPE, GPO_75_LEVEL+GPIO_75_STICKY+GPIO_75_PULLUP+GPIO_75_PULLDOWN, GPIO_75_SELECT},
	{GPIO_76_SELECT, GPIO_76_TYPE, GPO_76_LEVEL+GPIO_76_STICKY+GPIO_76_PULLUP+GPIO_76_PULLDOWN, GPIO_76_SELECT},
	{GPIO_77_SELECT, GPIO_77_TYPE, GPO_77_LEVEL+GPIO_77_STICKY+GPIO_77_PULLUP+GPIO_77_PULLDOWN, GPIO_77_SELECT},
	{GPIO_78_SELECT, GPIO_78_TYPE, GPO_78_LEVEL+GPIO_78_STICKY+GPIO_78_PULLUP+GPIO_78_PULLDOWN, GPIO_78_SELECT},
	{GPIO_79_SELECT, GPIO_79_TYPE, GPO_79_LEVEL+GPIO_79_STICKY+GPIO_79_PULLUP+GPIO_79_PULLDOWN, GPIO_79_SELECT},
	{GPIO_80_SELECT, GPIO_80_TYPE, GPO_80_LEVEL+GPIO_80_STICKY+GPIO_80_PULLUP+GPIO_80_PULLDOWN, GPIO_80_SELECT},
	{GPIO_81_SELECT, GPIO_81_TYPE, GPO_81_LEVEL+GPIO_81_STICKY+GPIO_81_PULLUP+GPIO_81_PULLDOWN, GPIO_81_SELECT},
	{GPIO_82_SELECT, GPIO_82_TYPE, GPO_82_LEVEL+GPIO_82_STICKY+GPIO_82_PULLUP+GPIO_82_PULLDOWN, GPIO_82_SELECT},
	{GPIO_83_SELECT, GPIO_83_TYPE, GPO_83_LEVEL+GPIO_83_STICKY+GPIO_83_PULLUP+GPIO_83_PULLDOWN, GPIO_83_SELECT},
	{GPIO_84_SELECT, GPIO_84_TYPE, GPO_84_LEVEL+GPIO_84_STICKY+GPIO_84_PULLUP+GPIO_84_PULLDOWN, GPIO_84_SELECT},
	{GPIO_85_SELECT, GPIO_85_TYPE, GPO_85_LEVEL+GPIO_85_STICKY+GPIO_85_PULLUP+GPIO_85_PULLDOWN, GPIO_85_SELECT},
	{GPIO_86_SELECT, GPIO_86_TYPE, GPO_86_LEVEL+GPIO_86_STICKY+GPIO_86_PULLUP+GPIO_86_PULLDOWN, GPIO_86_SELECT},
	{GPIO_87_SELECT, GPIO_87_TYPE, GPO_87_LEVEL+GPIO_87_STICKY+GPIO_87_PULLUP+GPIO_87_PULLDOWN, GPIO_87_SELECT},
	{GPIO_88_SELECT, GPIO_88_TYPE, GPO_88_LEVEL+GPIO_88_STICKY+GPIO_88_PULLUP+GPIO_88_PULLDOWN, GPIO_88_SELECT},
	{GPIO_89_SELECT, GPIO_89_TYPE, GPO_89_LEVEL+GPIO_89_STICKY+GPIO_89_PULLUP+GPIO_89_PULLDOWN, GPIO_89_SELECT},
	{GPIO_90_SELECT, GPIO_90_TYPE, GPO_90_LEVEL+GPIO_90_STICKY+GPIO_90_PULLUP+GPIO_90_PULLDOWN, GPIO_90_SELECT},
	{GPIO_91_SELECT, GPIO_91_TYPE, GPO_91_LEVEL+GPIO_91_STICKY+GPIO_91_PULLUP+GPIO_91_PULLDOWN, GPIO_91_SELECT},
	{GPIO_92_SELECT, GPIO_92_TYPE, GPO_92_LEVEL+GPIO_92_STICKY+GPIO_92_PULLUP+GPIO_92_PULLDOWN, GPIO_92_SELECT},
	{GPIO_93_SELECT, GPIO_93_TYPE, GPO_93_LEVEL+GPIO_93_STICKY+GPIO_93_PULLUP+GPIO_93_PULLDOWN, GPIO_93_SELECT},
	{GPIO_94_SELECT, GPIO_94_TYPE, GPO_94_LEVEL+GPIO_94_STICKY+GPIO_94_PULLUP+GPIO_94_PULLDOWN, GPIO_94_SELECT},
	{GPIO_95_SELECT, GPIO_95_TYPE, GPO_95_LEVEL+GPIO_95_STICKY+GPIO_95_PULLUP+GPIO_95_PULLDOWN, GPIO_95_SELECT},
	{GPIO_96_SELECT, GPIO_96_TYPE, GPO_96_LEVEL+GPIO_96_STICKY+GPIO_96_PULLUP+GPIO_96_PULLDOWN, GPIO_96_SELECT},
	{GPIO_97_SELECT, GPIO_97_TYPE, GPO_97_LEVEL+GPIO_97_STICKY+GPIO_97_PULLUP+GPIO_97_PULLDOWN, GPIO_97_SELECT},
	{GPIO_98_SELECT, GPIO_98_TYPE, GPO_98_LEVEL+GPIO_98_STICKY+GPIO_98_PULLUP+GPIO_98_PULLDOWN, GPIO_98_SELECT},
	{GPIO_99_SELECT, GPIO_99_TYPE, GPO_99_LEVEL+GPIO_99_STICKY+GPIO_99_PULLUP+GPIO_99_PULLDOWN, GPIO_99_SELECT},
	{GPIO_100_SELECT, GPIO_100_TYPE, GPO_100_LEVEL+GPIO_100_STICKY+GPIO_100_PULLUP+GPIO_100_PULLDOWN, GPIO_100_SELECT},
	{GPIO_101_SELECT, GPIO_101_TYPE, GPO_101_LEVEL+GPIO_101_STICKY+GPIO_101_PULLUP+GPIO_101_PULLDOWN, GPIO_101_SELECT},
	{GPIO_102_SELECT, GPIO_102_TYPE, GPO_102_LEVEL+GPIO_102_STICKY+GPIO_102_PULLUP+GPIO_102_PULLDOWN, GPIO_102_SELECT},
	{GPIO_103_SELECT, GPIO_103_TYPE, GPO_103_LEVEL+GPIO_103_STICKY+GPIO_103_PULLUP+GPIO_103_PULLDOWN, GPIO_103_SELECT},
	{GPIO_104_SELECT, GPIO_104_TYPE, GPO_104_LEVEL+GPIO_104_STICKY+GPIO_104_PULLUP+GPIO_104_PULLDOWN, GPIO_104_SELECT},
	{GPIO_105_SELECT, GPIO_105_TYPE, GPO_105_LEVEL+GPIO_105_STICKY+GPIO_105_PULLUP+GPIO_105_PULLDOWN, GPIO_105_SELECT},
	{GPIO_106_SELECT, GPIO_106_TYPE, GPO_106_LEVEL+GPIO_106_STICKY+GPIO_106_PULLUP+GPIO_106_PULLDOWN, GPIO_106_SELECT},
	{GPIO_107_SELECT, GPIO_107_TYPE, GPO_107_LEVEL+GPIO_107_STICKY+GPIO_107_PULLUP+GPIO_107_PULLDOWN, GPIO_107_SELECT},
	{GPIO_108_SELECT, GPIO_108_TYPE, GPO_108_LEVEL+GPIO_108_STICKY+GPIO_108_PULLUP+GPIO_108_PULLDOWN, GPIO_108_SELECT},
	{GPIO_109_SELECT, GPIO_109_TYPE, GPO_109_LEVEL+GPIO_109_STICKY+GPIO_109_PULLUP+GPIO_109_PULLDOWN, GPIO_109_SELECT},
	{GPIO_110_SELECT, GPIO_110_TYPE, GPO_110_LEVEL+GPIO_110_STICKY+GPIO_110_PULLUP+GPIO_110_PULLDOWN, GPIO_110_SELECT},
	{GPIO_111_SELECT, GPIO_111_TYPE, GPO_111_LEVEL+GPIO_111_STICKY+GPIO_111_PULLUP+GPIO_111_PULLDOWN, GPIO_111_SELECT},
	{GPIO_112_SELECT, GPIO_112_TYPE, GPO_112_LEVEL+GPIO_112_STICKY+GPIO_112_PULLUP+GPIO_112_PULLDOWN, GPIO_112_SELECT},
	{GPIO_113_SELECT, GPIO_113_TYPE, GPO_113_LEVEL+GPIO_113_STICKY+GPIO_113_PULLUP+GPIO_113_PULLDOWN, GPIO_113_SELECT},
	{GPIO_114_SELECT, GPIO_114_TYPE, GPO_114_LEVEL+GPIO_114_STICKY+GPIO_114_PULLUP+GPIO_114_PULLDOWN, GPIO_114_SELECT},
	{GPIO_115_SELECT, GPIO_115_TYPE, GPO_115_LEVEL+GPIO_115_STICKY+GPIO_115_PULLUP+GPIO_115_PULLDOWN, GPIO_115_SELECT},
	{GPIO_116_SELECT, GPIO_116_TYPE, GPO_116_LEVEL+GPIO_116_STICKY+GPIO_116_PULLUP+GPIO_116_PULLDOWN, GPIO_116_SELECT},
	{GPIO_117_SELECT, GPIO_117_TYPE, GPO_117_LEVEL+GPIO_117_STICKY+GPIO_117_PULLUP+GPIO_117_PULLDOWN, GPIO_117_SELECT},
	{GPIO_118_SELECT, GPIO_118_TYPE, GPO_118_LEVEL+GPIO_118_STICKY+GPIO_118_PULLUP+GPIO_118_PULLDOWN, GPIO_118_SELECT},
	{GPIO_119_SELECT, GPIO_119_TYPE, GPO_119_LEVEL+GPIO_119_STICKY+GPIO_119_PULLUP+GPIO_119_PULLDOWN, GPIO_119_SELECT},
	{GPIO_120_SELECT, GPIO_120_TYPE, GPO_120_LEVEL+GPIO_120_STICKY+GPIO_120_PULLUP+GPIO_120_PULLDOWN, GPIO_120_SELECT},
	{GPIO_121_SELECT, GPIO_121_TYPE, GPO_121_LEVEL+GPIO_121_STICKY+GPIO_121_PULLUP+GPIO_121_PULLDOWN, GPIO_121_SELECT},
	{GPIO_122_SELECT, GPIO_122_TYPE, GPO_122_LEVEL+GPIO_122_STICKY+GPIO_122_PULLUP+GPIO_122_PULLDOWN, GPIO_122_SELECT},
	{GPIO_123_SELECT, GPIO_123_TYPE, GPO_123_LEVEL+GPIO_123_STICKY+GPIO_123_PULLUP+GPIO_123_PULLDOWN, GPIO_123_SELECT},
	{GPIO_124_SELECT, GPIO_124_TYPE, GPO_124_LEVEL+GPIO_124_STICKY+GPIO_124_PULLUP+GPIO_124_PULLDOWN, GPIO_124_SELECT},
	{GPIO_125_SELECT, GPIO_125_TYPE, GPO_125_LEVEL+GPIO_125_STICKY+GPIO_125_PULLUP+GPIO_125_PULLDOWN, GPIO_125_SELECT},
	{GPIO_126_SELECT, GPIO_126_TYPE, GPO_126_LEVEL+GPIO_126_STICKY+GPIO_126_PULLUP+GPIO_126_PULLDOWN, GPIO_126_SELECT},
	{GPIO_127_SELECT, GPIO_127_TYPE, GPO_127_LEVEL+GPIO_127_STICKY+GPIO_127_PULLUP+GPIO_127_PULLDOWN, GPIO_127_SELECT},
	{GPIO_128_SELECT, GPIO_128_TYPE, GPO_128_LEVEL+GPIO_128_STICKY+GPIO_128_PULLUP+GPIO_128_PULLDOWN, GPIO_128_SELECT},
	{GPIO_129_SELECT, GPIO_129_TYPE, GPO_129_LEVEL+GPIO_129_STICKY+GPIO_129_PULLUP+GPIO_129_PULLDOWN, GPIO_129_SELECT},
	{GPIO_130_SELECT, GPIO_130_TYPE, GPO_130_LEVEL+GPIO_130_STICKY+GPIO_130_PULLUP+GPIO_130_PULLDOWN, GPIO_130_SELECT},
	{GPIO_131_SELECT, GPIO_131_TYPE, GPO_131_LEVEL+GPIO_131_STICKY+GPIO_131_PULLUP+GPIO_131_PULLDOWN, GPIO_131_SELECT},
	{GPIO_132_SELECT, GPIO_132_TYPE, GPO_132_LEVEL+GPIO_132_STICKY+GPIO_132_PULLUP+GPIO_132_PULLDOWN, GPIO_132_SELECT},
	{GPIO_133_SELECT, GPIO_133_TYPE, GPO_133_LEVEL+GPIO_133_STICKY+GPIO_133_PULLUP+GPIO_133_PULLDOWN, GPIO_133_SELECT},
	{GPIO_134_SELECT, GPIO_134_TYPE, GPO_134_LEVEL+GPIO_134_STICKY+GPIO_134_PULLUP+GPIO_134_PULLDOWN, GPIO_134_SELECT},
	{GPIO_135_SELECT, GPIO_135_TYPE, GPO_135_LEVEL+GPIO_135_STICKY+GPIO_135_PULLUP+GPIO_135_PULLDOWN, GPIO_135_SELECT},
	{GPIO_136_SELECT, GPIO_136_TYPE, GPO_136_LEVEL+GPIO_136_STICKY+GPIO_136_PULLUP+GPIO_136_PULLDOWN, GPIO_136_SELECT},
	{GPIO_137_SELECT, GPIO_137_TYPE, GPO_137_LEVEL+GPIO_137_STICKY+GPIO_137_PULLUP+GPIO_137_PULLDOWN, GPIO_137_SELECT},
	{GPIO_138_SELECT, GPIO_138_TYPE, GPO_138_LEVEL+GPIO_138_STICKY+GPIO_138_PULLUP+GPIO_138_PULLDOWN, GPIO_138_SELECT},
	{GPIO_139_SELECT, GPIO_139_TYPE, GPO_139_LEVEL+GPIO_139_STICKY+GPIO_139_PULLUP+GPIO_139_PULLDOWN, GPIO_139_SELECT},
	{GPIO_140_SELECT, GPIO_140_TYPE, GPO_140_LEVEL+GPIO_140_STICKY+GPIO_140_PULLUP+GPIO_140_PULLDOWN, GPIO_140_SELECT},
	{GPIO_141_SELECT, GPIO_141_TYPE, GPO_141_LEVEL+GPIO_141_STICKY+GPIO_141_PULLUP+GPIO_141_PULLDOWN, GPIO_141_SELECT},
	{GPIO_142_SELECT, GPIO_142_TYPE, GPO_142_LEVEL+GPIO_142_STICKY+GPIO_142_PULLUP+GPIO_142_PULLDOWN, GPIO_142_SELECT},
	{GPIO_143_SELECT, GPIO_143_TYPE, GPO_143_LEVEL+GPIO_143_STICKY+GPIO_143_PULLUP+GPIO_143_PULLDOWN, GPIO_143_SELECT},
	{GPIO_144_SELECT, GPIO_144_TYPE, GPO_144_LEVEL+GPIO_144_STICKY+GPIO_144_PULLUP+GPIO_144_PULLDOWN, GPIO_144_SELECT},
	{GPIO_145_SELECT, GPIO_145_TYPE, GPO_145_LEVEL+GPIO_145_STICKY+GPIO_145_PULLUP+GPIO_145_PULLDOWN, GPIO_145_SELECT},
	{GPIO_146_SELECT, GPIO_146_TYPE, GPO_146_LEVEL+GPIO_146_STICKY+GPIO_146_PULLUP+GPIO_146_PULLDOWN, GPIO_146_SELECT},
	{GPIO_147_SELECT, GPIO_147_TYPE, GPO_147_LEVEL+GPIO_147_STICKY+GPIO_147_PULLUP+GPIO_147_PULLDOWN, GPIO_147_SELECT},
	{GPIO_148_SELECT, GPIO_148_TYPE, GPO_148_LEVEL+GPIO_148_STICKY+GPIO_148_PULLUP+GPIO_148_PULLDOWN, GPIO_148_SELECT},
	{GPIO_149_SELECT, GPIO_149_TYPE, GPO_149_LEVEL+GPIO_149_STICKY+GPIO_149_PULLUP+GPIO_149_PULLDOWN, GPIO_149_SELECT},
	{GPIO_150_SELECT, GPIO_150_TYPE, GPO_150_LEVEL+GPIO_150_STICKY+GPIO_150_PULLUP+GPIO_150_PULLDOWN, GPIO_150_SELECT},
	{GPIO_151_SELECT, GPIO_151_TYPE, GPO_151_LEVEL+GPIO_151_STICKY+GPIO_151_PULLUP+GPIO_151_PULLDOWN, GPIO_151_SELECT},
	{GPIO_152_SELECT, GPIO_152_TYPE, GPO_152_LEVEL+GPIO_152_STICKY+GPIO_152_PULLUP+GPIO_152_PULLDOWN, GPIO_152_SELECT},
	{GPIO_153_SELECT, GPIO_153_TYPE, GPO_153_LEVEL+GPIO_153_STICKY+GPIO_153_PULLUP+GPIO_153_PULLDOWN, GPIO_153_SELECT},
	{GPIO_154_SELECT, GPIO_154_TYPE, GPO_154_LEVEL+GPIO_154_STICKY+GPIO_154_PULLUP+GPIO_154_PULLDOWN, GPIO_154_SELECT},
	{GPIO_155_SELECT, GPIO_155_TYPE, GPO_155_LEVEL+GPIO_155_STICKY+GPIO_155_PULLUP+GPIO_155_PULLDOWN, GPIO_155_SELECT},
	{GPIO_156_SELECT, GPIO_156_TYPE, GPO_156_LEVEL+GPIO_156_STICKY+GPIO_156_PULLUP+GPIO_156_PULLDOWN, GPIO_156_SELECT},
	{GPIO_157_SELECT, GPIO_157_TYPE, GPO_157_LEVEL+GPIO_157_STICKY+GPIO_157_PULLUP+GPIO_157_PULLDOWN, GPIO_157_SELECT},
	{GPIO_158_SELECT, GPIO_158_TYPE, GPO_158_LEVEL+GPIO_158_STICKY+GPIO_158_PULLUP+GPIO_158_PULLDOWN, GPIO_158_SELECT},
	{GPIO_159_SELECT, GPIO_159_TYPE, GPO_159_LEVEL+GPIO_159_STICKY+GPIO_159_PULLUP+GPIO_159_PULLDOWN, GPIO_159_SELECT},
	{GPIO_160_SELECT, GPIO_160_TYPE, GPO_160_LEVEL+GPIO_160_STICKY+GPIO_160_PULLUP+GPIO_160_PULLDOWN, GPIO_160_SELECT},
	{GPIO_161_SELECT, GPIO_161_TYPE, GPO_161_LEVEL+GPIO_161_STICKY+GPIO_161_PULLUP+GPIO_161_PULLDOWN, GPIO_161_SELECT},
	{GPIO_162_SELECT, GPIO_162_TYPE, GPO_162_LEVEL+GPIO_162_STICKY+GPIO_162_PULLUP+GPIO_162_PULLDOWN, GPIO_162_SELECT},
	{GPIO_163_SELECT, GPIO_163_TYPE, GPO_163_LEVEL+GPIO_163_STICKY+GPIO_163_PULLUP+GPIO_163_PULLDOWN, GPIO_163_SELECT},
	{GPIO_164_SELECT, GPIO_164_TYPE, GPO_164_LEVEL+GPIO_164_STICKY+GPIO_164_PULLUP+GPIO_164_PULLDOWN, GPIO_164_SELECT},
	{GPIO_165_SELECT, GPIO_165_TYPE, GPO_165_LEVEL+GPIO_165_STICKY+GPIO_165_PULLUP+GPIO_165_PULLDOWN, GPIO_165_SELECT},
	{GPIO_166_SELECT, GPIO_166_TYPE, GPO_166_LEVEL+GPIO_166_STICKY+GPIO_166_PULLUP+GPIO_166_PULLDOWN, GPIO_166_SELECT},
	{GPIO_167_SELECT, GPIO_167_TYPE, GPO_167_LEVEL+GPIO_167_STICKY+GPIO_167_PULLUP+GPIO_167_PULLDOWN, GPIO_167_SELECT},
	{GPIO_168_SELECT, GPIO_168_TYPE, GPO_168_LEVEL+GPIO_168_STICKY+GPIO_168_PULLUP+GPIO_168_PULLDOWN, GPIO_168_SELECT},
	{GPIO_169_SELECT, GPIO_169_TYPE, GPO_169_LEVEL+GPIO_169_STICKY+GPIO_169_PULLUP+GPIO_169_PULLDOWN, GPIO_169_SELECT},
	{GPIO_170_SELECT, GPIO_170_TYPE, GPO_170_LEVEL+GPIO_170_STICKY+GPIO_170_PULLUP+GPIO_170_PULLDOWN, GPIO_170_SELECT},
	{GPIO_171_SELECT, GPIO_171_TYPE, GPO_171_LEVEL+GPIO_171_STICKY+GPIO_171_PULLUP+GPIO_171_PULLDOWN, GPIO_171_SELECT},
	{GPIO_172_SELECT, GPIO_172_TYPE, GPO_172_LEVEL+GPIO_172_STICKY+GPIO_172_PULLUP+GPIO_172_PULLDOWN, GPIO_172_SELECT},
	{GPIO_173_SELECT, GPIO_173_TYPE, GPO_173_LEVEL+GPIO_173_STICKY+GPIO_173_PULLUP+GPIO_173_PULLDOWN, GPIO_173_SELECT},
	{GPIO_174_SELECT, GPIO_174_TYPE, GPO_174_LEVEL+GPIO_174_STICKY+GPIO_174_PULLUP+GPIO_174_PULLDOWN, GPIO_174_SELECT},
	{GPIO_175_SELECT, GPIO_175_TYPE, GPO_175_LEVEL+GPIO_175_STICKY+GPIO_175_PULLUP+GPIO_175_PULLDOWN, GPIO_175_SELECT},
	{GPIO_176_SELECT, GPIO_176_TYPE, GPO_176_LEVEL+GPIO_176_STICKY+GPIO_176_PULLUP+GPIO_176_PULLDOWN, GPIO_176_SELECT},
	{GPIO_177_SELECT, GPIO_177_TYPE, GPO_177_LEVEL+GPIO_177_STICKY+GPIO_177_PULLUP+GPIO_177_PULLDOWN, GPIO_177_SELECT},
	{GPIO_178_SELECT, GPIO_178_TYPE, GPO_178_LEVEL+GPIO_178_STICKY+GPIO_178_PULLUP+GPIO_178_PULLDOWN, GPIO_178_SELECT},
	{GPIO_179_SELECT, GPIO_179_TYPE, GPO_179_LEVEL+GPIO_179_STICKY+GPIO_179_PULLUP+GPIO_179_PULLDOWN, GPIO_179_SELECT},
	{GPIO_180_SELECT, GPIO_180_TYPE, GPO_180_LEVEL+GPIO_180_STICKY+GPIO_180_PULLUP+GPIO_180_PULLDOWN, GPIO_180_SELECT},
	{GPIO_181_SELECT, GPIO_181_TYPE, GPO_181_LEVEL+GPIO_181_STICKY+GPIO_181_PULLUP+GPIO_181_PULLDOWN, GPIO_181_SELECT},
	{GPIO_182_SELECT, GPIO_182_TYPE, GPO_182_LEVEL+GPIO_182_STICKY+GPIO_182_PULLUP+GPIO_182_PULLDOWN, GPIO_182_SELECT},
	{GPIO_183_SELECT, GPIO_183_TYPE, GPO_183_LEVEL+GPIO_183_STICKY+GPIO_183_PULLUP+GPIO_183_PULLDOWN, GPIO_183_SELECT},
	{GPIO_184_SELECT, GPIO_184_TYPE, GPO_184_LEVEL+GPIO_184_STICKY+GPIO_184_PULLUP+GPIO_184_PULLDOWN, GPIO_184_SELECT},
	{GPIO_185_SELECT, GPIO_185_TYPE, GPO_185_LEVEL+GPIO_185_STICKY+GPIO_185_PULLUP+GPIO_185_PULLDOWN, GPIO_185_SELECT},
	{GPIO_186_SELECT, GPIO_186_TYPE, GPO_186_LEVEL+GPIO_186_STICKY+GPIO_186_PULLUP+GPIO_186_PULLDOWN, GPIO_186_SELECT},
	{GPIO_187_SELECT, GPIO_187_TYPE, GPO_187_LEVEL+GPIO_187_STICKY+GPIO_187_PULLUP+GPIO_187_PULLDOWN, GPIO_187_SELECT},
	{GPIO_188_SELECT, GPIO_188_TYPE, GPO_188_LEVEL+GPIO_188_STICKY+GPIO_188_PULLUP+GPIO_188_PULLDOWN, GPIO_188_SELECT},
	{GPIO_189_SELECT, GPIO_189_TYPE, GPO_189_LEVEL+GPIO_189_STICKY+GPIO_189_PULLUP+GPIO_189_PULLDOWN, GPIO_189_SELECT},
	{GPIO_190_SELECT, GPIO_190_TYPE, GPO_190_LEVEL+GPIO_190_STICKY+GPIO_190_PULLUP+GPIO_190_PULLDOWN, GPIO_190_SELECT},
	{GPIO_191_SELECT, GPIO_191_TYPE, GPO_191_LEVEL+GPIO_191_STICKY+GPIO_191_PULLUP+GPIO_191_PULLDOWN, GPIO_191_SELECT},
	{GPIO_192_SELECT, GPIO_192_TYPE, GPO_192_LEVEL+GPIO_192_STICKY+GPIO_192_PULLUP+GPIO_192_PULLDOWN, GPIO_192_SELECT},
	{GPIO_193_SELECT, GPIO_193_TYPE, GPO_193_LEVEL+GPIO_193_STICKY+GPIO_193_PULLUP+GPIO_193_PULLDOWN, GPIO_193_SELECT},
	{GPIO_194_SELECT, GPIO_194_TYPE, GPO_194_LEVEL+GPIO_194_STICKY+GPIO_194_PULLUP+GPIO_194_PULLDOWN, GPIO_194_SELECT},
	{GPIO_195_SELECT, GPIO_195_TYPE, GPO_195_LEVEL+GPIO_195_STICKY+GPIO_195_PULLUP+GPIO_195_PULLDOWN, GPIO_195_SELECT},
	{GPIO_196_SELECT, GPIO_196_TYPE, GPO_196_LEVEL+GPIO_196_STICKY+GPIO_196_PULLUP+GPIO_196_PULLDOWN, GPIO_196_SELECT},
	{GPIO_197_SELECT, GPIO_197_TYPE, GPO_197_LEVEL+GPIO_197_STICKY+GPIO_197_PULLUP+GPIO_197_PULLDOWN, GPIO_197_SELECT},
	{GPIO_198_SELECT, GPIO_198_TYPE, GPO_198_LEVEL+GPIO_198_STICKY+GPIO_198_PULLUP+GPIO_198_PULLDOWN, GPIO_198_SELECT},
	{GPIO_199_SELECT, GPIO_199_TYPE, GPO_199_LEVEL+GPIO_199_STICKY+GPIO_199_PULLUP+GPIO_199_PULLDOWN, GPIO_199_SELECT},
	{GPIO_200_SELECT, GPIO_200_TYPE, GPO_200_LEVEL+GPIO_200_STICKY+GPIO_200_PULLUP+GPIO_200_PULLDOWN, GPIO_200_SELECT},
	{GPIO_201_SELECT, GPIO_201_TYPE, GPO_201_LEVEL+GPIO_201_STICKY+GPIO_201_PULLUP+GPIO_201_PULLDOWN, GPIO_201_SELECT},
	{GPIO_202_SELECT, GPIO_202_TYPE, GPO_202_LEVEL+GPIO_202_STICKY+GPIO_202_PULLUP+GPIO_202_PULLDOWN, GPIO_202_SELECT},
	{GPIO_203_SELECT, GPIO_203_TYPE, GPO_203_LEVEL+GPIO_203_STICKY+GPIO_203_PULLUP+GPIO_203_PULLDOWN, GPIO_203_SELECT},
	{GPIO_204_SELECT, GPIO_204_TYPE, GPO_204_LEVEL+GPIO_204_STICKY+GPIO_204_PULLUP+GPIO_204_PULLDOWN, GPIO_204_SELECT},
	{GPIO_205_SELECT, GPIO_205_TYPE, GPO_205_LEVEL+GPIO_205_STICKY+GPIO_205_PULLUP+GPIO_205_PULLDOWN, GPIO_205_SELECT},
	{GPIO_206_SELECT, GPIO_206_TYPE, GPO_206_LEVEL+GPIO_206_STICKY+GPIO_206_PULLUP+GPIO_206_PULLDOWN, GPIO_206_SELECT},
	{GPIO_207_SELECT, GPIO_207_TYPE, GPO_207_LEVEL+GPIO_207_STICKY+GPIO_207_PULLUP+GPIO_207_PULLDOWN, GPIO_207_SELECT},
	{GPIO_208_SELECT, GPIO_208_TYPE, GPO_208_LEVEL+GPIO_208_STICKY+GPIO_208_PULLUP+GPIO_208_PULLDOWN, GPIO_208_SELECT},
	{GPIO_209_SELECT, GPIO_209_TYPE, GPO_209_LEVEL+GPIO_209_STICKY+GPIO_209_PULLUP+GPIO_209_PULLDOWN, GPIO_209_SELECT},
	{GPIO_210_SELECT, GPIO_210_TYPE, GPO_210_LEVEL+GPIO_210_STICKY+GPIO_210_PULLUP+GPIO_210_PULLDOWN, GPIO_210_SELECT},
	{GPIO_211_SELECT, GPIO_211_TYPE, GPO_211_LEVEL+GPIO_211_STICKY+GPIO_211_PULLUP+GPIO_211_PULLDOWN, GPIO_211_SELECT},
	{GPIO_212_SELECT, GPIO_212_TYPE, GPO_212_LEVEL+GPIO_212_STICKY+GPIO_212_PULLUP+GPIO_212_PULLDOWN, GPIO_212_SELECT},
	{GPIO_213_SELECT, GPIO_213_TYPE, GPO_213_LEVEL+GPIO_213_STICKY+GPIO_213_PULLUP+GPIO_213_PULLDOWN, GPIO_213_SELECT},
	{GPIO_214_SELECT, GPIO_214_TYPE, GPO_214_LEVEL+GPIO_214_STICKY+GPIO_214_PULLUP+GPIO_214_PULLDOWN, GPIO_214_SELECT},
	{GPIO_215_SELECT, GPIO_215_TYPE, GPO_215_LEVEL+GPIO_215_STICKY+GPIO_215_PULLUP+GPIO_215_PULLDOWN, GPIO_215_SELECT},
	{GPIO_216_SELECT, GPIO_216_TYPE, GPO_216_LEVEL+GPIO_216_STICKY+GPIO_216_PULLUP+GPIO_216_PULLDOWN, GPIO_216_SELECT},
	{GPIO_217_SELECT, GPIO_217_TYPE, GPO_217_LEVEL+GPIO_217_STICKY+GPIO_217_PULLUP+GPIO_217_PULLDOWN, GPIO_217_SELECT},
	{GPIO_218_SELECT, GPIO_218_TYPE, GPO_218_LEVEL+GPIO_218_STICKY+GPIO_218_PULLUP+GPIO_218_PULLDOWN, GPIO_218_SELECT},
	{GPIO_219_SELECT, GPIO_219_TYPE, GPO_219_LEVEL+GPIO_219_STICKY+GPIO_219_PULLUP+GPIO_219_PULLDOWN, GPIO_219_SELECT},
	{GPIO_220_SELECT, GPIO_220_TYPE, GPO_220_LEVEL+GPIO_220_STICKY+GPIO_220_PULLUP+GPIO_220_PULLDOWN, GPIO_220_SELECT},
	{GPIO_221_SELECT, GPIO_221_TYPE, GPO_221_LEVEL+GPIO_221_STICKY+GPIO_221_PULLUP+GPIO_221_PULLDOWN, GPIO_221_SELECT},
	{GPIO_222_SELECT, GPIO_222_TYPE, GPO_222_LEVEL+GPIO_222_STICKY+GPIO_222_PULLUP+GPIO_222_PULLDOWN, GPIO_222_SELECT},
	{GPIO_223_SELECT, GPIO_223_TYPE, GPO_223_LEVEL+GPIO_223_STICKY+GPIO_223_PULLUP+GPIO_223_PULLDOWN, GPIO_223_SELECT},
	{GPIO_224_SELECT, GPIO_224_TYPE, GPO_224_LEVEL+GPIO_224_STICKY+GPIO_224_PULLUP+GPIO_224_PULLDOWN, GPIO_224_SELECT},
	{GPIO_225_SELECT, GPIO_225_TYPE, GPO_225_LEVEL+GPIO_225_STICKY+GPIO_225_PULLUP+GPIO_225_PULLDOWN, GPIO_225_SELECT},
	{GPIO_226_SELECT, GPIO_226_TYPE, GPO_226_LEVEL+GPIO_226_STICKY+GPIO_226_PULLUP+GPIO_226_PULLDOWN, GPIO_226_SELECT},
	{GPIO_227_SELECT, GPIO_227_TYPE, GPO_227_LEVEL+GPIO_227_STICKY+GPIO_227_PULLUP+GPIO_227_PULLDOWN, GPIO_227_SELECT},
	{GPIO_228_SELECT, GPIO_228_TYPE, GPO_228_LEVEL+GPIO_228_STICKY+GPIO_228_PULLUP+GPIO_228_PULLDOWN, GPIO_228_SELECT},
	{GPIO_229_SELECT, GPIO_229_TYPE, GPO_229_LEVEL+GPIO_229_STICKY+GPIO_229_PULLUP+GPIO_229_PULLDOWN, GPIO_229_SELECT},
};

typedef enum _GEVENT_COUNT
{
	GEVENT_00 = 0x60,
	GEVENT_01,
	GEVENT_02,
	GEVENT_03,
	GEVENT_04,
	GEVENT_05,
	GEVENT_06,
	GEVENT_07,
	GEVENT_08,
	GEVENT_09,
	GEVENT_10,
	GEVENT_11,
	GEVENT_12,
	GEVENT_13,
	GEVENT_14,
	GEVENT_15,
	GEVENT_16,
	GEVENT_17,
	GEVENT_18,
	GEVENT_19,
	GEVENT_20,
	GEVENT_21,
	GEVENT_22,
	GEVENT_23
} GEVENT_COUNT;

typedef struct _GEVENT_SETTINGS
{
	u8 EventEnable;      // 0: Disable, 1: Enable
	u8 SciTrig;          // 0: Falling Edge, 1: Rising Edge
	u8 SciLevl;          // 0: Edge trigger, 1: Level Trigger
	u8 SmiSciEn;         // 0: Not send SMI, 1: Send SMI
	u8 SciS0En;          // 0: Disable, 1: Enable
	u8 SciMap;           // 0000b->1111b
	u8 SmiTrig;          // 0: Active Low, 1: Active High
	u8 SmiControl;       // 0: Disable, 1: SMI 2: NMI 3: IRQ13
} GEVENT_SETTINGS;

const GEVENT_SETTINGS gevent_table[] =
{
	{GEVENT_00_EVENTENABLE, GEVENT_00_SCITRIG, GEVENT_00_SCILEVEL, GEVENT_00_SMISCIEN, GEVENT_00_SCIS0EN, GEVENT_00_SCIMAP, GEVENT_00_SMITRIG, GEVENT_00_SMICONTROL},
	{GEVENT_01_EVENTENABLE, GEVENT_01_SCITRIG, GEVENT_01_SCILEVEL, GEVENT_01_SMISCIEN, GEVENT_01_SCIS0EN, GEVENT_01_SCIMAP, GEVENT_01_SMITRIG, GEVENT_01_SMICONTROL},
	{GEVENT_02_EVENTENABLE, GEVENT_02_SCITRIG, GEVENT_02_SCILEVEL, GEVENT_02_SMISCIEN, GEVENT_02_SCIS0EN, GEVENT_02_SCIMAP, GEVENT_02_SMITRIG, GEVENT_02_SMICONTROL},
	{GEVENT_03_EVENTENABLE, GEVENT_03_SCITRIG, GEVENT_03_SCILEVEL, GEVENT_03_SMISCIEN, GEVENT_03_SCIS0EN, GEVENT_03_SCIMAP, GEVENT_03_SMITRIG, GEVENT_03_SMICONTROL},
	{GEVENT_04_EVENTENABLE, GEVENT_04_SCITRIG, GEVENT_04_SCILEVEL, GEVENT_04_SMISCIEN, GEVENT_04_SCIS0EN, GEVENT_04_SCIMAP, GEVENT_04_SMITRIG, GEVENT_04_SMICONTROL},
	{GEVENT_05_EVENTENABLE, GEVENT_05_SCITRIG, GEVENT_05_SCILEVEL, GEVENT_05_SMISCIEN, GEVENT_05_SCIS0EN, GEVENT_05_SCIMAP, GEVENT_05_SMITRIG, GEVENT_05_SMICONTROL},
	{GEVENT_06_EVENTENABLE, GEVENT_06_SCITRIG, GEVENT_06_SCILEVEL, GEVENT_06_SMISCIEN, GEVENT_06_SCIS0EN, GEVENT_06_SCIMAP, GEVENT_06_SMITRIG, GEVENT_06_SMICONTROL},
	{GEVENT_07_EVENTENABLE, GEVENT_07_SCITRIG, GEVENT_07_SCILEVEL, GEVENT_07_SMISCIEN, GEVENT_07_SCIS0EN, GEVENT_07_SCIMAP, GEVENT_07_SMITRIG, GEVENT_07_SMICONTROL},
	{GEVENT_08_EVENTENABLE, GEVENT_08_SCITRIG, GEVENT_08_SCILEVEL, GEVENT_08_SMISCIEN, GEVENT_08_SCIS0EN, GEVENT_08_SCIMAP, GEVENT_08_SMITRIG, GEVENT_08_SMICONTROL},
	{GEVENT_09_EVENTENABLE, GEVENT_09_SCITRIG, GEVENT_09_SCILEVEL, GEVENT_09_SMISCIEN, GEVENT_09_SCIS0EN, GEVENT_09_SCIMAP, GEVENT_09_SMITRIG, GEVENT_09_SMICONTROL},
	{GEVENT_10_EVENTENABLE, GEVENT_10_SCITRIG, GEVENT_10_SCILEVEL, GEVENT_10_SMISCIEN, GEVENT_10_SCIS0EN, GEVENT_10_SCIMAP, GEVENT_10_SMITRIG, GEVENT_10_SMICONTROL},
	{GEVENT_11_EVENTENABLE, GEVENT_11_SCITRIG, GEVENT_11_SCILEVEL, GEVENT_11_SMISCIEN, GEVENT_11_SCIS0EN, GEVENT_11_SCIMAP, GEVENT_11_SMITRIG, GEVENT_11_SMICONTROL},
	{GEVENT_12_EVENTENABLE, GEVENT_12_SCITRIG, GEVENT_12_SCILEVEL, GEVENT_12_SMISCIEN, GEVENT_12_SCIS0EN, GEVENT_12_SCIMAP, GEVENT_12_SMITRIG, GEVENT_12_SMICONTROL},
	{GEVENT_13_EVENTENABLE, GEVENT_13_SCITRIG, GEVENT_13_SCILEVEL, GEVENT_13_SMISCIEN, GEVENT_13_SCIS0EN, GEVENT_13_SCIMAP, GEVENT_13_SMITRIG, GEVENT_13_SMICONTROL},
	{GEVENT_14_EVENTENABLE, GEVENT_14_SCITRIG, GEVENT_14_SCILEVEL, GEVENT_14_SMISCIEN, GEVENT_14_SCIS0EN, GEVENT_14_SCIMAP, GEVENT_14_SMITRIG, GEVENT_14_SMICONTROL},
	{GEVENT_15_EVENTENABLE, GEVENT_15_SCITRIG, GEVENT_15_SCILEVEL, GEVENT_15_SMISCIEN, GEVENT_15_SCIS0EN, GEVENT_15_SCIMAP, GEVENT_15_SMITRIG, GEVENT_15_SMICONTROL},
	{GEVENT_16_EVENTENABLE, GEVENT_16_SCITRIG, GEVENT_16_SCILEVEL, GEVENT_16_SMISCIEN, GEVENT_16_SCIS0EN, GEVENT_16_SCIMAP, GEVENT_16_SMITRIG, GEVENT_16_SMICONTROL},
	{GEVENT_17_EVENTENABLE, GEVENT_17_SCITRIG, GEVENT_17_SCILEVEL, GEVENT_17_SMISCIEN, GEVENT_17_SCIS0EN, GEVENT_17_SCIMAP, GEVENT_17_SMITRIG, GEVENT_17_SMICONTROL},
	{GEVENT_18_EVENTENABLE, GEVENT_18_SCITRIG, GEVENT_18_SCILEVEL, GEVENT_18_SMISCIEN, GEVENT_18_SCIS0EN, GEVENT_18_SCIMAP, GEVENT_18_SMITRIG, GEVENT_18_SMICONTROL},
	{GEVENT_19_EVENTENABLE, GEVENT_19_SCITRIG, GEVENT_19_SCILEVEL, GEVENT_19_SMISCIEN, GEVENT_19_SCIS0EN, GEVENT_19_SCIMAP, GEVENT_19_SMITRIG, GEVENT_19_SMICONTROL},
	{GEVENT_20_EVENTENABLE, GEVENT_20_SCITRIG, GEVENT_20_SCILEVEL, GEVENT_20_SMISCIEN, GEVENT_20_SCIS0EN, GEVENT_20_SCIMAP, GEVENT_20_SMITRIG, GEVENT_20_SMICONTROL},
	{GEVENT_21_EVENTENABLE, GEVENT_21_SCITRIG, GEVENT_21_SCILEVEL, GEVENT_21_SMISCIEN, GEVENT_21_SCIS0EN, GEVENT_21_SCIMAP, GEVENT_21_SMITRIG, GEVENT_21_SMICONTROL},
	{GEVENT_22_EVENTENABLE, GEVENT_22_SCITRIG, GEVENT_22_SCILEVEL, GEVENT_22_SMISCIEN, GEVENT_22_SCIS0EN, GEVENT_22_SCIMAP, GEVENT_22_SMITRIG, GEVENT_22_SMICONTROL},
	{GEVENT_23_EVENTENABLE, GEVENT_23_SCITRIG, GEVENT_23_SCILEVEL, GEVENT_23_SMISCIEN, GEVENT_23_SCIS0EN, GEVENT_23_SCIMAP, GEVENT_23_SMITRIG, GEVENT_23_SMICONTROL},
};

#endif /* _GPIO_H_ */
