#!/usr/bin/env python3

# Script for editing APCB binaries, such as injecting SPDs and GPIO
# configurations.

import sys
import re
import argparse
from collections import namedtuple
from struct import *

GPIO_MAGIC = bytes.fromhex('fadeddad' * 3)
SPD_MAGIC = bytes.fromhex('f005ba110000')
EMPTY_SPD = b'\x00' * 512

spd_ssp_struct_fmt = '??B?IIBBBxIIBBBx'
spd_ssp_struct = namedtuple(
    'spd_ssp_struct', 'SpdValid, DimmPresent, \
                    PageAddress, NvDimmPresent, \
                    DramManufacturersIDCode, Address, \
                    SpdMuxPresent, MuxI2CAddress, MuxChannel, \
                    Technology, Package, SocketNumber, \
                    ChannelNumber, DimmNumber')


def parseargs():
    parser = argparse.ArgumentParser(description='Inject SPDs and SPD GPIO \
                     selection pins into APCB binaries')
    parser.add_argument(
        'apcb_in',
        nargs='?',
        type=argparse.FileType('rb'),
        default=sys.stdin,
        help='APCB input file')
    parser.add_argument(
        'apcb_out',
        nargs='?',
        type=argparse.FileType('wb'),
        default=sys.stdout,
        help='APCB output file')
    parser.add_argument(
        '--spd_0_0',
        type=argparse.FileType('rb'),
        help='SPD input file for channel 0, dimm 0')
    parser.add_argument(
        '--spd_0_1',
        type=argparse.FileType('rb'),
        help='SPD input file for channel 0, dimm 1')
    parser.add_argument(
        '--spd_1_0',
        type=argparse.FileType('rb'),
        help='SPD input file for channel 1, dimm 0')
    parser.add_argument(
        '--spd_1_1',
        type=argparse.FileType('rb'),
        help='SPD input file for channel 1, dimm 1')
    parser.add_argument(
        '--hex',
        action='store_true',
        help='SPD input file is hex encoded, binary otherwise')
    parser.add_argument(
        '--strip_manufacturer_information',
        action='store_true',
        help='Strip all manufacturer information from SPD')
    parser.add_argument(
        '--board_id_gpio0',
        type=int,
        required=True,
        nargs=3,
        help='Board ID GPIO 0: NUMBER IO_MUX BANK_CTRL')
    parser.add_argument(
        '--board_id_gpio1',
        type=int,
        required=True,
        nargs=3,
        help='Board ID GPIO 1: NUMBER IO_MUX BANK_CTRL')
    parser.add_argument(
        '--board_id_gpio2',
        type=int,
        required=True,
        nargs=3,
        help='Board ID GPIO 2: NUMBER IO_MUX BANK_CTRL')
    parser.add_argument(
        '--board_id_gpio3',
        type=int,
        required=True,
        nargs=3,
        help='Board ID GPIO 3: NUMBER IO_MUX BANK_CTRL')
    return parser.parse_args()


def chksum(data):
    sum = 0
    for b in data[:16] + data[17:]:
        sum = (sum + b) & 0xff
    return (0x100 - sum) & 0xff


def inject(orig, insert, offset):
    return b''.join([orig[:offset], insert, orig[offset + len(insert):]])


def main():
    args = parseargs()

    print("Reading input APCB from %s" % (args.apcb_in.name))

    apcb = args.apcb_in.read()

    orig_apcb_len = len(apcb)

    gpio_offset = apcb.find(GPIO_MAGIC)
    assert gpio_offset > 0, "GPIO magic number not found"
    print('GPIO magic number found at offset 0x%x' % gpio_offset)
    gpio_array = (args.board_id_gpio0 + args.board_id_gpio1 +
                  args.board_id_gpio2 + args.board_id_gpio3)
    print('Writing SPD GPIO array %s' % gpio_array)
    apcb = inject(apcb, pack('BBBBBBBBBBBB', *gpio_array), gpio_offset)

    spd_offset = 0
    while True:
        spd_offset = apcb.find(SPD_MAGIC, spd_offset)
        if spd_offset < 0:
            break

        spd_ssp_offset = spd_offset - calcsize(spd_ssp_struct_fmt)
        spd_ssp_bytes = apcb[spd_ssp_offset:spd_offset]
        spd_ssp = spd_ssp_struct._make(
            unpack(spd_ssp_struct_fmt, spd_ssp_bytes))

        assert spd_ssp.DimmNumber >= 0 and spd_ssp.DimmNumber <= 1, \
                "Unexpected dimm number found in APCB"
        assert spd_ssp.ChannelNumber >= 0 and spd_ssp.ChannelNumber <= 1, \
                "Unexpected channel number found in APCB"

        print("Found SPD magic number with channel %d and dimm %d "
              "at offset 0x%x" % (spd_ssp.ChannelNumber, spd_ssp.DimmNumber,
                                    spd_offset))

        dimm_channel = (spd_ssp.ChannelNumber, spd_ssp.DimmNumber)
        spd = None
        if dimm_channel == (0, 0) and args.spd_0_0:
            spd = args.spd_0_0.read()
        elif dimm_channel == (0, 1) and args.spd_0_1:
            spd = args.spd_0_1.read()
        elif dimm_channel == (1, 0) and args.spd_1_0:
            spd = args.spd_1_0.read()
        elif dimm_channel == (1, 1) and args.spd_1_1:
            spd = args.spd_1_0.read()

        if spd:
            if args.hex:
                spd = spd.decode()
                spd = re.sub(r'#.*', '', spd)
                spd = re.sub(r'\s+', '', spd)
                spd = bytes.fromhex(spd)

            assert len(spd) == 512, \
                            "Expected SPD to be 512 bytes, got %d" % len(spd)

            if args.strip_manufacturer_information:
                print("Stripping manufacturer information from SPD")
                spd = spd[0:320] + b'\x00'*64 + spd[320+64:]

                assert len(spd) == 512, \
                                "Error while stripping SPD manufacurer information"

            print("Enabling channel %d, dimm %d and injecting SPD" %
                  (spd_ssp.ChannelNumber, spd_ssp.DimmNumber))
            spd_ssp = spd_ssp._replace(SpdValid=True, DimmPresent=True)

        else:
            print("Disabling channel %d, dimm %d and clearing SPD" %
                  (spd_ssp.ChannelNumber, spd_ssp.DimmNumber))
            spd_ssp = spd_ssp._replace(SpdValid=False, DimmPresent=False)
            spd = EMPTY_SPD

        apcb = inject(apcb, pack(spd_ssp_struct_fmt, *spd_ssp), spd_ssp_offset)
        apcb = inject(apcb, spd, spd_offset)

        spd_offset += 512

    print("Fixing checksum and writing to %s" % (args.apcb_out.name))

    apcb = inject(apcb, bytes([chksum(apcb)]), 16)

    assert chksum(apcb) == apcb[16], "Checksum is invalid"
    assert orig_apcb_len == len(apcb), \
                "The size of the APCB binary changed, this should not happen."

    args.apcb_out.write(apcb)


if __name__ == "__main__":
    main()
