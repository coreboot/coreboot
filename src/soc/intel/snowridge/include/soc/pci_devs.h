/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_PCI_DEVS_H_
#define _SOC_SNOWRIDGE_PCI_DEVS_H_

#include <device/pci_def.h>

#if defined(__SIMPLE_DEVICE__)
#include <device/pci_type.h>
#define _SA_DEV(slot, func)    PCI_DEV(0x00, slot, func)
#define _PCH_DEV(slot, func)   PCI_DEV(0x00, slot, func)
#define _UBOX0_DEV(slot, func) PCI_DEV(0xFE, slot, func)
#define _UBOX1_DEV(slot, func) PCI_DEV(0xFF, slot, func)
#else
#include <device/device.h>
#define _SA_DEV(slot, func)    pcidev_path_on_bus(0, PCI_DEVFN(slot, func))
#define _PCH_DEV(slot, func)   pcidev_path_on_bus(0, PCI_DEVFN(slot, func))
#define _UBOX0_DEV(slot, func) pcidev_path_on_bus(0xFE, PCI_DEVFN(slot, func))
#define _UBOX1_DEV(slot, func) pcidev_path_on_bus(0xFF, PCI_DEVFN(slot, func))
#endif

/* Stack S0. */
#define SA_DEV_ROOT         _SA_DEV(0x00, 0)
#define PCH_DEVFN_QAT_1_7   PCI_DEVFN(0x06, 0)
#define PCH_DEVFN_PCIE_RP0  PCI_DEVFN(0x09, 0)
#define PCH_DEVFN_PCIE_RP1  PCI_DEVFN(0x0a, 0)
#define PCH_DEVFN_PCIE_RP2  PCI_DEVFN(0x0b, 0)
#define PCH_DEVFN_PCIE_RP3  PCI_DEVFN(0x0c, 0)
#define PCH_DEVFN_PCIE_RP8  PCI_DEVFN(0x14, 0)
#define PCH_DEVFN_PCIE_RP9  PCI_DEVFN(0x15, 0)
#define PCH_DEVFN_PCIE_RP10 PCI_DEVFN(0x16, 0)
#define PCH_DEVFN_PCIE_RP11 PCI_DEVFN(0x17, 0)
#define PCH_DEV_UART(func)  _PCH_DEV(0x1a, func)

#define MIN_PCH_SLOT PCI_SLOT(PCH_DEVFN_QAT_1_7)

/**
 * @note To use common block xhci, `PCH_DEVFN_XHCI` need to be defined as `PCI_DEV` not
 * `PCI_DEVFN`. Actually they are same for this SoC.
 */
#define PCH_DEVFN_XHCI PCI_DEV(0x0, 0x1e, 0)
#define PCH_DEV_XHCI   _PCH_DEV(0x1e, 0)

#define PCH_DEVFN_LPC   PCI_DEVFN(0x1f, 0)
#define PCH_DEV_LPC     _PCH_DEV(0x1f, 0)
#define PCH_DEVFN_P2SB  PCI_DEVFN(0x1f, 1)
#define PCH_DEV_P2SB    _PCH_DEV(0x1f, 1)
#define PCH_DEVFN_PMC   PCI_DEVFN(0x1f, 2)
#define PCH_DEVFN_SMBUS PCI_DEVFN(0x1f, 4)
#define PCH_DEV_SMBUS   _PCH_DEV(0x1f, 4)
#define PCH_DEVFN_SPI   PCI_DEVFN(0x1f, 5)
#define PCH_DEV_SPI     _PCH_DEV(0x1f, 5)

/* Stack S1. */
#define CPU_DEVFN_PCIE_RPA PCI_DEVFN(0x04, 0)
#define CPU_DEVFN_PCIE_RPB PCI_DEVFN(0x05, 0)
#define CPU_DEVFN_PCIE_RPC PCI_DEVFN(0x06, 0)
#define CPU_DEVFN_PCIE_RPD PCI_DEVFN(0x07, 0)

/* Stack S2. */
#define DLB_DEVFN PCI_DEVFN(0x00, 0)

/* Stack S3. */
#define NIS_DEVFN PCI_DEVFN(0x04, 0)

/* Stack S4. */
#define QAT_1_8_DEVFN PCI_DEVFN(0x05, 0)

/* Stack U0. */
#define UBOX_DEV_RACU   _UBOX0_DEV(0x00, 1)
#define UBOX_DEV_NCDECS _UBOX0_DEV(0x00, 2)

/* Stack U1. */
#define CHAALL_DEV(func)   _UBOX1_DEV(0x1d, func)

/**
 * Before calling FspMemoryInit(), the bus number for UBox 0 is 0x1E.
 */
#define IMC_SPD_DEV PCI_DEV(0x1e, 0x0b, 0)

#endif // _SOC_SNOWRIDGE_PCI_DEVS_H_
