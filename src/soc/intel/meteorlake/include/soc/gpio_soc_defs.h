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
#define GPP_V0			INC(GPP_VIDALERT_B)
#define GPP_V1			INC(GPP_V0)
#define GPP_V2			INC(GPP_V1)
#define GPP_V3			INC(GPP_V2)
#define GPP_V4			INC(GPP_V3)
#define GPP_V5			INC(GPP_V4)
#define GPP_V6			INC(GPP_V5)
#define GPP_V7			INC(GPP_V6)
#define GPP_V8			INC(GPP_V7)
#define GPP_V9			INC(GPP_V8)
#define GPP_V10			INC(GPP_V9)
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
#define GPP_C0			INC(GPP_V23)
#define GPP_C1			INC(GPP_C0)
#define GPP_C2			INC(GPP_C1)
#define GPP_C3			INC(GPP_C2)
#define GPP_C4			INC(GPP_C3)
#define GPP_C5			INC(GPP_C4)
#define GPP_C6			INC(GPP_C5)
#define GPP_C7			INC(GPP_C6)
#define GPP_C8			INC(GPP_C7)
#define GPP_C9			INC(GPP_C8)
#define GPP_C10			INC(GPP_C9)
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
#define GPP_A0			INC(GPP_C23)
#define GPP_A1			INC(GPP_A0)
#define GPP_A2			INC(GPP_A1)
#define GPP_A3			INC(GPP_A2)
#define GPP_A4			INC(GPP_A3)
#define GPP_A5			INC(GPP_A4)
#define GPP_A6			INC(GPP_A5)
#define GPP_A7			INC(GPP_A6)
#define GPP_A8			INC(GPP_A7)
#define GPP_A9			INC(GPP_A8)
#define GPP_A10			INC(GPP_A9)
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
#define GPP_E0			INC(GPP_ESPI_CLK_LPBK)
#define GPP_E1			INC(GPP_E0)
#define GPP_E2			INC(GPP_E1)
#define GPP_E3			INC(GPP_E2)
#define GPP_E4			INC(GPP_E3)
#define GPP_E5			INC(GPP_E4)
#define GPP_E6			INC(GPP_E5)
#define GPP_E7			INC(GPP_E6)
#define GPP_E8			INC(GPP_E7)
#define GPP_E9			INC(GPP_E8)
#define GPP_E10			INC(GPP_E9)
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

#define GPIO_COM1_START		GPP_A0
#define GPIO_COM1_END		GPP_THC0_GSPI_CLK_LPBK
#define NUM_GPIO_COM1_PADS	(GPP_THC0_GSPI_CLK_LPBK - GPP_A0 + 1)

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
#define GPP_H0			INC(GPP_THC0_GSPI_CLK_LPBK)
#define GPP_H1			INC(GPP_H0)
#define GPP_H2			INC(GPP_H1)
#define GPP_H3			INC(GPP_H2)
#define GPP_H4			INC(GPP_H3)
#define GPP_H5			INC(GPP_H4)
#define GPP_H6			INC(GPP_H5)
#define GPP_H7			INC(GPP_H6)
#define GPP_H8			INC(GPP_H7)
#define GPP_H9			INC(GPP_H8)
#define GPP_H10			INC(GPP_H9)
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
#define GPP_F0			INC(GPP_LPI3C0_CLK_LPBK)
#define GPP_F1			INC(GPP_F0)
#define GPP_F2			INC(GPP_F1)
#define GPP_F3			INC(GPP_F2)
#define GPP_F4			INC(GPP_F3)
#define GPP_F5			INC(GPP_F4)
#define GPP_F6			INC(GPP_F5)
#define GPP_F7			INC(GPP_F6)
#define GPP_F8			INC(GPP_F7)
#define GPP_F9			INC(GPP_F8)
#define GPP_F10			INC(GPP_F9)
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

#define GPIO_COM3_START		GPP_H0
#define GPIO_COM3_END		GPP_VGPIO3_THC3
#define NUM_GPIO_COM3_PADS	(GPP_VGPIO3_THC3 - GPP_H0 + 1)

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
#define GPP_S0			INC(GPP_VGPIO3_THC3)
#define GPP_S1			INC(GPP_S0)
#define GPP_S2			INC(GPP_S1)
#define GPP_S3			INC(GPP_S2)
#define GPP_S4			INC(GPP_S3)
#define GPP_S5			INC(GPP_S4)
#define GPP_S6			INC(GPP_S5)
#define GPP_S7			INC(GPP_S6)

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
#define GPP_JTAG_MBPB0		INC(GPP_S7)
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

#define GPIO_COM4_START		GPP_S0
#define GPIO_COM4_END		GPP_JTAG_TRST_B
#define NUM_GPIO_COM4_PADS	(GPP_JTAG_TRST_B - GPP_S0 + 1)

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
#define GPP_B0			INC(GPP_JTAG_TRST_B)
#define GPP_B1			INC(GPP_B0)
#define GPP_B2			INC(GPP_B1)
#define GPP_B3			INC(GPP_B2)
#define GPP_B4			INC(GPP_B3)
#define GPP_B5			INC(GPP_B4)
#define GPP_B6			INC(GPP_B5)
#define GPP_B7			INC(GPP_B6)
#define GPP_B8			INC(GPP_B7)
#define GPP_B9			INC(GPP_B8)
#define GPP_B10			INC(GPP_B9)
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
#define GPP_D0			INC(GPP_ACI3C0_CLK_LPBK)
#define GPP_D1			INC(GPP_D0)
#define GPP_D2			INC(GPP_D1)
#define GPP_D3			INC(GPP_D2)
#define GPP_D4			INC(GPP_D3)
#define GPP_D5			INC(GPP_D4)
#define GPP_D6			INC(GPP_D5)
#define GPP_D7			INC(GPP_D6)
#define GPP_D8			INC(GPP_D7)
#define GPP_D9			INC(GPP_D8)
#define GPP_D10			INC(GPP_D9)
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
#define GPP_VGPIO0		INC(GPP_BOOTHALT_B)
#define GPP_VGPIO4		INC(GPP_VGPIO0)
#define GPP_VGPIO5		INC(GPP_VGPIO4)
#define GPP_VGPIO6		INC(GPP_VGPIO5)
#define GPP_VGPIO7		INC(GPP_VGPIO6)
#define GPP_VGPIO8		INC(GPP_VGPIO7)
#define GPP_VGPIO9		INC(GPP_VGPIO8)
#define GPP_VGPIO10		INC(GPP_VGPIO9)
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

#define GPIO_COM5_START		GPP_B0
#define GPIO_COM5_END		GPP_VGPIO47
#define NUM_GPIO_COM5_PADS	(GPP_VGPIO47 - GPP_B0 + 1)

#define TOTAL_GPIO_COMM		(COMM_5 + 1)
#define TOTAL_PADS		(GPIO_COM5_END + 1)

#endif
