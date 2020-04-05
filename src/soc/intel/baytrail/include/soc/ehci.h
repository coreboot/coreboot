/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef BAYTRAIL_EHCI_H
#define BAYTRAIL_EHCI_H

/* EHCI PCI Registers */
#define EHCI_CMD_STS		0x04
# define  INTRDIS		  (1 << 10)
#define EHCI_SBRN_FLA_PWC	0x60
# define  PORTWKIMP		  (1 << 16)
# define  PORTWKCAPMASK		  (0x3ff << 17)
#define EHCI_USB2PDO		0x64

/* EHCI Memory Registers */
#define USB2CMD			0x20
# define  USB2CMD_ASE		  (1 << 5)
# define  USB2CMD_PSE		  (1 << 4)
# define  USB2CMD_HCRESET	  (1 << 1)
# define  USB2CMD_RS		  (1 << 0)
#define USB2STS			0x24
# define  USB2STS_HCHALT	  (1 << 12)

/* RCBA EHCI Registers */
#define RCBA_FUNC_DIS		0x220
# define  RCBA_EHCI_DIS		  (1 << 0)

#endif /* BAYTRAIL_EHCI_H */
