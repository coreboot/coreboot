#!/usr/bin/env python3

# Script for injecting SPDs into APCB_v3a binaries.

import re
import argparse
from collections import namedtuple
from struct import *

APCB_CHECKSUM_OFFSET = 16
SPD_ENTRY_MAGIC = bytes.fromhex('0200480000000000')
SPD_SIZE = 512
EMPTY_SPD = b'\x00' * SPD_SIZE
ZERO_BLOCKS = (2, 4, 6, 7)
SPD_BLOCK_SIZE = 64
SPD_BLOCK_HEADER_FMT = '<HHHH'
SPD_BLOCK_HEADER_SIZE = calcsize(SPD_BLOCK_HEADER_FMT)
spd_block_header = namedtuple(
    'spd_block_header', 'Type, Length, Key, Reserved')

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
    return parser.parse_args()


# Calculate checksum of APCB binary
def chksum(data):
    sum = 0
    for i, v in enumerate(data):
        if i == APCB_CHECKSUM_OFFSET: continue
        sum = (sum + v) & 0xff
    return (0x100 - sum) & 0xff


# Inject bytes into binary blob by overwriting
def inject(orig, insert, offset):
    return b''.join([orig[:offset], insert, orig[offset + len(insert):]])


def main():
    args = parseargs()

    # Load input APCB
    print(f'Reading input APCB from {args.apcb_in}')
    with open(args.apcb_in, 'rb') as f:
        apcb = f.read()
    assert chksum(apcb) == apcb[APCB_CHECKSUM_OFFSET], 'Initial checksum is invalid'
    orig_apcb_len = len(apcb)

    # Load SPDs
    print(f'Using SPD Sources = {", ".join(args.spd_sources)}')
    spds = []
    for spd_source in args.spd_sources:
        with open(spd_source, 'rb') as f:
            spd_data = bytes.fromhex(re.sub(r'\s+', '', f.read().decode()))
        assert len(spd_data) == SPD_SIZE, f'{spd_source} is not {SPD_SIZE} bytes'
        # Verify ZERO_BLOCKS are zero
        for b in ZERO_BLOCKS:
            assert all(v==0 for v in spd_data[b*SPD_BLOCK_SIZE:(b+1)*SPD_BLOCK_SIZE]), f'SPD block #{b} is not zero'
        spds.append(spd_data)
    assert len(spds) > 0, "No SPDs provided"

    # Inject SPDs into APCB
    apcb_offset = 0
    spd_idx = 0
    while True:
        apcb_offset = apcb.find(SPD_ENTRY_MAGIC, apcb_offset)
        if apcb_offset < 0:
            print(f'No more SPD entries found')
            assert spd_idx >= len(spds), f'Not enough SPD entries in APCB. Need {len(spds)}, found {spd_idx}'
            break

        if spd_idx < len(spds):
            print(f'Injecting SPD instance {spd_idx}')
            spd = spds[spd_idx]
        else:
            print(f'Injecting empty SPD for instance {spd_idx}')
            spd = EMPTY_SPD

        # Inject SPD blocks
        for b in range(int(SPD_SIZE/SPD_BLOCK_SIZE)):
            if b in ZERO_BLOCKS: continue
            header_data = apcb[apcb_offset:apcb_offset + SPD_BLOCK_HEADER_SIZE]
            header = spd_block_header._make(unpack(SPD_BLOCK_HEADER_FMT, header_data))
            socket = (header.Key >> 12) & 0xF
            channel = (header.Key >> 8) & 0xF
            dimm = (header.Key >> 4) & 0xF
            block_id = (header.Key >> 0) & 0xF

            assert header.Type == 2
            assert header.Length == SPD_BLOCK_HEADER_SIZE + SPD_BLOCK_SIZE
            assert socket == 0
            assert channel == 0
            assert block_id == b
            assert dimm == 0
            assert header.Reserved == 0

            spd_block = spd[b*SPD_BLOCK_SIZE:(b+1)*SPD_BLOCK_SIZE]
            apcb_offset += SPD_BLOCK_HEADER_SIZE
            apcb = inject(apcb, spd_block, apcb_offset)
            apcb_offset += SPD_BLOCK_SIZE

        spd_idx += 1

    # Fix APCB checksum
    print(f'Fixing APCB checksum')
    apcb = inject(apcb, bytes([chksum(apcb)]), APCB_CHECKSUM_OFFSET)
    assert chksum(apcb) == apcb[APCB_CHECKSUM_OFFSET], 'Final checksum is invalid'
    assert orig_apcb_len == len(apcb), 'The size of the APCB changed.'

    # Write APCB to file
    print(f'Writing {len(apcb)} byte APCB to {args.apcb_out}')
    with open(args.apcb_out, 'wb') as f:
        f.write(apcb)


if __name__ == "__main__":
    main()
