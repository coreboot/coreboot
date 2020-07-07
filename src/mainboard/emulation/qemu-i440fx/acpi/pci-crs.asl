/* SPDX-License-Identifier: GPL-2.0-only */

/* PCI CRS (current resources) definition. */
Scope(\_SB.PCI0) {

    Name(CRES, ResourceTemplate() {
        WordBusNumber(ResourceProducer, MinFixed, MaxFixed, PosDecode,
            0x0000,             // Address Space Granularity
            0x0000,             // Address Range Minimum
            0x00FF,             // Address Range Maximum
            0x0000,             // Address Translation Offset
            0x0100,             // Address Length
            ,, )
        IO(Decode16,
            0x0CF8,             // Address Range Minimum
            0x0CF8,             // Address Range Maximum
            0x01,               // Address Alignment
            0x08,               // Address Length
            )
        WordIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
            0x0000,             // Address Space Granularity
            0x0000,             // Address Range Minimum
            0x0CF7,             // Address Range Maximum
            0x0000,             // Address Translation Offset
            0x0CF8,             // Address Length
            ,, , TypeStatic)
        WordIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
            0x0000,             // Address Space Granularity
            0x0D00,             // Address Range Minimum
            0xFFFF,             // Address Range Maximum
            0x0000,             // Address Translation Offset
            0xF300,             // Address Length
            ,, , TypeStatic)
        DWordMemory(ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
            0x00000000,         // Address Space Granularity
            0x000A0000,         // Address Range Minimum
            0x000BFFFF,         // Address Range Maximum
            0x00000000,         // Address Translation Offset
            0x00020000,         // Address Length
            ,, , AddressRangeMemory, TypeStatic)
        DWordMemory(ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
            0x00000000,         // Address Space Granularity
            0xE0000000,         // Address Range Minimum
            0xFEBFFFFF,         // Address Range Maximum
            0x00000000,         // Address Translation Offset
            0x1EC00000,         // Address Length
            ,, PW32, AddressRangeMemory, TypeStatic)
    })

    Name(CR64, ResourceTemplate() {
        QWordMemory(ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
            0x00000000,          // Address Space Granularity
            0x8000000000,        // Address Range Minimum
            0xFFFFFFFFFF,        // Address Range Maximum
            0x00000000,          // Address Translation Offset
            0x8000000000,        // Address Length
            ,, PW64, AddressRangeMemory, TypeStatic)
    })

    Method(_CRS, 0) {
        Return (CRES)
    }
}
