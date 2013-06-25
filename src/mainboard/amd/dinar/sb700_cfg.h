/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#ifndef _SB700_CFG_H_
#define _SB700_CFG_H_

#include <stdint.h>


/**
 * @def BIOS_SIZE_1M
 * @def BIOS_SIZE_2M
 * @def BIOS_SIZE_4M
 * @def BIOS_SIZE_8M
 */
#define BIOS_SIZE_1M                    0
#define BIOS_SIZE_2M                    1
#define BIOS_SIZE_4M                    3
#define BIOS_SIZE_8M                    7

/* In SB700, default ROM size is 1M Bytes, if your platform ROM
 * bigger than 1M you have to set the ROM size outside CIMx module and
 * before AGESA module get call.
 */
#ifndef BIOS_SIZE
#if CONFIG_COREBOOT_ROMSIZE_KB_1024
#define BIOS_SIZE BIOS_SIZE_1M
#elif CONFIG_COREBOOT_ROMSIZE_KB_2048 == 1
#define BIOS_SIZE BIOS_SIZE_2M
#elif CONFIG_COREBOOT_ROMSIZE_KB_4096 == 1
#define BIOS_SIZE BIOS_SIZE_4M
#elif CONFIG_COREBOOT_ROMSIZE_KB_8192 == 1
#define BIOS_SIZE BIOS_SIZE_8M
#endif
#endif

/**
 * @def SPREAD_SPECTRUM
 * @brief
 *  0 - Disable Spread Spectrum function
 *  1 - Enable  Spread Spectrum function
 */
#define SPREAD_SPECTRUM                 0

/**
 * @def SB_HPET_TIMER
 * @brief
 *  0 - Disable hpet
 *  1 - Enable  hpet
 */
#define HPET_TIMER                      1

/**
 * @def USB_CONFIG
 * @brief bit[0-6] used to control USB
 *   0 - Disable
 *   1 - Enable
 *  Usb Ohci1 Contoller (Bus 0 Dev 18 Func0) is define at BIT0
 *  Usb Ehci1 Contoller (Bus 0 Dev 18 Func2) is define at BIT1
 *  Usb Ohci2 Contoller (Bus 0 Dev 19 Func0) is define at BIT2
 *  Usb Ehci2 Contoller (Bus 0 Dev 19 Func2) is define at BIT3
 *  Usb Ohci3 Contoller (Bus 0 Dev 22 Func0) is define at BIT4
 *  Usb Ehci3 Contoller (Bus 0 Dev 22 Func2) is define at BIT5
 *  Usb Ohci4 Contoller (Bus 0 Dev 20 Func5) is define at BIT6
 */
#define USB_CINFIG              0x7F

/**
 * @def PCI_CLOCK_CTRL
 * @brief bit[0-4] used for PCI Slots Clock Control,
 *   0 - disable
 *   1 - enable
 *  PCI SLOT 0 define at BIT0
 *  PCI SLOT 1 define at BIT1
 *  PCI SLOT 2 define at BIT2
 *  PCI SLOT 3 define at BIT3
 *  PCI SLOT 4 define at BIT4
 */
#define PCI_CLOCK_CTRL                  0x1F

/**
 * @def SATA_CONTROLLER
 * @brief INCHIP Sata Controller
 */
#ifndef SATA_CONTROLLER
#define SATA_CONTROLLER               1
#endif

/**
 * @def SATA_MODE
 * @brief INCHIP Sata Controller Mode
 *   NOTE: DO NOT ALLOW SATA & IDE use same mode
 */
#ifndef SATA_MODE
#define SATA_MODE                     NATIVE_IDE_MODE
#endif

/**
 * @brief INCHIP Sata IDE Controller Mode
 */
#define IDE_LEGACY_MODE                 0
#define IDE_NATIVE_MODE                 1

/**
 * @def SATA_IDE_MODE
 * @brief INCHIP Sata IDE Controller Mode
 *   NOTE: DO NOT ALLOW SATA & IDE use same mode
 */
#ifndef SATA_IDE_MODE
#define SATA_IDE_MODE                 IDE_LEGACY_MODE
#endif

/**
 * @def EXTERNAL_CLOCK
 * @brief 00/10: Reference clock from crystal oscillator via
 *  PAD_XTALI and PAD_XTALO
 *
 * @def INTERNAL_CLOCK
 * @brief 01/11: Reference clock from internal clock through
 *  CP_PLL_REFCLK_P and CP_PLL_REFCLK_N via RDL
 */
#define EXTERNAL_CLOCK          0x00
#define INTERNAL_CLOCK          0x01

#define SATA_CLOCK_SOURCE       EXTERNAL_CLOCK

/**
 * @def SATA_PORT_MULT_CAP_RESERVED
 * @brief 1 ON, 0 0FF
 */
#define SATA_PORT_MULT_CAP_RESERVED     1


/**
 * @def   AZALIA_AUTO
 * @brief Detect Azalia controller automatically.
 *
 * @def   AZALIA_DISABLE
 * @brief Disable Azalia controller.

 * @def   AZALIA_ENABLE
 * @brief Enable Azalia controller.
 */
#define AZALIA_AUTO                     0
#define AZALIA_DISABLE                  1
#define AZALIA_ENABLE                   2

/**
 * @brief INCHIP HDA controller
 */
#ifndef AZALIA_CONTROLLER
#define AZALIA_CONTROLLER             AZALIA_AUTO
#endif

/**
 * @def AZALIA_PIN_CONFIG
 * @brief
 *  0 - disable
 *  1 - enable
 */
#ifndef AZALIA_PIN_CONFIG
#define AZALIA_PIN_CONFIG             1
#endif

/**
 * @def AZALIA_SDIN_PIN
 * @brief
 *  SDIN0 is define at BIT0 & BIT1
 *   00 - GPIO PIN
 *   01 - Reserved
 *   10 - As a Azalia SDIN pin
 *  SDIN1 is define at BIT2 & BIT3
 *  SDIN2 is define at BIT4 & BIT5
 *  SDIN3 is define at BIT6 & BIT7
 */
#ifndef AZALIA_SDIN_PIN
//#define AZALIA_SDIN_PIN             0xAA
#define AZALIA_SDIN_PIN
#define AZALIA_SDIN_PIN_0             0x2
#define AZALIA_SDIN_PIN_1             0x2
#define AZALIA_SDIN_PIN_2             0x2
#define AZALIA_SDIN_PIN_3             0x0
#endif

/**
 * @def GPP_CONTROLLER
 */
#ifndef GPP_CONTROLLER
#define GPP_CONTROLLER                1
#endif

/**
 * @def GPP_CFGMODE
 * @brief GPP Link Configuration
 * four possible configuration:
 *  GPP_CFGMODE_X4000
 *  GPP_CFGMODE_X2200
 *  GPP_CFGMODE_X2110
 *  GPP_CFGMODE_X1111
 */
#ifndef GPP_CFGMODE
#define GPP_CFGMODE                   GPP_CFGMODE_X1111
#endif


/**
 * @brief South Bridge CIMx configuration
 *
 */
void sb700_cimx_config(AMDSBCFG *sb_cfg);

/**
 * @brief Entry point of Southbridge CIMx callout
 *
 * prototype UINT32 (*SBCIM_HOOK_ENTRY)(UINT32 Param1, UINT32 Param2, void* pConfig)
 *
 * @param[in] func    Southbridge CIMx Function ID.
 * @param[in] data    Southbridge Input Data.
 * @param[in] sb_cfg  Southbridge configuration structure pointer.
 *
 */
u32 sb700_callout_entry(u32 func, u32 data, void* sb_cfg);

#endif //_SB700_CFG_H_
