/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

Device (CRHW)
{
	Name(_HID, EISAID("GGL0001"))

	Method(_STA, 0, Serialized)
	{
		Return (0xb)
	}

	Method(CHSW, 0, Serialized)
	{
		Name (WSHC, Package() { VBT3 })
		Return (WSHC)
	}

	Method(FWID, 0, Serialized)
	{
		Name (DIW1, "")
		ToString(VBT5, 63, DIW1)
		Name (DIWF, Package() { DIW1 })
		Return(DIWF)
	}

	Method(FRID, 0, Serialized)
	{
		Name (DIR1, "")
		ToString(VBT6, 63, DIR1)
		Name (DIRF, Package() { DIR1 })
		Return (DIRF)
	}

	Method(HWID, 0, Serialized)
	{
		Name (DIW0, "")
		ToString(VBT4, 255, DIW0)
		Name (DIWH, Package() { DIW0 })
		Return (DIWH)
	}

	Method(BINF, 0, Serialized)
	{
		Name (FNIB, Package() { VBT0, VBT1, VBT2, VBT7, VBT8 })
		Return (FNIB)
	}

	Method(GPIO, 0, Serialized)
	{
		Name(OIPG, Package() {
			Package() { 0x001, 0, 22, "CougarPoint" }, // recovery button
			Package() { 0x002, 1, 57, "CougarPoint" }, // developer switch
			Package() { 0x003, 0, 48, "CougarPoint" }, // firmware write protect
			Package() { 0x100, 0,  9, "CougarPoint" }, // debug header gpio
			Package() { 0x101, 0, 10, "CougarPoint" }, // debug header gpio 1
			Package() { 0x102, 0, 12, "CougarPoint" }, // debug header gpio 2
			Package() { 0x103, 0, 13, "CougarPoint" }, // debug header gpio 3
			Package() { 0x104, 0, 14, "CougarPoint" }, // debug header gpio 4
			Package() { 0x105, 0, 15, "CougarPoint" }, // debug header gpio 5
			Package() { 0x106, 0, 24, "CougarPoint" }, // debug header gpio 6
			Package() { 0x107, 0, 26, "CougarPoint" }, // debug header gpio 7
		})
		Return (OIPG)

	}

	Method(VBNV, 0, Serialized)
	{
		Name(VNBV, Package() {
			// See src/vendorcode/google/chromeos/Kconfig
			// for the definition of these:
			CONFIG_VBNV_OFFSET,
			CONFIG_VBNV_SIZE
		})
		Return(VNBV)
	}

	Method(VDAT, 0, Serialized)
	{
		Name(TAD0,"")
		ToBuffer(CHVD, TAD0)
		Name (TADV, Package() { TAD0 })
		Return (TADV)
	}

	Method(FMAP, 0, Serialized)
	{
		Name(PAMF, Package() { VBT9 })
		Return(PAMF)
	}

	Method(MECK, 0, Serialized)
	{
		Name(HASH, Package() { MEHH })
		Return(HASH)
	}

	Method(MLST, 0, Serialized)
	{
		Name(TSLM, Package() { "CHSW", "FWID", "HWID", "FRID", "BINF",
			   "GPIO", "VBNV", "VDAT", "FMAP", "MECK"
		})
		Return (TSLM)
	}
}
