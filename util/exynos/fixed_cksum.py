#!/usr/bin/env python3
#
# SPDX-License-Identifier: BSD-3-Clause

"""
This utility computes and fills Exynos ROM checksum (for BL1 or BL2).
(Algorithm from U-Boot: tools/mkexynosspl.c)

Input: IN OUT DATA_SIZE

Output:

  IN padded out to DATA_SIZE, checksum at the end, written to OUT.
"""

import struct
import sys

def main(argv):
  if len(argv) != 4:
    exit('usage: %s IN OUT DATA_SIZE' % argv[0])

  in_name, out_name = argv[1:3]
  size = int(argv[3], 0)
  checksum_format = "<I"
  with open(in_name, "rb") as in_file, open(out_name, "wb") as out_file:
    data = in_file.read()
    checksum_size = struct.calcsize(checksum_format)
    data_size = size - checksum_size
    assert len(data) <= data_size
    checksum = struct.pack(checksum_format, sum(data))
    out_file.write(data + bytearray(data_size - len(data)) + checksum)


if __name__ == '__main__':
  main(sys.argv)
