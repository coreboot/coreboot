/*
 * Memory map definitions for Allwinner A10 CPUs
 *
 * Copyright (C) 2007-2011 Allwinner Technology Co., Ltd.
 *	Tom Cubie <tangliang@allwinnertech.com>
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef CPU_ALLWINNER_A10_MEMMAP_H
#define CPU_ALLWINNER_A10_MEMMAP_H

#define A1X_SRAM_A1_BASE		0x00000000
#define A1X_SRAM_A1_SIZE		(16 * 1024)	/* 16 kiB */

#define A1X_SRAM_A2_BASE		0x00004000	/* 16 kiB */
#define A1X_SRAM_A3_BASE		0x00008000	/* 13 kiB */
#define A1X_SRAM_A4_BASE		0x0000b400	/* 3 kiB */
#define A1X_SRAM_D_BASE			0x01c00000
#define A1X_SRAM_B_BASE			0x01c00000	/* 64 kiB (secure) */

#define A1X_SRAMC_BASE			0x01c00000
#define A1X_DRAMC_BASE			0x01c01000
#define A1X_DMA_BASE			0x01c02000
#define A1X_NFC_BASE			0x01c03000
#define A1X_TS_BASE			0x01c04000
#define A1X_SPI0_BASE			0x01c05000
#define A1X_SPI1_BASE			0x01c06000
#define A1X_MS_BASE			0x01c07000
#define A1X_TVD_BASE			0x01c08000
#define A1X_CSI0_BASE			0x01c09000
#define A1X_TVE0_BASE			0x01c0a000
#define A1X_EMAC_BASE			0x01c0b000
#define A1X_LCD0_BASE			0x01c0C000
#define A1X_LCD1_BASE			0x01c0d000
#define A1X_VE_BASE			0x01c0e000
#define A1X_MMC0_BASE			0x01c0f000
#define A1X_MMC1_BASE			0x01c10000
#define A1X_MMC2_BASE			0x01c11000
#define A1X_MMC3_BASE			0x01c12000
#define A1X_USB0_BASE			0x01c13000
#define A1X_USB1_BASE			0x01c14000
#define A1X_SS_BASE			0x01c15000
#define A1X_HDMI_BASE			0x01c16000
#define A1X_SPI2_BASE			0x01c17000
#define A1X_SATA_BASE			0x01c18000
#define A1X_PATA_BASE			0x01c19000
#define A1X_ACE_BASE			0x01c1a000
#define A1X_TVE1_BASE			0x01c1b000
#define A1X_USB2_BASE			0x01c1c000
#define A1X_CSI1_BASE			0x01c1d000
#define A1X_TZASC_BASE			0x01c1e000
#define A1X_SPI3_BASE			0x01c1f000

#define A1X_CCM_BASE			0x01c20000
#define A1X_INTC_BASE			0x01c20400
#define A1X_PIO_BASE			0x01c20800
#define A1X_TIMER_BASE			0x01c20c00
#define A1X_SPDIF_BASE			0x01c21000
#define A1X_AC97_BASE			0x01c21400
#define A1X_IR0_BASE			0x01c21800
#define A1X_IR1_BASE			0x01c21c00

#define A1X_IIS_BASE			0x01c22400
#define A1X_LRADC_BASE			0x01c22800
#define A1X_AD_DA_BASE			0x01c22c00
#define A1X_KEYPAD_BASE			0x01c23000
#define A1X_TZPC_BASE			0x01c23400
#define A1X_SID_BASE			0x01c23800
#define A1X_SJTAG_BASE			0x01c23c00

#define A1X_TP_BASE			0x01c25000
#define A1X_PMU_BASE			0x01c25400
#define A1X_CPUCFG_BASE			0x01c25c00	/* sun7i only ? */

#define A1X_UART0_BASE			0x01c28000
#define A1X_UART1_BASE			0x01c28400
#define A1X_UART2_BASE			0x01c28800
#define A1X_UART3_BASE			0x01c28c00
#define A1X_UART4_BASE			0x01c29000
#define A1X_UART5_BASE			0x01c29400
#define A1X_UART6_BASE			0x01c29800
#define A1X_UART7_BASE			0x01c29c00
#define A1X_PS2_0_BASE			0x01c2a000
#define A1X_PS2_1_BASE			0x01c2a400

#define A1X_TWI0_BASE			0x01c2ac00
#define A1X_TWI1_BASE			0x01c2b000
#define A1X_TWI2_BASE			0x01c2b400

#define A1X_CAN_BASE			0x01c2bc00

#define A1X_SCR_BASE			0x01c2c400

#define A1X_GPS_BASE			0x01c30000
#define A1X_MALI400_BASE		0x01c40000

/* module sram */
#define A1X_SRAM_C_BASE			0x01d00000

#define A1X_DE_FE0_BASE			0x01e00000
#define A1X_DE_FE1_BASE			0x01e20000
#define A1X_DE_BE0_BASE			0x01e60000
#define A1X_DE_BE1_BASE			0x01e40000
#define A1X_MP_BASE			0x01e80000
#define A1X_AVG_BASE			0x01ea0000

/* CoreSight Debug Module */
#define A1X_CSDM_BASE			0x3f500000

#define A1X_DRAM_BASE			0x40000000	/* 2 GiB */

#define A1X_BROM_BASE			0xffff0000	/* 32 kiB */

#define A1X_CPU_CFG			(A1X_TIMER_BASE + 0x13c)

#endif				/* CPU_ALLWINNER_A10_MEMMAP_H */
