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


//AMD k8 util for BUSB and res range

    Scope (\_SB)
    {

        Name (OSTB, Ones)
        Method (OSTP, 0, NotSerialized)
        {
            If (LEqual (^OSTB, Ones))
            {
                Store (0x00, ^OSTB)
            }

            Return (^OSTB)
        }

       Method (SEQL, 2, Serialized)
        {
            Store (SizeOf (Arg0), Local0)
            Store (SizeOf (Arg1), Local1)
            If (LNot (LEqual (Local0, Local1))) { Return (Zero) }

            Name (BUF0, Buffer (Local0) {})
            Store (Arg0, BUF0)
            Name (BUF1, Buffer (Local0) {})
            Store (Arg1, BUF1)
            Store (Zero, Local2)
            While (LLess (Local2, Local0))
            {
                Store (DerefOf (Index (BUF0, Local2)), Local3)
                Store (DerefOf (Index (BUF1, Local2)), Local4)
                If (LNot (LEqual (Local3, Local4))) { Return (Zero) }

                Increment (Local2)
            }

            Return (One)
        }


        Method (DADD, 2, NotSerialized)
        {
                Store( Arg1, Local0)
                Store( Arg0, Local1)
                Add( ShiftLeft(Local1,16), Local0, Local0)
                Return (Local0)
        }


	Method (GHCE, 1, NotSerialized)
	{
                Store (DerefOf (Index (\_SB.PCI0.HCLK, Arg0)), Local1)
                if(LEqual ( And(Local1, 0x01), 0x01)) { Return (0x0F) }
                Else { Return (0x00) }
	}

        Method (GHCN, 1, NotSerialized)
        {
                Store (0x00, Local0)
                Store (DerefOf (Index (\_SB.PCI0.HCLK, Arg0)), Local1)
		Store (ShiftRight( And (Local1, 0xf0), 0x04), Local0)
		Return (Local0)
        }

        Method (GHCL, 1, NotSerialized)
        {
                Store (0x00, Local0)
                Store (DerefOf (Index (\_SB.PCI0.HCLK, Arg0)), Local1)
                Store (ShiftRight( And (Local1, 0xf00), 0x08), Local0)
                Return (Local0)
        }

        Method (GBUS, 2, NotSerialized)
        {
            Store (0x00, Local0)
            While (LLess (Local0, 0x04))
            {
                Store (DerefOf (Index (\_SB.PCI0.BUSN, Local0)), Local1)
                If (LEqual (And (Local1, 0x03), 0x03))
                {
                    If (LEqual (Arg0, ShiftRight (And (Local1, 0x70), 0x04)))
                    {
                        If (LOr (LEqual (Arg1, 0xFF), LEqual (Arg1, ShiftRight (And (Local1, 0x0300), 0x08))))
                        {
                            Return (ShiftRight (And (Local1, 0x00FF0000), 0x10))
                        }
                    }
                }

                Increment (Local0)
            }

            Return (0x00)
        }

        Method (GWBN, 2, NotSerialized)
        {
            Name (BUF0, ResourceTemplate ()
            {
                WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
                    0x0000, // Address Space Granularity
                    0x0000, // Address Range Minimum
                    0x0000, // Address Range Maximum
                    0x0000, // Address Translation Offset
                    0x0000,,,)
            })
            CreateWordField (BUF0, 0x08, BMIN)
            CreateWordField (BUF0, 0x0A, BMAX)
            CreateWordField (BUF0, 0x0E, BLEN)
            Store (0x00, Local0)
            While (LLess (Local0, 0x04))
            {
                Store (DerefOf (Index (\_SB.PCI0.BUSN, Local0)), Local1)
                If (LEqual (And (Local1, 0x03), 0x03))
                {
                    If (LEqual (Arg0, ShiftRight (And (Local1, 0x70), 0x04)))
                    {
                        If (LOr (LEqual (Arg1, 0xFF), LEqual (Arg1, ShiftRight (And (Local1, 0x0300), 0x08))))
                        {
                            Store (ShiftRight (And (Local1, 0x00FF0000), 0x10), BMIN)
                            Store (ShiftRight (Local1, 0x18), BMAX)
                            Subtract (BMAX, BMIN, BLEN)
                            Increment (BLEN)
                            Return (RTAG (BUF0))
                        }
                    }
                }

                Increment (Local0)
            }

            Return (RTAG (BUF0))
        }

        Method (GMEM, 2, NotSerialized)
        {
            Name (BUF0, ResourceTemplate ()
            {
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                    0x00000000, // Address Space Granularity
                    0x00000000, // Address Range Minimum
                    0x00000000, // Address Range Maximum
                    0x00000000, // Address Translation Offset
                    0x00000000,,,
                    , AddressRangeMemory, TypeStatic)
            })
            CreateDWordField (BUF0, 0x0A, MMIN)
            CreateDWordField (BUF0, 0x0E, MMAX)
            CreateDWordField (BUF0, 0x16, MLEN)
            Store (0x00, Local0)
            Store (0x00, Local4)
	    Store (0x00, Local3)
            While (LLess (Local0, 0x10))
            {
                Store (DerefOf (Index (\_SB.PCI0.MMIO, Local0)), Local1)
                Increment (Local0)
                Store (DerefOf (Index (\_SB.PCI0.MMIO, Local0)), Local2)
                If (LEqual (And (Local1, 0x03), 0x03))
                {
                    If (LEqual (Arg0, And (Local2, 0x07)))
                    {
                        If (LOr (LEqual (Arg1, 0xFF), LEqual (Arg1, ShiftRight (And (Local2, 0x30), 0x04))))
                        {
                            Store (ShiftLeft (And (Local1, 0xFFFFFF00), 0x08), MMIN)
                            Store (ShiftLeft (And (Local2, 0xFFFFFF00), 0x08), MMAX)
                            Or (MMAX, 0xFFFF, MMAX)
                            Subtract (MMAX, MMIN, MLEN)

                            If (Local4)
                            {
                                Concatenate (RTAG (BUF0), Local3, Local5)
                               	Store (Local5, Local3)
                            }
                            Else
                            {
                                If (LOr (LAnd (LEqual (Arg1, 0xFF), LEqual (Arg0, 0x00)), LEqual (Arg1, \_SB.PCI0.SBLK)))
                                {
                                    Store (\_SB.PCI0.TOM1, MMIN)
                                    Subtract (MMAX, MMIN, MLEN)
                                    Increment (MLEN)
                                }

                                Store (RTAG (BUF0), Local3)
                            }

                            Increment (Local4)
                        }
                    }
                }

                Increment (Local0)
            }

            If (LNot (Local4))
            {
                Store (BUF0, Local3)
            }

            Return (Local3)
        }

        Method (GIOR, 2, NotSerialized)
        {
            Name (BUF0, ResourceTemplate ()
            {
                DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                    0x00000000, // Address Space Granularity
                    0x00000000, // Address Range Minimum
                    0x00000000, // Address Range Maximum
                    0x00000000, // Address Translation Offset
                    0x00000000,,,
                    , TypeStatic)
            })
            CreateDWordField (BUF0, 0x0A, PMIN)
            CreateDWordField (BUF0, 0x0E, PMAX)
            CreateDWordField (BUF0, 0x16, PLEN)
            Store (0x00, Local0)
            Store (0x00, Local4)
	    Store (0x00, Local3)
            While (LLess (Local0, 0x08))
            {
                Store (DerefOf (Index (\_SB.PCI0.PCIO, Local0)), Local1)
                Increment (Local0)
                Store (DerefOf (Index (\_SB.PCI0.PCIO, Local0)), Local2)
                If (LEqual (And (Local1, 0x03), 0x03))
                {
                    If (LEqual (Arg0, And (Local2, 0x07)))
                    {
                        If (LOr (LEqual (Arg1, 0xFF), LEqual (Arg1, ShiftRight (And (Local2, 0x30), 0x04))))
                        {
                            Store (And (Local1, 0x01FFF000), PMIN)
                            Store (And (Local2, 0x01FFF000), PMAX)
                            Or (PMAX, 0x0FFF, PMAX)
                            Subtract (PMAX, PMIN, PLEN)
                            Increment (PLEN)

                            If (Local4)
                            {
                                Concatenate (RTAG (BUF0), Local3, Local5)
       	                        Store (Local5, Local3)
                            }
                            Else
                            {
                                If (LGreater (PMAX, PMIN))
                                {
                                    If (LOr (LAnd (LEqual (Arg1, 0xFF), LEqual (Arg0, 0x00)), LEqual (Arg1, \_SB.PCI0.SBLK)))
                                    {
                                        Store (0x0D00, PMIN)
                                        Subtract (PMAX, PMIN, PLEN)
                                        Increment (PLEN)
                                    }

                                    Store (RTAG (BUF0), Local3)
                                    Increment (Local4)
                                }

                                If (And (Local1, 0x10))
                                {
                                    Store (0x03B0, PMIN)
                                    Store (0x03DF, PMAX)
                                    Store (0x30, PLEN)
                                    If (Local4)
                                    {
                                        Concatenate (RTAG (BUF0), Local3, Local5)
                                        Store (Local5, Local3)
                                    }
                                    Else
                                    {
                                        Store (RTAG (BUF0), Local3)
                                    }
                                }
                            }

                            Increment (Local4)
                        }
                    }
                }

                Increment (Local0)
            }

            If (LNot (Local4))
            {
                Store (RTAG (BUF0), Local3)
            }

            Return (Local3)
        }

        Method (RTAG, 1, NotSerialized)
        {
            Store (Arg0, Local0)
            Store (SizeOf (Local0), Local1)
            Subtract (Local1, 0x02, Local1)
            Multiply (Local1, 0x08, Local1)
            CreateField (Local0, 0x00, Local1, RETB)
            Store (RETB, Local2)
            Return (Local2)
        }
    }

