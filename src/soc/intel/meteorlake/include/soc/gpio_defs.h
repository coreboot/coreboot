/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_METEORLAKE_GPIO_DEFS_H_
#define _SOC_METEORLAKE_GPIO_DEFS_H_

#ifndef __ACPI__
#include <stddef.h>
#endif
#include <soc/gpio_soc_defs.h>

#define GPIO_NUM_PAD_CFG_REGS	4 /* DW0, DW1, DW2, DW3 */

#define NUM_GPIO_COMx_GPI_REGS(n)	\
		(ALIGN_UP((n), GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define NUM_GPIO_COM0_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM0_PADS)
#define NUM_GPIO_COM1_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM1_PADS)
#define NUM_GPIO_COM3_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM3_PADS)
#define NUM_GPIO_COM4_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM4_PADS)
#define NUM_GPIO_COM5_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM5_PADS)

#define NUM_GPI_STATUS_REGS	\
		((NUM_GPIO_COM0_GPI_REGS) +\
		(NUM_GPIO_COM1_GPI_REGS) +\
		(NUM_GPIO_COM3_GPI_REGS) +\
		(NUM_GPIO_COM4_GPI_REGS) +\
		(NUM_GPIO_COM5_GPI_REGS))

#define PAD_CFG_LOCK_OFFSET			0x110

/*
 * IOxAPIC IRQs for the GPIOs
 */

/* Group V */
#define GPP_V00_IRQ				0x18
#define GPP_V01_IRQ				0x19
#define GPP_V02_IRQ				0x1A
#define GPP_V03_IRQ				0x1B
#define GPP_V04_IRQ				0x1C
#define GPP_V05_IRQ				0x1D
#define GPP_V06_IRQ				0x1E
#define GPP_V07_IRQ				0x1F
#define GPP_V08_IRQ				0x20
#define GPP_V09_IRQ				0x21
#define GPP_V10_IRQ				0x22
#define GPP_V11_IRQ				0x23
#define GPP_V12_IRQ				0x24
#define GPP_V13_IRQ				0x25
#define GPP_V14_IRQ				0x26
#define GPP_V15_IRQ				0x27
#define GPP_V16_IRQ				0x28
#define GPP_V17_IRQ				0x29
#define GPP_V18_IRQ				0x2A
#define GPP_V19_IRQ				0x2B
#define GPP_V20_IRQ				0x2C
#define GPP_V21_IRQ				0x2D
#define GPP_V22_IRQ				0x2E
#define GPP_V23_IRQ				0x2F

/* Group C */
#define GPP_C00_IRQ				0x30
#define GPP_C01_IRQ				0x31
#define GPP_C02_IRQ				0x32
#define GPP_C03_IRQ				0x33
#define GPP_C04_IRQ				0x34
#define GPP_C05_IRQ				0x35
#define GPP_C06_IRQ				0x36
#define GPP_C07_IRQ				0x37
#define GPP_C08_IRQ				0x38
#define GPP_C09_IRQ				0x39
#define GPP_C10_IRQ				0x3A
#define GPP_C11_IRQ				0x3B
#define GPP_C12_IRQ				0x3C
#define GPP_C13_IRQ				0x3D
#define GPP_C14_IRQ				0x3E
#define GPP_C15_IRQ				0x3F
#define GPP_C16_IRQ				0x40
#define GPP_C17_IRQ				0x41
#define GPP_C18_IRQ				0x42
#define GPP_C19_IRQ				0x43
#define GPP_C20_IRQ				0x44
#define GPP_C21_IRQ				0x45
#define GPP_C22_IRQ				0x46
#define GPP_C23_IRQ				0x47

/* Group A */
#define GPP_A00_IRQ				0x48
#define GPP_A01_IRQ				0x49
#define GPP_A02_IRQ				0x4A
#define GPP_A03_IRQ				0x4B
#define GPP_A04_IRQ				0x4C
#define GPP_A05_IRQ				0x4D
#define GPP_A06_IRQ				0x4E
#define GPP_A07_IRQ				0x4F
#define GPP_A08_IRQ				0x50
#define GPP_A09_IRQ				0x51
#define GPP_A10_IRQ				0x52
#define GPP_A11_IRQ				0x53
#define GPP_A12_IRQ				0x54
#define GPP_A13_IRQ				0x55
#define GPP_A14_IRQ				0x56
#define GPP_A15_IRQ				0x57
#define GPP_A16_IRQ				0x58
#define GPP_A17_IRQ				0x59
#define GPP_A18_IRQ				0x5A
#define GPP_A19_IRQ				0x5B
#define GPP_A20_IRQ				0x5C
#define GPP_A21_IRQ				0x5D
#define GPP_A22_IRQ				0x5E
#define GPP_A23_IRQ				0x5F

/* Group E */
#define GPP_E00_IRQ				0x60
#define GPP_E01_IRQ				0x61
#define GPP_E02_IRQ				0x62
#define GPP_E03_IRQ				0x63
#define GPP_E04_IRQ				0x64
#define GPP_E05_IRQ				0x65
#define GPP_E06_IRQ				0x66
#define GPP_E07_IRQ				0x67
#define GPP_E08_IRQ				0x68
#define GPP_E09_IRQ				0x69
#define GPP_E10_IRQ				0x6A
#define GPP_E11_IRQ				0x6B
#define GPP_E12_IRQ				0x6C
#define GPP_E13_IRQ				0x6D
#define GPP_E14_IRQ				0x6E
#define GPP_E15_IRQ				0x6F
#define GPP_E16_IRQ				0x70
#define GPP_E17_IRQ				0x71
#define GPP_E18_IRQ				0x72
#define GPP_E19_IRQ				0x73
#define GPP_E20_IRQ				0x74
#define GPP_E21_IRQ				0x75
#define GPP_E22_IRQ				0x76
#define GPP_E23_IRQ				0x77

/* Group H */
#define GPP_H00_IRQ				0x18
#define GPP_H01_IRQ				0x19
#define GPP_H02_IRQ				0x1A
#define GPP_H03_IRQ				0x1B
#define GPP_H04_IRQ				0x1C
#define GPP_H05_IRQ				0x1D
#define GPP_H06_IRQ				0x1E
#define GPP_H07_IRQ				0x1F
#define GPP_H08_IRQ				0x20
#define GPP_H09_IRQ				0x21
#define GPP_H10_IRQ				0x22
#define GPP_H11_IRQ				0x23
#define GPP_H12_IRQ				0x24
#define GPP_H13_IRQ				0x25
#define GPP_H14_IRQ				0x26
#define GPP_H15_IRQ				0x27
#define GPP_H16_IRQ				0x28
#define GPP_H17_IRQ				0x29
#define GPP_H18_IRQ				0x2A
#define GPP_H19_IRQ				0x2B
#define GPP_H20_IRQ				0x2C
#define GPP_H21_IRQ				0x2D
#define GPP_H22_IRQ				0x2E
#define GPP_H23_IRQ				0x2F

/* Group F */
#define GPP_F00_IRQ				0x30
#define GPP_F01_IRQ				0x31
#define GPP_F02_IRQ				0x32
#define GPP_F03_IRQ				0x33
#define GPP_F04_IRQ				0x34
#define GPP_F05_IRQ				0x35
#define GPP_F06_IRQ				0x36
#define GPP_F07_IRQ				0x37
#define GPP_F08_IRQ				0x38
#define GPP_F09_IRQ				0x39
#define GPP_F10_IRQ				0x3A
#define GPP_F11_IRQ				0x3B
#define GPP_F12_IRQ				0x3C
#define GPP_F13_IRQ				0x3D
#define GPP_F14_IRQ				0x3E
#define GPP_F15_IRQ				0x3F
#define GPP_F16_IRQ				0x40
#define GPP_F17_IRQ				0x41
#define GPP_F18_IRQ				0x42
#define GPP_F19_IRQ				0x43
#define GPP_F20_IRQ				0x44
#define GPP_F21_IRQ				0x45
#define GPP_F22_IRQ				0x46
#define GPP_F23_IRQ				0x47

/* Group S */
#define GPP_S00_IRQ				0x50
#define GPP_S01_IRQ				0x51
#define GPP_S02_IRQ				0x52
#define GPP_S03_IRQ				0x53
#define GPP_S04_IRQ				0x54
#define GPP_S05_IRQ				0x55
#define GPP_S06_IRQ				0x56
#define GPP_S07_IRQ				0x57

/* Group B */
#define GPP_B00_IRQ				0x58
#define GPP_B01_IRQ				0x59
#define GPP_B02_IRQ				0x5A
#define GPP_B03_IRQ				0x5B
#define GPP_B04_IRQ				0x5C
#define GPP_B05_IRQ				0x5D
#define GPP_B06_IRQ				0x5E
#define GPP_B07_IRQ				0x5F
#define GPP_B08_IRQ				0x60
#define GPP_B09_IRQ				0x61
#define GPP_B10_IRQ				0x62
#define GPP_B11_IRQ				0x63
#define GPP_B12_IRQ				0x64
#define GPP_B13_IRQ				0x65
#define GPP_B14_IRQ				0x66
#define GPP_B15_IRQ				0x67
#define GPP_B16_IRQ				0x68
#define GPP_B17_IRQ				0x69
#define GPP_B18_IRQ				0x6A
#define GPP_B19_IRQ				0x6B
#define GPP_B20_IRQ				0x6C
#define GPP_B21_IRQ				0x6D
#define GPP_B22_IRQ				0x6E
#define GPP_B23_IRQ				0x6F

/* Group D */
#define GPP_D00_IRQ				0x70
#define GPP_D01_IRQ				0x71
#define GPP_D02_IRQ				0x72
#define GPP_D03_IRQ				0x73
#define GPP_D04_IRQ				0x74
#define GPP_D05_IRQ				0x75
#define GPP_D06_IRQ				0x76
#define GPP_D07_IRQ				0x77
#define GPP_D08_IRQ				0x18
#define GPP_D09_IRQ				0x19
#define GPP_D10_IRQ				0x1A
#define GPP_D11_IRQ				0x1B
#define GPP_D12_IRQ				0x1C
#define GPP_D13_IRQ				0x1D
#define GPP_D14_IRQ				0x1E
#define GPP_D15_IRQ				0x1F
#define GPP_D16_IRQ				0x20
#define GPP_D17_IRQ				0x21
#define GPP_D18_IRQ				0x22
#define GPP_D19_IRQ				0x23
#define GPP_D20_IRQ				0x24
#define GPP_D21_IRQ				0x25
#define GPP_D22_IRQ				0x26
#define GPP_D23_IRQ				0x27

/* Register defines. */
#define GPIO_MISCCFG				0x10
#define  GPE_DW_SHIFT				8
#define  GPE_DW_MASK				0xfff00
#define HOSTSW_OWN_REG_0			0x140
#define GPI_INT_STS_0				0x200
#define GPI_INT_EN_0				0x210
#define GPI_GPE_STS_0				0x230
#define GPI_GPE_EN_0				0x250
#define GPI_SMI_STS_0				0x270
#define GPI_SMI_EN_0				0x290
#define GPI_NMI_STS_0				0x2b0
#define GPI_NMI_EN_0				0x2d0
#define PAD_CFG_BASE				0x600

#endif
