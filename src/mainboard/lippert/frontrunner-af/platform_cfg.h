/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _PLATFORM_CFG_H_
#define _PLATFORM_CFG_H_

/**
 * @def BIOS_SIZE
 *      BIOS_SIZE_{1,2,4,8,16}M
 *
 * In SB800, default ROM size is 1M Bytes, if your platform ROM
 * bigger than 1M you have to set the ROM size outside CIMx module and
 * before AGESA module get call.
 */
#ifndef BIOS_SIZE
#define BIOS_SIZE ((CONFIG_COREBOOT_ROMSIZE_KB >> 10) - 1)
#endif /* BIOS_SIZE */

/**
 * @def SPREAD_SPECTRUM
 * @brief
 *  0 - Disable Spread Spectrum function
 *  1 - Enable  Spread Spectrum function
 */
#define SPREAD_SPECTRUM			0

/**
 * @def SB_HPET_TIMER
 * @brief
 *  0 - Disable hpet
 *  1 - Enable  hpet
 */
#define HPET_TIMER			1

/**
 * @def USB_CONFIG
 * @brief bit[0-6] used to control USB
 *   0 - Disable
 *   1 - Enable
 *  Usb Ohci1 Controller (Bus 0 Dev 18 Func0) is defined at BIT0
 *  Usb Ehci1 Controller (Bus 0 Dev 18 Func2) is defined at BIT1
 *  Usb Ohci2 Controller (Bus 0 Dev 19 Func0) is defined at BIT2
 *  Usb Ehci2 Controller (Bus 0 Dev 19 Func2) is defined at BIT3
 *  Usb Ohci3 Controller (Bus 0 Dev 22 Func0) is defined at BIT4
 *  Usb Ehci3 Controller (Bus 0 Dev 22 Func2) is defined at BIT5
 *  Usb Ohci4 Controller (Bus 0 Dev 20 Func5) is defined at BIT6
 */
#if CONFIG(BOARD_LIPPERT_FRONTRUNNER_AF)
#define USB_CONFIG		0x3F
#elif CONFIG(BOARD_LIPPERT_TOUCAN_AF)
#define USB_CONFIG		0x0F
#endif

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
#if CONFIG(BOARD_LIPPERT_FRONTRUNNER_AF)
#define PCI_CLOCK_CTRL			0x1F
#elif CONFIG(BOARD_LIPPERT_TOUCAN_AF)
#define PCI_CLOCK_CTRL			0x1E
#endif

/**
 * @def SATA_CONTROLLER
 * @brief INCHIP Sata Controller
 */
#define SATA_CONTROLLER		CIMX_OPTION_ENABLED

/**
 * @def SATA_MODE
 * @brief INCHIP Sata Controller Mode
 *   NOTE: DO NOT ALLOW SATA & IDE use same mode
 */
#define SATA_MODE			CONFIG_SB800_SATA_MODE

/**
 * @brief INCHIP Sata IDE Controller Mode
 */
#define IDE_LEGACY_MODE			0
#define IDE_NATIVE_MODE			1

/**
 * @def SATA_IDE_MODE
 * @brief INCHIP Sata IDE Controller Mode
 *   NOTE: DO NOT ALLOW SATA & IDE use same mode
 */
#define SATA_IDE_MODE			IDE_LEGACY_MODE

/**
 * @def EXTERNAL_CLOCK
 * @brief 00/10: Reference clock from crystal oscillator via
 *  PAD_XTALI and PAD_XTALO
 *
 * @def INTERNAL_CLOCK
 * @brief 01/11: Reference clock from internal clock through
 *  CP_PLL_REFCLK_P and CP_PLL_REFCLK_N via RDL
 */
#define EXTERNAL_CLOCK		0x00
#define INTERNAL_CLOCK		0x01

/* NOTE: inagua have to using internal clock,
 * otherwise can not detect sata drive
 */
#define SATA_CLOCK_SOURCE	INTERNAL_CLOCK

/**
 * @def SATA_PORT_MULT_CAP_RESERVED
 * @brief 1 ON, 0 0FF
 */
#define SATA_PORT_MULT_CAP_RESERVED	1


/**
 * @def   AZALIA_AUTO
 * @brief Detect Azalia controller automatically.
 *
 * @def   AZALIA_DISABLE
 * @brief Disable Azalia controller.

 * @def   AZALIA_ENABLE
 * @brief Enable Azalia controller.
 */
#define AZALIA_AUTO			0
#define AZALIA_DISABLE			1
#define AZALIA_ENABLE			2

/**
 * @brief INCHIP HDA controller
 */
#define AZALIA_CONTROLLER		AZALIA_AUTO

/**
 * @def AZALIA_PIN_CONFIG
 * @brief
 *  0 - disable
 *  1 - enable
 */
#define AZALIA_PIN_CONFIG		1

/**
 * @def AZALIA_SDIN_PIN
 * @brief
 *  SDIN0 is defined at BIT0 & BIT1
 *   00 - GPIO PIN
 *   01 - Reserved
 *   10 - As a Azalia SDIN pin
 *  SDIN1 is defined at BIT2 & BIT3
 *  SDIN2 is defined at BIT4 & BIT5
 *  SDIN3 is defined at BIT6 & BIT7
 */
#if CONFIG(BOARD_LIPPERT_FRONTRUNNER_AF)
#define AZALIA_SDIN_PIN			0x02
#elif CONFIG(BOARD_LIPPERT_TOUCAN_AF)
#define AZALIA_SDIN_PIN			0x2A
#endif

/**
 * @def GPP_CONTROLLER
 */
#if CONFIG(BOARD_LIPPERT_FRONTRUNNER_AF)
#define GPP_CONTROLLER			CIMX_OPTION_DISABLED
#elif CONFIG(BOARD_LIPPERT_TOUCAN_AF)
#define GPP_CONTROLLER			CIMX_OPTION_ENABLED
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
#define GPP_CFGMODE			GPP_CFGMODE_X1111

/**
 * @def NB_SB_GEN2
 *    0  - Disable
 *    1  - Enable
 */
#define NB_SB_GEN2			TRUE

/**
 * @def SB_GPP_GEN2
 *    0  - Disable
 *    1  - Enable
 */
#define SB_GPP_GEN2			TRUE

/**
 * @def SB_GPP_UNHIDE_PORTS
 *    TRUE   - ports visible always, even port empty
 *    FALSE  - ports invisible if port empty
 */
#define SB_GPP_UNHIDE_PORTS		FALSE

/**
 * @def   GEC_CONFIG
 *    0  - Enable
 *    1  - Disable
 */
#define GEC_CONFIG			1

static const CODECENTRY frontrunneraf_codec_alc886[] = /* Realtek ALC886/8 */
{
	/* NID, PinConfig (Verbs 71F..C) */
	{0x11, 0x411111F0}, /* NPC */
	{0x12, 0x411111F0}, /* DMIC */
	{0x14, 0x01214110}, /* FRONT (Port-D) */
	{0x15, 0x01011112}, /* SURR (Port-A) */
	{0x16, 0x01016111}, /* CEN/LFE (Port-G) */
	{0x17, 0x411111F0}, /* SIDESURR (Port-H) */
	{0x18, 0x01A19930}, /* MIC1 (Port-B) */
	{0x19, 0x411111F0}, /* MIC2 (Port-F) */
	{0x1A, 0x0181313F}, /* LINE1 (Port-C) */
	{0x1B, 0x411111F0}, /* LINE2 (Port-E) */
	{0x1C, 0x411111F0}, /* CD-IN */
	{0x1D, 0x40132601}, /* BEEP-IN */
	{0x1E, 0x01441120}, /* S/PDIF-OUT */
	{0x1F, 0x01C46140}, /* S/PDIF-IN */
	{0xff, 0xffffffff} /* end of table */
};

static const CODECTBLLIST codec_tablelist[] =
{
	{0x10ec0888, (CODECENTRY*)&frontrunneraf_codec_alc886[0]},
	{0xFFFFFFFF, (CODECENTRY*)0xFFFFFFFFL}
};

/**
 * @def AZALIA_OEM_VERB_TABLE
 *  Mainboard specific codec verb table list
 */
#define AZALIA_OEM_VERB_TABLE		(&codec_tablelist[0])

#endif
