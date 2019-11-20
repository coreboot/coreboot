#!/usr/bin/python
#============================================================================
#
#/** @file qgpt.py
#
# GENERAL DESCRIPTION
#   Generates QCom GPT header for wrapping Bootblock
#
# Copyright (c) 2018, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#     * Neither the name of The Linux Foundation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#**/
#

import os
import math
import random
import re
import struct
import sys
import tempfile

from binascii import crc32
from optparse import OptionParser
from types import *


def UpdateMBR(options, GPTBlobBuffer):
    i = 0x1BE
    GPTBlobBuffer[i + 0] = 0x00                  # not bootable
    GPTBlobBuffer[i + 1] = 0x00                  # head
    GPTBlobBuffer[i + 2] = 0x01                  # sector
    GPTBlobBuffer[i + 3] = 0x00                  # cylinder
    GPTBlobBuffer[i + 4] = 0xEE                  # type
    GPTBlobBuffer[i + 5] = 0xFF                  # head
    GPTBlobBuffer[i + 6] = 0xFF                  # sector
    GPTBlobBuffer[i + 7] = 0xFF                  # cylinder
    GPTBlobBuffer[i + 8:i + 8 + 4] = [0x01, 0x00, 0x00, 0x00]

    GPTBlobBuffer[i + 12:i + 16] = [0x00, 0x0f, 0x00, 0x00]

    # magic byte for MBR partitioning - always at this location regardless of
    # options.sector
    GPTBlobBuffer[510:512] = [0x55, 0xAA]
    return i


def UpdatePartitionEntry(options, GPTBlobBuffer):

    i = 2 * options.sector_size
    # GUID of Boot Block
    GPTBlobBuffer[i:i + 16] = [0x2c, 0xba, 0xa0, 0xde, 0xdd, 0xcb, 0x05, 0x48,
        0xb4, 0xf9, 0xf4, 0x28, 0x25, 0x1c, 0x3e, 0x98]
    i += 16

    #This is to set Unique Partition GUID. Below Hex Value is : 00ChezaBootblock00
    GPTBlobBuffer[i:i + 16] = [0x00, 0x43, 0x68, 0x65, 0x7a, 0x61, 0x42, 0x6f,
        0x6f, 0x74, 0x62, 0x6c, 0x6f, 0x63, 0x6b, 0x00]
    i += 16

    # LBA of BootBlock Start Content
    GPTBlobBuffer[i:i + 8] = [0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
    i += 8

    # End LBA of BootBlock Content
    GPTBlobBuffer[i] = options.end_lba & 0xFF
    GPTBlobBuffer[i+1] = (options.end_lba>>8) & 0xFF
    GPTBlobBuffer[i+2] = (options.end_lba>>16) & 0xFF
    GPTBlobBuffer[i+3] = (options.end_lba>>24) & 0xFF
    GPTBlobBuffer[i+4] = (options.end_lba>>32) & 0xFF
    GPTBlobBuffer[i+5] = (options.end_lba>>40) & 0xFF
    GPTBlobBuffer[i+6] = (options.end_lba>>48) & 0xFF
    GPTBlobBuffer[i+7] = (options.end_lba>>56) & 0xFF
    i += 8

    # Attributes
    GPTBlobBuffer[i:i + 8] = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
    i += 8

    # Label
    GPTBlobBuffer[i:i + 17] = [0x62, 0x00, 0x6f, 0x00, 0x6f, 0x00, 0x74, 0x00,
        0x62, 0x00, 0x6c, 0x00, 0x6f, 0x00, 0x63, 0x00, 0x6b]

    return i

def UpdateGPTHeader(options, GPTBlobBuffer):

    i = options.sector_size
    # Signature and Revision and HeaderSize i.e. "EFI PART" and 00 00 01 00
    # and 5C 00 00 00
    GPTBlobBuffer[i:i + 16] = [0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54,
        0x00, 0x00, 0x01, 0x00, 0x5C, 0x00, 0x00, 0x00]
    i += 16

    # CRC is zeroed out till calculated later
    GPTBlobBuffer[i:i + 4] = [0x00, 0x00, 0x00, 0x00]
    i += 4

    # Reserved, set to 0
    GPTBlobBuffer[i:i + 4] = [0x00, 0x00, 0x00, 0x00]
    i += 4

    # Current LBA
    GPTBlobBuffer[i:i + 8] = [0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
    i += 8

    # Backup LBA, No Backup Gpt Used
    GPTBlobBuffer[i:i + 8] = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
    i += 8

    # First Usuable LBA (qc_sec + bootblock location)
    GPTBlobBuffer[i:i + 8] = [0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
    i += 8

    # Last Usuable LBA (qc_sec + bootblock end location)
    GPTBlobBuffer[i] = options.end_lba & 0xFF
    GPTBlobBuffer[i+1] = (options.end_lba>>8) & 0xFF
    GPTBlobBuffer[i+2] = (options.end_lba>>16) & 0xFF
    GPTBlobBuffer[i+3] = (options.end_lba>>24) & 0xFF
    GPTBlobBuffer[i+4] = (options.end_lba>>32) & 0xFF
    GPTBlobBuffer[i+5] = (options.end_lba>>40) & 0xFF
    GPTBlobBuffer[i+6] = (options.end_lba>>48) & 0xFF
    GPTBlobBuffer[i+7] = (options.end_lba>>56) & 0xFF
    i += 8

    # GUID
    GPTBlobBuffer[i:i + 16] = [0x32,0x1B,0x10,0x98,0xE2,0xBB,0xF2,0x4B,
        0xA0,0x6E,0x2B,0xB3,0x3D,0x00,0x0C,0x20]
    i += 16

    # Partition Table Entry LBA
    GPTBlobBuffer[i:i + 8] = [0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
    i += 8

    # Number of Partition Entries
    GPTBlobBuffer[i:i + 4] = [0x01, 0x00, 0x00, 0x00]
    i += 4

    # Size of One Partition Entry
    GPTBlobBuffer[i:i + 4] = [0x80, 0x00, 0x00, 0x00]
    i += 4

    # CRC of Partition Entry

    PartEntry = GPTBlobBuffer[options.sector_size*2:options.sector_size*2 + 128]
    CalcEntryCRC = crc32(b''.join(struct.pack("B", x) for x in PartEntry))

    GPTBlobBuffer[i] = CalcEntryCRC & 0xFF
    GPTBlobBuffer[i+1] = (CalcEntryCRC>>8) & 0xFF
    GPTBlobBuffer[i+2] = (CalcEntryCRC>>16) & 0xFF
    GPTBlobBuffer[i+3] = (CalcEntryCRC>>24) & 0xFF
    i += 4

    # CRC of Partition Table Header
    GPTHeader = GPTBlobBuffer[options.sector_size:options.sector_size + 92]
    CalcEntryCRC = crc32(b''.join(struct.pack("B", x) for x in GPTHeader))
    i = options.sector_size + 16

    GPTBlobBuffer[i] = CalcEntryCRC & 0xFF
    GPTBlobBuffer[i+1] = (CalcEntryCRC>>8) & 0xFF
    GPTBlobBuffer[i+2] = (CalcEntryCRC>>16) & 0xFF
    GPTBlobBuffer[i+3] = (CalcEntryCRC>>24) & 0xFF

    return i


if __name__ == '__main__':
    usage = 'usage: %prog [OPTIONS] INFILE OUTFILE\n\n' + \
            'Packages IMAGE in a GPT format.'
    parser = OptionParser(usage)
    parser.add_option('-s', type="int", dest='sector_size', default=4096,
                      help='Sector size in bytes [Default:4096(4KB)]',
                      metavar='SIZE')

    (options, args) = parser.parse_args()
    if len(args) != 2:
        print("Invalid arguments! Exiting...\n")
        parser.print_help()
        sys.exit(1)

    if options.sector_size != 4096 and options.sector_size != 512:
        print("Invalid Sector Size")
        sys.exit(1)

    options.inputfile = args[0]
    options.outputfile = args[1]

    with open(options.inputfile, 'rb+') as fin:
      bb_buffer = fin.read()

    # Round up to next sector if bootblock size not evenly divisible
    options.end_lba = ((len(bb_buffer) + options.sector_size - 1) //
      options.sector_size)
    # Add 3 sectors for MBR, GPT header and GPT partition entry
    options.end_lba += 3
    # Subtract one because this is last usable LBA, not amount of LBAs
    options.end_lba -= 1

    GPTBlobBuffer = [0] * (options.sector_size*3) #Size of MBR+GPT+PART_ENTRY

    UpdateMBR(options, GPTBlobBuffer)

    UpdatePartitionEntry(options, GPTBlobBuffer)

    UpdateGPTHeader(options, GPTBlobBuffer)

    with open(options.outputfile, 'wb') as fout:
      for b in GPTBlobBuffer:
        fout.write(struct.pack("B", b))
      fout.write(bb_buffer)
