/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Scope (\_GPE)
{
	/*  PCIE WLAN Wake event  */
	Method (_L08)
	{
		/* DBGO ("\\_GPE\\_L08\n") */
	}

	/*  EHCI USB controller PME#  SCIMAP24*/
	Method (_L18)
	{
		/* DBGO ("\\_GPE\\_L18\n") */
		Notify (\_SB.PCI0.EHC0, 0x02)	/* NOTIFY_DEVICE_WAKE */
	}

	/*  XHCI USB controller PME#  SCIMAP56*/
	Method (_L1F)
	{
		/* DBGO ("\\_GPE\\_L1F\n") */
		Notify (\_SB.PCI0.XHC0, 0x02)	/* NOTIFY_DEVICE_WAKE */
	}
}	/* End Scope GPE */
