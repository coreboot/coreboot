/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_GPIO_DEFS_H_
#define _SOC_GPIO_DEFS_H_

#if CONFIG(SKYLAKE_SOC_PCH_H)
# include <soc/gpio_pch_h_defs.h>
#else
# include <soc/gpio_soc_defs.h>
#endif

#define GPIO_NUM_PAD_CFG_REGS   2 /* DW0, DW1 */

#define NUM_GPIO_COMx_GPI_REGS(n)	\
	(ALIGN_UP((n), GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define NUM_GPIO_COM0_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM0_PADS)
#define NUM_GPIO_COM1_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM1_PADS)
#define NUM_GPIO_COM2_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM2_PADS)
#define NUM_GPIO_COM3_GPI_REGS NUM_GPIO_COMx_GPI_REGS(NUM_GPIO_COM3_PADS)

#define NUM_GPI_STATUS_REGS	\
		((NUM_GPIO_COM0_GPI_REGS) +\
		(NUM_GPIO_COM1_GPI_REGS) +\
		(NUM_GPIO_COM3_GPI_REGS) +\
		(NUM_GPIO_COM2_GPI_REGS))

/*
 * IOxAPIC IRQs for the GPIOs
 */

/* Group A */
#define GPP_A0_IRQ		0x18
#define GPP_A1_IRQ		0x19
#define GPP_A2_IRQ		0x1a
#define GPP_A3_IRQ		0x1b
#define GPP_A4_IRQ		0x1c
#define GPP_A5_IRQ		0x1d
#define GPP_A6_IRQ		0x1e
#define GPP_A7_IRQ		0x1f
#define GPP_A8_IRQ		0x20
#define GPP_A9_IRQ		0x21
#define GPP_A10_IRQ		0x22
#define GPP_A11_IRQ		0x23
#define GPP_A12_IRQ		0x24
#define GPP_A13_IRQ		0x25
#define GPP_A14_IRQ		0x26
#define GPP_A15_IRQ		0x27
#define GPP_A16_IRQ		0x28
#define GPP_A17_IRQ		0x29
#define GPP_A18_IRQ		0x2a
#define GPP_A19_IRQ		0x2b
#define GPP_A20_IRQ		0x2c
#define GPP_A21_IRQ		0x2d
#define GPP_A22_IRQ		0x2e
#define GPP_A23_IRQ		0x2f
/* Group B */
#define GPP_B0_IRQ		0x30
#define GPP_B1_IRQ		0x31
#define GPP_B2_IRQ		0x32
#define GPP_B3_IRQ		0x33
#define GPP_B4_IRQ		0x34
#define GPP_B5_IRQ		0x35
#define GPP_B6_IRQ		0x36
#define GPP_B7_IRQ		0x37
#define GPP_B8_IRQ		0x38
#define GPP_B9_IRQ		0x39
#define GPP_B10_IRQ		0x3a
#define GPP_B11_IRQ		0x3b
#define GPP_B12_IRQ		0x3c
#define GPP_B13_IRQ		0x3d
#define GPP_B14_IRQ		0x3e
#define GPP_B15_IRQ		0x3f
#define GPP_B16_IRQ		0x40
#define GPP_B17_IRQ		0x41
#define GPP_B18_IRQ		0x42
#define GPP_B19_IRQ		0x43
#define GPP_B20_IRQ		0x44
#define GPP_B21_IRQ		0x45
#define GPP_B22_IRQ		0x46
#define GPP_B23_IRQ		0x47

/* Group C */
#define GPP_C0_IRQ		0x48
#define GPP_C1_IRQ		0x49
#define GPP_C2_IRQ		0x4a
#define GPP_C3_IRQ		0x4b
#define GPP_C4_IRQ		0x4c
#define GPP_C5_IRQ		0x4d
#define GPP_C6_IRQ		0x4e
#define GPP_C7_IRQ		0x4f
#define GPP_C8_IRQ		0x50
#define GPP_C9_IRQ		0x51
#define GPP_C10_IRQ		0x52
#define GPP_C11_IRQ		0x53
#define GPP_C12_IRQ		0x54
#define GPP_C13_IRQ		0x55
#define GPP_C14_IRQ		0x56
#define GPP_C15_IRQ		0x57
#define GPP_C16_IRQ		0x58
#define GPP_C17_IRQ		0x59
#define GPP_C18_IRQ		0x5a
#define GPP_C19_IRQ		0x5b
#define GPP_C20_IRQ		0x5c
#define GPP_C21_IRQ		0x5d
#define GPP_C22_IRQ		0x5e
#define GPP_C23_IRQ		0x5f
/* Group D */
#define GPP_D0_IRQ		0x60
#define GPP_D1_IRQ		0x61
#define GPP_D2_IRQ		0x62
#define GPP_D3_IRQ		0x63
#define GPP_D4_IRQ		0x64
#define GPP_D5_IRQ		0x65
#define GPP_D6_IRQ		0x66
#define GPP_D7_IRQ		0x67
#define GPP_D8_IRQ		0x68
#define GPP_D9_IRQ		0x69
#define GPP_D10_IRQ		0x6a
#define GPP_D11_IRQ		0x6b
#define GPP_D12_IRQ		0x6c
#define GPP_D13_IRQ		0x6d
#define GPP_D14_IRQ		0x6e
#define GPP_D15_IRQ		0x6f
#define GPP_D16_IRQ		0x70
#define GPP_D17_IRQ		0x71
#define GPP_D18_IRQ		0x72
#define GPP_D19_IRQ		0x73
#define GPP_D20_IRQ		0x74
#define GPP_D21_IRQ		0x75
#define GPP_D22_IRQ		0x76
#define GPP_D23_IRQ		0x77
/* Group E */
#define GPP_E0_IRQ		0x18
#define GPP_E1_IRQ		0x19
#define GPP_E2_IRQ		0x1a
#define GPP_E3_IRQ		0x1b
#define GPP_E4_IRQ		0x1c
#define GPP_E5_IRQ		0x1d
#define GPP_E6_IRQ		0x1e
#define GPP_E7_IRQ		0x1f
#define GPP_E8_IRQ		0x20
#define GPP_E9_IRQ		0x21
#define GPP_E10_IRQ		0x22
#define GPP_E11_IRQ		0x23
#define GPP_E12_IRQ		0x24
#define GPP_E13_IRQ		0x25
#define GPP_E14_IRQ		0x26
#define GPP_E15_IRQ		0x27
#define GPP_E16_IRQ		0x28
#define GPP_E17_IRQ		0x29
#define GPP_E18_IRQ		0x2a
#define GPP_E19_IRQ		0x2b
#define GPP_E20_IRQ		0x2c
#define GPP_E21_IRQ		0x2d
#define GPP_E22_IRQ		0x2e
#define GPP_E23_IRQ		0x2f
/* Group F */
#define GPP_F0_IRQ		0x30
#define GPP_F1_IRQ		0x31
#define GPP_F2_IRQ		0x32
#define GPP_F3_IRQ		0x33
#define GPP_F4_IRQ		0x34
#define GPP_F5_IRQ		0x35
#define GPP_F6_IRQ		0x36
#define GPP_F7_IRQ		0x37
#define GPP_F8_IRQ		0x38
#define GPP_F9_IRQ		0x39
#define GPP_F10_IRQ		0x3a
#define GPP_F11_IRQ		0x3b
#define GPP_F12_IRQ		0x3c
#define GPP_F13_IRQ		0x3d
#define GPP_F14_IRQ		0x3e
#define GPP_F15_IRQ		0x3f
#define GPP_F16_IRQ		0x40
#define GPP_F17_IRQ		0x41
#define GPP_F18_IRQ		0x42
#define GPP_F19_IRQ		0x43
#define GPP_F20_IRQ		0x44
#define GPP_F21_IRQ		0x45
#define GPP_F22_IRQ		0x46
#define GPP_F23_IRQ		0x47
/* Group G */
#define GPP_G0_IRQ		0x48
#define GPP_G1_IRQ		0x49
#define GPP_G2_IRQ		0x4a
#define GPP_G3_IRQ		0x4b
#define GPP_G4_IRQ		0x4c
#define GPP_G5_IRQ		0x4d
#define GPP_G6_IRQ		0x4e
#define GPP_G7_IRQ		0x4f
/* Group GPD */
#define GPD0_IRQ		0x50
#define GPD1_IRQ		0x51
#define GPD2_IRQ		0x52
#define GPD3_IRQ		0x53
#define GPD4_IRQ		0x54
#define GPD5_IRQ		0x55
#define GPD6_IRQ		0x56
#define GPD7_IRQ		0x57
#define GPD8_IRQ		0x58
#define GPD9_IRQ		0x59
#define GPD10_IRQ		0x5a
#define GPD11_IRQ		0x5b

/* Register defines. */
#define GPIO_MISCCFG		0x10
#define  GPIO_DRIVER_IRQ_ROUTE_MASK    8
#define  GPIO_DRIVER_IRQ_ROUTE_IRQ14   0
#define  GPIO_DRIVER_IRQ_ROUTE_IRQ15   8
#define HOSTSW_OWN_REG_0	0xd0
#define PAD_CFG_BASE		0x400
#define GPI_INT_STS_0		0x100
#define GPI_INT_EN_0		0x120
#define GPI_SMI_STS_0		0x180
#define GPI_SMI_EN_0		0x1a0
#define GPI_NMI_STS_0		0x1c0
#define GPI_NMI_EN_0		0x1e0

#endif /* _SOC_GPIO_DEFS_H_ */
