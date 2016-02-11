#ifndef _SOC_APOLLOLAKE_PCI_DEVS_H_
#define _SOC_APOLLOLAKE_PCI_DEVS_H_

#include <rules.h>

#define _NB_DEVFN(slot)				PCI_DEVFN(NB_DEV_SLOT_ ## slot, 0)
#define _LPSS_PCI_DEVFN(slot, func)		PCI_DEVFN(LPSS_DEV_SLOT_##slot, func)


#if ENV_RAMSTAGE
#include <device/device.h>
#include <device/pci_def.h>
#define _NB_DEV(slot)				dev_find_slot(0, _NB_DEVFN(slot))
#define _LPSS_PCI_DEV(slot, func)		dev_find_slot(0, _LPSS_PCI_DEVFN(slot, func))
#else
#include <arch/io.h>
#define _NB_DEV(slot)				PCI_DEV(0, NB_DEV_SLOT_ ## slot, 0)
#define _LPSS_PCI_DEV(slot, func)		PCI_DEV(0, LPSS_DEV_SLOT_##slot, func)
#endif

/* North bridge devices */
#define NB_DEV_SLOT_ROOT	0x00
#define  NB_DEVFN_ROOT		_NB_DEVFN(ROOT)
#define  NB_DEV_ROOT		_NB_DEV(ROOT)

/* LPSS UART */
#define LPSS_DEV_SLOT_UART	0x18
#define  LPSS_DEVFN_UART0	_LPSS_PCI_DEVFN(UART, 0)
#define  LPSS_DEVFN_UART1	_LPSS_PCI_DEVFN(UART, 1)
#define  LPSS_DEVFN_UART2	_LPSS_PCI_DEVFN(UART, 2)
#define  LPSS_DEVFN_UART3	_LPSS_PCI_DEVFN(UART, 3)
#define  LPSS_DEV_UART0		_LPSS_PCI_DEV(UART, 0)
#define  LPSS_DEV_UART1		_LPSS_PCI_DEV(UART, 1)
#define  LPSS_DEV_UART2		_LPSS_PCI_DEV(UART, 2)
#define  LPSS_DEV_UART3		_LPSS_PCI_DEV(UART, 3)

#define P2SB_DEV		PCI_DEV(0, 0xd, 0)
#define PMC_DEV			PCI_DEV(0, 0xd, 1)

#endif
