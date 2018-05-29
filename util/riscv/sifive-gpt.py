#!/usr/bin/python3
# This file is part of the coreboot project.
#
# Copyright (C) 2018 Jonathan Neuschäfer
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

import sys, os, struct, uuid, zlib, io

# This script wraps the bootblock in a GPT partition, because that's what
# SiFive's bootrom will load.


# Size of a GPT disk block, in bytes
BLOCK_SIZE = 512
BLOCK_MASK = BLOCK_SIZE - 1

# Size of the bootcode part of the MBR
MBR_BOOTCODE_SIZE = 0x1be

# A protecive MBR, without the bootcode part
PROTECTIVE_MBR_FOOTER = bytes([
    0x00, 0x00, 0x02, 0x00, 0xee, 0xff, 0xff, 0xff,
    0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x55, 0xaa
])


# A "protective MBR"[1], which may also contain some boot code.
# [1]: https://en.wikipedia.org/wiki/GUID_Partition_Table#PROTECTIVE-MBR
class ProtectiveMBR:
    def __init__(self):
        self.bootcode = bytes(MBR_BOOTCODE_SIZE)

    def generate(self, stream):
        assert len(self.bootcode) == MBR_BOOTCODE_SIZE
        mbr = self.bootcode + PROTECTIVE_MBR_FOOTER
        assert len(mbr) == BLOCK_SIZE
        stream.write(mbr)


# Generate a GUID from a string
class GUID(uuid.UUID):
    def __init__(self, string):
        super().__init__(string)

    def get_bytes(self):
        return self.bytes_le

DUMMY_GUID_DISK_UNIQUE = GUID('17145242-abaa-441d-916a-3f26c970aba2')
DUMMY_GUID_PART_UNIQUE = GUID('7552133d-c8de-4a20-924c-0e85f5ea81f2')
GUID_TYPE_FSBL = GUID('5B193300-FC78-40CD-8002-E86C45580B47')


# A GPT disk header
# https://en.wikipedia.org/wiki/GUID_Partition_Table#Partition_table_header_(LBA_1)
class GPTHeader:
    def __init__(self):
        self.current_lba = 1
        self.backup_lba = 1
        self.first_usable_lba = 2
        self.last_usable_lba = 0xff     # dummy value
        self.uniq = DUMMY_GUID_DISK_UNIQUE
        self.part_entries_lba = 2
        self.part_entries_number = 0
        self.part_entries_crc32 = 0
        self.part_entry_size = 128

    def pack_with_crc(self, crc):
        header_size = 92
        header = struct.pack('<8sIIIIQQQQ16sQIII',
                b'EFI PART', 0x100, header_size, crc, 0,
                self.current_lba, self.backup_lba, self.first_usable_lba,
                self.last_usable_lba, self.uniq.get_bytes(),
                self.part_entries_lba, self.part_entries_number,
                self.part_entry_size, self.part_entries_crc32)
        assert len(header) == header_size
        return header

    def generate(self, stream):
        crc = zlib.crc32(self.pack_with_crc(0))
        header = self.pack_with_crc(crc)
        stream.write(header.ljust(BLOCK_SIZE, b'\0'))


# A GPT partition entry.
# https://en.wikipedia.org/wiki/GUID_Partition_Table#Partition_entries_(LBA_2-33)
class GPTPartition:
    def __init__(self):
        self.type = GUID('00000000-0000-0000-0000-000000000000')
        self.uniq = GUID('00000000-0000-0000-0000-000000000000')
        self.first_lba = 0
        self.last_lba = 0
        self.attr = 0
        self.name = ''

    def generate(self, stream):
        name_utf16 = self.name.encode('UTF-16LE')
        part = struct.pack('<16s16sQQQ72s',
                self.type.get_bytes(), self.uniq.get_bytes(),
                self.first_lba, self.last_lba, self.attr,
                name_utf16.ljust(72, b'\0'))
        assert len(part) == 128
        stream.write(part)


class GPTImage:
    # The final image consists of:
    # - A protective MBR
    # - A GPT header
    # - A few GPT partition entries
    # - The content of the bootblock
    def __init__(self):
        self.mbr = ProtectiveMBR()
        self.header = GPTHeader()
        self.partitions = [ GPTPartition() for i in range(8) ]
        self.bootblock = b''


    # Fix up a few numbers to ensure consistency between the different
    # components.
    def fixup(self):
        # Align the bootblock to a whole number to LBA blocks
        bootblock_size = (len(self.bootblock) + BLOCK_SIZE - 1) & ~BLOCK_MASK
        self.bootblock = self.bootblock.ljust(bootblock_size)

        # Propagate the number of partition entries
        self.header.part_entries_number = len(self.partitions)
        self.header.first_usable_lba = 2 + self.header.part_entries_number // 4

        # Create a partition entry for the bootblock
        self.partitions[0].type = GUID_TYPE_FSBL
        self.partitions[0].uniq = DUMMY_GUID_PART_UNIQUE
        self.partitions[0].first_lba = self.header.first_usable_lba
        self.partitions[0].last_lba = \
            self.header.first_usable_lba + bootblock_size // BLOCK_SIZE

        # Calculate the CRC32 checksum of the partitions array
        partition_array = io.BytesIO()
        for part in self.partitions:
            part.generate(partition_array)
        self.header.part_entries_crc32 = zlib.crc32(partition_array.getvalue())


    def generate(self, stream):
        self.mbr.generate(stream)
        self.header.generate(stream)
        for part in self.partitions:
            part.generate(stream)
        stream.write(self.bootblock)


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Usage:', file=sys.stderr)
        print('    %s bootblock.raw.bin bootblock.bin' % sys.argv[0],
                file=sys.stderr)
        sys.exit(1)

    image = GPTImage()

    with open(sys.argv[1], 'rb') as f:
        image.bootblock = f.read()

    image.fixup()

    with open(sys.argv[2], 'wb') as f:
        image.generate(f)
