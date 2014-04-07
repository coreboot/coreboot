#!/usr/bin/python
#
# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

import os
import struct
import sys

PROG_NAME = os.path.basename(sys.argv[0])

def create_header(base, size):
    """Returns a packed MBN header image with the specified base and size.

    @arg base: integer, specifies the image load address in RAM
    @arg size: integer, specifies the size of the image
    @returns: string, the MBN header
    """

    # SBLs require size to be 4 bytes aligned.
    size = (size + 3) & 0xfffffffc

    # We currently do not support appending certificates. Signing GPL
    # code might violate the GPL. So U-Boot will never be signed. So
    # this is not required for U-Boot.

    header = [
        0x5,         # Type: APPSBL
        0x3,         # Version: 3
        0x0,         # Image source pointer
        base,        # Image destination pointer
        size,        # Code Size + Cert Size + Signature Size
        size,        # Code Size
        base + size, # Destination + Code Size
        0x0,         # Signature Size
        base + size, # Destination + Code Size + Signature Size
        0x0,         # Cert Size
    ]

    header_packed = struct.pack('<10I', *header)
    return header_packed

def mkheader(base_addr, infname, outfname):
    """Prepends the image with the MBN header.

    @arg base_addr: integer, specifies the image load address in RAM
    @arg infname: string, image filename
    @arg outfname: string, output image with header prepended
    @raises IOError: if reading/writing input/output file fails
    """
    with open(infname, "rb") as infp:
        image = infp.read()
        insize = len(image)

    if base_addr > 0xFFFFFFFF:
        raise ValueError("invalid base address")

    if base_addr + insize > 0xFFFFFFFF:
        raise ValueError("invalid destination range")

    header = create_header(base_addr, insize)
    with open(outfname, "wb") as outfp:
        outfp.write(header)
        outfp.write(image)

def usage(msg=None):
    """Print command usage.

    @arg msg: string, error message if any (default: None)
    """
    if msg != None:
        sys.stderr.write("%s: %s\n" % (PROG_NAME, msg))

    print "Usage: %s <base-addr> <input-file> <output-file>" % PROG_NAME

    if msg != None:
        exit(1)

def main():
    """Main entry function"""

    if len(sys.argv) != 4:
        usage("incorrect number of arguments")

    try:
        base_addr = int(sys.argv[1], 0)
        infname = sys.argv[2]
        outfname = sys.argv[3]
    except ValueError as e:
        sys.stderr.write("mkheader: invalid base address '%s'\n" % sys.argv[1])
        exit(1)

    try:
        mkheader(base_addr, infname, outfname)
    except IOError as e:
        sys.stderr.write("%s: %s\n" % (PROG_NAME, e))
        exit(1)
    except ValueError as e:
        sys.stderr.write("%s: %s\n" % (PROG_NAME, e))
        exit(1)

if __name__ == "__main__":
    main()

