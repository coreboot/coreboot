#!/usr/bin/env python
# spdtool - Tool for partial deblobbing of UEFI firmware images
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Parse a blob and search for SPD files.
# First it is searched for a possible SPD header.
#
# For each candidate the function verify_match is invoked to check
# additional fields (known bits, reserved bits, CRC, ...)
#
# Dumps the found SPDs into the current folder.
#
# Implemented:
#  DDR4 SPDs
#

import argparse
import crc16
import struct


class Parser(object):
    def __init__(self, blob, verbose=False, ignorecrc=False):
        self.blob = blob
        self.ignorecrc = ignorecrc
        self.verbose = verbose

    @staticmethod
    def get_matches():
        """Return the first byte to look for"""
        raise Exception("Function not implemented")

    def verify_match(self, header, offset):
        """Return true if it looks like a SPD"""
        raise Exception("Function not implemented")

    def get_len(self, header, offset):
        """Return the length of the SPD"""
        raise Exception("Function not implemented")

    def get_part_number(self, offset):
        """Return the part number in SPD"""
        return ""

    def get_manufacturer_id(self, offset):
        """Return the manufacturer ID in SPD"""
        return 0xffff

    def get_mtransfers(self, offset):
        """Return the number of MT/s"""
        return 0

    def get_manufacturer(self, offset):
        """Return manufacturer as string"""
        id = self.get_manufacturer_id(offset)
        if id == 0xffff:
            return "Unknown"
        ids = {
            0x2c80: "Crucial/Micron",
            0x4304: "Ramaxel",
            0x4f01: "Transcend",
            0x9801: "Kingston",
            0x987f: "Hynix",
            0x9e02: "Corsair",
            0xb004: "OCZ",
            0xad80: "Hynix/Hyundai",
            0xb502: "SuperTalent",
            0xcd04: "GSkill",
            0xce80: "Samsung",
            0xfe02: "Elpida",
            0xff2c: "Micron",
        }
        if id in ids:
            return ids[id]
        return "Unknown"

    def blob_as_ord(self, offset):
        """Helper for python2/python3 compatibility"""
        return self.blob[offset] if type(self.blob[offset]) is int \
            else ord(self.blob[offset])

    def search(self, start):
        """Search for SPD at start. Returns -1 on error or offset
           if found.
        """
        for i in self.get_matches():
            for offset in range(start, len(self.blob)):
                if self.blob_as_ord(offset) == i and \
                    self.verify_match(i, offset):
                    return offset, self.get_len(i, offset)
        return -1, 0


class SPD4Parser(Parser):
    @staticmethod
    def get_matches():
        """Return DDR4 possible header candidates"""
        ret = []
        for i in [1, 2, 3, 4]:
            for j in [1, 2]:
                ret.append(i + j * 16)
        return ret

    def verify_match(self, header, offset):
        """Verify DDR4 specific bit fields."""
        # offset 0 is a candidate, no need to validate
        if self.blob_as_ord(offset + 1) == 0xff:
            return False
        if self.blob_as_ord(offset + 2) != 0x0c:
            return False
        if self.blob_as_ord(offset + 5) & 0xc0 > 0:
            return False
        if self.blob_as_ord(offset + 6) & 0xc > 0:
            return False
        if self.blob_as_ord(offset + 7) & 0xc0 > 0:
            return False
        if self.blob_as_ord(offset + 8) != 0:
            return False
        if self.blob_as_ord(offset + 9) & 0xf > 0:
            return False
        if self.verbose:
            print("%x: Looks like DDR4 SPD" % offset)

        crc = crc16.crc16xmodem(self.blob[offset:offset + 0x7d + 1])
        # Vendors ignore the endianness...
        crc_spd1 = self.blob_as_ord(offset + 0x7f)
        crc_spd1 |= (self.blob_as_ord(offset + 0x7e) << 8)
        crc_spd2 = self.blob_as_ord(offset + 0x7e)
        crc_spd2 |= (self.blob_as_ord(offset + 0x7f) << 8)
        if crc != crc_spd1 and crc != crc_spd2:
            if self.verbose:
                print("%x: CRC16 doesn't match" % offset)
            if not self.ignorecrc:
                return False

        return True

    def get_len(self, header, offset):
        """Return the length of the SPD found."""
        if (header >> 4) & 7 == 1:
            return 256
        if (header >> 4) & 7 == 2:
            return 512
        return 0

    def get_part_number(self, offset):
        """Return part number as string"""
        if offset + 0x15c >= len(self.blob):
            return ""
        tmp = self.blob[offset + 0x149:offset + 0x15c + 1]
        return tmp.decode('utf-8').rstrip()

    def get_manufacturer_id(self, offset):
        """Return manufacturer ID"""
        if offset + 0x141 >= len(self.blob):
            return 0xffff
        tmp = self.blob[offset + 0x140:offset + 0x141 + 1]
        return struct.unpack('H', tmp)[0]

    def get_mtransfers(self, offset):
        """Return MT/s as specified by MTB and FTB"""
        if offset + 0x7d >= len(self.blob):
            return 0

        if self.blob_as_ord(offset + 0x11) != 0:
            return 0
        mtb = 8.0
        ftb = 1000.0
        tmp = self.blob[offset + 0x12:offset + 0x12 + 1]
        tckm = struct.unpack('B', tmp)[0]
        tmp = self.blob[offset + 0x7d:offset + 0x7d + 1]
        tckf = struct.unpack('b', tmp)[0]
        return int(2000 / (tckm / mtb + tckf / ftb))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='SPD rom dumper')
    parser.add_argument('--blob', required=True,
                        help='The ROM to search SPDs in.')
    parser.add_argument('--spd4', action='store_true', default=False,
                        help='Search for DDR4 SPDs.')
    parser.add_argument('--hex', action='store_true', default=False,
                        help='Store SPD in hex format otherwise binary.')
    parser.add_argument('-v', '--verbose', help='increase output verbosity',
                        action='store_true')
    parser.add_argument('--ignorecrc', help='Ignore CRC mismatch',
                        action='store_true', default=False)
    args = parser.parse_args()

    blob = open(args.blob, "rb").read()

    if args.spd4:
        p = SPD4Parser(blob, args.verbose, args.ignorecrc)
    else:
        raise Exception("Must specify one of the following arguments:\n--spd4")

    offset = 0
    cnt = 0
    while True:
        offset, length = p.search(offset)
        if length == 0:
            break
        print("Found SPD at 0x%x" % offset)
        print(" '%s', size %d, manufacturer %s (0x%04x) %d MT/s\n" %
              (p.get_part_number(offset), length, p.get_manufacturer(offset),
               p.get_manufacturer_id(offset), p.get_mtransfers(offset)))
        filename = "spd-%d-%s-%s.bin" % (cnt, p.get_part_number(offset),
            p.get_manufacturer(offset))
        filename = filename.replace("/", "_")
        filename = "".join([c for c in filename if c.isalpha() or c.isdigit()
                    or c == '-' or c == '.' or c == '_']).rstrip()
        if not args.hex:
            open(filename, "wb").write(blob[offset:offset + length])
        else:
            filename += ".hex"
            with open(filename, "w") as fn:
                j = 0
                for i in blob[offset:offset + length]:
                    fn.write("%02X" % struct.unpack('B', i)[0])
                    fn.write(" " if j < 15 else "\n")
                    j = (j + 1) % 16
        offset += 1
        cnt += 1
