/*
 * Copyright (c) 2012 - 2013, 2015 The Linux Foundation. All rights reserved.
 *
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2012, Code Aurora Forum. All rights reserved.
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

#ifndef __SOC_QUALCOMM_IPQ806X_IOMAP_H_
#define __SOC_QUALCOMM_IPQ806X_IOMAP_H_

#include <arch/io.h>
#include <soc/cdp.h>

/* Typecast to allow integers being passed as address
   This needs to be included because vendor code is not compliant with our
   macros for read/write. Hence, special macros for readl_i and writel_i are
   included to do this in one place for all occurrences in vendor code
 */
#define readl_i(a)           read32((const void *)(a))
#define writel_i(v,a)        write32((void *)a, v)
#define clrsetbits_le32_i(addr, clear, set)  \
	clrsetbits_le32(((void *)(addr)), (clear), (set))

#define MSM_CLK_CTL_BASE    ((void *)0x00900000)

#define MSM_TMR_BASE        ((void *)0x0200A000)
#define MSM_GPT_BASE        (MSM_TMR_BASE + 0x04)
#define MSM_DGT_BASE        (MSM_TMR_BASE + 0x24)

#define GPT_REG(off)        (MSM_GPT_BASE + (off))
#define DGT_REG(off)        (MSM_DGT_BASE + (off))

#define APCS_WDT0_EN        (MSM_TMR_BASE + 0x0040)
#define APCS_WDT0_RST       (MSM_TMR_BASE + 0x0038)
#define APCS_WDT0_BARK_TIME (MSM_TMR_BASE + 0x004C)
#define APCS_WDT0_BITE_TIME (MSM_TMR_BASE + 0x005C)

#define APCS_WDT0_CPU0_WDOG_EXPIRED_ENABLE (MSM_CLK_CTL_BASE + 0x3820)

#define GPT_MATCH_VAL        GPT_REG(0x0000)
#define GPT_COUNT_VAL        GPT_REG(0x0004)
#define GPT_ENABLE           GPT_REG(0x0008)
#define GPT_CLEAR            GPT_REG(0x000C)

#define GPT1_MATCH_VAL       GPT_REG(0x00010)
#define GPT1_COUNT_VAL       GPT_REG(0x00014)
#define GPT1_ENABLE          GPT_REG(0x00018)
#define GPT1_CLEAR           GPT_REG(0x0001C)

#define DGT_MATCH_VAL        DGT_REG(0x0000)
#define DGT_COUNT_VAL        DGT_REG(0x0004)
#define DGT_ENABLE           DGT_REG(0x0008)
#define DGT_CLEAR            DGT_REG(0x000C)
#define DGT_CLK_CTL          DGT_REG(0x0010)

/* RPM interface constants */
#define RPM_INT           ((void *)0x63020)
#define RPM_INT_ACK       ((void *)0x63060)
#define RPM_SIGNAL_COOKIE ((void *)0x47C20)
#define RPM_SIGNAL_ENTRY  ((void *)0x47C24)
#define RPM_FW_MAGIC_NUM 0x4D505242

#define TLMM_BASE_ADDR      ((void *)0x00800000)
#define GPIO_CONFIG_ADDR(x) (TLMM_BASE_ADDR + 0x1000 + (x)*0x10)
#define GPIO_IN_OUT_ADDR(x) (GPIO_CONFIG_ADDR(x) + 4)

/* Yes, this is not a typo... host2 is actually mapped before host1. */
#define USB_HOST2_XHCI_BASE	0x10000000
#define USB_HOST2_DWC3_BASE	0x1000C100
#define USB_HOST2_PHY_BASE	0x100F8800
#define USB_HOST1_XHCI_BASE	0x11000000
#define USB_HOST1_DWC3_BASE	0x1100C100
#define USB_HOST1_PHY_BASE	0x110F8800

#define GSBI_4			4
#define UART1_DM_BASE		0x12450000
#define UART_GSBI1_BASE		0x12440000
#define UART2_DM_BASE		0x12490000
#define UART_GSBI2_BASE		0x12480000
#define UART4_DM_BASE		0x16340000
#define UART_GSBI4_BASE		0x16300000

#define UART2_DM_BASE           0x12490000
#define UART_GSBI2_BASE         0x12480000

#define GSBI1_BASE		((void *)0x12440000)
#define GSBI2_BASE		((void *)0x12480000)
#define GSBI3_BASE		((void *)0x16200000)
#define GSBI4_BASE		((void *)0x16300000)
#define GSBI5_BASE		((void *)0x1A200000)
#define GSBI6_BASE		((void *)0x16500000)
#define GSBI7_BASE		((void *)0x16600000)

#define GSBI1_CTL_REG		(GSBI1_BASE + (0x0))
#define GSBI2_CTL_REG		(GSBI2_BASE + (0x0))
#define GSBI3_CTL_REG		(GSBI3_BASE + (0x0))
#define GSBI4_CTL_REG		(GSBI4_BASE + (0x0))
#define GSBI5_CTL_REG		(GSBI5_BASE + (0x0))
#define GSBI6_CTL_REG		(GSBI6_BASE + (0x0))
#define GSBI7_CTL_REG		(GSBI7_BASE + (0x0))

#define GSBI_QUP1_BASE		(GSBI1_BASE + 0x20000)
#define GSBI_QUP2_BASE		(GSBI2_BASE + 0x20000)
#define GSBI_QUP3_BASE		(GSBI3_BASE + 0x80000)
#define GSBI_QUP4_BASE		(GSBI4_BASE + 0x80000)
#define GSBI_QUP5_BASE		(GSBI5_BASE + 0x80000)
#define GSBI_QUP6_BASE		(GSBI6_BASE + 0x80000)
#define GSBI_QUP7_BASE		(GSBI7_BASE + 0x80000)

#define GSBI_CTL_PROTO_I2C		2
#define GSBI_CTL_PROTO_CODE_SFT		4
#define GSBI_CTL_PROTO_CODE_MSK		0x7
#define GSBI_HCLK_CTL_GATE_ENA		6
#define GSBI_HCLK_CTL_BRANCH_ENA	4
#define GSBI_QUP_APPS_M_SHFT		16
#define GSBI_QUP_APPS_M_MASK		0xFF
#define GSBI_QUP_APPS_D_SHFT		0
#define GSBI_QUP_APPS_D_MASK		0xFF
#define GSBI_QUP_APPS_N_SHFT		16
#define GSBI_QUP_APPS_N_MASK		0xFF
#define GSBI_QUP_APPS_ROOT_ENA_SFT	11
#define GSBI_QUP_APPS_BRANCH_ENA_SFT	9
#define GSBI_QUP_APPS_MNCTR_EN_SFT	8
#define GSBI_QUP_APPS_MNCTR_MODE_MSK	0x3
#define GSBI_QUP_APPS_MNCTR_MODE_SFT	5
#define GSBI_QUP_APPS_PRE_DIV_MSK	0x3
#define GSBI_QUP_APPS_PRE_DIV_SFT	3
#define GSBI_QUP_APPS_SRC_SEL_MSK	0x7


#define GSBI_QUP_APSS_MD_REG(gsbi_n)	((MSM_CLK_CTL_BASE + 0x29c8) + \
							(32*(gsbi_n-1)))
#define GSBI_QUP_APSS_NS_REG(gsbi_n)	((MSM_CLK_CTL_BASE + 0x29cc) + \
							(32*(gsbi_n-1)))
#define GSBI_HCLK_CTL(n)		((MSM_CLK_CTL_BASE + 0x29C0) + \
							(32*(n-1)))
#endif // __SOC_QUALCOMM_IPQ806X_IOMAP_H_
