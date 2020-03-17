/*
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __SOC_QUALCOMM_QCS405_IOMAP_H_
#define __SOC_QUALCOMM_QCS405_IOMAP_H_

#include <device/mmio.h>
#include <soc/cdp.h>
#include <soc/blsp.h>

/* Typecast to allow integers being passed as address
   This needs to be included because vendor code is not compliant with our
   macros for read/write. Hence, special macros for readl_i and writel_i are
   included to do this in one place for all occurrences in vendor code
 */
#define readl_i(a)           read32((const void *)(a))
#define writel_i(v, a)       write32((void *)a, v)
#define clrsetbits32_i(addr, clear, set)  \
	clrsetbits32(((void *)(addr)), (clear), (set))

#define GCC_CLK_CTL_REG			((void *)0x01800000u)
#define MSM_CLK_CTL_BASE		GCC_CLK_CTL_REG
#define GCC_CLK_BRANCH_ENA		(GCC_CLK_CTL_REG + 0x6000)
#define		IMEM_AXI		(1 << 17)
#define		SYS_NOC_APSS_AHB	(1 << 16)
#define		BIMC_AXI_M0		(1 << 15)
#define		APSS_AHB		(1 << 14)
#define		APSS_AXI		(1 << 13)
#define		MPM_AHB			(1 << 12)
#define		GMEM_SYS_NOC_AXI	(1 << 11)
#define		BLSP1_AHB		(1 << 10)
#define		BLSP1_SLEEP		(1 << 9)
#define		PRNG_AHB		(1 << 8)
#define		BOOT_ROM_AHB		(1 << 7)
#define		MSG_RAM_AHB		(1 << 6)
#define		TLMM_AHB		(1 << 5)
#define		TLMM			(1 << 4)
#define		SPMI_PCNOC_AHB		(1 << 3)
#define		CRYPTO			(1 << 2)
#define		CRYPTO_AXI		(1 << 1)
#define		CRYPTO_AHB		(1 << 0)

#define GCC_BLSP1_QUP1_I2C_APPS_CBCR		(MSM_CLK_CTL_BASE + 0x2008)
#define GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR	(MSM_CLK_CTL_BASE + 0x200c)
#define GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR	(MSM_CLK_CTL_BASE + 0x2010)
#define GCC_BLSP1_QUP2_I2C_APPS_CBCR		(MSM_CLK_CTL_BASE + 0x3010)
#define GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR	(MSM_CLK_CTL_BASE + 0x3000)
#define GCC_BLSP1_QUP2_I2C_APPS_CFG_RCGR	(MSM_CLK_CTL_BASE + 0x3004)

#define GCNT_GLOBAL_CTRL_BASE	((void *)0x004a0000u)
#define GCNT_CNTCR		(GCNT_GLOBAL_CTRL_BASE + 0x1000)
#define GCNT_GLB_CNTCV_LO	(GCNT_GLOBAL_CTRL_BASE + 0x1008)
#define GCNT_GLB_CNTCV_HI	(GCNT_GLOBAL_CTRL_BASE + 0x100c)
#define GCNT_CNTCV_LO		(GCNT_GLOBAL_CTRL_BASE + 0x2000)
#define GCNT_CNTCV_HI		(GCNT_GLOBAL_CTRL_BASE + 0x2004)

#define GCNT_PSHOLD		((void *)0x004AB000u)

/* RPM interface constants */
#define RPM_INT           ((void *)0x63020)
#define RPM_INT_ACK       ((void *)0x63060)
#define RPM_SIGNAL_COOKIE ((void *)0x47C20)
#define RPM_SIGNAL_ENTRY  ((void *)0x47C24)
#define RPM_FW_MAGIC_NUM 0x4D505242

#define TLMM_BASE_ADDR      ((void *)0x01000000)
#define GPIO_CONFIG_ADDR(x) (TLMM_BASE_ADDR + 0x1000 * (x))
#define GPIO_IN_OUT_ADDR(x) (GPIO_CONFIG_ADDR(x) + 4)

/* Yes, this is not a typo... host2 is actually mapped before host1. */
#define USB_HOST2_XHCI_BASE	0x10000000
#define USB_HOST2_DWC3_BASE	0x1000C100
#define USB_HOST2_PHY_BASE	0x100F8800
#define USB_HOST1_XHCI_BASE	0x11000000
#define USB_HOST1_DWC3_BASE	0x1100C100
#define USB_HOST1_PHY_BASE	0x110F8800

#define UART1_DM_BASE		((void *)0x078af000)
#define UART2_DM_BASE		((void *)0x078B1000)

enum {
	BLSP1_UART1,
	BLSP1_UART2,
};

#define GCC_BLSP1_UART_BCR_BASE		(GCC_CLK_CTL_REG + 0x2038)
#define GCC_BLSP1_UART_BCR(x)		(GCC_BLSP1_UART_BCR_BASE + (x) * 0xff0)
#define GCC_BLSP1_UART_APPS_CBCR(x)	(GCC_BLSP1_UART_BCR(x) + 4)
#define GCC_BLSP1_UART_APPS_CMD_RCGR(x)	(GCC_BLSP1_UART_APPS_CBCR(x) + 8)
#define GCC_BLSP1_UART_APPS_CFG_RCGR(x)	(GCC_BLSP1_UART_APPS_CMD_RCGR(x) + 4)
#define GCC_BLSP1_UART_APPS_M(x)	(GCC_BLSP1_UART_APPS_CFG_RCGR(x) + 4)
#define GCC_BLSP1_UART_APPS_N(x)	(GCC_BLSP1_UART_APPS_M(x) + 4)
#define GCC_BLSP1_UART_APPS_D(x)	(GCC_BLSP1_UART_APPS_N(x) + 4)
#define GCC_BLSP1_UART_MISC(x)		(GCC_BLSP1_UART_APPS_D(x) + 4)

#define BLSP1_QUP0_BASE		((void *)0x078B5000)
#define BLSP1_QUP1_BASE		((void *)0x078B6000)
#define BLSP1_QUP2_BASE		((void *)0x078B7000)
#define BLSP1_QUP3_BASE		((void *)0x078B8000)

#define TCSR_BOOT_MISC_DETECT		((void *)0x0193D100)
#define TCSR_RESET_DEBUG_SW_ENTRY	((void *)0x01940000)

static inline void *blsp_qup_base(blsp_qup_id_t id)
{
	switch (id) {
	case BLSP_QUP_ID_0: return BLSP1_QUP0_BASE;
	case BLSP_QUP_ID_1: return BLSP1_QUP1_BASE;
	case BLSP_QUP_ID_2: return BLSP1_QUP2_BASE;
	case BLSP_QUP_ID_3: return BLSP1_QUP3_BASE;
	}
	return NULL;
}

#define BLSP_MINI_CORE_SHIFT		8
#define BLSP_MINI_CORE_I2C		(0x2u << BLSP_MINI_CORE_SHIFT)
#define BLSP_MINI_CORE_MASK		(0xfu << BLSP_MINI_CORE_SHIFT)

#define ETIMEDOUT -10
#define EINVAL -11
#define EIO -12

#endif
