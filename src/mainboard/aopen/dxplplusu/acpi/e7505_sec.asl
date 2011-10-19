
            Name (PBRS, ResourceTemplate ()
            {
                WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
                    0x0000, 0x0000, 0x00FF, 0x0000, 0x0100, ,, )

		/* System IO */
                WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                    0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, ,,, TypeStatic)
                IO (Decode16, 0x0CF8, 0x0CF8, 0x08, 0x08, )

                /* Video RAM */
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x00000000, 0x000A0000, 0x000BFFFF,
                    0x00000000, 0x00020000, ,,, AddressRangeMemory, TypeStatic)

                /* Video ROM */
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x00000000, 0x000C0000, 0x000C7FFF,
                    0x00000000, 0x00008000, ,,, AddressRangeMemory, TypeStatic)
                   
                /* Option ROMs ? */
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x00000000, 0x000C8000, 0x000DFFFF,
                    0x00000000, 0x00018000, ,,, AddressRangeMemory, TypeStatic)

		/* Top Of Lowmemory to IOAPIC */
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x00000000, 0x02000000, 0xFEBFFFFF,
                    0x00000000, 0xFCC00000, ,, _Y08, AddressRangeMemory, TypeStatic)
            })
            
            Method (_CRS, 0, NotSerialized)
            {

		/* Top Of Lowmemory to IOAPIC */
                CreateDWordField (PBRS, \_SB.PCI0._Y08._MIN, MEML)
                CreateDWordField (PBRS, \_SB.PCI0._Y08._LEN, LENM)
                And (\_SB.PCI0.TOLM, 0xF800, Local1)
                ShiftRight (Local1, 0x04, Local1)
                ShiftLeft (Local1, 0x14, MEML)
                Subtract (0xFEC00000, MEML, LENM)

                Return (PBRS)
            }

            Method (_STA, 0, NotSerialized)
            {
                Return (0x0F)
            }
            
