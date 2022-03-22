#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only

"""
This utilty generate json output to post comment in gerrit.

INPUT: output of checkpatch.pl.
OUTPUT: json format output that can be used to post comment in gerrit
"""
import os
import sys
import json
import re

data = {}
data['comments'] = []
list_temp = {}

def update_struct( file_path, msg_output, line_number):
    if file_path not in list_temp:
        list_temp[file_path] = []
    list_temp[file_path].append({
        "robot_id" : "checkpatch",
        "robot_run_id" : sys.argv[3],
        "url" : sys.argv[4],
        "line" : line_number,
        "message" : msg_output,}
    )

def parse_file(input_file):
    fp = open (input_file, "r")
    for line in fp:
        if line.startswith("ERROR:"):
            msg_output = line.split("ERROR:")[1].strip()
        elif line.startswith("WARNING:"):
            msg_output = line.split("WARNING:")[1].strip()
        elif ": FILE:" in line:
            temp = line.split("FILE:")
            file_path = temp[1].split(":")[0]
            line_number = temp[1].split(":")[1]
            update_struct( file_path.strip(), msg_output, str(line_number)  )
        elif re.search("^\d+:\Z",line) != "None" and line.startswith("#"):
            file_path="/COMMIT_MSG"
            line = line.replace('#', '')
            line_number = int(line.split(":")[0]) + 2
            update_struct( file_path.strip(), msg_output, str(line_number)  )
        else:
            continue
    fp.close()

def main():
    if (len(sys.argv) < 5) or (sys.argv[1] == "-h"):
        print("HELP:")
        print(sys.argv[0] + " <input file> <output file in json> <job-id> <job-url>")
        sys.exit()

    print(sys.argv[1])
    parse_file(sys.argv[1])
    data['robot_comments'] = list_temp
    print(json.dumps(data))
    out_file = open( sys.argv[2] , "w")
    json.dump(data, out_file, sort_keys=True, indent=4)
    out_file.close()

if __name__ == "__main__":
    main()
