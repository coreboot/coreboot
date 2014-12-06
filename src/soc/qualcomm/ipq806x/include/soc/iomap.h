/*
 * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.
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
#define writel_i(v,a)        write32(v,(void *)a)
#define clrsetbits_le32_i(addr, clear, set)  \
	clrsetbits_le32(((void *)(addr)), (clear), (set))

#define MSM_CLK_CTL_BASE    0x00900000

#define MSM_TMR_BASE        0x0200A000
#define MSM_GPT_BASE        (MSM_TMR_BASE + 0x04)
#define MSM_DGT_BASE        (MSM_TMR_BASE + 0x24)

#define GPT_REG(off)        ((void *)(MSM_GPT_BASE + (off)))
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

#define TLMM_BASE_ADDR      ((char *)0x00800000)
#define GPIO_CONFIG_ADDR(x) (TLMM_BASE_ADDR + 0x1000 + (x)*0x10)
#define GPIO_IN_OUT_ADDR(x) (GPIO_CONFIG_ADDR(x) + 4)

/* Yes, this is not a typo... host2 is actually mapped before host1. */
#define USB_HOST2_XHCI_BASE	0x10000000
#define USB_HOST2_DWC3_BASE	0x1000C100
#define USB_HOST2_PHY_BASE	0x100F8800
#define USB_HOST1_XHCI_BASE	0x11000000
#define USB_HOST1_DWC3_BASE	0x1100C100
#define USB_HOST1_PHY_BASE	0x110F8800

#define GSBI_1			1
#define GSBI_2			2
#define GSBI_4			4
#define GSBI_2                  2
#define UART1_DM_BASE         	0x12450000
#define UART_GSBI1_BASE       	0x12440000
#define UART2_DM_BASE		0x12490000
#define UART_GSBI2_BASE		0x12480000
#define UART4_DM_BASE         	0x16340000
#define UART_GSBI4_BASE       	0x16300000

#define UART2_DM_BASE           0x12490000
#define UART_GSBI2_BASE         0x12480000

#define GSBI_QUP1_BASE		0x12460000
#define GSBI_QUP2_BASE		0x124A0000
#define GSBI_QUP3_BASE		0x16280000
#define GSBI_QUP4_BASE		0x16380000
#define GSBI_QUP5_BASE		0x1A280000
#define GSBI_QUP6_BASE		0x16580000
#define GSBI_QUP7_BASE		0x16680000
#endif // __SOC_QUALCOMM_IPQ806X_IOMAP_H_
