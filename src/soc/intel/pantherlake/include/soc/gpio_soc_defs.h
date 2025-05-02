/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PANTHERLAKE_GPIO_SOC_DEFS_H_
#define _SOC_PANTHERLAKE_GPIO_SOC_DEFS_H_

#define INC(x) ((x) + 1)

/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 */
/* GPIO COMM 0 */
#define GPP_V		0x0
#define GPP_C		0x1
/* GPIO COMM 1 */
#define GPP_F		0x2
#define GPP_E		0x3
/* GPIO COMM 3 */
#define GPP_A		0x4
#define GPP_H		0x5
/* GPIO COMM 4 */
#define GPP_S		0x9
/* GPIO COMM 5 */
#define GPP_B		0x7
#define GPP_D		0x8
#define GPP_VGPIO	0x6

/* for ACPI _UID */
#define GPP_COMM0_ID 0
#define GPP_COMM1_ID 1
#define GPP_COMM3_ID 3
#define GPP_COMM4_ID 4
#define GPP_COMM5_ID 5

#define GPP_V_NAME		"GPP_V"
#define GPP_C_NAME		"GPP_C"
#define GPP_F_NAME		"GPP_F"
#define GPP_E_NAME		"GPP_E"
#define GPP_H_NAME		"GPP_H"
#define GPP_A_NAME		"GPP_A"
#define GPP_VGPIO3_NAME		"vGPIO_3"
#define GPP_S_NAME		"GPP_S"
#define GPP_B_NAME		"GPP_B"
#define GPP_D_NAME		"GPP_D"
#define GPP_VGPIO_NAME		"vGPIO"

#if CONFIG(SOC_INTEL_WILDCATLAKE)
#define GPIO_MAX_NUM_PER_GROUP	29
#else
#define GPIO_MAX_NUM_PER_GROUP	28
#endif

#define COMM_0		0
#define COMM_1		INC(COMM_0)
#define COMM_3		INC(COMM_1)
#define COMM_4		INC(COMM_3)
#define COMM_5		INC(COMM_4)

/*
 * GPIOs are ordered monotonically increasing to match ACPI/OS driver.
 */
/*
 * +----------------------------+
 * | Community 0                |
 * +----------------------------+
 */

/*
 * +----------------------------------+
 * |             Group V              |
 * +------------------+---------------+
 * |                  |  PTL-U/H/WCL  |
 * +------------------+---------------+
 * | Pad Start Number |       0       |
 * +------------------+---------------+
 * | Pad End Number   |      23       |
 * +------------------+---------------+
 */
#define GPP_V00			0
#define GPP_V01			INC(GPP_V00)
#define GPP_V02			INC(GPP_V01)
#define GPP_V03			INC(GPP_V02)
#define GPP_V04			INC(GPP_V03)
#define GPP_V05			INC(GPP_V04)
#define GPP_V06			INC(GPP_V05)
#define GPP_V07			INC(GPP_V06)
#define GPP_V08			INC(GPP_V07)
#define GPP_V09			INC(GPP_V08)
#define GPP_V10			INC(GPP_V09)
#define GPP_V11			INC(GPP_V10)
#define GPP_V12			INC(GPP_V11)
#define GPP_V13			INC(GPP_V12)
#define GPP_V14			INC(GPP_V13)
#define GPP_V15			INC(GPP_V14)
#define GPP_V16			INC(GPP_V15)
#define GPP_V17			INC(GPP_V16)

#define GPP_PWROK		INC(GPP_V17)
#define GPP_RESET_B		INC(GPP_PWROK)
#define GPP_BKLTEN		INC(GPP_RESET_B)
#define GPP_BKLTCTL		INC(GPP_BKLTEN)
#define GPP_VDDEN		INC(GPP_BKLTCTL)
#define GPP_RST_B		INC(GPP_VDDEN)

#define NUM_GRP_V_PADS		(GPP_RST_B - GPP_V00 + 1)
#define NUM_GPP_V_PADS		(GPP_V17 - GPP_V00 + 1)

/*
 * +----------------------------------+
 * |             Group C              |
 * +------------------+---------------+
 * |                  |  PTL-U/H/WCL  |
 * +------------------+---------------+
 * | Pad Start Number |      24       |
 * +------------------+---------------+
 * | Pad End Number   |      47       |
 * +------------------+---------------+
 */
#define GPP_C00			INC(GPP_RST_B)
#define GPP_C01			INC(GPP_C00)
#define GPP_C02			INC(GPP_C01)
#define GPP_C03			INC(GPP_C02)
#define GPP_C04			INC(GPP_C03)
#define GPP_C05			INC(GPP_C04)
#define GPP_C06			INC(GPP_C05)
#define GPP_C07			INC(GPP_C06)
#define GPP_C08			INC(GPP_C07)
#define GPP_C09			INC(GPP_C08)
#define GPP_C10			INC(GPP_C09)
#define GPP_C11			INC(GPP_C10)
#define GPP_C12			INC(GPP_C11)
#define GPP_C13			INC(GPP_C12)
#define GPP_C14			INC(GPP_C13)
#define GPP_C15			INC(GPP_C14)
#define GPP_C16			INC(GPP_C15)
#define GPP_C17			INC(GPP_C16)
#define GPP_C18			INC(GPP_C17)
#define GPP_C19			INC(GPP_C18)
#define GPP_C20			INC(GPP_C19)
#define GPP_C21			INC(GPP_C20)
#define GPP_C22			INC(GPP_C21)
#define GPP_C23			INC(GPP_C22)

#define NUM_GRP_C_PADS		(GPP_C23 - GPP_C00 + 1)
#define NUM_GPP_C_PADS		(GPP_C23 - GPP_C00 + 1)

#define COM0_GRP_PAD_START	GPP_V00
#define COM0_GRP_PAD_END	GPP_C23
#define NUM_COM0_GRP_PADS	(COM0_GRP_PAD_END - COM0_GRP_PAD_START + 1)
#define NUM_COM0_GPP_PADS	(NUM_GPP_V_PADS + NUM_GPP_C_PADS)
#define NUM_COM0_GROUPS		2

/*
 * +----------------------------+
 * | Community 1                |
 * +----------------------------+
 */

/*
 * +----------------------------------+
 * |             Group F              |
 * +------------------+---------------+
 * |                  |  PTL-U/H/WCL  |
 * +------------------+---------------+
 * | Pad Start Number |      48       |
 * +------------------+---------------+
 * | Pad End Number   |      73       |
 * +------------------+---------------+
 */
#define GPP_F00			INC(GPP_C23)
#define GPP_F01			INC(GPP_F00)
#define GPP_F02			INC(GPP_F01)
#define GPP_F03			INC(GPP_F02)
#define GPP_F04			INC(GPP_F03)
#define GPP_F05			INC(GPP_F04)
#define GPP_F06			INC(GPP_F05)
#define GPP_F07			INC(GPP_F06)
#define GPP_F08			INC(GPP_F07)
#define GPP_F09			INC(GPP_F08)
#define GPP_F10			INC(GPP_F09)
#define GPP_F11			INC(GPP_F10)
#define GPP_F12			INC(GPP_F11)
#define GPP_F13			INC(GPP_F12)
#define GPP_F14			INC(GPP_F13)
#define GPP_F15			INC(GPP_F14)
#define GPP_F16			INC(GPP_F15)
#define GPP_F17			INC(GPP_F16)
#define GPP_F18			INC(GPP_F17)
#define GPP_F19			INC(GPP_F18)
#define GPP_F20			INC(GPP_F19)
#define GPP_F21			INC(GPP_F20)
#define GPP_F22			INC(GPP_F21)
#define GPP_F23			INC(GPP_F22)
#define GPP_THC1_GSPI1_CLK_LPBK	INC(GPP_F23)
#define GPP_GSPI0_CLK_LOOPBK	INC(GPP_THC1_GSPI1_CLK_LPBK)

#define NUM_GRP_F_PADS		(GPP_GSPI0_CLK_LOOPBK - GPP_F00 + 1)
#define NUM_GPP_F_PADS		(GPP_F23 - GPP_F00 + 1)

/*
 * +----------------------------------+
 * |             Group E              |
 * +------------------+---------------+
 * |                  |  PTL-U/H/WCL  |
 * +------------------+---------------+
 * | Pad Start Number |      74       |
 * +------------------+---------------+
 * | Pad End Number   |      98       |
 * +------------------+---------------+
 *
 * NOTE: GPP_E00 is not a connected PAD in PTL and should be treated
 * as other internal used only PADs. It does not meant to be used.
 * ref doc: Panther Lake H GPIO Implementation Summary (#817954)
 */
#define GPP_E00			INC(GPP_GSPI0_CLK_LOOPBK)
#define GPP_E01			INC(GPP_E00)
#define GPP_E02			INC(GPP_E01)
#define GPP_E03			INC(GPP_E02)
#define GPP_E04			INC(GPP_E03)
#define GPP_E05			INC(GPP_E04)
#define GPP_E06			INC(GPP_E05)
#define GPP_E07			INC(GPP_E06)
#define GPP_E08			INC(GPP_E07)
#define GPP_E09			INC(GPP_E08)
#define GPP_E10			INC(GPP_E09)
#define GPP_E11			INC(GPP_E10)
#define GPP_E12			INC(GPP_E11)
#define GPP_E13			INC(GPP_E12)
#define GPP_E14			INC(GPP_E13)
#define GPP_E15			INC(GPP_E14)
#define GPP_E16			INC(GPP_E15)
#define GPP_E17			INC(GPP_E16)
#define GPP_E18			INC(GPP_E17)
#define GPP_E19			INC(GPP_E18)
#define GPP_E20			INC(GPP_E19)
#define GPP_E21			INC(GPP_E20)
#define GPP_E22			INC(GPP_E21)
#define GPP_BOOTHALT_B		INC(GPP_E22)
#define GPP_THC0_GSPI_CLK_LPBK	INC(GPP_BOOTHALT_B)

#define NUM_GRP_E_PADS		(GPP_THC0_GSPI_CLK_LPBK - GPP_E00 + 1)
#define NUM_GPP_E_PADS		(GPP_E22 - GPP_E00 + 1)

#define COM1_GRP_PAD_START	GPP_F00
#define COM1_GRP_PAD_END	GPP_THC0_GSPI_CLK_LPBK
#define NUM_COM1_GRP_PADS	(COM1_GRP_PAD_END - COM1_GRP_PAD_START + 1)
#define NUM_COM1_GPP_PADS	(NUM_GPP_F_PADS + NUM_GPP_E_PADS)
#define NUM_COM1_GROUPS		2

/*
 * +----------------------------+
 * | Community 3                |
 * +----------------------------+
 */

#if (!CONFIG(SOC_INTEL_WILDCATLAKE))
/*
 * +----------------------------+
 * |        Group CPUJTAG       |
 * +------------------+---------+
 * |                  | PTL-UH/H|
 * +------------------+---------+
 * | Pad Start Number |    99   |
 * +------------------+---------+
 * | Pad End Number   |    113  |
 * +------------------+---------+
 */

#define GPP_EPD_ON		INC(GPP_THC0_GSPI_CLK_LPBK)
#define GPP_VDD2_PWRGD		INC(GPP_EPD_ON)

#define GPP_JTAG_MBPB0		INC(GPP_VDD2_PWRGD)
#define GPP_JTAG_MBPB1		INC(GPP_JTAG_MBPB0)
#define GPP_JTAG_MBPB2		INC(GPP_JTAG_MBPB1)
#define GPP_JTAG_MBPB3		INC(GPP_JTAG_MBPB2)
#define GPP_JTAG_TD0		INC(GPP_JTAG_MBPB3)
#define GPP_PRDY_B		INC(GPP_JTAG_TD0)
#define GPP_PREQ_B		INC(GPP_PRDY_B)
#define GPP_JTAG_TDI		INC(GPP_PREQ_B)
#define GPP_JTAG_TMS		INC(GPP_JTAG_TDI)
#define GPP_JTAG_TCK		INC(GPP_JTAG_TMS)
#define GPP_DBG_PMODE		INC(GPP_JTAG_TCK)
#define GPP_JTAG_TRST_B		INC(GPP_DBG_PMODE)
#define GPP_DDSP_HPDALV		INC(GPP_JTAG_TRST_B)

#define NUM_GRP_RSVD_PADS	(GPP_DDSP_HPDALV - GPP_EPD_ON + 1)
#endif

/*
 * +----------------------------------+
 * |                Group H           |
 * +------------------+---------+-----+
 * |                  | PTL-UH/H| WCL |
 * +------------------+---------+-----+
 * | Pad Start Number |   114   |  99 |
 * +------------------+---------+-----+
 * | Pad End Number   |   141   | 127 |
 * +------------------+---------+-----+
 */
#if CONFIG(SOC_INTEL_WILDCATLAKE)
#define GPP_H00			INC(GPP_THC0_GSPI_CLK_LPBK)
#else
#define GPP_H00			INC(GPP_DDSP_HPDALV)
#endif
#define GPP_H01			INC(GPP_H00)
#define GPP_H02			INC(GPP_H01)
#define GPP_H03			INC(GPP_H02)
#define GPP_H04			INC(GPP_H03)
#define GPP_H05			INC(GPP_H04)
#define GPP_H06			INC(GPP_H05)
#define GPP_H07			INC(GPP_H06)
#define GPP_H08			INC(GPP_H07)
#define GPP_H09			INC(GPP_H08)
#define GPP_H10			INC(GPP_H09)
#define GPP_H11			INC(GPP_H10)
#define GPP_H12			INC(GPP_H11)
#define GPP_H13			INC(GPP_H12)
#define GPP_H14			INC(GPP_H13)
#define GPP_H15			INC(GPP_H14)
#define GPP_H16			INC(GPP_H15)
#define GPP_H17			INC(GPP_H16)
#define GPP_H18			INC(GPP_H17)
#define GPP_H19			INC(GPP_H18)
#define GPP_H20			INC(GPP_H19)
#define GPP_H21			INC(GPP_H20)
#define GPP_H22			INC(GPP_H21)
#define GPP_H23			INC(GPP_H22)
#define GPP_H24			INC(GPP_H23)
#define GPP_LPI3C1_CLK_LPBK	INC(GPP_H24)
#define GPP_LPI3C0_CLK_LPBK	INC(GPP_LPI3C1_CLK_LPBK)
#define GPP_ISHI3C1_CLK_LPBK	INC(GPP_LPI3C0_CLK_LPBK)
#if CONFIG(SOC_INTEL_WILDCATLAKE)
#define GPP_SHD3C_CLK_LPBK	INC(GPP_ISHI3C1_CLK_LPBK)
#define NUM_GRP_H_PADS		(GPP_SHD3C_CLK_LPBK - GPP_H00 + 1)
#else
#define NUM_GRP_H_PADS		(GPP_ISHI3C1_CLK_LPBK - GPP_H00 + 1)
#endif
#define NUM_GPP_H_PADS		(GPP_H24 - GPP_H00 + 1)

/*
 * +----------------------------------+
 * |                Group A           |
 * +------------------+---------+-----+
 * |                  | PTL-UH/H| WCL |
 * +------------------+---------+-----+
 * | Pad Start Number |   142   | 128 |
 * +------------------+---------+-----+
 * | Pad End Number   |   169   | 155 |
 * +------------------+---------+-----+
 */
#if CONFIG(SOC_INTEL_WILDCATLAKE)
#define GPP_A00			INC(GPP_SHD3C_CLK_LPBK)
#else
#define GPP_A00			INC(GPP_ISHI3C1_CLK_LPBK)
#endif
#define GPP_A01			INC(GPP_A00)
#define GPP_A02			INC(GPP_A01)
#define GPP_A03			INC(GPP_A02)
#define GPP_A04			INC(GPP_A03)
#define GPP_A05			INC(GPP_A04)
#define GPP_A06			INC(GPP_A05)
#define GPP_A07			INC(GPP_A06)
#define GPP_A08			INC(GPP_A07)
#define GPP_A09			INC(GPP_A08)
#define GPP_A10			INC(GPP_A09)
#define GPP_A11			INC(GPP_A10)
#define GPP_A12			INC(GPP_A11)
#define GPP_A13			INC(GPP_A12)
#define GPP_A14			INC(GPP_A13)
#define GPP_A15			INC(GPP_A14)
#define GPP_A16			INC(GPP_A15)
#define GPP_A17			INC(GPP_A16)
/* SPI0 PADs */
#define GPP_SPI0_IO_2		INC(GPP_A17)
#define GPP_SPI0_IO_3		INC(GPP_SPI0_IO_2)
#define GPP_SPI0_MOSI_IO_0	INC(GPP_SPI0_IO_3)
#define GPP_SPI0_MOSI_IO_1	INC(GPP_SPI0_MOSI_IO_0)
#define GPP_SPI0_TPM_CS_B	INC(GPP_SPI0_MOSI_IO_1)
#define GPP_SPI0_FLASH_0_CS_B	INC(GPP_SPI0_TPM_CS_B)
#define GPP_SPI0_FLASH_1_CS_B	INC(GPP_SPI0_FLASH_0_CS_B)
#define GPP_SPI0_CLK		INC(GPP_SPI0_FLASH_1_CS_B)
#define GPP_ESPI_CLK_LPBK	INC(GPP_SPI0_CLK)
#define GPP_SPI0_CLK_LOOPBK	INC(GPP_ESPI_CLK_LPBK)

#define NUM_GRP_A_PADS		(GPP_SPI0_CLK_LOOPBK - GPP_A00 + 1)
#define NUM_GPP_A_PADS		(GPP_A17 - GPP_A00 + 1)

/*
 * +----------------------------------+
 * |              Group vGPIO3        |
 * +------------------+---------+-----+
 * |                  | PTL-UH/H| WCL |
 * +------------------+---------+-----+
 * | Pad Start Number |    170  | 156 |
 * +------------------+---------+-----+
 * | Pad End Number   |    183  | 169 |
 * +------------------+---------+-----+
 */
#define GPP_VGPIO3_USB0		INC(GPP_SPI0_CLK_LOOPBK)
#define GPP_VGPIO3_USB1		INC(GPP_VGPIO3_USB0)
#define GPP_VGPIO3_USB2		INC(GPP_VGPIO3_USB1)
#define GPP_VGPIO3_USB3		INC(GPP_VGPIO3_USB2)
#define GPP_VGPIO3_USB4		INC(GPP_VGPIO3_USB3)
#define GPP_VGPIO3_USB5		INC(GPP_VGPIO3_USB4)
#define GPP_VGPIO3_USB6		INC(GPP_VGPIO3_USB5)
#define GPP_VGPIO3_USB7		INC(GPP_VGPIO3_USB6)
#define GPP_VGPIO3_TS0		INC(GPP_VGPIO3_USB7)
#define GPP_VGPIO3_TS1		INC(GPP_VGPIO3_TS0)
#define GPP_VGPIO3_THC0		INC(GPP_VGPIO3_TS1)
#define GPP_VGPIO3_THC1		INC(GPP_VGPIO3_THC0)
#define GPP_VGPIO3_THC2		INC(GPP_VGPIO3_THC1)
#define GPP_VGPIO3_THC3		INC(GPP_VGPIO3_THC2)

#define NUM_GRP_VGPIO3_PADS	(GPP_VGPIO3_THC3 - GPP_VGPIO3_USB0 + 1)

#if CONFIG(SOC_INTEL_WILDCATLAKE)
#define COM3_GRP_PAD_START	GPP_H00
#define NUM_COM3_GROUPS		3
#else
#define COM3_GRP_PAD_START	GPP_EPD_ON
#define NUM_COM3_GROUPS		4
#endif
#define COM3_GRP_PAD_END	GPP_VGPIO3_THC3
#define NUM_COM3_GRP_PADS	(COM3_GRP_PAD_END - COM3_GRP_PAD_START + 1)
#define NUM_COM3_GPP_PADS	(NUM_GPP_H_PADS + NUM_GPP_A_PADS)

/*
 * +----------------------------+
 * | Community 4                |
 * +----------------------------+
 */

/*
 * +----------------------------------+
 * |                Group S           |
 * +------------------+---------+-----+
 * |                  | PTL-UH/H| WCL |
 * +------------------+---------+-----+
 * | Pad Start Number |    184  | 170 |
 * +------------------+---------+-----+
 * | Pad End Number   |    191  | 177 |
 * +------------------+---------+-----+
 */
#define GPP_S00			INC(GPP_VGPIO3_THC3)
#define GPP_S01			INC(GPP_S00)
#define GPP_S02			INC(GPP_S01)
#define GPP_S03			INC(GPP_S02)
#define GPP_S04			INC(GPP_S03)
#define GPP_S05			INC(GPP_S04)
#define GPP_S06			INC(GPP_S05)
#define GPP_S07			INC(GPP_S06)

#define NUM_GRP_S_PADS		(GPP_S07 - GPP_S00 + 1)
#define NUM_GPP_S_PADS		(GPP_S07 - GPP_S00 + 1)

#if CONFIG(SOC_INTEL_WILDCATLAKE)
/*
 * +------------------------+
 * |      Group CPUJTAG     |
 * +------------------+-----+
 * |                  | WCL |
 * +------------------+-----+
 * | Pad Start Number | 178 |
 * +------------------+-----+
 * | Pad End Number   | 192 |
 * +------------------+-----+
 */

#define GPP_EPD_ON		INC(GPP_S07)
#define GPP_VDD2_PWRGD		INC(GPP_EPD_ON)

#define GPP_JTAG_MBPB0		INC(GPP_VDD2_PWRGD)
#define GPP_JTAG_MBPB1		INC(GPP_JTAG_MBPB0)
#define GPP_JTAG_MBPB2		INC(GPP_JTAG_MBPB1)
#define GPP_JTAG_MBPB3		INC(GPP_JTAG_MBPB2)
#define GPP_JTAG_TD0		INC(GPP_JTAG_MBPB3)
#define GPP_PRDY_B		INC(GPP_JTAG_TD0)
#define GPP_PREQ_B		INC(GPP_PRDY_B)
#define GPP_JTAG_TDI		INC(GPP_PREQ_B)
#define GPP_JTAG_TMS		INC(GPP_JTAG_TDI)
#define GPP_JTAG_TCK		INC(GPP_JTAG_TMS)
#define GPP_DBG_PMODE		INC(GPP_JTAG_TCK)
#define GPP_JTAG_TRST_B		INC(GPP_DBG_PMODE)
#define GPP_DDSP_HPDALV		INC(GPP_JTAG_TRST_B)

#define NUM_GRP_RSVD_PADS	(GPP_DDSP_HPDALV - GPP_EPD_ON + 1)
#endif
#define COM4_GRP_PAD_START	GPP_S00

#if CONFIG(SOC_INTEL_WILDCATLAKE)
#define COM4_GRP_PAD_END	GPP_DDSP_HPDALV
#define NUM_COM4_GPP_PADS	(NUM_GPP_S_PADS)
#define NUM_COM4_GROUPS		2
#else
#define COM4_GRP_PAD_END	GPP_S07
#define NUM_COM4_GPP_PADS	(GPP_S07 - GPP_S00 + 1)
#define NUM_COM4_GROUPS		1
#endif
#define NUM_COM4_GRP_PADS	(COM4_GRP_PAD_END - COM4_GRP_PAD_START + 1)
/*
 * +----------------------------+
 * | Community 5                |
 * +----------------------------+
 */

/*
 * +----------------------------------+
 * |              Group B             |
 * +------------------+---------+-----+
 * |                  | PTL-UH/H| WCL |
 * +------------------+---------+-----+
 * | Pad Start Number |    192  | 193 |
 * +------------------+---------+-----+
 * | Pad End Number   |    218  | 219 |
 * +------------------+---------+-----+
 */
#if CONFIG(SOC_INTEL_WILDCATLAKE)
#define GPP_B00			INC(GPP_DDSP_HPDALV)
#else
#define GPP_B00			INC(GPP_S07)
#endif
#define GPP_B01			INC(GPP_B00)
#define GPP_B02			INC(GPP_B01)
#define GPP_B03			INC(GPP_B02)
#define GPP_B04			INC(GPP_B03)
#define GPP_B05			INC(GPP_B04)
#define GPP_B06			INC(GPP_B05)
#define GPP_B07			INC(GPP_B06)
#define GPP_B08			INC(GPP_B07)
#define GPP_B09			INC(GPP_B08)
#define GPP_B10			INC(GPP_B09)
#define GPP_B11			INC(GPP_B10)
#define GPP_B12			INC(GPP_B11)
#define GPP_B13			INC(GPP_B12)
#define GPP_B14			INC(GPP_B13)
#define GPP_B15			INC(GPP_B14)
#define GPP_B16			INC(GPP_B15)
#define GPP_B17			INC(GPP_B16)
#define GPP_B18			INC(GPP_B17)
#define GPP_B19			INC(GPP_B18)
#define GPP_B20			INC(GPP_B19)
#define GPP_B21			INC(GPP_B20)
#define GPP_B22			INC(GPP_B21)
#define GPP_B23			INC(GPP_B22)
#define GPP_B24			INC(GPP_B23)
#define GPP_B25			INC(GPP_B24)
#define GPP_ISHI3C0_CLK_LPBK	INC(GPP_B25)

#define NUM_GRP_B_PADS		(GPP_ISHI3C0_CLK_LPBK - GPP_B00 + 1)
#define NUM_GPP_B_PADS		(GPP_B25 - GPP_B00 + 1)

/*
 * +----------------------------------+
 * |              Group D             |
 * +------------------+---------+-----+
 * |                  | PTL-UH/H| WCL |
 * +------------------+---------+-----+
 * | Pad Start Number |    219  | 220 |
 * +------------------+---------+-----+
 * | Pad End Number   |    245  | 246 |
 * +------------------+---------+-----+
 */
#define GPP_D00			INC(GPP_ISHI3C0_CLK_LPBK)
#define GPP_D01			INC(GPP_D00)
#define GPP_D02			INC(GPP_D01)
#define GPP_D03			INC(GPP_D02)
#define GPP_D04			INC(GPP_D03)
#define GPP_D05			INC(GPP_D04)
#define GPP_D06			INC(GPP_D05)
#define GPP_D07			INC(GPP_D06)
#define GPP_D08			INC(GPP_D07)
#define GPP_D09			INC(GPP_D08)
#define GPP_D10			INC(GPP_D09)
#define GPP_D11			INC(GPP_D10)
#define GPP_D12			INC(GPP_D11)
#define GPP_D13			INC(GPP_D12)
#define GPP_D14			INC(GPP_D13)
#define GPP_D15			INC(GPP_D14)
#define GPP_D16			INC(GPP_D15)
#define GPP_D17			INC(GPP_D16)
#define GPP_D18			INC(GPP_D17)
#define GPP_D19			INC(GPP_D18)
#define GPP_D20			INC(GPP_D19)
#define GPP_D21			INC(GPP_D20)
#define GPP_D22			INC(GPP_D21)
#define GPP_D23			INC(GPP_D22)
#define GPP_D24			INC(GPP_D23)
#define GPP_D25			INC(GPP_D24)
#define GPP_ISDI3C1_CLK_LPBK	INC(GPP_D25)

#define NUM_GRP_D_PADS		(GPP_ISDI3C1_CLK_LPBK - GPP_D00 + 1)
#define NUM_GPP_D_PADS		(GPP_D25 - GPP_D00 + 1)

/*
 * +----------------------------------+
 * |             Group vGPIO          |
 * +------------------+---------+-----+
 * |                  | PTL-UH/H| WCL |
 * +------------------+---------+-----+
 * | Pad Start Number |    246  | 247 |
 * +------------------+---------+-----+
 * | Pad End Number   |    263  | 264 |
 * +------------------+---------+-----+
 */
#define GPP_VGPIO0		INC(GPP_ISDI3C1_CLK_LPBK)
#define GPP_VGPIO5		INC(GPP_VGPIO0)
#define GPP_VGPIO30		INC(GPP_VGPIO5)
#define GPP_VGPIO31		INC(GPP_VGPIO30)
#define GPP_VGPIO32		INC(GPP_VGPIO31)
#define GPP_VGPIO33		INC(GPP_VGPIO32)
#define GPP_VGPIO34		INC(GPP_VGPIO33)
#define GPP_VGPIO35		INC(GPP_VGPIO34)
#define GPP_VGPIO36		INC(GPP_VGPIO35)
#define GPP_VGPIO37		INC(GPP_VGPIO36)
#define GPP_VGPIO40		INC(GPP_VGPIO37)
#define GPP_VGPIO41		INC(GPP_VGPIO40)
#define GPP_VGPIO42		INC(GPP_VGPIO41)
#define GPP_VGPIO43		INC(GPP_VGPIO42)
#define GPP_VGPIO44		INC(GPP_VGPIO43)
#define GPP_VGPIO45		INC(GPP_VGPIO44)
#define GPP_VGPIO46		INC(GPP_VGPIO45)
#define GPP_VGPIO47		INC(GPP_VGPIO46)

#define NUM_GRP_VGPIO_PADS	(GPP_VGPIO47 - GPP_VGPIO0 + 1)

#define COM5_GRP_PAD_START	GPP_B00
#define COM5_GRP_PAD_END	GPP_VGPIO47
#define NUM_COM5_GRP_PADS	(COM5_GRP_PAD_END - COM5_GRP_PAD_START + 1)
#define NUM_COM5_GPP_PADS	(NUM_GPP_B_PADS + NUM_GPP_D_PADS)
#define NUM_COM5_GROUPS		3

#define TOTAL_GPIO_COMM		(COMM_5 + 1)
#define TOTAL_PADS		(COM5_GRP_PAD_END + 1)

#endif /* _SOC_PANTHERLAKE_GPIO_SOC_DEFS_H_ */
