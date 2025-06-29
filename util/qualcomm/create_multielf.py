#!/usr/bin/env python3
# ============================================================================
#
# @file create_multielf.py
#
# GENERAL DESCRIPTION
#   This tool takes in files and concatenates all the files in a single blob output.
#   Files are appended sequentially in order as provided in the arguments.
#
#  SPDX-License-Identifier: BSD-3-Clause
#
# ----------------------------------------------------------------------------
#
#                          EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
# 05/09/20   ds     Initial revision


import sys
import argparse
import os
import os.path
from datetime import datetime as dt

# default output filename is "multibin_<current time>.melf"
outfile_name = "multibin_" + ((dt.now()).strftime("%H_%M_%S")) + ".melf"
# default output path is current working directory
outfile_path = os.getcwd()


def check_outfile_path(dir_path):
    try:
        return os.makedirs(dir_path, exist_ok=True)
    except Exception as e:
        if type(e) == TypeError:
        # exception if we use python2
            pass
        else:
            print(e)
            sys.exit("invalid path provided for output file \n absolute path required")

    if not os.path.isdir(dir_path):
        try:
        #python2 compatible
            return os.makedirs(dir_path)
        except Exception as e:
            print(e)
            sys.exit("invalid path provided for output file \n absolute path required")


def process_out_file_arg(outfile_arg):
    global outfile_name
    global outfile_path

    if outfile_arg == os.path.join(outfile_path, outfile_name):
        return

    out_path, file_name = os.path.split(outfile_arg)

    if out_path and file_name and os.path.splitext(file_name)[1]:
        outfile_name = file_name
        check_outfile_path(out_path)
        outfile_path = os.path.abspath(out_path)
    elif str(outfile_arg).endswith(os.path.sep):
        check_outfile_path(outfile_arg)
        outfile_path = os.path.abspath(out_path)
    elif os.path.split(outfile_arg)[1] and os.path.splitext(os.path.split(outfile_arg)[1])[1]:
        outfile_name = file_name
    else:
        sys.exit("invalid output path or output file. use --help for help menu")


def get_abs_path_input_files(in_files):
    abs_path_list = []
    for infile in in_files:
        if not os.path.isfile(infile):
            sys.exit("-f OR --in_files argument error. {} is missing. use --help for help menu".format(infile))
        abs_path_list.append(os.path.abspath(infile))
    return abs_path_list


def make_concatenated_binary(input_file_list):
    infiles_abs_path_list = get_abs_path_input_files(input_file_list)
    out_file = os.path.join(outfile_path, outfile_name)
    if os.path.exists(out_file):
        os.remove(out_file)
    output_bin = open(out_file, mode='ab')
    for file in infiles_abs_path_list:
        in_file = open(file, mode='rb')
        output_bin.write(in_file.read())
        in_file.close()
    output_bin.close()


# main function for the tool
def main():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)

    # input file argument
    parser.add_argument('-f', '--in_files', help=r'comma separated absolute paths of files to be concatenated'
                                                 r'\neg: "-f C:\Temp\myelf1.elf,C:\Temp\myelf2.elf"',
                        type=lambda s: [str(item).strip() for item in s.split(',')],
                        required=True)

    # optional output file argument
    parser.add_argument('-o', '--out_file', help=r'* give file name OR absolute path OR file name with absolute path *'
                                                 r'\neg: "-o C:\Temp\\" OR "-o C:\Temp\myelf.melf" OR "-o myelf.melf"'
                                                 r'\n(if only giving output path - terminate by path separator)',
                        type=str,
                        nargs='?',
                        default=os.path.join(outfile_path, outfile_name))
    args = parser.parse_args()

    # check if minimum two input files are given to concatenate
    if len(args.in_files) < 2:
        sys.exit("minimum 2 input files are required for concatenating")

    # process the output path and file argument
    process_out_file_arg(args.out_file)

    # create the concatenated binary from the list of input files
    make_concatenated_binary(args.in_files)

    print("\n\n *** Created concatenated binary: " + outfile_name + " at " + outfile_path + os.path.sep + " ***")


if __name__ == "__main__":
    main()
