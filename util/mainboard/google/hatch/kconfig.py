#!/usr/bin/python3
"""Add a new variant to the Kconfig and Kconfig.name for the baseboard

To start a new variant of an existing baseboard, we need to add
the variant into the Kconfig and Kconfig.name files for the
baseboard. In Kconfig, we have three sections that need additional
entries, GBB_HWID, MAINBOARD_PART_NUMBER, and VARIANT_DIR.

In GBB_HWID, we need to add a HWID that includes a numeric suffix.
The numeric suffix is the CRC-32 of the all-caps ASCII name,
modulo 10000.
For example, if the board name is "Fizz", we calculate the CRC of
"FIZZ TEST", which is 0x598C492D. In decimal, the value is 1502365997,
modulo 10000 is 5997. So the HWID string is "FIZZ TEST 5997"
In the past, we have used an online CRC-32 calculator such as
https://www.lammertbies.nl/comm/info/crc-calculation.html, and then
used the calculator app to convert to decimal and take the last
4 digits.

The MAINBOARD_PART_NUMBER and VARIANT_DIR are simpler, just using
various capitalizations of the variant name to create the strings.

Kconfig.name adds an entire section for the new variant, and all
of these use various capitalizations of the variant name. The strings
in this section are SOC-specific, so we'll need versions for each
SOC that we support.

Copyright 2019 Google LLC.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
"""

import argparse
import zlib


def main():
  parser = argparse.ArgumentParser(
    description="Add strings to coreboot Kconfig for a new board variant")
  parser.add_argument('--name', type=str, required=True,
    help='Name of the board variant')
  args = parser.parse_args()

  add_to_Kconfig(args.name)
  add_to_Kconfig_name(args.name)


def get_gbb_hwid(variant_name):
  """Create the GBB_HWID for a variant

  variant_name    The name of the board variant, e.g. 'kohaku'

  Returns:
    GBB_HWID string for the board variant, e.g. 'KOHAKU TEST 1953'

  Note that the case of the variant name does not matter; it gets
  converted to all uppercase as part of this function."""
  hwid = variant_name + ' test'
  upperhwid = hwid.upper()
  # Force conversion to unsigned by bitwise AND with (2^32)-1.
  # See the docs for crc32 at https://docs.python.org/3/library/zlib.html
  # for why '& 0xffffffff' is necessary.
  crc = zlib.crc32(upperhwid.encode('UTF-8')) & 0xffffffff
  gbb_hwid = upperhwid + ' ' + str(crc % 10000).zfill(4)
  return gbb_hwid


def add_to_Kconfig(variant_name):
  """Add options for the variant to the Kconfig

  Open the Kconfig file and read it line-by-line. When we detect that we're
  in one of the sections of interest, wait until we get a blank line
  (signalling the end of that section), and then add our new line before
  the blank line. The updated lines are written out to Kconfig.new in the
  same directory as Kconfig.

  variant_name    The name of the board variant, e.g. 'kohaku'"""
  # These are the part of the strings that we'll add to the sections
  BOARD = 'BOARD_GOOGLE_' + variant_name.upper()
  gbb_hwid = get_gbb_hwid(variant_name)
  lowercase = variant_name.lower()
  capitalized = lowercase.capitalize()

  # These flags track whether we're in a section where we need to add an option
  in_gbb_hwid = False
  in_mainboard_part_number = False
  in_variant_dir = False

  inputname = 'Kconfig'
  outputname = 'Kconfig.new'
  with open(outputname, 'w') as outfile:
    with open(inputname, 'r') as infile:
      for rawline in infile:
        line = rawline.rstrip('\r\n')

        # Are we in one of the sections of interest?
        if line == 'config GBB_HWID':
          in_gbb_hwid = True
        if line == 'config MAINBOARD_PART_NUMBER':
          in_mainboard_part_number = True
        if line == 'config VARIANT_DIR':
          in_variant_dir = True

        # Are we at the end of a section, and if so, is it one of the
        # sections of interest?
        if line == '':
          if in_gbb_hwid:
            print('\tdefault "' + gbb_hwid + '" if ' + BOARD, file=outfile)
            in_gbb_hwid = False
          if in_mainboard_part_number:
            print('\tdefault "' + capitalized + '" if ' + BOARD, file=outfile)
            in_mainboard_part_number = False
          if in_variant_dir:
            print('\tdefault "' + lowercase + '" if ' + BOARD, file=outfile)
            in_variant_dir = False

        print(line, file=outfile)


def add_to_Kconfig_name(variant_name):
  """Add a config section for the variant to the Kconfig.name

  Kconfig.name is easier to modify than Kconfig; it only has a block at
  the end with the new variant's details.

  config BOARD_GOOGLE_${VARIANT}

  variant_name    The name of the board variant, e.g. 'kohaku'"""
  # Board name for the config section
  uppercase = variant_name.upper()
  BOARD = 'BOARD_GOOGLE_' + uppercase
  capitalized = variant_name.lower().capitalize()

  inputname = 'Kconfig.name'
  outputname = 'Kconfig.name.new'
  with open(outputname, 'w') as outfile:
    with open(inputname, 'r') as infile:
      # Copy all input lines to output
      for rawline in infile:
        line = rawline.rstrip('\r\n')
        print(line, file=outfile)

      # Now add the new section
      print('\nconfig ' + BOARD, file=outfile)
      print('\tbool "-> ' + capitalized + '"', file=outfile)
      print('\tselect BOARD_GOOGLE_BASEBOARD_HATCH', file=outfile)
      print('\tselect BOARD_ROMSIZE_KB_16384', file=outfile)


if __name__ == '__main__':
  main()
