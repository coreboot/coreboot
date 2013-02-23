//
// This file is part of the coreboot project.
//
// Copyright (C) 2007 Advanced Micro Devices, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//

//AMD8111 pic LNKA B C D

	Device (LNKA)
	{
		Name (_HID, EisaId ("PNP0C0F"))
		Name (_UID, 0x01)
		Method (_STA, 0, NotSerialized)
		{
			And (\_SB.PCI0.SBC3.PIBA, 0x0F, Local0)
			If (LEqual (Local0, 0x00)) { Return (0x09) } //Disabled
			Else { Return (0x0B) } //Enabled
		}

		Method (_PRS, 0, NotSerialized)
		{
			 Name (BUFA, ResourceTemplate ()
			{
				IRQ (Level, ActiveLow, Shared) {3,5,10,11}
			})
			Return (BUFA)
		}

		Method (_DIS, 0, NotSerialized)
		{
			Store (0x01, Local3)
			And (\_SB.PCI0.SBC3.PIBA, 0x0F, Local1)
			Store (Local1, Local2)
			If (LGreater (Local1, 0x07))
			{
				Subtract (Local1, 0x08, Local1)
			}

			ShiftLeft (Local3, Local1, Local3)
			Not (Local3, Local3)
			And (\_SB.PCI0.SBC3.PIBA, 0xF0, \_SB.PCI0.SBC3.PIBA)
		}

		Method (_CRS, 0, NotSerialized)
		{
			Name (BUFA, ResourceTemplate ()
			{
				IRQ (Level, ActiveLow, Shared) {}
			})
			CreateByteField (BUFA, 0x01, IRA1)
			CreateByteField (BUFA, 0x02, IRA2)
			Store (0x00, Local3)
			Store (0x00, Local4)
			And (\_SB.PCI0.SBC3.PIBA, 0x0F, Local1)
			If (LNot (LEqual (Local1, 0x00)))
			{	// Routing enable
				If (LGreater (Local1, 0x07))
				{
					Subtract (Local1, 0x08, Local2)
					ShiftLeft (One, Local2, Local4)
				}
				Else
				{
					If (LGreater (Local1, 0x00))
					{
						ShiftLeft (One, Local1, Local3)
					}
				}

				Store (Local3, IRA1)
				Store (Local4, IRA2)
			}

			Return (BUFA)
		}

		Method (_SRS, 1, NotSerialized)
		{
			CreateByteField (Arg0, 0x01, IRA1)
			CreateByteField (Arg0, 0x02, IRA2)
			ShiftLeft (IRA2, 0x08, Local0)
			Or (Local0, IRA1, Local0)
			Store (0x00, Local1)
			ShiftRight (Local0, 0x01, Local0)
			While (LGreater (Local0, 0x00))
			{
				Increment (Local1)
				ShiftRight (Local0, 0x01, Local0)
			}

			And (\_SB.PCI0.SBC3.PIBA, 0xF0, \_SB.PCI0.SBC3.PIBA)
			Or (\_SB.PCI0.SBC3.PIBA, Local1, \_SB.PCI0.SBC3.PIBA)
		}
	}

	Device (LNKB)
	{
		Name (_HID, EisaId ("PNP0C0F"))
		Name (_UID, 0x02)
		Method (_STA, 0, NotSerialized)
		{
			And (\_SB.PCI0.SBC3.PIBA, 0xF0, Local0)
			If (LEqual (Local0, 0x00)) { Return (0x09) }
			Else { Return (0x0B) }
		}

		Method (_PRS, 0, NotSerialized)
		{
			Name (BUFB, ResourceTemplate ()
			{
				IRQ (Level, ActiveLow, Shared) {3,5,10,11}
			})
			Return (BUFB)
		}

		Method (_DIS, 0, NotSerialized)
		{
			Store (0x01, Local3)
			And (\_SB.PCI0.SBC3.PIBA, 0xF0, Local1)
			ShiftRight (Local1, 0x04, Local1)
			Store (Local1, Local2)
			If (LGreater (Local1, 0x07))
			{
				Subtract (Local1, 0x08, Local1)
			}

			ShiftLeft (Local3, Local1, Local3)
			Not (Local3, Local3)
			And (\_SB.PCI0.SBC3.PIBA, 0x0F, \_SB.PCI0.SBC3.PIBA)
		}

		Method (_CRS, 0, NotSerialized)
		{
			Name (BUFB, ResourceTemplate ()
			{
				IRQ (Level, ActiveLow, Shared) {}
			})
			CreateByteField (BUFB, 0x01, IRB1)
			CreateByteField (BUFB, 0x02, IRB2)
			Store (0x00, Local3)
			Store (0x00, Local4)
			And (\_SB.PCI0.SBC3.PIBA, 0xF0, Local1)
			ShiftRight (Local1, 0x04, Local1)
			If (LNot (LEqual (Local1, 0x00)))
			{
				If (LGreater (Local1, 0x07))
				{
				Subtract (Local1, 0x08, Local2)
				ShiftLeft (One, Local2, Local4)
				}
				Else
				{
					If (LGreater (Local1, 0x00))
					{
						ShiftLeft (One, Local1, Local3)
					}
				}

				Store (Local3, IRB1)
				Store (Local4, IRB2)
			}

			Return (BUFB)
		}

		Method (_SRS, 1, NotSerialized)
		{
			CreateByteField (Arg0, 0x01, IRB1)
			CreateByteField (Arg0, 0x02, IRB2)
			ShiftLeft (IRB2, 0x08, Local0)
			Or (Local0, IRB1, Local0)
			Store (0x00, Local1)
			ShiftRight (Local0, 0x01, Local0)
			While (LGreater (Local0, 0x00))
			{
				Increment (Local1)
				ShiftRight (Local0, 0x01, Local0)
			}

			And (\_SB.PCI0.SBC3.PIBA, 0x0F, \_SB.PCI0.SBC3.PIBA)
			ShiftLeft (Local1, 0x04, Local1)
			Or (\_SB.PCI0.SBC3.PIBA, Local1, \_SB.PCI0.SBC3.PIBA)
		}
	}

	Device (LNKC)
	{
		Name (_HID, EisaId ("PNP0C0F"))
		Name (_UID, 0x03)
		Method (_STA, 0, NotSerialized)
		{
			And (\_SB.PCI0.SBC3.PIDC, 0x0F, Local0)
			If (LEqual (Local0, 0x00)) { Return (0x09) }
			Else { Return (0x0B) }
		}

		Method (_PRS, 0, NotSerialized)
		{
			Name (BUFA, ResourceTemplate ()
			{
				IRQ (Level, ActiveLow, Shared) {3,5,10,11}
			})
			Return (BUFA)
		}

		Method (_DIS, 0, NotSerialized)
		{
			Store (0x01, Local3)
			And (\_SB.PCI0.SBC3.PIDC, 0x0F, Local1)
			Store (Local1, Local2)
			If (LGreater (Local1, 0x07))
			{
				Subtract (Local1, 0x08, Local1)
			}

			ShiftLeft (Local3, Local1, Local3)
			Not (Local3, Local3)
			And (\_SB.PCI0.SBC3.PIDC, 0xF0, \_SB.PCI0.SBC3.PIDC)
		}

		Method (_CRS, 0, NotSerialized)
		{
			Name (BUFA, ResourceTemplate ()
			{
				IRQ (Level, ActiveLow, Shared) {}
			})
			CreateByteField (BUFA, 0x01, IRA1)
			CreateByteField (BUFA, 0x02, IRA2)
			Store (0x00, Local3)
			Store (0x00, Local4)
			And (\_SB.PCI0.SBC3.PIDC, 0x0F, Local1)
			If (LNot (LEqual (Local1, 0x00)))
			{
				If (LGreater (Local1, 0x07))
				{
					Subtract (Local1, 0x08, Local2)
					ShiftLeft (One, Local2, Local4)
				}
				Else
				{
					If (LGreater (Local1, 0x00))
					{
						ShiftLeft (One, Local1, Local3)
					}
				}

				Store (Local3, IRA1)
				Store (Local4, IRA2)
			}

			Return (BUFA)
		}

		Method (_SRS, 1, NotSerialized)
		{
			CreateByteField (Arg0, 0x01, IRA1)
			CreateByteField (Arg0, 0x02, IRA2)
			ShiftLeft (IRA2, 0x08, Local0)
			Or (Local0, IRA1, Local0)
			Store (0x00, Local1)
			ShiftRight (Local0, 0x01, Local0)
			While (LGreater (Local0, 0x00))
			{
				Increment (Local1)
				ShiftRight (Local0, 0x01, Local0)
			}

			And (\_SB.PCI0.SBC3.PIDC, 0xF0, \_SB.PCI0.SBC3.PIDC)
			Or (\_SB.PCI0.SBC3.PIDC, Local1, \_SB.PCI0.SBC3.PIDC)
		}
	}

	Device (LNKD)
	{
		Name (_HID, EisaId ("PNP0C0F"))
		Name (_UID, 0x04)
		Method (_STA, 0, NotSerialized)
		{
			And (\_SB.PCI0.SBC3.PIDC, 0xF0, Local0)
			If (LEqual (Local0, 0x00)) { Return (0x09) }
			Else { Return (0x0B) }
		}

		Method (_PRS, 0, NotSerialized)
		{
			Name (BUFB, ResourceTemplate ()
			{
				IRQ (Level, ActiveLow, Shared) {3,5,10,11}
			})
			Return (BUFB)
		}

		Method (_DIS, 0, NotSerialized)
		{
			Store (0x01, Local3)
			And (\_SB.PCI0.SBC3.PIDC, 0xF0, Local1)
			ShiftRight (Local1, 0x04, Local1)
			Store (Local1, Local2)
			If (LGreater (Local1, 0x07))
			{
				Subtract (Local1, 0x08, Local1)
			}

			ShiftLeft (Local3, Local1, Local3)
			Not (Local3, Local3)
			And (\_SB.PCI0.SBC3.PIDC, 0x0F, \_SB.PCI0.SBC3.PIDC)
		}

		Method (_CRS, 0, NotSerialized)
		{
			Name (BUFB, ResourceTemplate ()
			{
				IRQ (Level, ActiveLow, Shared) {}
			})
			CreateByteField (BUFB, 0x01, IRB1)
			CreateByteField (BUFB, 0x02, IRB2)
			Store (0x00, Local3)
			Store (0x00, Local4)
			And (\_SB.PCI0.SBC3.PIDC, 0xF0, Local1)
			ShiftRight (Local1, 0x04, Local1)
			If (LNot (LEqual (Local1, 0x00)))
			{
				If (LGreater (Local1, 0x07))
				{
					Subtract (Local1, 0x08, Local2)
					ShiftLeft (One, Local2, Local4)
				}
				Else
				{
					If (LGreater (Local1, 0x00))
					{
						ShiftLeft (One, Local1, Local3)
					}
				}

				Store (Local3, IRB1)
				Store (Local4, IRB2)
			}

			Return (BUFB)
		}

		Method (_SRS, 1, NotSerialized)
		{
			CreateByteField (Arg0, 0x01, IRB1)
			CreateByteField (Arg0, 0x02, IRB2)
			ShiftLeft (IRB2, 0x08, Local0)
			Or (Local0, IRB1, Local0)
			Store (0x00, Local1)
			ShiftRight (Local0, 0x01, Local0)
			While (LGreater (Local0, 0x00))
			{
				Increment (Local1)
				ShiftRight (Local0, 0x01, Local0)
			}

			And (\_SB.PCI0.SBC3.PIDC, 0x0F, \_SB.PCI0.SBC3.PIDC)
			ShiftLeft (Local1, 0x04, Local1)
			Or (\_SB.PCI0.SBC3.PIDC, Local1, \_SB.PCI0.SBC3.PIDC)
		}
	}


