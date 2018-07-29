/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

	External(LCD0, DeviceObj)

	Name (BRCT, 0)

	Method(BRID, 1, NotSerialized)
	{
		Store (Match (BRIG, MEQ, Arg0, MTR, Zero, 2), Local0)
		If (LEqual (Local0, Ones))
		{
			Return (Subtract(SizeOf(BRIG), One))
		}
		Return (Local0)
	}

	Method (XBCL, 0, NotSerialized)
	{
		Store (1, BRCT)
		Return (BRIG)
	}

	/* Display Output Switching */
	Method (_DOS, 1)
	{
		/* Windows 2000 and Windows XP call _DOS to enable/disable
		 * Display Output Switching during init and while a switch
		 * is already active
		 */
		Store (And(Arg0, 7), DSEN)
	}

	/*
	 * Decrement display brightness.
	 *
	 * Using Notify is the right way. But Windows doesn't handle
	 * it well. So use both method in a way to avoid double action.
	 */
	Method (DECB, 0, NotSerialized)
	{
		If (BRCT)
		{
			Notify (LCD0, 0x87)
		} Else {
			Store (BRID (XBQC ()), Local0)
			If (LNotEqual (Local0, 2))
			{
				Decrement (Local0)
			}
			XBCM (DerefOf (Index (BRIG, Local0)))
		}
	}

	/*
	 * Increment display brightness.
	 */
	Method (INCB, 0, NotSerialized)
	{
		If (BRCT)
		{
			Notify (LCD0, 0x86)
		} Else {
			Store (BRID (XBQC ()), Local0)
			If (LNotEqual (Local0, Subtract(SizeOf(BRIG), One)))
			{
				Increment (Local0)
			}
			XBCM (DerefOf (Index (BRIG, Local0)))
		}
	}

	/* Device Current Status */
	Method(XDCS, 1)
	{
		TRAP(1)
		If (And(CSTE, ShiftLeft (1, Arg0))) {
			Return (0x1f)
		}
		Return(0x1d)
	}

	/* Query Device Graphics State */
	Method(XDGS, 1)
	{
		If (And(NSTE, ShiftLeft (1, Arg0))) {
			Return(1)
		}
		Return(0)
	}

	/* Device Set State */
	Method(XDSS, 1)
	{
		/* If Parameter Arg0 is (1 << 31) | (1 << 30), the
		 * display switch was completed
		 */
		If (LEqual(And(Arg0, 0xc0000000), 0xc0000000)) {
			Store (NSTE, CSTE)
		}
	}
