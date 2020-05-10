#!/usr/bin/env python3
# Copyright (c) 2014, The Linux Foundation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause

import struct
import sys
import os

"""A utility to generate ipq8064 uber SBL..

The very first blob (aka 'uber SBL') read out of NOR SPI flash by the IPQ8064
maskrom is supposed to be a concatenation of up to three binaries: one to run
on the RPM, another one to run on the AP, and the third one - the actual
coreboot bootblock.

The uber SBL starts with the combined header descriptor of 80 bytes, with the
first two 4 byte words set to certain values, and the total size of the
payload saved at offsets 28 and 32.

To generate the uber SBL this utility expects two or three input file names in
the command line, the first file including the described header, and the
following one(s) - in QCA MBN format. This allows to create the uber SBL in
one or two invocations.

The input files are concatenated together aligned at 256 byte boundary offset
from the combined header. See Usage() below for more details.

The resulting uber SBL file is prepended by the same combined header adjusted
to reflect the new total file size.
"""

DEFAULT_OUTPUT_FILE_NAME = 'sbl-ro.mbn'

class NorSbl:
    """Object representing the uber SBL."""

    NOR_SBL1_HEADER    = '<II72s'
    NOR_SBL1_HEADER_SZ = struct.calcsize(NOR_SBL1_HEADER)
    ALIGNMENT          = 256    # Make sure this == UBER_SBL_PAD_SIZE
    NOR_CODE_WORD      = 0x844bdcd1
    MAGIC_NUM          = 0x73d71034

    def __init__(self, sbl1, verbose):
        """Initialize the object and verify the first file in the sequence.

        Args:
          sbl1: string, the name of the first out of the three input blobs,
                must be prepended by the combined header.
          verbose: boolean, if True - print debug information on the console.
        """
        self.verbose = verbose
        self.mbn_file_names = []
        if self.verbose:
            print('Reading ' + sbl1)

        try:
            self.sbl1 = open(sbl1, 'rb').read()
        except IOError as e:
            print('I/O error({0}): {1}'.format(e.errno, e.strerror))
            raise

        (codeword, magic,  _) = struct.unpack_from(
            self.NOR_SBL1_HEADER, self.sbl1)

        if codeword != self.NOR_CODE_WORD:
            print('\n\nError: Unexpected Codeword!')
            print('Codeword    : ' + ('0x%x' % self.NOR_CODE_WORD) + \
                ' != ' + ('0x%x' % codeword))
            sys.exit(-1)

        if magic != self.MAGIC_NUM:
            print('\n\nError: Unexpected Magic!')
            print('Magic    : ' + ('0x%x' % self.MAGIC_NUM) + \
                ' != ' + ('0x%x' % magic))
            sys.exit(-1)

    def Append(self, src):
        """Add a file to the list of files to be concatenated"""
        self.mbn_file_names.append(src)

    def PadOutput(self, outfile, size):
        """Pad output file to the required alignment.

        Adds 0xff to the passed in file to get its size to the ALIGNMENT
        boundary.

        Args:
          outfile: file handle of the file to be padded
          size: int, current size of the file

       Returns number of bytes in the added padding.
       """

        # Is padding needed?
        overflow = size % self.ALIGNMENT
        if overflow:
            pad_size = self.ALIGNMENT - overflow
            pad = b'\377' * pad_size
            outfile.write(pad)
            if self.verbose:
                print('Added %d byte padding' % pad_size)
            return pad_size
        return 0

    def Create(self, out_file_name):
        """Create the uber SBL.

        Concatenate input files with the appropriate padding and update the
        combined header to reflect the new blob size.

        Args:
          out_file_name: string, name of the file to save the generated uber
                         SBL in.
       """
        outfile = open(out_file_name, 'wb')
        total_size = len(self.sbl1) - self.NOR_SBL1_HEADER_SZ
        outfile.write(self.sbl1)

        for mbn_file_name in self.mbn_file_names:
            total_size += self.PadOutput(outfile, total_size)
            mbn_file_data = open(mbn_file_name, 'rb').read()
            outfile.write(mbn_file_data)
            if self.verbose:
                print('Added %s (%d bytes)' % (mbn_file_name,
                                               len(mbn_file_data)))
            total_size += len(mbn_file_data)

        outfile.seek(28)
        outfile.write(struct.pack('<I', total_size))
        outfile.write(struct.pack('<I', total_size))


def Usage(v):
    print('%s: [-v] [-h] [-o Output MBN] sbl1 sbl2 [bootblock]' % (
        os.path.basename(sys.argv[0])))
    print()
    print('Concatenates up to three mbn files: two SBLs and a coreboot bootblock')
    print('    -h This message')
    print('    -v verbose')
    print('    -o Output file name, (default: %s)\n' % DEFAULT_OUTPUT_FILE_NAME)
    sys.exit(v)

def main():
    verbose = 0
    mbn_output = DEFAULT_OUTPUT_FILE_NAME
    i = 0

    while i < (len(sys.argv) - 1):
        i += 1
        if (sys.argv[i] == '-h'):
            Usage(0)    # doesn't return

        if (sys.argv[i] == '-o'):
            mbn_output = sys.argv[i + 1]
            i += 1
            continue

        if (sys.argv[i] == '-v'):
            verbose = 1
            continue

        break

    argv = sys.argv[i:]
    if len(argv) < 2 or len(argv) > 3:
        Usage(-1)

    nsbl = NorSbl(argv[0], verbose)

    for mbnf in argv[1:]:
        nsbl.Append(mbnf)

    nsbl.Create(mbn_output)

if __name__ == '__main__':
    main()
