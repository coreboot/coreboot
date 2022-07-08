/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _SOC_METEORLAKE_GPIO_SOC_DEFS_H_
#define _SOC_METEORLAKE_GPIO_SOC_DEFS_H_

#define INC(x) ((x) + 1)

/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 */
#define GPP_V			0x0
#define GPP_C			INC(GPP_V)
#define GPP_A			INC(GPP_C)
#define GPP_E			INC(GPP_A)
#define GPP_H			INC(GPP_E)
#define GPP_F			INC(GPP_H)
#define GPP_VGPIO3		INC(GPP_F)
#define GPP_VGPIO		INC(GPP_VGPIO3)
#define GPP_S			INC(GPP_VGPIO)
#define GPP_B			INC(GPP_S)
#define GPP_D			INC(GPP_B)

#define GPIO_MAX_NUM_PER_GROUP	26

#define COMM_0			0
#define COMM_1			INC(COMM_0)
#define COMM_3			INC(COMM_1)
#define COMM_4			INC(COMM_3)
#define COMM_5			INC(COMM_4)

/*
 * GPIOs are ordered monotonically increasing to match ACPI/OS driver.
 */
/*
 * +----------------------------+
 * |        Group CPU           |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    0    |
 * +------------------+---------+
 * | Pad End Number   |    4    |
 * +------------------+---------+
 */
#define GPP_PECI		0
#define GPP_UFS_RST_B		INC(GPP_PECI)
#define GPP_VIDSOUT		INC(GPP_UFS_RST_B)
#define GPP_VIDSCK		INC(GPP_VIDSOUT)
#define GPP_VIDALERT_B		INC(GPP_VIDSCK)

/*
 * +----------------------------+
 * |         Group V            |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    5    |
 * +------------------+---------+
 * | Pad End Number   |    28   |
 * +------------------+---------+
 */
#define GPP_V00			INC(GPP_VIDALERT_B)
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
#define GPP_V18			INC(GPP_V17)
#define GPP_V19			INC(GPP_V18)
#define GPP_V20			INC(GPP_V19)
#define GPP_V21			INC(GPP_V20)
#define GPP_V22			INC(GPP_V21)
#define GPP_V23			INC(GPP_V22)

/*
 * +----------------------------+
 * |         Group C            |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    29   |
 * +------------------+---------+
 * | Pad End Number   |    52   |
 * +------------------+---------+
 */
#define GPP_C00			INC(GPP_V23)
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

#define GPIO_COM0_START		GPP_PECI
#define GPIO_COM0_END		GPP_C23
#define NUM_GPIO_COM0_PADS	(GPP_C23 - GPP_PECI + 1)

/*
 * +----------------------------+
 * |         Group A            |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    53   |
 * +------------------+---------+
 * | Pad End Number   |    77   |
 * +------------------+---------+
 */
#define GPP_A00			INC(GPP_C23)
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
#define GPP_A18			INC(GPP_A17)
#define GPP_A19			INC(GPP_A18)
#define GPP_A20			INC(GPP_A19)
#define GPP_A21			INC(GPP_A20)
#define GPP_A22			INC(GPP_A21)
#define GPP_A23			INC(GPP_A22)
#define GPP_ESPI_CLK_LPBK	INC(GPP_A23)

/*
 * +----------------------------+
 * |         Group E            |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    78   |
 * +------------------+---------+
 * | Pad End Number   |    102  |
 * +------------------+---------+
 */
#define GPP_E00			INC(GPP_ESPI_CLK_LPBK)
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
#define GPP_E23			INC(GPP_E22)
#define GPP_THC0_GSPI_CLK_LPBK	INC(GPP_E23)

#define GPIO_COM1_START		GPP_A00
#define GPIO_COM1_END		GPP_THC0_GSPI_CLK_LPBK
#define NUM_GPIO_COM1_PADS	(GPP_THC0_GSPI_CLK_LPBK - GPP_A00 + 1)

/*
 * +----------------------------+
 * |         Group H            |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    103  |
 * +------------------+---------+
 * | Pad End Number   |    128  |
 * +------------------+---------+
 */
#define GPP_H00			INC(GPP_THC0_GSPI_CLK_LPBK)
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
#define GPP_LPI3C1_CLK_LPBK	INC(GPP_H23)
#define GPP_LPI3C0_CLK_LPBK	INC(GPP_LPI3C1_CLK_LPBK)

/*
 * +----------------------------+
 * |         Group F            |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    129  |
 * +------------------+---------+
 * | Pad End Number   |    154  |
 * +------------------+---------+
 */
#define GPP_F00			INC(GPP_LPI3C0_CLK_LPBK)
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
#define GPP_GSPI0A_CLK_LOOPBK	INC(GPP_THC1_GSPI1_CLK_LPBK)

/*
 * +----------------------------+
 * |        Group SPI0          |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    155  |
 * +------------------+---------+
 * | Pad End Number   |    169  |
 * +------------------+---------+
 */
#define GPP_SPI0_IO_2		INC(GPP_GSPI0A_CLK_LOOPBK)
#define GPP_SPI0_IO_3		INC(GPP_SPI0_IO_2)
#define GPP_SPI0_MOSI_IO_0	INC(GPP_SPI0_IO_3)
#define GPP_SPI0_MOSI_IO_1	INC(GPP_SPI0_MOSI_IO_0)
#define GPP_SPI0_TPM_CS_B	INC(GPP_SPI0_MOSI_IO_1)
#define GPP_SPI0_FLASH_0_CS_B	INC(GPP_SPI0_TPM_CS_B)
#define GPP_SPI0_FLASH_1_CS_B	INC(GPP_SPI0_FLASH_0_CS_B)
#define GPP_SPI0_CLK		INC(GPP_SPI0_FLASH_1_CS_B)
#define GPP_BKLTEN		INC(GPP_SPI0_CLK)
#define GPP_BKLTCTL		INC(GPP_BKLTEN)
#define GPP_VDDEN		INC(GPP_BKLTCTL)
#define GPP_SYS_PWROK		INC(GPP_VDDEN)
#define GPP_SYS_RESET_B		INC(GPP_SYS_PWROK)
#define GPP_MLK_RST_B		INC(GPP_SYS_RESET_B)
#define GPP_SPI0_CLK_LOOPBK	INC(GPP_MLK_RST_B)

/*
 * +----------------------------+
 * |        Group VGPIO3        |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    170  |
 * +------------------+---------+
 * | Pad End Number   |    183  |
 * +------------------+---------+
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

#define GPIO_COM3_START		GPP_H00
#define GPIO_COM3_END		GPP_VGPIO3_THC3
#define NUM_GPIO_COM3_PADS	(GPP_VGPIO3_THC3 - GPP_H00 + 1)

/*
 * +----------------------------+
 * |         Group S            |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    184  |
 * +------------------+---------+
 * | Pad End Number   |    191  |
 * +------------------+---------+
 */
#define GPP_S00			INC(GPP_VGPIO3_THC3)
#define GPP_S01			INC(GPP_S00)
#define GPP_S02			INC(GPP_S01)
#define GPP_S03			INC(GPP_S02)
#define GPP_S04			INC(GPP_S03)
#define GPP_S05			INC(GPP_S04)
#define GPP_S06			INC(GPP_S05)
#define GPP_S07			INC(GPP_S06)

/*
 * +----------------------------+
 * |        Group JTAG          |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    192  |
 * +------------------+---------+
 * | Pad End Number   |    203  |
 * +------------------+---------+
 */
#define GPP_JTAG_MBPB0		INC(GPP_S07)
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

#define GPIO_COM4_START		GPP_S00
#define GPIO_COM4_END		GPP_JTAG_TRST_B
#define NUM_GPIO_COM4_PADS	(GPP_JTAG_TRST_B - GPP_S00 + 1)

/*
 * +----------------------------+
 * |         Group B            |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    204  |
 * +------------------+---------+
 * | Pad End Number   |    228  |
 * +------------------+---------+
 */
#define GPP_B00			INC(GPP_JTAG_TRST_B)
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
#define GPP_ACI3C0_CLK_LPBK	INC(GPP_B23)

/*
 * +----------------------------+
 * |         Group D            |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    229  |
 * +------------------+---------+
 * | Pad End Number   |    253  |
 * +------------------+---------+
 */
#define GPP_D00			INC(GPP_ACI3C0_CLK_LPBK)
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
#define GPP_BOOTHALT_B		INC(GPP_D23)

/*
 * +----------------------------+
 * |       Group VGPIO          |
 * +------------------+---------+
 * |                  | MTL-M/P |
 * +------------------+---------+
 * | Pad Start Number |    254  |
 * +------------------+---------+
 * | Pad End Number   |    288  |
 * +------------------+---------+
 */
#define GPP_VGPIO00		INC(GPP_BOOTHALT_B)
#define GPP_VGPIO04		INC(GPP_VGPIO00)
#define GPP_VGPIO05		INC(GPP_VGPIO04)
#define GPP_VGPIO06		INC(GPP_VGPIO05)
#define GPP_VGPIO07		INC(GPP_VGPIO06)
#define GPP_VGPIO08		INC(GPP_VGPIO07)
#define GPP_VGPIO09		INC(GPP_VGPIO08)
#define GPP_VGPIO10		INC(GPP_VGPIO09)
#define GPP_VGPIO11		INC(GPP_VGPIO10)
#define GPP_VGPIO12		INC(GPP_VGPIO11)
#define GPP_VGPIO13		INC(GPP_VGPIO12)
#define GPP_VGPIO18		INC(GPP_VGPIO13)
#define GPP_VGPIO19		INC(GPP_VGPIO18)
#define GPP_VGPIO20		INC(GPP_VGPIO19)
#define GPP_VGPIO21		INC(GPP_VGPIO20)
#define GPP_VGPIO22		INC(GPP_VGPIO21)
#define GPP_VGPIO23		INC(GPP_VGPIO22)
#define GPP_VGPIO24		INC(GPP_VGPIO23)
#define GPP_VGPIO25		INC(GPP_VGPIO24)
#define GPP_VGPIO30		INC(GPP_VGPIO25)
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
#define	GPP_VGPIO43		INC(GPP_VGPIO42)
#define GPP_VGPIO44		INC(GPP_VGPIO43)
#define GPP_VGPIO45		INC(GPP_VGPIO44)
#define GPP_VGPIO46		INC(GPP_VGPIO45)
#define GPP_VGPIO47		INC(GPP_VGPIO46)

#define GPIO_COM5_START		GPP_B00
#define GPIO_COM5_END		GPP_VGPIO47
#define NUM_GPIO_COM5_PADS	(GPP_VGPIO47 - GPP_B00 + 1)

#define TOTAL_GPIO_COMM		(COMM_5 + 1)
#define TOTAL_PADS		(GPIO_COM5_END + 1)

#endif
