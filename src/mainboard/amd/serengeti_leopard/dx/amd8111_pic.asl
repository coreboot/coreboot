/*============================================================================
Copyright 2005 ADVANCED MICRO DEVICES, INC. All Rights Reserved.
This software and any related documentation (the "Materials") are the
confidential proprietary information of AMD. Unless otherwise provided in a
software agreement specifically licensing the Materials, the Materials are
provided in confidence and may not be distributed, modified, or reproduced in
whole or in part by any means.
LIMITATION OF LIABILITY: THE MATERIALS ARE PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO
WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY
PARTICULAR PURPOSE, OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR
USAGE OF TRADE. IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY
DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS,
BUSINESS INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF THE USE OF OR
INABILITY TO USE THE MATERIALS, EVEN IF AMD HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES. BECAUSE SOME JURISDICTIONS PROHIBIT THE EXCLUSION
OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE
LIMITATION MAY NOT APPLY TO YOU.
AMD does not assume any responsibility for any errors which may appear in the
Materials nor any responsibility to support or update the Materials. AMD
retains the right to modify the Materials at any time, without notice, and is
not obligated to provide such modified Materials to you.
NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
further information, software, technical information, know-how, or show-how
available to you.
U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to the
restrictions as set forth in FAR 52.227-14 and DFAR 252.227-7013, et seq., or
its successor. Use of the Materials by the Government constitutes
acknowledgement of AMD's proprietary rights in them.
============================================================================*/
// 2005.9 serengeti support
// by yhlu
//
//=
//AMD8111 pic LNKA B C D

            Device (LNKA)
            {
                Name (_HID, EisaId ("PNP0C0F"))
                Name (_UID, 0x01)
                Method (_STA, 0, NotSerialized)
                {
                    And (\_SB.PCI1.SBC3.PIBA, 0x0F, Local0)
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
                    And (\_SB.PCI1.SBC3.PIBA, 0x0F, Local1)
                    Store (Local1, Local2)
                    If (LGreater (Local1, 0x07))
                    {
                        Subtract (Local1, 0x08, Local1)
                    }

                    ShiftLeft (Local3, Local1, Local3)
                    Not (Local3, Local3)
                    And (\_SB.PCI1.SBC3.PIBA, 0xF0, \_SB.PCI1.SBC3.PIBA)
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
                    And (\_SB.PCI1.SBC3.PIBA, 0x0F, Local1)
                    If (LNot (LEqual (Local1, 0x00)))
                    {  // Routing enable
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

                    And (\_SB.PCI1.SBC3.PIBA, 0xF0, \_SB.PCI1.SBC3.PIBA)
                    Or (\_SB.PCI1.SBC3.PIBA, Local1, \_SB.PCI1.SBC3.PIBA)
                }
            }

            Device (LNKB)
            {
                Name (_HID, EisaId ("PNP0C0F"))
                Name (_UID, 0x02)
                Method (_STA, 0, NotSerialized)
                {
                    And (\_SB.PCI1.SBC3.PIBA, 0xF0, Local0)
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
                    And (\_SB.PCI1.SBC3.PIBA, 0xF0, Local1)
                    ShiftRight (Local1, 0x04, Local1)
                    Store (Local1, Local2)
                    If (LGreater (Local1, 0x07))
                    {
                        Subtract (Local1, 0x08, Local1)
                    }

                    ShiftLeft (Local3, Local1, Local3)
                    Not (Local3, Local3)
                    And (\_SB.PCI1.SBC3.PIBA, 0x0F, \_SB.PCI1.SBC3.PIBA)
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
                    And (\_SB.PCI1.SBC3.PIBA, 0xF0, Local1)
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

                    And (\_SB.PCI1.SBC3.PIBA, 0x0F, \_SB.PCI1.SBC3.PIBA)
                    ShiftLeft (Local1, 0x04, Local1)
                    Or (\_SB.PCI1.SBC3.PIBA, Local1, \_SB.PCI1.SBC3.PIBA)
                }
            }

            Device (LNKC)
            {
                Name (_HID, EisaId ("PNP0C0F"))
                Name (_UID, 0x03)
                Method (_STA, 0, NotSerialized)
                {
                    And (\_SB.PCI1.SBC3.PIDC, 0x0F, Local0)
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
                    And (\_SB.PCI1.SBC3.PIDC, 0x0F, Local1)
                    Store (Local1, Local2)
                    If (LGreater (Local1, 0x07))
                    {
                        Subtract (Local1, 0x08, Local1)
                    }

                    ShiftLeft (Local3, Local1, Local3)
                    Not (Local3, Local3)
                    And (\_SB.PCI1.SBC3.PIDC, 0xF0, \_SB.PCI1.SBC3.PIDC)
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
                    And (\_SB.PCI1.SBC3.PIDC, 0x0F, Local1)
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

                    And (\_SB.PCI1.SBC3.PIDC, 0xF0, \_SB.PCI1.SBC3.PIDC)
                    Or (\_SB.PCI1.SBC3.PIDC, Local1, \_SB.PCI1.SBC3.PIDC)
                }
            }

            Device (LNKD)
            {
                Name (_HID, EisaId ("PNP0C0F"))
                Name (_UID, 0x04)
                Method (_STA, 0, NotSerialized)
                {
                    And (\_SB.PCI1.SBC3.PIDC, 0xF0, Local0)
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
                    And (\_SB.PCI1.SBC3.PIDC, 0xF0, Local1)
                    ShiftRight (Local1, 0x04, Local1)
                    Store (Local1, Local2)
                    If (LGreater (Local1, 0x07))
                    {
                        Subtract (Local1, 0x08, Local1)
                    }

                    ShiftLeft (Local3, Local1, Local3)
                    Not (Local3, Local3)
                    And (\_SB.PCI1.SBC3.PIDC, 0x0F, \_SB.PCI1.SBC3.PIDC)
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
                    And (\_SB.PCI1.SBC3.PIDC, 0xF0, Local1)
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

                    And (\_SB.PCI1.SBC3.PIDC, 0x0F, \_SB.PCI1.SBC3.PIDC)
                    ShiftLeft (Local1, 0x04, Local1)
                    Or (\_SB.PCI1.SBC3.PIDC, Local1, \_SB.PCI1.SBC3.PIDC)
                }
            }


