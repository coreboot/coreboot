 /* SPDX-License-Identifier: GPL-2.0-only */

/* Machine Specific Values for SMDK5420 board based on Exynos5 */

#ifndef CPU_SAMSUNG_EXYNOS5420_SETUP_H
#define CPU_SAMSUNG_EXYNOS5420_SETUP_H

struct exynos5_dmc;
enum ddr_mode;
struct exynos5_phy_control;

#define NOT_AVAILABLE		0
#define DATA_MASK		0xFFFFF

#define ENABLE_BIT		0x1
#define DISABLE_BIT		0x0
#define CA_SWAP_EN		(1 << 0)

/* TZPC : Register Offsets */
#define TZPC0_BASE		0x10100000
#define TZPC1_BASE		0x10110000
#define TZPC2_BASE		0x10120000
#define TZPC3_BASE		0x10130000
#define TZPC4_BASE		0x10140000
#define TZPC5_BASE		0x10150000
#define TZPC6_BASE		0x10160000
#define TZPC7_BASE		0x10170000
#define TZPC8_BASE		0x10180000
#define TZPC9_BASE		0x10190000

#define APLL_FOUT	(1 << 0)
#define KPLL_FOUT	(1 << 0)

#define CLK_DIV_CPERI1_VAL	0x3f3f0000

/* APLL_CON1	*/
#define APLL_CON1_VAL	(0x0020f300)

/* MPLL_CON1	*/
#define MPLL_CON1_VAL   (0x0020f300)

/* CPLL_CON1	*/
#define CPLL_CON1_VAL	(0x0020f300)

/* DPLL_CON1 */
#define DPLL_CON1_VAL	(0x0020f300)

/* GPLL_CON1	*/
#define GPLL_CON1_VAL	(NOT_AVAILABLE)

/* EPLL_CON1, CON2	*/
#define EPLL_CON1_VAL	0x00000000
#define EPLL_CON2_VAL	0x00000080

/* VPLL_CON1, CON2	*/
#define VPLL_CON1_VAL	0x0020f300
#define VPLL_CON2_VAL	NOT_AVAILABLE

/* RPLL_CON1, CON2 */
#define RPLL_CON1_VAL	0x00000000
#define RPLL_CON2_VAL	0x00000080

/* BPLL_CON1	*/
#define BPLL_CON1_VAL	0x0020f300

/* SPLL_CON1 */
#define SPLL_CON1_VAL	0x0020f300

/* IPLL_CON1 */
#define IPLL_CON1_VAL	0x00000080

/* KPLL_CON1 */
#define KPLL_CON1_VAL	0x200000

/* Set PLL */
#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)

/* CLK_SRC_CPU	*/
/* 0 = MOUTAPLL,  1 = SCLKMPLL	*/
#define MUX_HPM_SEL             1
#define MUX_CPU_SEL             0
#define MUX_APLL_SEL            1

#define CLK_SRC_CPU_VAL		((MUX_HPM_SEL << 20)    \
				| (MUX_CPU_SEL << 16)  \
				| (MUX_APLL_SEL))

/* MEMCONTROL register bit fields */
#define DMC_MEMCONTROL_CLK_STOP_DISABLE	(0 << 0)
#define DMC_MEMCONTROL_DPWRDN_DISABLE	(0 << 1)
#define DMC_MEMCONTROL_DPWRDN_ACTIVE_PRECHARGE	(0 << 2)
#define DMC_MEMCONTROL_DSREF_DISABLE	(0 << 5)
#define DMC_MEMCONTROL_DSREF_ENABLE	(1 << 5)
#define DMC_MEMCONTROL_ADD_LAT_PALL_CYCLE(x)    (x << 6)

#define DMC_MEMCONTROL_MEM_TYPE_LPDDR3  (7 << 8)
#define DMC_MEMCONTROL_MEM_TYPE_DDR3    (6 << 8)
#define DMC_MEMCONTROL_MEM_TYPE_LPDDR2  (5 << 8)

#define DMC_MEMCONTROL_MEM_WIDTH_32BIT  (2 << 12)

#define DMC_MEMCONTROL_NUM_CHIP_1       (0 << 16)
#define DMC_MEMCONTROL_NUM_CHIP_2       (1 << 16)

#define DMC_MEMCONTROL_BL_8             (3 << 20)
#define DMC_MEMCONTROL_BL_4             (2 << 20)

#define DMC_MEMCONTROL_PZQ_DISABLE      (0 << 24)

#define DMC_MEMCONTROL_MRR_BYTE_7_0     (0 << 25)
#define DMC_MEMCONTROL_MRR_BYTE_15_8    (1 << 25)
#define DMC_MEMCONTROL_MRR_BYTE_23_16   (2 << 25)
#define DMC_MEMCONTROL_MRR_BYTE_31_24   (3 << 25)

/* MEMCONFIG0 register bit fields */
#define DMC_MEMCONFIGx_CHIP_MAP_INTERLEAVED     (1 << 12)
#define DMC_MEMCONFIG_CHIP_MAP_SPLIT		(2 << 12)
#define DMC_MEMCONFIGx_CHIP_COL_10              (3 << 8)
#define DMC_MEMCONFIGx_CHIP_ROW_14              (2 << 4)
#define DMC_MEMCONFIGx_CHIP_ROW_15              (3 << 4)
#define DMC_MEMCONFIGx_CHIP_ROW_16              (4 << 4)
#define DMC_MEMCONFIGx_CHIP_BANK_8              (3 << 0)

#define DMC_MEMBASECONFIG0_VAL  DMC_MEMBASECONFIG_VAL(0x40)
#define DMC_MEMBASECONFIG1_VAL  DMC_MEMBASECONFIG_VAL(0x80)

#define DMC_PRECHCONFIG_VAL             0xFF000000
#define DMC_PWRDNCONFIG_VAL             0xFFFF00FF

#define DMC_CONCONTROL_RESET_VAL	0x0FFF0000
#define DFI_INIT_START		(1 << 28)
#define EMPTY			(1 << 8)
#define AREF_EN			(1 << 5)

#define DFI_INIT_COMPLETE_CHO	(1 << 2)
#define DFI_INIT_COMPLETE_CH1	(1 << 3)

#define RDLVL_COMPLETE_CHO	(1 << 14)
#define RDLVL_COMPLETE_CH1	(1 << 15)

#define CLK_STOP_EN	(1 << 0)
#define DPWRDN_EN	(1 << 1)
#define DSREF_EN	(1 << 5)

/* COJCONTROL register bit fields */
#define DMC_CONCONTROL_IO_PD_CON_DISABLE	(0 << 3)
#define DMC_CONCONTROL_AREF_EN_DISABLE		(0 << 5)
#define DMC_CONCONTROL_RD_FETCH_DISABLE		(0x0 << 12)
#define DMC_CONCONTROL_TIMEOUT_LEVEL0		(0xFFF << 16)
#define DMC_CONCONTROL_DFI_INIT_START_DISABLE	(0 << 28)

/* CLK_FSYS */
#define CLK_SRC_FSYS0_VAL              0x33033300
#define CLK_DIV_FSYS0_VAL	       0x0
#define CLK_DIV_FSYS1_VAL	       0x04f13c4f
#define CLK_DIV_FSYS2_VAL	       0x041d0000

#define DMC_CONCONTROL_IO_PD_CON(x)	(x << 6)

/* CLK_DIV_CPU1	*/
#define HPM_RATIO               0x2
#define COPY_RATIO              0x0

/* CLK_DIV_CPU1 = 0x00000003 */
#define CLK_DIV_CPU1_VAL        ((HPM_RATIO << 4)		\
				| (COPY_RATIO))

/* CLK_SRC_CORE0 */
#define CLK_SRC_CORE0_VAL       0x00000000

/* CLK_SRC_CORE1 */
#define CLK_SRC_CORE1_VAL       0x100

/* CLK_DIV_CORE0 */
#define CLK_DIV_CORE0_VAL       0x00120000

/* CLK_DIV_CORE1 */
#define CLK_DIV_CORE1_VAL       0x07070700

/* CLK_DIV_SYSRGT */
#define CLK_DIV_SYSRGT_VAL      0x00000111

/* CLK_DIV_ACP */
#define CLK_DIV_ACP_VAL         0x12

/* CLK_DIV_SYSLFT */
#define CLK_DIV_SYSLFT_VAL      0x00000311

/* CLK_SRC_CDREX */
#define CLK_SRC_CDREX_VAL       0x00000001
#define MUX_MCLK_CDR_MSPLL	(1 << 4)
#define MUX_BPLL_SEL_FOUTBPLL   (1 << 0)
#define BPLL_SEL_MASK   0x7
#define FOUTBPLL        2

/* CLK_DIV_CDREX */
#define CLK_DIV_CDREX0_VAL	0x30010100
#define CLK_DIV_CDREX1_VAL	0x300

#define CLK_DIV_CDREX_VAL	0x17010100

/* CLK_DIV_CPU0_VAL */
#define CLK_DIV_CPU0_VAL	0x01440020

/* CLK_SRC_TOP */
#define CLK_SRC_TOP0_VAL	0x11101102
#define CLK_SRC_TOP1_VAL	0x00200000
#define CLK_SRC_TOP2_VAL	0x11101010
#define CLK_SRC_TOP3_VAL	0x11111111
#define CLK_SRC_TOP4_VAL	0x11110111
#define CLK_SRC_TOP5_VAL	0x11111111
#define CLK_SRC_TOP6_VAL	0x11110111
#define CLK_SRC_TOP7_VAL	0x00022200

/* CLK_DIV_TOP */
#define CLK_DIV_TOP0_VAL	0x22512211
#define CLK_DIV_TOP1_VAL	0x13200900
#define CLK_DIV_TOP2_VAL	0x11101110

/* APLL_LOCK */
#define APLL_LOCK_VAL	(0x320)
/* MPLL_LOCK */
#define MPLL_LOCK_VAL	(0x258)
/* BPLL_LOCK */
#define BPLL_LOCK_VAL	(0x258)
/* CPLL_LOCK */
#define CPLL_LOCK_VAL	(0x190)
/* DPLL_LOCK */
#define DPLL_LOCK_VAL	(0x190)
/* GPLL_LOCK */
#define GPLL_LOCK_VAL	NOT_AVAILABLE
/* IPLL_LOCK */
#define IPLL_LOCK_VAL	(0x320)
/* KPLL_LOCK */
#define KPLL_LOCK_VAL	(0x258)
/* SPLL_LOCK */
#define SPLL_LOCK_VAL	(0x320)
/* RPLL_LOCK */
#define RPLL_LOCK_VAL	(0x2328)
/* EPLL_LOCK */
#define EPLL_LOCK_VAL	(0x2328)
/* VPLL_LOCK */
#define VPLL_LOCK_VAL	(0x258)

#define MUX_APLL_SEL_MASK	(1 << 0)
#define MUX_MPLL_SEL_MASK	(1 << 8)
#define MPLL_SEL_MOUT_MPLLFOUT	(2 << 8)
#define MUX_CPLL_SEL_MASK	(1 << 8)
#define MUX_EPLL_SEL_MASK	(1 << 12)
#define MUX_VPLL_SEL_MASK	(1 << 16)
#define MUX_GPLL_SEL_MASK	(1 << 28)
#define MUX_BPLL_SEL_MASK	(1 << 0)
#define MUX_HPM_SEL_MASK	(1 << 20)
#define HPM_SEL_SCLK_MPLL	(1 << 21)
#define PLL_LOCKED		(1 << 29)
#define APLL_CON0_LOCKED	(1 << 29)
#define MPLL_CON0_LOCKED	(1 << 29)
#define BPLL_CON0_LOCKED	(1 << 29)
#define CPLL_CON0_LOCKED	(1 << 29)
#define EPLL_CON0_LOCKED	(1 << 29)
#define GPLL_CON0_LOCKED	(1 << 29)
#define VPLL_CON0_LOCKED	(1 << 29)
#define CLK_REG_DISABLE		0x0
#define TOP2_VAL		0x0110000

/* CLK_SRC_LEX */
#define CLK_SRC_LEX_VAL         0x0

/* CLK_DIV_LEX */
#define CLK_DIV_LEX_VAL         0x10

/* CLK_DIV_R0X */
#define CLK_DIV_R0X_VAL         0x10

/* CLK_DIV_L0X */
#define CLK_DIV_R1X_VAL         0x10

/* CLK_DIV_ISP2 */
#define CLK_DIV_ISP2_VAL        0x1

/* CLK_SRC_KFC */
#define SRC_KFC_HPM_SEL		(1 << 15)

/* CLK_SRC_KFC */
#define CLK_SRC_KFC_VAL		0x00008001

/* CLK_DIV_KFC */
#define CLK_DIV_KFC_VAL		0x03300110

/* CLK_DIV2_RATIO */
#define CLK_DIV2_RATIO		0x10111150

/* CLK_DIV4_RATIO */
#define CLK_DIV4_RATIO		0x00000003

/* CLK_DIV_G2D */
#define CLK_DIV_G2D		0x00000010

/* CLK_SRC_PERIC0 */
#define SPDIF_SEL	1
#define PWM_SEL		3
#define UART4_SEL	3
#define UART3_SEL	3
#define UART2_SEL	3
#define UART1_SEL	3
#define UART0_SEL	3
/* SRC_CLOCK = SCLK_RPLL */
#define CLK_SRC_PERIC0_VAL	((SPDIF_SEL << 28)	\
				| (PWM_SEL << 24)	\
				| (UART4_SEL << 20)	\
				| (UART3_SEL << 16)	\
				| (UART2_SEL << 12)	\
				| (UART1_SEL << 8)	\
				| (UART0_SEL << 4))

/* CLK_SRC_PERIC1 */
/* SRC_CLOCK = SCLK_MPLL */
#define SPI0_SEL		3
#define SPI1_SEL		3
#define SPI2_SEL		3
/* SRC_CLOCK = SCLK_EPLL */
#define AUDIO0_SEL		6
#define AUDIO1_SEL		6
#define AUDIO2_SEL		6
#define CLK_SRC_PERIC1_VAL	((SPI2_SEL << 28)	\
				| (SPI1_SEL << 24)	\
				| (SPI0_SEL << 20)	\
				| (AUDIO2_SEL << 16)	\
				| (AUDIO2_SEL << 12)	\
				| (AUDIO2_SEL << 8))

/* CLK_SRC_ISP */
#define CLK_SRC_ISP_VAL		0x33366000
#define CLK_DIV_ISP0_VAL	0x13131300
#define CLK_DIV_ISP1_VAL	0xbb110202

/* SCLK_DIV_ISP - set SPI0/1 to 0xf = divide by 16 */
#define SPI0_ISP_RATIO		0xf
#define SPI1_ISP_RATIO		0xf
#define SCLK_DIV_ISP_VAL	(SPI1_ISP_RATIO << 12) \
				| (SPI0_ISP_RATIO << 0)

/* CLK_DIV_PERIL0	*/
#define PWM_RATIO	8
#define UART4_RATIO	9
#define UART3_RATIO	9
#define UART2_RATIO	9
#define UART1_RATIO	9
#define UART0_RATIO	9

#define CLK_DIV_PERIC0_VAL	((PWM_RATIO << 28)	\
				| (UART4_RATIO << 24)	\
				| (UART3_RATIO << 20)    \
				| (UART2_RATIO << 16)    \
				| (UART1_RATIO << 12)    \
				| (UART0_RATIO << 8))

/* CLK_DIV_PERIC1 */
#define SPI2_RATIO		0x1
#define SPI1_RATIO		0x1
#define SPI0_RATIO		0x1
#define CLK_DIV_PERIC1_VAL	((SPI2_RATIO << 28)	\
				| (SPI1_RATIO << 24)	\
				| (SPI0_RATIO << 20))

/* CLK_DIV_PERIC2 */
#define PCM2_RATIO		0x3
#define PCM1_RATIO		0x3
#define CLK_DIV_PERIC2_VAL	((PCM2_RATIO << 24) \
				| (PCM1_RATIO << 16))

/* CLK_DIV_PERIC3 */
#define AUDIO2_RATIO		0x5
#define AUDIO1_RATIO		0x5
#define AUDIO0_RATIO		0x5
#define CLK_DIV_PERIC3_VAL	((AUDIO2_RATIO << 28)	\
				| (AUDIO1_RATIO << 24)	\
				| (AUDIO0_RATIO << 20))

/* CLK_DIV_PERIC4 */
#define SPI2_PRE_RATIO		0x3
#define SPI1_PRE_RATIO		0x3
#define SPI0_PRE_RATIO		0x3
#define CLK_DIV_PERIC4_VAL	((SPI2_PRE_RATIO << 24)	\
				| (SPI1_PRE_RATIO << 16) \
				| (SPI0_PRE_RATIO << 8))

/* CLK_DIV_FSYS2 */
#define MMC2_RATIO_MASK		0xf
#define MMC2_RATIO_VAL		0x3
#define MMC2_RATIO_OFFSET	0

#define MMC2_PRE_RATIO_MASK	0xff
#define MMC2_PRE_RATIO_VAL	0x9
#define MMC2_PRE_RATIO_OFFSET	8

#define MMC3_RATIO_MASK		0xf
#define MMC3_RATIO_VAL		0x1
#define MMC3_RATIO_OFFSET	16

#define MMC3_PRE_RATIO_MASK	0xff
#define MMC3_PRE_RATIO_VAL	0x0
#define MMC3_PRE_RATIO_OFFSET	24

/* CLK_SRC_LEX */
#define CLK_SRC_LEX_VAL         0x0

/* CLK_DIV_LEX */
#define CLK_DIV_LEX_VAL         0x10

/* CLK_DIV_R0X */
#define CLK_DIV_R0X_VAL         0x10

/* CLK_DIV_L0X */
#define CLK_DIV_R1X_VAL         0x10

/* CLK_DIV_ISP2 */
#define CLK_DIV_ISP2_VAL        0x1

/* CLK_SRC_DISP1_0 */
#define CLK_SRC_DISP1_0_VAL	0x10006000
#define CLK_DIV_DISP1_0_VAL	0x01050210

/*
 * DIV_DISP1_0
 * For DP, divisor should be 2
 */
#define CLK_DIV_DISP1_0_FIMD1	(2 << 0)

/* CLK_GATE_IP_DISP1 */
#define CLK_GATE_DP1_ALLOW	(1 << 4)

/* CLK_GATE_IP_SYSRGT */
#define CLK_C2C_MASK		(1 << 1)

/* CLK_GATE_IP_ACP */
#define CLK_SMMUG2D_MASK	(1 << 7)
#define CLK_SMMUSSS_MASK	(1 << 6)
#define CLK_SMMUMDMA_MASK	(1 << 5)
#define CLK_ID_REMAPPER_MASK	(1 << 4)
#define CLK_G2D_MASK		(1 << 3)
#define CLK_SSS_MASK		(1 << 2)
#define CLK_MDMA_MASK		(1 << 1)
#define CLK_SECJTAG_MASK	(1 << 0)

/* CLK_GATE_BUS_SYSLFT */
#define CLK_EFCLK_MASK		(1 << 16)

/* CLK_GATE_IP_ISP0 */
#define CLK_UART_ISP_MASK	(1 << 31)
#define CLK_WDT_ISP_MASK	(1 << 30)
#define CLK_PWM_ISP_MASK	(1 << 28)
#define CLK_MTCADC_ISP_MASK	(1 << 27)
#define CLK_I2C1_ISP_MASK	(1 << 26)
#define CLK_I2C0_ISP_MASK	(1 << 25)
#define CLK_MPWM_ISP_MASK	(1 << 24)
#define CLK_MCUCTL_ISP_MASK	(1 << 23)
#define CLK_INT_COMB_ISP_MASK	(1 << 22)
#define CLK_SMMU_MCUISP_MASK	(1 << 13)
#define CLK_SMMU_SCALERP_MASK	(1 << 12)
#define CLK_SMMU_SCALERC_MASK	(1 << 11)
#define CLK_SMMU_FD_MASK	(1 << 10)
#define CLK_SMMU_DRC_MASK	(1 << 9)
#define CLK_SMMU_ISP_MASK	(1 << 8)
#define CLK_GICISP_MASK		(1 << 7)
#define CLK_ARM9S_MASK		(1 << 6)
#define CLK_MCUISP_MASK		(1 << 5)
#define CLK_SCALERP_MASK	(1 << 4)
#define CLK_SCALERC_MASK	(1 << 3)
#define CLK_FD_MASK		(1 << 2)
#define CLK_DRC_MASK		(1 << 1)
#define CLK_ISP_MASK		(1 << 0)

/* CLK_GATE_IP_ISP1 */
#define CLK_SPI1_ISP_MASK	(1 << 13)
#define CLK_SPI0_ISP_MASK	(1 << 12)
#define CLK_SMMU3DNR_MASK	(1 << 7)
#define CLK_SMMUDIS1_MASK	(1 << 6)
#define CLK_SMMUDIS0_MASK	(1 << 5)
#define CLK_SMMUODC_MASK	(1 << 4)
#define CLK_3DNR_MASK		(1 << 2)
#define CLK_DIS_MASK		(1 << 1)
#define CLK_ODC_MASK		(1 << 0)

/* CLK_GATE_IP_GSCL */
#define CLK_SMMUFIMC_LITE2_MASK	(1 << 20)
#define CLK_SMMUFIMC_LITE1_MASK	(1 << 12)
#define CLK_SMMUFIMC_LITE0_MASK	(1 << 11)
#define CLK_SMMUGSCL3_MASK	(1 << 10)
#define CLK_SMMUGSCL2_MASK	(1 << 9)
#define CLK_SMMUGSCL1_MASK	(1 << 8)
#define CLK_SMMUGSCL0_MASK	(1 << 7)
#define CLK_GSCL_WRAP_B_MASK	(1 << 6)
#define CLK_GSCL_WRAP_A_MASK	(1 << 5)
#define CLK_CAMIF_TOP_MASK	(1 << 4)
#define CLK_GSCL3_MASK		(1 << 3)
#define CLK_GSCL2_MASK		(1 << 2)
#define CLK_GSCL1_MASK		(1 << 1)
#define CLK_GSCL0_MASK		(1 << 0)

/* CLK_GATE_IP_MFC */
#define CLK_SMMUMFCR_MASK	(1 << 2)
#define CLK_SMMUMFCL_MASK	(1 << 1)
#define CLK_MFC_MASK		(1 << 0)

#define SCLK_MPWM_ISP_MASK	(1 << 0)

/* CLK_GATE_IP_DISP1 */
#define CLK_SMMUTVX_MASK	(1 << 9)
#define CLK_ASYNCTVX_MASK	(1 << 7)
#define CLK_HDMI_MASK		(1 << 6)
#define CLK_MIXER_MASK		(1 << 5)
#define CLK_DSIM1_MASK		(1 << 3)

/* AUDIO CLK SEL */
#define AUDIO0_SEL_EPLL		(0x6 << 28)
#define AUDIO0_RATIO		0x5
#define PCM0_RATIO		0x3
#define DIV_MAU_VAL		(PCM0_RATIO << 24 | AUDIO0_RATIO << 20)

/* CLK_GATE_IP_GEN */
#define CLK_SMMUMDMA1_MASK	(1 << 9)
#define CLK_SMMUJPEG_MASK	(1 << 7)
#define CLK_SMMUROTATOR_MASK	(1 << 6)
#define CLK_MDMA1_MASK		(1 << 4)
#define CLK_JPEG_MASK		(1 << 2)
#define CLK_ROTATOR_MASK	(1 << 1)

/* CLK_GATE_IP_FSYS */
#define CLK_WDT_IOP_MASK	(1 << 30)
#define CLK_SMMUMCU_IOP_MASK	(1 << 26)
#define CLK_SATA_PHY_I2C_MASK	(1 << 25)
#define CLK_SATA_PHY_CTRL_MASK	(1 << 24)
#define CLK_MCUCTL_MASK		(1 << 23)
#define CLK_NFCON_MASK		(1 << 22)
#define CLK_SMMURTIC_MASK	(1 << 11)
#define CLK_RTIC_MASK		(1 << 9)
#define CLK_MIPI_HSI_MASK	(1 << 8)
#define CLK_USBOTG_MASK		(1 << 7)
#define CLK_SATA_MASK		(1 << 6)
#define CLK_PDMA1_MASK		(1 << 2)
#define CLK_PDMA0_MASK		(1 << 1)
#define CLK_MCU_IOP_MASK	(1 << 0)

/* CLK_GATE_IP_PERIC */
#define CLK_HS_I2C3_MASK	(1 << 31)
#define CLK_HS_I2C2_MASK	(1 << 30)
#define CLK_HS_I2C1_MASK	(1 << 29)
#define CLK_HS_I2C0_MASK	(1 << 28)
#define CLK_AC97_MASK		(1 << 27)
#define CLK_SPDIF_MASK		(1 << 26)
#define CLK_PCM2_MASK		(1 << 23)
#define CLK_PCM1_MASK		(1 << 22)
#define CLK_I2S2_MASK		(1 << 21)
#define CLK_I2S1_MASK		(1 << 20)
#define CLK_SPI2_MASK		(1 << 18)
#define CLK_SPI0_MASK		(1 << 16)
#define CLK_I2CHDMI_MASK	(1 << 14)
#define CLK_I2C7_MASK		(1 << 13)
#define CLK_I2C6_MASK		(1 << 12)
#define CLK_I2C5_MASK		(1 << 11)
#define CLK_I2C4_MASK		(1 << 10)
#define CLK_I2C3_MASK		(1 << 9)
#define CLK_I2C2_MASK		(1 << 8)
#define CLK_I2C1_MASK		(1 << 7)
#define CLK_I2C0_MASK		(1 << 6)

/* CLK_GATE_IP_PERIS */
#define CLK_RTC_MASK		(1 << 20)
#define CLK_TZPC9_MASK		(1 << 15)
#define CLK_TZPC8_MASK		(1 << 14)
#define CLK_TZPC7_MASK		(1 << 13)
#define CLK_TZPC6_MASK		(1 << 12)
#define CLK_TZPC5_MASK		(1 << 11)
#define CLK_TZPC4_MASK		(1 << 10)
#define CLK_TZPC3_MASK		(1 << 9)
#define CLK_TZPC2_MASK		(1 << 8)
#define CLK_TZPC1_MASK		(1 << 7)
#define CLK_TZPC0_MASK		(1 << 6)
#define CLK_CHIPID_MASK		(1 << 0)

/* CLK_GATE_BLOCK */
#define CLK_ACP_MASK	(1 << 7)

/* CLK_GATE_IP_CDREX */
#define CLK_TZASC_DRBXW_MASK	(1 << 23)
#define CLK_TZASC_DRBXR_MASK	(1 << 22)
#define CLK_TZASC_XLBXW_MASK	(1 << 21)
#define CLK_TZASC_XLBXR_MASK	(1 << 20)
#define CLK_TZASC_XR1BXW_MASK	(1 << 19)
#define CLK_TZASC_XR1BXR_MASK	(1 << 18)
#define CLK_DPHY1_MASK		(1 << 5)
#define CLK_DPHY0_MASK		(1 << 4)

/*
 * TZPC Register Value :
 * R0SIZE: 0x0 : Size of secured ram
 */
#define R0SIZE			0x0

/*
 * TZPC Decode Protection Register Value :
 * DECPROTXSET: 0xFF : Set Decode region to non-secure
 */
#define DECPROTXSET		0xFF

#define LPDDR3PHY_CTRL_PHY_RESET	(1 << 0)
#define LPDDR3PHY_CTRL_PHY_RESET_OFF	(0 << 0)

/* FIXME(dhendrix): misleading name. The reset value is 0x17021a40, bits 12:11
+   default to 0x3 which indicates LPDDR3. We want DDR3, so we use 0x1. */
#define PHY_CON0_RESET_VAL	0x17020a40
#define P0_CMD_EN		(1 << 14)
#define BYTE_RDLVL_EN		(1 << 13)
#define CTRL_SHGATE		(1 << 8)

#define PHY_CON1_RESET_VAL	0x09210100
#define RDLVL_PASS_ADJ_VAL	0x6
#define RDLVL_PASS_ADJ_OFFSET	16
#define CTRL_GATEDURADJ_MASK	(0xf << 20)
#define READ_LEVELLING_DDR3	0x0100

#define PHY_CON2_RESET_VAL	0x00010004
#define INIT_DESKEW_EN		(1 << 6)
#define DLL_DESKEW_EN		(1 << 12)
#define RDLVL_GATE_EN		(1 << 24)
#define RDLVL_EN		(1 << 25)
#define RDLVL_INCR_ADJ		(0x1 << 16)

/* DREX_PAUSE */
#define DREX_PAUSE_EN	(1 << 0)

#define BYPASS_EN	(1 << 22)

/********-----MEMMORY VAL----------***/
#define PHY_CON0_VAL	0x17021A00

#define PHY_CON12_RESET_VAL	0x10100070
#define PHY_CON12_VAL		0x10107F50
#define CTRL_START		(1 << 6)
#define CTRL_DLL_ON		(1 << 5)
#define CTRL_FORCE_MASK		(0x7F << 8)
#define CTRL_LOCK_COARSE_MASK	(0x7F << 10)

#define CTRL_OFFSETD_RESET_VAL	0x8
#define CTRL_OFFSETD_VAL	0x7F

#define CTRL_OFFSETR0		0x7F
#define CTRL_OFFSETR1		0x7F
#define CTRL_OFFSETR2		0x7F
#define CTRL_OFFSETR3		0x7F
#define PHY_CON4_VAL	(CTRL_OFFSETR0 << 0 | \
				CTRL_OFFSETR1 << 8 | \
				CTRL_OFFSETR2 << 16 | \
				CTRL_OFFSETR3 << 24)
#define PHY_CON4_RESET_VAL	0x08080808

#define CTRL_OFFSETW0		0x7F
#define CTRL_OFFSETW1		0x7F
#define CTRL_OFFSETW2		0x7F
#define CTRL_OFFSETW3		0x7F
#define PHY_CON6_VAL	(CTRL_OFFSETW0 << 0 | \
				CTRL_OFFSETW1 << 8 | \
				CTRL_OFFSETW2 << 16 | \
				CTRL_OFFSETW3 << 24)
#define PHY_CON6_RESET_VAL	0x08080808

#define PHY_CON14_RESET_VAL	0x001F0000
#define CTRL_PULLD_DQS		0xF
#define CTRL_PULLD_DQS_OFFSET	0

/*ZQ Configurations */
#define PHY_CON16_RESET_VAL	0x08000304

#define ZQ_CLK_EN		(1 << 27)
#define ZQ_CLK_DIV_EN		(1 << 18)
#define ZQ_MANUAL_MODE_OFFSET	2
#define ZQ_LONG_CALIBRATION	0x1
#define ZQ_MANUAL_STR		(1 << 1)
#define ZQ_DONE			(1 << 0)
#define ZQ_MODE_DDS_OFFSET	24

#define LONG_CALIBRATION	(ZQ_LONG_CALIBRATION << ZQ_MANUAL_MODE_OFFSET)

#define CTRL_RDLVL_GATE_ENABLE	1
#define CTRL_RDLVL_GATE_DISABLE	0

#define CTRL_RDLVL_DATA_ENABLE	(1 << 1)
/* Direct Command */
#define DIRECT_CMD_NOP			0x07000000
#define DIRECT_CMD_PALL			0x01000000
#define DIRECT_CMD_ZQINIT		0x0a000000
#define DIRECT_CMD_CHANNEL_SHIFT	28
#define DIRECT_CMD_CHIP_SHIFT		20
#define DIRECT_CMD_BANK_SHIFT		16
#define DIRECT_CMD_REFA		(5 << 24)
#define DIRECT_CMD_MRS1		0x71C00
#define DIRECT_CMD_MRS2		0x10BFC
#define DIRECT_CMD_MRS3		0x0050C
#define DIRECT_CMD_MRS4		0x00868
#define DIRECT_CMD_MRS5		0x00C04

/* Drive Strength */
#define IMPEDANCE_48_OHM	4
#define IMPEDANCE_40_OHM	5
#define IMPEDANCE_34_OHM	6
#define IMPEDANCE_30_OHM	7
#define PHY_CON39_VAL_48_OHM	0x09240924
#define PHY_CON39_VAL_40_OHM	0x0B6D0B6D
#define PHY_CON39_VAL_34_OHM	0x0DB60DB6
#define PHY_CON39_VAL_30_OHM	0x0FFF0FFF

#define CTRL_BSTLEN_OFFSET	8
#define CTRL_RDLAT_OFFSET	0

#define CMD_DEFAULT_LPDDR3	0xF
#define CMD_DEFAULT_OFFSET	0
#define T_WRDATA_EN		0x7
#define T_WRDATA_EN_DDR3	0x8	/* FIXME(dhendrix): 6 for DDR3? see T_wrdata_en */
#define T_WRDATA_EN_OFFSET	16
#define T_WRDATA_EN_MASK	0x1f

#define PHY_CON31_VAL	0x0C183060
#define PHY_CON32_VAL	0x60C18306
#define PHY_CON33_VAL	0x00000030

#define PHY_CON31_RESET_VAL	0x0
#define PHY_CON32_RESET_VAL	0x0
#define PHY_CON33_RESET_VAL	0x0

#define SL_DLL_DYN_CON_EN	(1 << 1)
#define FP_RESYNC	(1 << 3)
#define CTRL_START	(1 << 6)

#define DMC_AREF_EN		(1 << 5)
#define DMC_CONCONTROL_EMPTY	(1 << 8)
#define DFI_INIT_START		(1 << 28)

#define DMC_MEMCONTROL_VAL	0x00312700
#define CLK_STOP_EN		(1 << 0)
#define DPWRDN_EN		(1 << 1)
#define DSREF_EN		(1 << 5)

/* AXI base address mask */
#define DMC_CHIP_MASK_256MB	0x7f0
#define DMC_CHIP_MASK_512MB	0x7e0
#define DMC_CHIP_MASK_1GB	0x7c0
#define DMC_CHIP_MASK_2GB	0x780
#define DMC_CHIP_MASK_4GB	0x700

#define MEMCONFIG_VAL	0x1323
#define PRECHCONFIG_DEFAULT_VAL	0xFF000000
#define PWRDNCONFIG_DEFAULT_VAL	0xFFFF00FF

#define DFI_INIT_COMPLETE	(1 << 3)

#define BRBRSVCONTROL_VAL	0x00000033
#define BRBRSVCONFIG_VAL	0x88778877

/* Clock Gating Control (CGCONTROL) register */
#define MEMIF_CG_EN	(1 << 3) /* Memory interface clock gating */
#define SCG_CG_EN	(1 << 2) /* Scheduler clock gating */
#define BUSIF_WR_CG_EN	(1 << 1) /* Bus interface write channel clock gating */
#define BUSIF_RD_CG_EN	(1 << 0) /* Bus interface read channel clock gating */
#define DMC_INTERNAL_CG	(MEMIF_CG_EN | SCG_CG_EN | \
				 BUSIF_WR_CG_EN | BUSIF_RD_CG_EN)

/* DMC PHY Control0 register */
#define PHY_CONTROL0_RESET_VAL	0x0
#define MEM_TERM_EN	(1 << 31)	/* Termination enable for memory */
#define PHY_TERM_EN	(1 << 30)	/* Termination enable for PHY */
#define DMC_CTRL_SHGATE	(1 << 29)	/* Duration of DQS gating signal */
#define CTRL_ATGATE		(1 << 6)
#define FP_RSYNC	(1 << 3)	/* Force DLL resynchronization */

/* Driver strength for CK, CKE, CS & CA */
#define IMP_OUTPUT_DRV_40_OHM	0x5
#define IMP_OUTPUT_DRV_30_OHM	0x7
#define DA_3_DS_OFFSET		25
#define DA_2_DS_OFFSET		22
#define DA_1_DS_OFFSET		19
#define DA_0_DS_OFFSET		16
#define CA_CK_DRVR_DS_OFFSET	9
#define CA_CKE_DRVR_DS_OFFSET	6
#define CA_CS_DRVR_DS_OFFSET	3
#define CA_ADR_DRVR_DS_OFFSET	0

#define PHY_CON42_CTRL_BSTLEN_SHIFT	8
#define PHY_CON42_CTRL_RDLAT_SHIFT	0

struct mem_timings;

/* Errors that we can encounter in low-level setup */
enum {
	SETUP_ERR_OK,
	SETUP_ERR_RDLV_COMPLETE_TIMEOUT = -1,
	SETUP_ERR_ZQ_CALIBRATION_FAILURE = -2,
};

/* Functions common between LPDDR2 and DDR3 */

/* CPU info initialization code */
void cpu_info_init(void);

void mem_ctrl_init(void);
/*
 * Memory variant specific initialization code
 *
 * @param mem		Memory timings for this memory type.
 * @param mem_iv_size	Memory interleaving size is a configurable parameter
 *			which the DMC uses to decide how to split a memory
 *			chunk into smaller chunks to support concurrent
 *			accesses; may vary across boards.
 * @param mem_reset	Reset memory during initialization.
 * @return 0 if ok, SETUP_ERR_... if there is a problem
 */
int ddr3_mem_ctrl_init(struct mem_timings *mem, int interleave_size, int reset);

/* Memory variant specific initialization code for LPDDR3 */
int lpddr3_mem_ctrl_init(int reset);

/*
 * Configure ZQ I/O interface
 *
 * @param mem		Memory timings for this memory type.
 * @param phy0_ctrl	Pointer to struct containing PHY0 control reg
 * @param phy1_ctrl	Pointer to struct containing PHY1 control reg
 * @return 0 if ok, -1 on error
 */
int dmc_config_zq(struct mem_timings *mem,
		  struct exynos5_phy_control *phy0_ctrl,
		  struct exynos5_phy_control *phy1_ctrl);

/*
 * Send NOP and MRS/EMRS Direct commands
 *
 * @param mem		Memory timings for this memory type.
 * @param dmc		Pointer to struct of DMC registers
 */
void dmc_config_mrs(struct mem_timings *mem, struct exynos5_dmc *dmc);

/*
 * Send PALL Direct commands
 *
 * @param mem		Memory timings for this memory type.
 * @param dmc		Pointer to struct of DMC registers
 */
void dmc_config_prech(struct mem_timings *mem, struct exynos5_dmc *dmc);

/*
 * Configure the memconfig and membaseconfig registers
 *
 * @param mem		Memory timings for this memory type.
 * @param exynos5_dmc	Pointer to struct of DMC registers
 */
void dmc_config_memory(struct mem_timings *mem, struct exynos5_dmc *dmc);

/*
 * Reset the DLL. This function is common between DDR3 and LPDDR2.
 * However, the reset value is different. So we are passing a flag
 * ddr_mode to distinguish between LPDDR2 and DDR3.
 *
 * @param exynos5_dmc	Pointer to struct of DMC registers
 * @param ddr_mode	Type of DDR memory
 */
void update_reset_dll(struct exynos5_dmc *, enum ddr_mode);
#endif
