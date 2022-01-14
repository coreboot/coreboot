/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _SOC_ALDERLAKE_GPIO_SOC_DEFS_H_
#define _SOC_ALDERLAKE_GPIO_SOC_DEFS_H_

#define INC(x) ((x) + 1)
/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 *
 * Refer to Alder Lake PCH EDS Chapter 27, MISCCFG register offset 0x10
 * for each GPIO community to get GPIO group to GPE_DWx assignment.
 */
#define GPP_B			0x0
#define GPP_T			INC(GPP_B)
#define GPP_A			INC(GPP_T)
#define GPP_R			INC(GPP_A)
#define GPD			INC(GPP_R)
#define GPP_S			INC(GPD)
#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
#define GPP_I			INC(GPP_S)
#define GPP_H			INC(GPP_I)
#else
#define GPP_H			INC(GPP_S)
#endif
#define GPP_D			INC(GPP_H)
#define GPP_F			0xA
#define GPP_C			INC(GPP_F)
#define GPP_E			INC(GPP_C)

#define GPIO_MAX_NUM_PER_GROUP	26

#define COMM_0			0
#define COMM_1			INC(COMM_0)
#define COMM_2			INC(COMM_1)
#define COMM_3			INC(COMM_2)
#define COMM_4			INC(COMM_3)
#define COMM_5			INC(COMM_4)
/*
 * GPIOs are ordered monotonically increasing to match ACPI/OS driver.
 */
/*
 * +------------------------------------+
 * |               Group B              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |    0    |   0   |
 * +------------------+---------+-------+
 * | Pad End Number   |    25   |   25  |
 * +------------------+---------+-------+
 */
#define GPP_B0			0
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
#define GPP_B24			INC(GPP_B23) /* GSPI0_CLK_LOOPBK */
#define GPP_B25			INC(GPP_B24) /* GSPI1_CLK_LOOPBK */

/*
 * +------------------------------------+
 * |               Group T              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |    26   |   26  |
 * +------------------+---------+-------+
 * | Pad End Number   |    41   |   41  |
 * +------------------+---------+-------+
 */
#define GPP_T0			INC(GPP_B25)
#define GPP_T1			INC(GPP_T0)
#define GPP_T2			INC(GPP_T1)
#define GPP_T3			INC(GPP_T2)
#define GPP_T4			INC(GPP_T3)
#define GPP_T5			INC(GPP_T4)
#define GPP_T6			INC(GPP_T5)
#define GPP_T7			INC(GPP_T6)
#define GPP_T8			INC(GPP_T7)
#define GPP_T9			INC(GPP_T8)
#define GPP_T10			INC(GPP_T9)
#define GPP_T11			INC(GPP_T10)
#define GPP_T12			INC(GPP_T11)
#define GPP_T13			INC(GPP_T12)
#define GPP_T14			INC(GPP_T13)
#define GPP_T15			INC(GPP_T14)

/*
 * +------------------------------------+
 * |               Group A              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |    42   |   42  |
 * +------------------+---------+-------+
 * | Pad End Number   |    66   |   66  |
 * +------------------+---------+-------+
 */
#define GPP_A0			INC(GPP_T15)
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
#define GPP_ESPI_CLK_LOOPBK	INC(GPP_A23)

#define GPIO_COM0_START		GPP_B0
#define GPIO_COM0_END		GPP_ESPI_CLK_LOOPBK
#define NUM_GPIO_COM0_PADS	(GPIO_COM0_END - GPIO_COM0_START + 1)

/*
 * +------------------------------------+
 * |               Group S              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |    67   |   67  |
 * +------------------+---------+-------+
 * | Pad End Number   |    74   |   74  |
 * +------------------+---------+-------+
 */
#define GPP_S0			INC(GPP_ESPI_CLK_LOOPBK)
#define GPP_S1			INC(GPP_S0)
#define GPP_S2			INC(GPP_S1)
#define GPP_S3			INC(GPP_S2)
#define GPP_S4			INC(GPP_S3)
#define GPP_S5			INC(GPP_S4)
#define GPP_S6			INC(GPP_S5)
#define GPP_S7			INC(GPP_S6)

#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
/*
 * +------------------------------------+
 * |               Group I              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |    NA   |   75  |
 * +------------------+---------+-------+
 * | Pad End Number   |    NA   |   94  |
 * +------------------+---------+-------+
 */
#define GPP_I0			INC(GPP_S7)
#define GPP_I1			INC(GPP_I0)
#define GPP_I2			INC(GPP_I1)
#define GPP_I3			INC(GPP_I2)
#define GPP_I4			INC(GPP_I3)
#define GPP_I5			INC(GPP_I4)
#define GPP_I6			INC(GPP_I5)
#define GPP_I7			INC(GPP_I6)
#define GPP_I8			INC(GPP_I7)
#define GPP_I9			INC(GPP_I8)
#define GPP_I10			INC(GPP_I9)
#define GPP_I11			INC(GPP_I10)
#define GPP_I12			INC(GPP_I11)
#define GPP_I13			INC(GPP_I12)
#define GPP_I14			INC(GPP_I13)
#define GPP_I15			INC(GPP_I14)
#define GPP_I16			INC(GPP_I15)
#define GPP_I17			INC(GPP_I16)
#define GPP_I18			INC(GPP_I17)
#define GPP_I19			INC(GPP_I18)

/*
 * +------------------------------------+
 * |               Group H              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |    75   |   95  |
 * +------------------+---------+-------+
 * | Pad End Number   |    98   |  118  |
 * +------------------+---------+-------+
 */
#define GPP_H0			INC(GPP_I19)
#else
#define GPP_H0			INC(GPP_S7)
#endif
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

/*
 * +------------------------------------+
 * |               Group D              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |    99   |  119  |
 * +------------------+---------+-------+
 * | Pad End Number   |   119   |  139  |
 * +------------------+---------+-------+
 */
#define GPP_D0			INC(GPP_H23)
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
#define GPP_GSPI2_CLK_LOOPBK	INC(GPP_D19)

#if !CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
/*
 * +------------------------------------+
 * |           Reserved GPIOs           |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |   120   |   NA  |
 * +------------------+---------+-------+
 * | Pad End Number   |   143   |   NA  |
 * +------------------+---------+-------+
 */
#define GPP_CPU_RSVD_1		INC(GPP_GSPI2_CLK_LOOPBK)
#define GPP_CPU_RSVD_2		INC(GPP_CPU_RSVD_1)
#define GPP_CPU_RSVD_3		INC(GPP_CPU_RSVD_2)
#define GPP_CPU_RSVD_4		INC(GPP_CPU_RSVD_3)
#define GPP_CPU_RSVD_5		INC(GPP_CPU_RSVD_4)
#define GPP_CPU_RSVD_6		INC(GPP_CPU_RSVD_5)
#define GPP_CPU_RSVD_7		INC(GPP_CPU_RSVD_6)
#define GPP_CPU_RSVD_8		INC(GPP_CPU_RSVD_7)
#define GPP_CPU_RSVD_9		INC(GPP_CPU_RSVD_8)
#define GPP_CPU_RSVD_10		INC(GPP_CPU_RSVD_9)
#define GPP_CPU_RSVD_11		INC(GPP_CPU_RSVD_10)
#define GPP_CPU_RSVD_12		INC(GPP_CPU_RSVD_11)
#define GPP_CPU_RSVD_13		INC(GPP_CPU_RSVD_12)
#define GPP_CPU_RSVD_14		INC(GPP_CPU_RSVD_13)
#define GPP_CPU_RSVD_15		INC(GPP_CPU_RSVD_14)
#define GPP_CPU_RSVD_16		INC(GPP_CPU_RSVD_15)
#define GPP_CPU_RSVD_17		INC(GPP_CPU_RSVD_16)
#define GPP_CPU_RSVD_18		INC(GPP_CPU_RSVD_17)
#define GPP_CPU_RSVD_19		INC(GPP_CPU_RSVD_18)
#define GPP_CPU_RSVD_20		INC(GPP_CPU_RSVD_19)
#define GPP_CPU_RSVD_21		INC(GPP_CPU_RSVD_20)
#define GPP_CPU_RSVD_22		INC(GPP_CPU_RSVD_21)
#define GPP_CPU_RSVD_23		INC(GPP_CPU_RSVD_22)
#define GPP_CPU_RSVD_24		INC(GPP_CPU_RSVD_23)

/*
 * +------------------------------------+
 * |             Group VGPIO            |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |   144   |  140  |
 * +------------------+---------+-------+
 * | Pad End Number   |   170   |  168  |
 * +------------------+---------+-------+
 */
#define GPP_VGPIO_0		INC(GPP_CPU_RSVD_24)
#else
#define GPP_VGPIO_0		INC(GPP_GSPI2_CLK_LOOPBK)
#endif
#define GPP_VGPIO_4		INC(GPP_VGPIO_0)
#define GPP_VGPIO_5		INC(GPP_VGPIO_4)
#define GPP_VGPIO_6		INC(GPP_VGPIO_5)
#define GPP_VGPIO_7		INC(GPP_VGPIO_6)
#define GPP_VGPIO_8		INC(GPP_VGPIO_7)
#define GPP_VGPIO_9		INC(GPP_VGPIO_8)
#define GPP_VGPIO_10		INC(GPP_VGPIO_9)
#define GPP_VGPIO_11		INC(GPP_VGPIO_10)
#define GPP_VGPIO_12		INC(GPP_VGPIO_11)
#define GPP_VGPIO_13		INC(GPP_VGPIO_12)
#define GPP_VGPIO_18		INC(GPP_VGPIO_13)
#define GPP_VGPIO_19		INC(GPP_VGPIO_18)
#define GPP_VGPIO_20		INC(GPP_VGPIO_19)
#define GPP_VGPIO_21		INC(GPP_VGPIO_20)
#define GPP_VGPIO_22		INC(GPP_VGPIO_21)
#define GPP_VGPIO_23		INC(GPP_VGPIO_22)
#define GPP_VGPIO_24		INC(GPP_VGPIO_23)
#define GPP_VGPIO_25		INC(GPP_VGPIO_24)
#define GPP_VGPIO_30		INC(GPP_VGPIO_25)
#define GPP_VGPIO_31		INC(GPP_VGPIO_30)
#define GPP_VGPIO_32		INC(GPP_VGPIO_31)
#define GPP_VGPIO_33		INC(GPP_VGPIO_32)
#define GPP_VGPIO_34		INC(GPP_VGPIO_33)
#define GPP_VGPIO_35		INC(GPP_VGPIO_34)
#define GPP_VGPIO_36		INC(GPP_VGPIO_35)
#define GPP_VGPIO_37		INC(GPP_VGPIO_36)

#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
#define GPP_VGPIO_THC0		INC(GPP_VGPIO_37)
#define GPP_VGPIO_THC1		INC(GPP_VGPIO_THC0)
#endif

#define GPIO_COM1_START		GPP_S0
#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
#define GPIO_COM1_END		GPP_VGPIO_THC1
#else
#define GPIO_COM1_END		GPP_VGPIO_37
#endif
#define NUM_GPIO_COM1_PADS	(GPIO_COM1_END - GPIO_COM1_START + 1)

#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
/*
 * +------------------------------------+
 * |              Group GPD             |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |   171   |  169  |
 * +------------------+---------+-------+
 * | Pad End Number   |   187   |  185  |
 * +------------------+---------+-------+
 */
#define GPD0			INC(GPP_VGPIO_THC1)
#else
#define GPD0			INC(GPP_VGPIO_37)
#endif
#define GPD1			INC(GPD0)
#define GPD2			INC(GPD1)
#define GPD3			INC(GPD2)
#define GPD4			INC(GPD3)
#define GPD5			INC(GPD4)
#define GPD6			INC(GPD5)
#define GPD7			INC(GPD6)
#define GPD8			INC(GPD7)
#define GPD9			INC(GPD8)
#define GPD10			INC(GPD9)
#define GPD11			INC(GPD10)
#define GPD_INPUT3VSEL		INC(GPD11)
#define GPD_SLP_LANB		INC(GPD_INPUT3VSEL)
#define GPD_SLP_SUSB		INC(GPD_SLP_LANB)
#define GPD_WAKEB		INC(GPD_SLP_SUSB)
#define GPD_DRAM_RESETB		INC(GPD_WAKEB)

#define GPIO_COM2_START		GPD0
#define GPIO_COM2_END		GPD_DRAM_RESETB
#define NUM_GPIO_COM2_PADS	(GPIO_COM2_END - GPIO_COM2_START + 1)

/*
 * +------------------------------------+
 * |          PCIE VGPIO Group          |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |   188   |  186  |
 * +------------------+---------+-------+
 * | Pad End Number   |   294   |  292  |
 * +------------------+---------+-------+
 */
#define GPP_CPU_RSVD_25		INC(GPD_DRAM_RESETB)
#define GPP_CPU_RSVD_26		INC(GPP_CPU_RSVD_25)
#define GPP_CPU_RSVD_27		INC(GPP_CPU_RSVD_26)
#define GPP_CPU_RSVD_28		INC(GPP_CPU_RSVD_27)
#define GPP_CPU_RSVD_29		INC(GPP_CPU_RSVD_28)
#define GPP_CPU_RSVD_30		INC(GPP_CPU_RSVD_29)
#define GPP_CPU_RSVD_31		INC(GPP_CPU_RSVD_30)
#define GPP_CPU_RSVD_32		INC(GPP_CPU_RSVD_31)
#define GPP_CPU_RSVD_33		INC(GPP_CPU_RSVD_32)
#define GPP_CPU_RSVD_34		INC(GPP_CPU_RSVD_33)
#define GPP_CPU_RSVD_35		INC(GPP_CPU_RSVD_34)
#define GPP_CPU_RSVD_36		INC(GPP_CPU_RSVD_35)
#define GPP_CPU_RSVD_37		INC(GPP_CPU_RSVD_36)
#define GPP_CPU_RSVD_38		INC(GPP_CPU_RSVD_37)
#define GPP_CPU_RSVD_39		INC(GPP_CPU_RSVD_38)
#define GPP_vGPIO_PCIE_0	INC(GPP_CPU_RSVD_39)
#define GPP_vGPIO_PCIE_1	INC(GPP_vGPIO_PCIE_0)
#define GPP_vGPIO_PCIE_2	INC(GPP_vGPIO_PCIE_1)
#define GPP_vGPIO_PCIE_3	INC(GPP_vGPIO_PCIE_2)
#define GPP_vGPIO_PCIE_4	INC(GPP_vGPIO_PCIE_3)
#define GPP_vGPIO_PCIE_5	INC(GPP_vGPIO_PCIE_4)
#define GPP_vGPIO_PCIE_6	INC(GPP_vGPIO_PCIE_5)
#define GPP_vGPIO_PCIE_7	INC(GPP_vGPIO_PCIE_6)
#define GPP_vGPIO_PCIE_8	INC(GPP_vGPIO_PCIE_7)
#define GPP_vGPIO_PCIE_9	INC(GPP_vGPIO_PCIE_8)
#define GPP_vGPIO_PCIE_10	INC(GPP_vGPIO_PCIE_9)
#define GPP_vGPIO_PCIE_11	INC(GPP_vGPIO_PCIE_10)
#define GPP_vGPIO_PCIE_12	INC(GPP_vGPIO_PCIE_11)
#define GPP_vGPIO_PCIE_13	INC(GPP_vGPIO_PCIE_12)
#define GPP_vGPIO_PCIE_14	INC(GPP_vGPIO_PCIE_13)
#define GPP_vGPIO_PCIE_15	INC(GPP_vGPIO_PCIE_14)
#define GPP_vGPIO_PCIE_16	INC(GPP_vGPIO_PCIE_15)
#define GPP_vGPIO_PCIE_17	INC(GPP_vGPIO_PCIE_16)
#define GPP_vGPIO_PCIE_18	INC(GPP_vGPIO_PCIE_17)
#define GPP_vGPIO_PCIE_19	INC(GPP_vGPIO_PCIE_18)
#define GPP_vGPIO_PCIE_20	INC(GPP_vGPIO_PCIE_19)
#define GPP_vGPIO_PCIE_21	INC(GPP_vGPIO_PCIE_20)
#define GPP_vGPIO_PCIE_22	INC(GPP_vGPIO_PCIE_21)
#define GPP_vGPIO_PCIE_23	INC(GPP_vGPIO_PCIE_22)
#define GPP_vGPIO_PCIE_24	INC(GPP_vGPIO_PCIE_23)
#define GPP_vGPIO_PCIE_25	INC(GPP_vGPIO_PCIE_24)
#define GPP_vGPIO_PCIE_26	INC(GPP_vGPIO_PCIE_25)
#define GPP_vGPIO_PCIE_27	INC(GPP_vGPIO_PCIE_26)
#define GPP_vGPIO_PCIE_28	INC(GPP_vGPIO_PCIE_27)
#define GPP_vGPIO_PCIE_29	INC(GPP_vGPIO_PCIE_28)
#define GPP_vGPIO_PCIE_30	INC(GPP_vGPIO_PCIE_29)
#define GPP_vGPIO_PCIE_31	INC(GPP_vGPIO_PCIE_30)
#define GPP_vGPIO_PCIE_32	INC(GPP_vGPIO_PCIE_31)
#define GPP_vGPIO_PCIE_33	INC(GPP_vGPIO_PCIE_32)
#define GPP_vGPIO_PCIE_34	INC(GPP_vGPIO_PCIE_33)
#define GPP_vGPIO_PCIE_35	INC(GPP_vGPIO_PCIE_34)
#define GPP_vGPIO_PCIE_36	INC(GPP_vGPIO_PCIE_35)
#define GPP_vGPIO_PCIE_37	INC(GPP_vGPIO_PCIE_36)
#define GPP_vGPIO_PCIE_38	INC(GPP_vGPIO_PCIE_37)
#define GPP_vGPIO_PCIE_39	INC(GPP_vGPIO_PCIE_38)
#define GPP_vGPIO_PCIE_40	INC(GPP_vGPIO_PCIE_39)
#define GPP_vGPIO_PCIE_41	INC(GPP_vGPIO_PCIE_40)
#define GPP_vGPIO_PCIE_42	INC(GPP_vGPIO_PCIE_41)
#define GPP_vGPIO_PCIE_43	INC(GPP_vGPIO_PCIE_42)
#define GPP_vGPIO_PCIE_44	INC(GPP_vGPIO_PCIE_43)
#define GPP_vGPIO_PCIE_45	INC(GPP_vGPIO_PCIE_44)
#define GPP_vGPIO_PCIE_46	INC(GPP_vGPIO_PCIE_45)
#define GPP_vGPIO_PCIE_47	INC(GPP_vGPIO_PCIE_46)
#define GPP_vGPIO_PCIE_48	INC(GPP_vGPIO_PCIE_47)
#define GPP_vGPIO_PCIE_49	INC(GPP_vGPIO_PCIE_48)
#define GPP_vGPIO_PCIE_50	INC(GPP_vGPIO_PCIE_49)
#define GPP_vGPIO_PCIE_51	INC(GPP_vGPIO_PCIE_50)
#define GPP_vGPIO_PCIE_52	INC(GPP_vGPIO_PCIE_51)
#define GPP_vGPIO_PCIE_53	INC(GPP_vGPIO_PCIE_52)
#define GPP_vGPIO_PCIE_54	INC(GPP_vGPIO_PCIE_53)
#define GPP_vGPIO_PCIE_55	INC(GPP_vGPIO_PCIE_54)
#define GPP_vGPIO_PCIE_56	INC(GPP_vGPIO_PCIE_55)
#define GPP_vGPIO_PCIE_57	INC(GPP_vGPIO_PCIE_56)
#define GPP_vGPIO_PCIE_58	INC(GPP_vGPIO_PCIE_57)
#define GPP_vGPIO_PCIE_59	INC(GPP_vGPIO_PCIE_58)
#define GPP_vGPIO_PCIE_60	INC(GPP_vGPIO_PCIE_59)
#define GPP_vGPIO_PCIE_61	INC(GPP_vGPIO_PCIE_60)
#define GPP_vGPIO_PCIE_62	INC(GPP_vGPIO_PCIE_61)
#define GPP_vGPIO_PCIE_63	INC(GPP_vGPIO_PCIE_62)
#define GPP_vGPIO_PCIE_64	INC(GPP_vGPIO_PCIE_63)
#define GPP_vGPIO_PCIE_65	INC(GPP_vGPIO_PCIE_64)
#define GPP_vGPIO_PCIE_66	INC(GPP_vGPIO_PCIE_65)
#define GPP_vGPIO_PCIE_67	INC(GPP_vGPIO_PCIE_66)
#define GPP_vGPIO_PCIE_68	INC(GPP_vGPIO_PCIE_67)
#define GPP_vGPIO_PCIE_69	INC(GPP_vGPIO_PCIE_68)
#define GPP_vGPIO_PCIE_70	INC(GPP_vGPIO_PCIE_69)
#define GPP_vGPIO_PCIE_71	INC(GPP_vGPIO_PCIE_70)
#define GPP_vGPIO_PCIE_72	INC(GPP_vGPIO_PCIE_71)
#define GPP_vGPIO_PCIE_73	INC(GPP_vGPIO_PCIE_72)
#define GPP_vGPIO_PCIE_74	INC(GPP_vGPIO_PCIE_73)
#define GPP_vGPIO_PCIE_75	INC(GPP_vGPIO_PCIE_74)
#define GPP_vGPIO_PCIE_76	INC(GPP_vGPIO_PCIE_75)
#define GPP_vGPIO_PCIE_77	INC(GPP_vGPIO_PCIE_76)
#define GPP_vGPIO_PCIE_78	INC(GPP_vGPIO_PCIE_77)
#define GPP_vGPIO_PCIE_79	INC(GPP_vGPIO_PCIE_78)
#define GPP_CPU_RSVD_40		INC(GPP_vGPIO_PCIE_79)
#define GPP_CPU_RSVD_41		INC(GPP_CPU_RSVD_40)
#define GPP_CPU_RSVD_42		INC(GPP_CPU_RSVD_41)
#define GPP_CPU_RSVD_43		INC(GPP_CPU_RSVD_42)
#define GPP_CPU_RSVD_44		INC(GPP_CPU_RSVD_43)
#define GPP_CPU_RSVD_45		INC(GPP_CPU_RSVD_44)
#define GPP_CPU_RSVD_46		INC(GPP_CPU_RSVD_45)
#define GPP_CPU_RSVD_47		INC(GPP_CPU_RSVD_46)
#define GPP_vGPIO_PCIE_80	INC(GPP_CPU_RSVD_47)
#define GPP_vGPIO_PCIE_81	INC(GPP_vGPIO_PCIE_80)
#define GPP_vGPIO_PCIE_82	INC(GPP_vGPIO_PCIE_81)
#define GPP_vGPIO_PCIE_83	INC(GPP_vGPIO_PCIE_82)

#define GPIO_COM3_START		GPP_CPU_RSVD_25
#define GPIO_COM3_END		GPP_vGPIO_PCIE_83
#define NUM_GPIO_COM3_PADS      (GPIO_COM3_END - GPIO_COM3_START + 1)

/*
 * +------------------------------------+
 * |               Group C              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |   295   |  293  |
 * +------------------+---------+-------+
 * | Pad End Number   |   318   |  316  |
 * +------------------+---------+-------+
 */
#define GPP_C0			INC(GPP_vGPIO_PCIE_83)
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

/*
 * +------------------------------------+
 * |               Group F              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |   319   |  317  |
 * +------------------+---------+-------+
 * | Pad End Number   |   343   |  341  |
 * +------------------+---------+-------+
 */
#define GPP_F0			INC(GPP_C23)
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
#define GPP_F_CLK_LOOPBK	INC(GPP_F23)

/*
 * +------------------------------------+
 * |            Group HVCMOS            |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |   344   |  342  |
 * +------------------+---------+-------+
 * | Pad End Number   |   349   |  347  |
 * +------------------+---------+-------+
 */
#define GPP_L_BKLTEN		INC(GPP_F_CLK_LOOPBK)
#define GPP_L_BKLTCTL		INC(GPP_L_BKLTEN)
#define GPP_L_VDDEN		INC(GPP_L_BKLTCTL)
#define GPP_SYS_PWROK		INC(GPP_L_VDDEN)
#define GPP_SYS_RESETB		INC(GPP_SYS_PWROK)
#define GPP_MLK_RSTB		INC(GPP_SYS_RESETB)

/*
 * +------------------------------------+
 * |               Group E              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |   350   |  348  |
 * +------------------+---------+-------+
 * | Pad End Number   |   374   |  372  |
 * +------------------+---------+-------+
 */
#define GPP_E0			INC(GPP_MLK_RSTB)
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
#define GPP_E_CLK_LOOPBK	INC(GPP_E23)

#define GPIO_COM4_START		GPP_C0
#define GPIO_COM4_END		GPP_E_CLK_LOOPBK
#define NUM_GPIO_COM4_PADS	(GPIO_COM4_END - GPIO_COM4_START + 1)

/*
 * +------------------------------------+
 * |               Group R              |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |   375   |  373  |
 * +------------------+---------+-------+
 * | Pad End Number   |   382   |  380  |
 * +------------------+---------+-------+
 */
#define GPP_R0			INC(GPP_E_CLK_LOOPBK)
#define GPP_R1			INC(GPP_R0)
#define GPP_R2			INC(GPP_R1)
#define GPP_R3			INC(GPP_R2)
#define GPP_R4			INC(GPP_R3)
#define GPP_R5			INC(GPP_R4)
#define GPP_R6			INC(GPP_R5)
#define GPP_R7			INC(GPP_R6)

/*
 * +------------------------------------+
 * |             Group SPI0             |
 * +------------------+---------+-------+
 * |                  | ADL-P/M | ADL-N |
 * +------------------+---------+-------+
 * | Pad Start Number |   383   |  381  |
 * +------------------+---------+-------+
 * | Pad End Number   |   390   |  388  |
 * +------------------+---------+-------+
 */
#define GPP_SPI0_IO_2		INC(GPP_R7)
#define GPP_SPI0_IO_3		INC(GPP_SPI0_IO_2)
#define GPP_SPI0_MOSI_IO_0	INC(GPP_SPI0_IO_3)
#define GPP_SPI0_MOSI_IO_1	INC(GPP_SPI0_MOSI_IO_0)
#define GPP_SPI0_TPM_CSB	INC(GPP_SPI0_MOSI_IO_1)
#define GPP_SPI0_FLASH_0_CSB	INC(GPP_SPI0_TPM_CSB)
#define GPP_SPI0_FLASH_1_CSB	INC(GPP_SPI0_FLASH_0_CSB)
#define GPP_SPI0_CLK		INC(GPP_SPI0_FLASH_1_CSB)

#define GPIO_COM5_START		GPP_R0
#define GPIO_COM5_END		GPP_SPI0_CLK
#define NUM_GPIO_COM5_PADS	(GPIO_COM5_END - GPIO_COM5_START + 1)

#define TOTAL_GPIO_COMM		(COMM_5 + 1)
#define TOTAL_PADS		(GPIO_COM5_END + 1)

#endif
