#!/usr/bin/env python3

# Script for editing APCB_V3 binaries, such as injecting SPDs.

import sys
import re
import argparse
from collections import namedtuple
from struct import *
import binascii
import os

# SPD_MAGIC matches the expected SPD header:
# Byte 0 = 0x23 = 512 bytes total / 384 bytes used
# Byte 1 = 0x11 = Revision 1.1
# Byte 2 = 0x11 = LPDDR4X SDRAM
#        = 0x13 = LP5 SDRAM
#        = 0x15 = LP5X SDRAM
# Byte 3 = 0x0E = Non-DIMM Solution
LP4_SPD_MAGIC = bytes.fromhex('2311110E')
LP5_SPD_MAGIC = bytes.fromhex('2311130E')
LP5X_SPD_MAGIC = bytes.fromhex('2311150E')
EMPTY_SPD = b'\x00' * 512

spd_ssp_struct_fmt = '??B?IIBBBxIIBBBx'
spd_ssp_struct = namedtuple(
    'spd_ssp_struct', 'SpdValid, DimmPresent, \
                    PageAddress, NvDimmPresent, \
                    DramManufacturersIDCode, Address, \
                    SpdMuxPresent, MuxI2CAddress, MuxChannel, \
                    Technology, Package, SocketNumber, \
                    ChannelNumber, DimmNumber')

apcb_v3_header_fmt = 'HHHHBBBBBBH'
apcb_v3_header = namedtuple(
    'apcb_v3_header', 'GroupId, TypeId, SizeOfType, \
    InstanceId, ContextType, ContextFormat, UnitSize, \
    PriorityMask, KeySize, KeyPos, BoardMask')

def parseargs():
    parser = argparse.ArgumentParser(description='Inject SPDs into APCB binaries')
    parser.add_argument(
        'apcb_in',
        type=str,
        help='APCB input file')
    parser.add_argument(
        'apcb_out',
        type=str,
        help='APCB output file')
    parser.add_argument(
        '--spd_sources',
        nargs='+',
        help='List of SPD sources')
    parser.add_argument(
        '--mem_type',
        type=str,
        default='lp4',
        help='Memory type [lp4|lp5|lp5x]. Default = lp4')
    return parser.parse_args()


def chksum(data):
    sum = 0
    for b in data[:16] + data[17:]:
        sum = (sum + b) & 0xff
    return (0x100 - sum) & 0xff


def inject(orig, insert, offset):
    return b''.join([orig[:offset], insert, orig[offset + len(insert):]])


def main():
    spd_magic = LP4_SPD_MAGIC

    args = parseargs()

    print(f'Reading input APCB from {args.apcb_in}')

    with open(args.apcb_in, 'rb') as f:
        apcb = f.read()

    orig_apcb_len = len(apcb)

    assert chksum(apcb) == apcb[16], f'ERROR: {args.apcb_in} checksum is invalid'

    print(f'Using SPD Sources = {args.spd_sources}')

    if args.mem_type == 'lp5':
        spd_magic = LP5_SPD_MAGIC
    elif args.mem_type == 'lp5x':
        spd_magic = LP5X_SPD_MAGIC

    spds = []
    for spd_source in args.spd_sources:
        with open(spd_source, 'rb') as f:
            spd_data = bytes.fromhex(re.sub(r'\s+', '', f.read().decode()))
        assert(len(spd_data) == 512), f'ERROR: {spd_source} not 512 bytes'
        spds.append(spd_data)

    spd_offset = 0
    instance = 0
    while True:
        spd_offset = apcb.find(spd_magic, spd_offset)
        if spd_offset < 0:
            print('No more SPD magic numbers in APCB')
            break

        spd_ssp_offset = spd_offset - calcsize(spd_ssp_struct_fmt)
        spd_ssp_bytes = apcb[spd_ssp_offset:spd_offset]
        spd_ssp = spd_ssp_struct._make(
            unpack(spd_ssp_struct_fmt, spd_ssp_bytes))

        assert spd_ssp.DimmNumber >= 0 and spd_ssp.DimmNumber <= 1, \
                'ERROR: Unexpected dimm number found in APCB'
        assert spd_ssp.ChannelNumber >= 0 and spd_ssp.ChannelNumber <= 1, \
                'ERROR: Unexpected channel number found in APCB'

        print(f'Found SPD instance {instance} with channel {spd_ssp.ChannelNumber} '
              f'and dimm {spd_ssp.DimmNumber} at offset {spd_offset}')

        # APCB V3 header is above first channel 0 entry
        if spd_ssp.ChannelNumber == 0:
            apcb_v3_header_offset = spd_ssp_offset - \
                calcsize(apcb_v3_header_fmt) - 4
            apcb_v3_header_bytes = apcb[apcb_v3_header_offset:
                                        apcb_v3_header_offset + calcsize(apcb_v3_header_fmt)]
            apcb_v3 = apcb_v3_header._make(
                unpack(apcb_v3_header_fmt, apcb_v3_header_bytes))
            apcb_v3 = apcb_v3._replace(BoardMask=(1 << instance))

        if instance < len(spds):
            print(f'Enabling channel {spd_ssp.ChannelNumber}, '
                  f'dimm {spd_ssp.DimmNumber} and injecting SPD')
            spd_ssp = spd_ssp._replace(SpdValid=True, DimmPresent=True)
            spd = spds[instance]
        else:
             print(f'Disabling channel {spd_ssp.ChannelNumber}, '
                   f'dimm {spd_ssp.DimmNumber} and clearing SPD')
             spd_ssp = spd_ssp._replace(SpdValid=False, DimmPresent=False)
             spd = EMPTY_SPD

        assert len(spd) == 512, f'ERROR: Expected SPD to be 512 bytes, got {len(spd)}'

        apcb = inject(apcb, pack(spd_ssp_struct_fmt, *spd_ssp), spd_ssp_offset)
        apcb = inject(apcb, spd, spd_offset)
        if spd_ssp.ChannelNumber == 0:
            apcb = inject(apcb, pack(apcb_v3_header_fmt, *apcb_v3), apcb_v3_header_offset)
        else:
            instance += 1

        spd_offset += 512

    assert instance >= len(spds), \
            f'ERROR: Not enough SPD slots in APCB, found {instance}, need {len(spds)}'

    print(f'Fixing checksum and writing to {args.apcb_out}')

    apcb = inject(apcb, bytes([chksum(apcb)]), 16)

    assert chksum(apcb) == apcb[16], 'ERROR: Final checksum is invalid'
    assert orig_apcb_len == len(apcb), \
                'ERROR: The size of the APCB binary changed.'

    print(f'Writing {len(apcb)} bytes to {args.apcb_out}')

    with open(args.apcb_out, 'wb') as f:
        f.write(apcb)


if __name__ == "__main__":
    main()
