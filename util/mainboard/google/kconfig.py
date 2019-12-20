#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Add a new variant to the Kconfig and Kconfig.name for the baseboard

To start a new variant of an existing baseboard, we need to add
the variant into the Kconfig and Kconfig.name files for the
baseboard. In Kconfig, we have two sections that need additional
entries, MAINBOARD_PART_NUMBER and VARIANT_DIR.

The MAINBOARD_PART_NUMBER and VARIANT_DIR just use various
capitalizations of the variant name to create the strings.

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

from __future__ import print_function
import argparse
import sys


def main():
  parser = argparse.ArgumentParser(
      description='Add strings to coreboot Kconfig for a new board variant')
  parser.add_argument('--board', type=str, required=True,
                      help='Name of the baseboard')
  parser.add_argument('--variant', type=str, required=True,
                      help='Name of the board variant')
  args = parser.parse_args()

  if args.board not in ['hatch', 'volteer']:
    print('Unsupported baseboard "' + args.board + '"')
    sys.exit(1)

  add_to_Kconfig(args.variant)
  add_to_Kconfig_name(args.board, args.variant)


def add_to_Kconfig(variant_name):
  """Add options for the variant to the Kconfig

  Open the Kconfig file and read it line-by-line. When we detect that we're
  in one of the sections of interest, wait until we get a blank line
  (signalling the end of that section), and then add our new line before
  the blank line. The updated lines are written out to Kconfig.new in the
  same directory as Kconfig.

  variant_name    The name of the board variant, e.g. 'kohaku'
  """
  # These are the part of the strings that we'll add to the sections
  BOARD = 'BOARD_GOOGLE_' + variant_name.upper()
  lowercase = variant_name.lower()
  capitalized = lowercase.capitalize()

  # These flags track whether we're in a section where we need to add an option
  in_mainboard_part_number = False
  in_variant_dir = False

  inputname = 'Kconfig'
  outputname = 'Kconfig.new'
  with open(outputname, 'w') as outfile:
    with open(inputname, 'r') as infile:
      for rawline in infile:
        line = rawline.rstrip('\r\n')

        # Are we in one of the sections of interest?
        if line == 'config MAINBOARD_PART_NUMBER':
          in_mainboard_part_number = True
        if line == 'config VARIANT_DIR':
          in_variant_dir = True

        # Are we at the end of a section, and if so, is it one of the
        # sections of interest?
        if line == '':
          if in_mainboard_part_number:
            print('\tdefault "' + capitalized + '" if ' + BOARD, file=outfile)
            in_mainboard_part_number = False
          if in_variant_dir:
            print('\tdefault "' + lowercase + '" if ' + BOARD, file=outfile)
            in_variant_dir = False

        print(line, file=outfile)


def add_to_Kconfig_name(baseboard_name, variant_name):
  """Add a config section for the variant to the Kconfig.name

  Kconfig.name is easier to modify than Kconfig; it only has a block at
  the end with the new variant's details.

  baseboard_name  The name of the baseboard, e.g. 'hatch'
                  We expect the caller to have checked that it is one we support
  variant_name    The name of the board variant, e.g. 'kohaku'
  """
  # Board name for the config section
  uppercase = variant_name.upper()
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
      if baseboard_name == 'hatch':
        print('\nconfig ' + 'BOARD_GOOGLE_' + uppercase, file=outfile)
        print('\tbool "-> ' + capitalized + '"', file=outfile)
        print('\tselect BOARD_GOOGLE_BASEBOARD_HATCH', file=outfile)
        print('\tselect BOARD_ROMSIZE_KB_16384', file=outfile)

      if baseboard_name == 'volteer':
        print('\nconfig ' + 'BOARD_GOOGLE_' + uppercase, file=outfile)
        print('\tbool "-> ' + capitalized + '"', file=outfile)
        print('\tselect BOARD_GOOGLE_BASEBOARD_VOLTEER', file=outfile)


if __name__ == '__main__':
  main()
