#ifndef	CARDS_H
#define CARDS_H

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 */

#include "nic.h"

/* OK, this is how the PCI support hack works:  if pci.h is included before
 * this file is included, assume that the driver supports PCI.  This means that
 * this file is usually included last.  */

#ifdef	PCI_H
#define PCI_ARG(x) ,x
#else
#define PCI_ARG(x)
#endif

#ifdef	INCLUDE_WD
extern struct nic	*wd_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_3C503
extern struct nic	*t503_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_VIA_RHINE
extern struct nic	*rhine_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_NE
extern struct nic	*ne_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_NS8390
extern struct nic	*nepci_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_3C509
extern struct nic	*t509_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_3C529
extern struct nic	*t529_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_3C595
extern struct nic	*t595_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_3C90X
extern struct nic	*a3c90x_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_EEPRO
extern struct nic	*eepro_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_EEPRO100
extern struct nic	*eepro100_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_EPIC100
extern struct nic	*epic100_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_OTULIP
extern struct nic	*otulip_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_TULIP
extern struct nic	*tulip_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_DAVICOM
extern struct nic	*davicom_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_CS89X0
extern struct nic	*cs89x0_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_LANCE
extern struct nic	*lancepci_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_NE2100
extern struct nic	*ne2100_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_NI6510
extern struct nic	*ni6510_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_SK_G16
extern struct nic	*SK_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_3C507
extern struct nic	*t507_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_NI5010
extern struct nic	*ni5010_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_NI5210
extern struct nic	*ni5210_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_EXOS205
extern struct nic	*exos205_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_SMC9000
extern struct nic	*smc9000_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_TIARA
extern struct nic	*tiara_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_DEPCA
extern struct nic	*depca_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_RTL8139
extern struct nic	*rtl8139_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_W89C840
extern struct nic	*w89c840_probe(struct nic *, unsigned short *
	PCI_ARG(struct pci_device *));
#endif

#ifdef	INCLUDE_SIS900
extern struct nic	*sis900_probe(struct nic *, unsigned short *
        PCI_ARG(struct pci_device *));
#endif

#endif	/* CARDS_H */
