#!/usr/bin/python
# Copyright (C) 2018 Intel Corporation.
# written by Naresh G Solanki<naresh.solanki@intel.com> and
#            Maulik V Vaghela <maulik.v.vaghela@intel.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

"""
This utilty generate json output to post comment in gerrit.

INPUT: output of checkpatch.pl.
OUTPUT: json format output that can be used to post comment in gerrit
"""
import os
import sys
import json

data = {}
data['comments'] = []
list_temp = {}

def update_struct( file_path, msg_output, line_number):
    if file_path not in list_temp:
        list_temp[file_path] = []
    list_temp[file_path].append({
        "line" : line_number,
        "message" : msg_output,}
    )

def parse_file(input_file):
    fp = open (input_file, "r")
    for line in fp:
        if "ERROR" in line:
            msg_output = line.split("ERROR:")[1].strip()
        elif "WARNING" in line:
            msg_output = line.split("WARNING:")[1].strip()
        elif "FILE" in line:
            temp = line.split("FILE:")
            file_path = temp[1].split(":")[0]
            line_number = temp[1].split(":")[1]
            update_struct( file_path.strip(), msg_output, str(line_number)  )
        else:
            continue
    fp.close()

def main():
    if (len(sys.argv) < 3) or (sys.argv[1] == "-h"):
        print "HELP:"
        print  sys.argv[0] + " <input file> <output file in json>"
        sys.exit()

    print sys.argv[1]
    parse_file(sys.argv[1])
    data['comments'] = list_temp
    print json.dumps(data)
    out_file = open( sys.argv[2] , "w")
    json.dump(data, out_file, sort_keys=True, indent=4)
    out_file.close()

if __name__ == "__main__":
    main()
