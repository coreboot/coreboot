/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PHOENIX_XHCI_H
#define AMD_PHOENIX_XHCI_H

#include <static.h>

#define SOC_XHCI_0 DEV_PTR(xhci_0)
#define SOC_XHCI_1 DEV_PTR(xhci_1)
/* Beware that there is no XHCI 2 on Phoenix  */
#define SOC_XHCI_2 DEV_PTR(usb4_xhci_0) /* XHCI 3 */
#define SOC_XHCI_3 DEV_PTR(usb4_xhci_1) /* XHCI 4 */
#define SOC_XHCI_4 NULL
#define SOC_XHCI_5 NULL
#define SOC_XHCI_6 NULL
#define SOC_XHCI_7 NULL

#endif /* AMD_PHOENIX_XHCI_H */
