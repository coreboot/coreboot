/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
