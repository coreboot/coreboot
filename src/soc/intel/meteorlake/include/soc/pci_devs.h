/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_METEORLAKE_PCI_DEVS_H_
#define _SOC_METEORLAKE_PCI_DEVS_H_

#include <device/pci_def.h>

#define _PCI_DEVFN(slot, func)	PCI_DEVFN(PCI_DEV_SLOT_ ## slot, func)
#if !defined(__SIMPLE_DEVICE__)
#include <device/device.h>
#define _PCI_DEV(slot, func)	pcidev_path_on_root_debug(_PCI_DEVFN(slot, func), __func__)
#else
#define _PCI_DEV(slot, func)	PCI_DEV(0, PCI_DEV_SLOT_ ## slot, func)
#endif

/* System Agent Devices */
#define PCI_DEV_SLOT_ROOT	0x00
#define  PCI_DEVFN_ROOT		_PCI_DEVFN(ROOT, 0)
#if defined(__SIMPLE_DEVICE__)
#define  PCI_DEV_ROOT		_PCI_DEV(ROOT, 0)
#endif

#define PCI_DEV_SLOT_PCIE_3	0x1
#define  PCI_DEVFN_PCIE12	_PCI_DEVFN(PCIE_3, 0)
#define  PCI_DEV_PCIE12		_PCI_DEV(PCIE_3, 0)

#define PCI_DEV_SLOT_IGD	0x02
#define  PCI_DEVFN_IGD		_PCI_DEVFN(IGD, 0)
#define  PCI_DEV_IGD		_PCI_DEV(IGD, 0)

#define PCI_DEV_SLOT_DPTF	0x04
#define  PCI_DEVFN_DPTF		_PCI_DEVFN(DPTF, 0)
#define  PCI_DEV_DPTF		_PCI_DEV(DPTF, 0)

#define PCI_DEV_SLOT_IPU	0x05
#define  PCI_DEVFN_IPU		_PCI_DEVFN(IPU, 0)
#define  PCI_DEV_IPU		_PCI_DEV(IPU, 0)

#define PCI_DEV_SLOT_PCIE_2	0x6
#define  PCI_DEVFN_PCIE9	_PCI_DEVFN(PCIE_2, 0)
#define  PCI_DEVFN_PCIE10	_PCI_DEVFN(PCIE_2, 1)
#define  PCI_DEVFN_PCIE11	_PCI_DEVFN(PCIE_2, 2)
#define  PCI_DEV_PCIE9		_PCI_DEV(PCIE_2, 0)
#define  PCI_DEV_PCIE10		_PCI_DEV(PCIE_2, 1)
#define  PCI_DEV_PCIE11		_PCI_DEV(PCIE_2, 2)

#define PCI_DEV_SLOT_TBT	0x07
#define PCI_DEVFN_TBT(x)	_PCI_DEVFN(TBT, (x))
#define NUM_TBT_FUNCTIONS	4
#define  PCI_DEVFN_TBT0		_PCI_DEVFN(TBT, 0)
#define  PCI_DEVFN_TBT1		_PCI_DEVFN(TBT, 1)
#define  PCI_DEVFN_TBT2		_PCI_DEVFN(TBT, 2)
#define  PCI_DEVFN_TBT3		_PCI_DEVFN(TBT, 3)
#define  PCI_DEV_TBT0		_PCI_DEV(TBT, 0)
#define  PCI_DEV_TBT1		_PCI_DEV(TBT, 1)
#define  PCI_DEV_TBT2		_PCI_DEV(TBT, 2)
#define  PCI_DEV_TBT3		_PCI_DEV(TBT, 3)

#define PCI_DEV_SLOT_GNA	0x08
#define  PCI_DEVFN_GNA		_PCI_DEVFN(GNA, 0)
#define  PCI_DEV_GNA		_PCI_DEV(GNA, 0)

#define PCI_DEV_SLOT_TCSS	0x0d
#define NUM_TCSS_DMA_FUNCTIONS	2
#define  PCI_DEVFN_TCSS_DMA(x)	_PCI_DEVFN(TCSS, ((x) + 2))
#define  PCI_DEVFN_TCSS_XHCI	_PCI_DEVFN(TCSS, 0)
#define  PCI_DEVFN_TCSS_XDCI	_PCI_DEVFN(TCSS, 1)
#define  PCI_DEVFN_TCSS_DMA0	_PCI_DEVFN(TCSS, 2)
#define  PCI_DEVFN_TCSS_DMA1	_PCI_DEVFN(TCSS, 3)
#define  PCI_DEV_TCSS_XHCI	_PCI_DEV(TCSS, 0)
#define  PCI_DEV_TCSS_XDCI	_PCI_DEV(TCSS, 1)
#define  PCI_DEV_TCSS_DMA0	_PCI_DEV(TCSS, 2)
#define  PCI_DEV_TCSS_DMA1	_PCI_DEV(TCSS, 3)

#define PCI_DEV_SLOT_VMD	0x0e
#define  PCI_DEVFN_VMD		_PCI_DEVFN(VMD, 0)
#define  PCI_DEV_VMD		_PCI_DEV(VMD, 0)

#define PCI_DEV_SLOT_THC	0x10
#define  PCI_DEVFN_THC0		_PCI_DEVFN(THC, 0)
#define  PCI_DEVFN_THC1		_PCI_DEVFN(THC, 1)
#define  PCI_DEV_THC0		_PCI_DEV(THC, 0)
#define  PCI_DEV_THC1		_PCI_DEV(THC, 1)

#define PCI_DEV_SLOT_ISH	0x12
#define  PCI_DEVFN_ISH		_PCI_DEVFN(ISH, 0)
#define  PCI_DEVFN_GSPI2	_PCI_DEVFN(ISH, 6)
#define  PCI_DEVFN_UFS		_PCI_DEVFN(ISH, 7)
#define  PCI_DEV_ISH		_PCI_DEV(ISH, 0)
#define  PCI_DEV_GSPI2		_PCI_DEV(ISH, 6)
#define  PCI_DEV_UFS		_PCI_DEV(ISH, 7)

#define PCI_DEV_SLOT_IOE	0x13
#define  PCI_DEVFN_IOE_P2SB	_PCI_DEVFN(IOE, 0)
#define  PCI_DEVFN_IOE_PMC	_PCI_DEVFN(IOE, 2)
#define  PCI_DEVFN_IOE_SRAM	_PCI_DEVFN(IOE, 3)
#define  PCI_DEV_IOE_P2SB	_PCI_DEV(IOE, 0)
#define  PCI_DEV_IOE_PMC	_PCI_DEV(IOE, 2)
#define  PCI_DEV_IOE_SRAM	_PCI_DEV(IOE, 3)

#define PCI_DEV_SLOT_XHCI	0x14
#define  PCI_DEVFN_XHCI		_PCI_DEVFN(XHCI, 0)
#define  PCI_DEVFN_USBOTG	_PCI_DEVFN(XHCI, 1)
#define  PCI_DEVFN_SRAM		_PCI_DEVFN(XHCI, 2)
#define  PCI_DEVFN_CNVI_WIFI	_PCI_DEVFN(XHCI, 3)
#define  PCI_DEV_XHCI		_PCI_DEV(XHCI, 0)
#define  PCI_DEV_USBOTG		_PCI_DEV(XHCI, 1)
#define  PCI_DEV_SRAM		_PCI_DEV(XHCI, 2)
#define  PCI_DEV_CNVI_WIFI	_PCI_DEV(XHCI, 3)

#define PCI_DEV_SLOT_SIO0	0x15
#define  PCI_DEVFN_I2C0		_PCI_DEVFN(SIO0, 0)
#define  PCI_DEVFN_I2C1		_PCI_DEVFN(SIO0, 1)
#define  PCI_DEVFN_I2C2		_PCI_DEVFN(SIO0, 2)
#define  PCI_DEVFN_I2C3		_PCI_DEVFN(SIO0, 3)
#define  PCI_DEV_I2C0		_PCI_DEV(SIO0, 0)
#define  PCI_DEV_I2C1		_PCI_DEV(SIO0, 1)
#define  PCI_DEV_I2C2		_PCI_DEV(SIO0, 2)
#define  PCI_DEV_I2C3		_PCI_DEV(SIO0, 3)

#define PCI_DEV_SLOT_CSE	0x16
#define  PCI_DEVFN_CSE		_PCI_DEVFN(CSE, 0)
#define  PCI_DEVFN_CSE_2	_PCI_DEVFN(CSE, 1)
#define  PCI_DEVFN_CSE_IDER	_PCI_DEVFN(CSE, 2)
#define  PCI_DEVFN_CSE_KT	_PCI_DEVFN(CSE, 3)
#define  PCI_DEVFN_CSE_3	_PCI_DEVFN(CSE, 4)
#define  PCI_DEVFN_CSE_4	_PCI_DEVFN(CSE, 5)
#define  PCI_DEV_CSE		_PCI_DEV(CSE, 0)
#define  PCI_DEV_CSE_2		_PCI_DEV(CSE, 1)
#define  PCI_DEV_CSE_IDER	_PCI_DEV(CSE, 2)
#define  PCI_DEV_CSE_KT		_PCI_DEV(CSE, 3)
#define  PCI_DEV_CSE_3		_PCI_DEV(CSE, 4)
#define  PCI_DEV_CSE_4		_PCI_DEV(CSE, 5)

#define PCI_DEV_SLOT_SATA	0x17
#define  PCI_DEVFN_SATA		_PCI_DEVFN(SATA, 0)
#define  PCI_DEV_SATA		_PCI_DEV(SATA, 0)

#define PCI_DEV_SLOT_ESE	0x18
#define  PCI_DEVFN_ESE1		_PCI_DEVFN(ESE, 0)
#define  PCI_DEVFN_ESE2		_PCI_DEVFN(ESE, 1)
#define  PCI_DEVFN_ESE3		_PCI_DEVFN(ESE, 2)
#define  PCI_DEV_ESE1		_PCI_DEV(ESE, 0)
#define  PCI_DEV_ESE2		_PCI_DEV(ESE, 1)
#define  PCI_DEV_ESE3		_PCI_DEV(ESE, 2)

#define PCI_DEV_SLOT_SIO1	0x19
#define  PCI_DEVFN_I2C4		_PCI_DEVFN(SIO1, 0)
#define  PCI_DEVFN_I2C5		_PCI_DEVFN(SIO1, 1)
#define  PCI_DEVFN_UART2	_PCI_DEVFN(SIO1, 2)
#define  PCI_DEV_I2C4		_PCI_DEV(SIO1, 0)
#define  PCI_DEV_I2C5		_PCI_DEV(SIO1, 1)
#define  PCI_DEV_UART2		_PCI_DEV(SIO1, 2)

#define PCI_DEV_SLOT_PCIE_1	0x1c
#define  PCI_DEVFN_PCIE1	_PCI_DEVFN(PCIE_1, 0)
#define  PCI_DEVFN_PCIE2	_PCI_DEVFN(PCIE_1, 1)
#define  PCI_DEVFN_PCIE3	_PCI_DEVFN(PCIE_1, 2)
#define  PCI_DEVFN_PCIE4	_PCI_DEVFN(PCIE_1, 3)
#define  PCI_DEVFN_PCIE5	_PCI_DEVFN(PCIE_1, 4)
#define  PCI_DEVFN_PCIE6	_PCI_DEVFN(PCIE_1, 5)
#define  PCI_DEVFN_PCIE7	_PCI_DEVFN(PCIE_1, 6)
#define  PCI_DEVFN_PCIE8	_PCI_DEVFN(PCIE_1, 7)
#define  PCI_DEV_PCIE1		_PCI_DEV(PCIE_1, 0)
#define  PCI_DEV_PCIE2		_PCI_DEV(PCIE_1, 1)
#define  PCI_DEV_PCIE3		_PCI_DEV(PCIE_1, 2)
#define  PCI_DEV_PCIE4		_PCI_DEV(PCIE_1, 3)
#define  PCI_DEV_PCIE5		_PCI_DEV(PCIE_1, 4)
#define  PCI_DEV_PCIE6		_PCI_DEV(PCIE_1, 5)
#define  PCI_DEV_PCIE7		_PCI_DEV(PCIE_1, 6)
#define  PCI_DEV_PCIE8		_PCI_DEV(PCIE_1, 7)

#define PCI_DEV_SLOT_SIO2	0x1e
#define  PCI_DEVFN_UART0	_PCI_DEVFN(SIO2, 0)
#define  PCI_DEVFN_UART1	_PCI_DEVFN(SIO2, 1)
#define  PCI_DEVFN_GSPI0	_PCI_DEVFN(SIO2, 2)
#define  PCI_DEVFN_GSPI1	_PCI_DEVFN(SIO2, 3)
#define  PCI_DEVFN_TSN1		_PCI_DEVFN(SIO2, 4)
#define  PCI_DEVFN_TSN2		_PCI_DEVFN(SIO2, 5)
#define  PCI_DEV_UART0		_PCI_DEV(SIO2, 0)
#define  PCI_DEV_UART1		_PCI_DEV(SIO2, 1)
#define  PCI_DEV_GSPI0		_PCI_DEV(SIO2, 2)
#define  PCI_DEV_GSPI1		_PCI_DEV(SIO2, 3)
#define  PCI_DEV_TSN1		_PCI_DEV(SIO2, 4)
#define  PCI_DEV_TSN2		_PCI_DEV(SIO2, 5)

#define PCI_DEV_SLOT_ESPI	0x1f
#define  PCI_DEVFN_ESPI		_PCI_DEVFN(ESPI, 0)
#define  PCI_DEVFN_P2SB		_PCI_DEVFN(ESPI, 1)
#define  PCI_DEVFN_PMC		_PCI_DEVFN(ESPI, 2)
#define  PCI_DEVFN_HDA		_PCI_DEVFN(ESPI, 3)
#define  PCI_DEVFN_SMBUS	_PCI_DEVFN(ESPI, 4)
#define  PCI_DEVFN_SPI		_PCI_DEVFN(ESPI, 5)
#define  PCI_DEVFN_GBE		_PCI_DEVFN(ESPI, 6)
#define  PCI_DEVFN_NPK		_PCI_DEVFN(ESPI, 7)
#define  PCI_DEV_ESPI		_PCI_DEV(ESPI, 0)
#define  PCI_DEV_P2SB		_PCI_DEV(ESPI, 1)

#if !ENV_RAMSTAGE
/*
 * PCI_DEV_PMC is intentionally not defined in RAMSTAGE since PMC device gets
 * hidden from PCI bus after call to FSP-S. This leads to resource allocator
 * dropping it from the root bus as unused device. All references to PCI_DEV_PMC
 * would then return NULL and can go unnoticed if not handled properly. Since,
 * this device does not have any special chip config associated with it, it is
 * okay to not provide the definition for it in ramstage.
 */
#define  PCI_DEV_PMC		_PCI_DEV(ESPI, 2)
#endif

#define  PCI_DEV_HDA		_PCI_DEV(ESPI, 3)
#define  PCI_DEV_SMBUS		_PCI_DEV(ESPI, 4)
#define  PCI_DEV_SPI		_PCI_DEV(ESPI, 5)
#define  PCI_DEV_GBE		_PCI_DEV(ESPI, 6)
#define  PCI_DEV_NPK		_PCI_DEV(ESPI, 7)

#endif

/* for common code */
#define PCH_DEV_SLOT_CSE        PCI_DEV_SLOT_CSE
#define PCH_DEVFN_CSE		PCI_DEVFN_CSE
#define PCH_DEV_CSE		PCI_DEV_CSE
#define PCH_DEV_SPI		PCI_DEV_SPI
#define PCH_DEV_LPC		PCI_DEV_ESPI
#define PCH_DEV_P2SB		PCI_DEV_P2SB
#define PCH_DEV_SMBUS		PCI_DEV_SMBUS
#define PCH_DEV_XHCI		PCI_DEV_XHCI
#define PCH_DEVFN_XHCI		PCI_DEVFN_XHCI
#define PCH_DEVFN_PMC		PCI_DEVFN_PMC
#define PCH_DEV_SLOT_ISH	PCI_DEV_SLOT_ISH
#define SA_DEV_ROOT		PCI_DEV_ROOT
#define SA_DEVFN_ROOT		PCI_DEVFN_ROOT
#define SA_DEVFN_TCSS_DMA0	PCI_DEVFN_TCSS_DMA0
#define SA_DEVFN_TCSS_DMA1	PCI_DEVFN_TCSS_DMA1
#define SA_DEVFN_IGD		PCI_DEVFN_IGD
