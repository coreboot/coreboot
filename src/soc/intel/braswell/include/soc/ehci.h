/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_EHCI_H_
#define _SOC_EHCI_H_

/* EHCI PCI Registers */
#define EHCI_CMD_STS		0x04
# define  INTRDIS		  (1 << 10)

#endif /* _SOC_EHCI_H_ */
