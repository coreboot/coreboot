/* Source : APQ8064 LK Boot */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __IPQ860X_CLOCK_H_
#define __IPQ860X_CLOCK_H_

#include <soc/iomap.h>
#include <types.h>

/* UART clock @ 7.3728 MHz */
#define UART_DM_CLK_RX_TX_BIT_RATE 0xCC

/* UART specific definitions */

#define Uart_ns_val  NS(BIT_POS_31,BIT_POS_16,N_VALUE,M_VALUE, 5, 4, 3, 1, 2, 0,3)
#define Uart_clk_ns_mask  (BM(BIT_POS_31, BIT_POS_16) | BM(BIT_POS_6, BIT_POS_0))
#define Uart_mnd_en_mask  BIT(8) * !!(625)
#define Uart_en_mask  BIT(11)
#define MD16(m, n) (BVAL(BIT_POS_31, BIT_POS_16, m) | BVAL(BIT_POS_15, BIT_POS_0, ~(n)))
#define Uart_ns_val_rumi  NS(BIT_POS_31, BIT_POS_16, N_VALUE, M_VALUE, 5, 4, 3, 1, 2, 0,0)
#define GSBIn_UART_APPS_MD_REG(n)           REG(0x29D0+(0x20*((n)-1)))
#define GSBIn_UART_APPS_NS_REG(n)           REG(0x29D4+(0x20*((n)-1)))
#define GSBIn_HCLK_CTL_REG(n)               REG(0x29C0+(0x20*((n)-1)))
#define BB_PLL_ENA_SC0_REG                  REG(0x34C0)
#define BB_PLL8_STATUS_REG                  REG(0x3158)
#define REG(off)        ((void *)(MSM_CLK_CTL_BASE + (off)))
#define PLL8_STATUS_BIT                     16

#define PLL_LOCK_DET_STATUS_REG             REG(0x03420)
#define SFAB_AHB_S3_FCLK_CTL_REG            REG(0x0216C)
#define CFPB_CLK_NS_REG                     REG(0x0264C)
#define CFPB0_HCLK_CTL_REG                  REG(0x02650)
#define SFAB_CFPB_S_HCLK_CTL_REG            REG(0x026C0)
#define CFPB_SPLITTER_HCLK_CTL_REG          REG(0x026E0)
#define EBI2_CLK_CTL_REG                    REG(0x03B00)

#define USB30_MASTER_CLK_CTL_REG            REG(0x3b24)
#define USB30_MASTER_CLK_MD                 REG(0x3b28)
#define USB30_MASTER_CLK_NS                 REG(0x3b2c)
#define USB30_1_MASTER_CLK_CTL_REG          REG(0x3b34)
#define USB30_MOC_UTMI_CLK_MD               REG(0x3b40)
#define USB30_MOC_UTMI_CLK_NS               REG(0x3b44)
#define USB30_MOC_UTMI_CLK_CTL              REG(0x3b48)
#define USB30_1_MOC_UTMI_CLK_CTL            REG(0x3b4c)
#define USB30_RESET                         REG(0x3b50)

#define ALWAYS_ON_CLK_BRANCH_ENA(i)         ((i) << 8)

#define CLK_BRANCH_ENA_MASK                 0x00000010
#define CLK_BRANCH_ENA_ENABLE               0x00000010
#define CLK_BRANCH_ENA_DISABLE              0x00000000
#define CLK_BRANCH_ENA(i)                   ((i) << 4)

/* Register: CFPB_CLK_NS */
#define CLK_DIV_MASK                        0x00000003
#define CLK_DIV_DIV_1                       0x00000000
#define CLK_DIV_DIV_2                       0x00000001
#define CLK_DIV_DIV_3                       0x00000002
#define CLK_DIV_DIV_4                       0x00000003
#define CLK_DIV(i)                          ((i) << 0)

#define MN_MODE_DUAL_EDGE 0x2
#define BIT_POS_31 31
#define BIT_POS_16 16
#define BIT_POS_6  6
#define BIT_POS_0  0
#define BIT_POS_15 15

#define BM(m, l) (((((unsigned int)-1) << (31-m)) >> (31-m+l)) << l)
#define BVAL(m, l, val)     (((val) << l) & BM(m, l))

/* MD Registers */
#define MD4(m_lsb, m, n_lsb, n) \
    (BVAL((m_lsb+3), m_lsb, m) | BVAL((n_lsb+3), n_lsb, ~(n)))

#define MD8(m_lsb, m, n_lsb, n) \
    (BVAL((m_lsb+7), m_lsb, m) | BVAL((n_lsb+7), n_lsb, ~(n)))

/* NS Registers */
#define NS(n_msb, n_lsb, n, m, mde_lsb, d_msb, d_lsb, d, s_msb, s_lsb, s) \
    (BVAL(n_msb, n_lsb, ~(n-m)) \
     | (BVAL((mde_lsb+1), mde_lsb, MN_MODE_DUAL_EDGE) * !!(n)) \
     | BVAL(d_msb, d_lsb, (d-1)) | BVAL(s_msb, s_lsb, s))

#define NS_MM(n_msb, n_lsb, n, m, d_msb, d_lsb, d, s_msb, s_lsb, s) \
    (BVAL(n_msb, n_lsb, ~(n-m)) | BVAL(d_msb, d_lsb, (d-1)) \
     | BVAL(s_msb, s_lsb, s))

#define NS_DIVSRC(d_msb , d_lsb, d, s_msb, s_lsb, s) \
    (BVAL(d_msb, d_lsb, (d-1)) | BVAL(s_msb, s_lsb, s))

#define NS_DIV(d_msb , d_lsb, d) \
    BVAL(d_msb, d_lsb, (d-1))

#define NS_SRC_SEL(s_msb, s_lsb, s) \
    BVAL(s_msb, s_lsb, s)

#define GMAC_AHB_RESET			0x903E24

#define SRC_SEL_PLL0			(0x2 << 0)
#define MNCNTR_MODE_DUAL_EDGE		(0x2 << 5)
#define MNCNTR_ENABLE			(0x1 << 8)
#define MNCNTR_RST_ACTIVE		(0x1 << 7)
#define N_VAL				15

#define GMAC_CORE_RESET(n)	\
		((void *)(0x903CBC + ((n) * 0x20)))

#define GMACSEC_CORE_RESET(n)	\
		((void *)(0x903E28 + ((n - 1) * 4)))

#define GMAC_COREn_CLCK_SRC_CTL(N)	\
		(0x00900000 + (0x3CA0 + (32*(N-1))))

#define GMAC_COREn_CLCK_SRC0_MD(N)	\
		(0x00900000 + (0x3CA4 + (32*(N-1))))

#define GMAC_COREn_CLCK_SRC1_MD(N)	\
		(0x00900000 + (0x3CA8	+ (32*(N-1))))

#define GMAC_COREn_CLCK_SRC0_NS(N)	\
		(0x00900000 + (0x3CAC + (32*(N-1))))

#define GMAC_COREn_CLCK_SRC1_NS(N)	\
		(0x00900000 + (0x3CB0 + (32*(N-1))))

#define DISABLE_DUAL_MN8_SEL		(0)
#define DISABLE_CLK_LOW_PWR		(0 << 2)
#define GMAC_CORE_CLCK_ROOT_ENABLE	(1 << 1)

/* GMAC_COREn_CLK_SRC[0,1]_MD register bits (Assuming 133MHz) */
#define GMAC_CORE_CLCK_M		0x32
#define GMAC_CORE_CLCK_D		0	/* NOT(2*D) value */
#define GMAC_CORE_CLCK_M_SHIFT		16
#define GMAC_CORE_CLCK_D_SHIFT		0
#define GMAC_CORE_CLCK_M_VAL		(GMAC_CORE_CLCK_M << GMAC_CORE_CLCK_M_SHIFT)
#define GMAC_CORE_CLCK_D_VAL		(GMAC_CORE_CLCK_D << GMAC_CORE_CLCK_D_SHIFT)

/* GMAC_COREn_CLK_SRC[0,1]_NS register bits (Assuming 133MHz) */
#define GMAC_CORE_CLCK_N			0x4		/* NOT(N-M) value, N=301 */
#define GMAC_CORE_CLCK_N_SHIFT			16
#define GMAC_CORE_CLCK_N_VAL			(GMAC_CORE_CLCK_N << GMAC_CORE_CLCK_N_SHIFT)
#define GMAC_CORE_CLCK_MNCNTR_EN		0x00000100	/* Enable M/N counter */
#define GMAC_CORE_CLCK_MNCNTR_RST		0x00000080	/* Activate reset for M/N counter */
#define GMAC_CORE_CLCK_MNCNTR_MODE_MASK		0x00000060	/* M/N counter mode mask */
#define GMAC_CORE_CLCK_MNCNTR_MODE_SHIFT		5
#define GMAC_CORE_CLCK_MNCNTR_MODE_DUAL		(2 << GMAC_CORE_CLCK_MNCNTR_MODE_SHIFT) /* M/N counter mode dual-edge */
#define GMAC_CORE_CLCK_PRE_DIV_SEL_MASK		0x00000018	/* Pre divider select mask */
#define GMAC_CORE_CLCK_PRE_DIV_SEL_SHIFT		3
#define GMAC_CORE_CLCK_PRE_DIV_SEL_BYP		(0 << GMAC_CORE_CLCK_PRE_DIV_SEL_SHIFT)  /* Pre divider bypass */
#define GMAC_CORE_CLCK_SRC_SEL_MASK		0x00000007	/* clk source Mux select mask */
#define GMAC_CORE_CLCK_SRC_SEL_SHIFT		0
#define GMAC_CORE_CLCK_SRC_SEL_PLL0		(2 << GMAC_CORE_CLCK_SRC_SEL_SHIFT)	/* output of clk source Mux is PLL0 */
#define GMAC_COREn_CLCK_CTL(N)			(0x00900000 + (0x3CB4 + (32*(N-1))))

#define GMAC_COREn_CLCK_INV_DISABLE		(0 << 5)
#define GMAC_COREn_CLCK_BRANCH_ENA		(1 << 4)

/* Uart specific clock settings */

void uart_pll_vote_clk_enable(unsigned int);
void uart_clock_config(unsigned int gsbi_port, unsigned int m, unsigned int n,
		unsigned int d, unsigned int clk_dummy);
void nand_clock_config(void);
void usb_clock_config(void);
int audio_clock_config(unsigned int frequency);

#endif /* __IPQ860X_CLOCK_H_ */
