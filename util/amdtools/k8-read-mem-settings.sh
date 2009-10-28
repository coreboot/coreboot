#!/bin/bash

# This is an example that generates data files that are understood by the
# k8-interpret-extended-memory-settings.pl script. Adjust the pci ids for your
# board (0:18.2 and 0:19.2 are correct for supermicro h8dme)

# Ward Vandewege, 2009-09-04


for OFFSET in 00, 01, 02, 03, 04, 05, 06, 07, 20, 21, 22, 23, 24, 25, 26, 27, 10, 13, 16, 19, 30, 33, 36, 39; do

  setpci -s 0:18.2 98.l=$OFFSET
  echo 0:18.2 98.l: `setpci -s 0:18.2 98.l`
  echo 0:18.2 9C.l: `setpci -s 0:18.2 9C.l`

done

for OFFSET in 00, 01, 02, 03, 04, 05, 06, 07, 20, 21, 22, 23, 24, 25, 26, 27, 10, 13, 16, 19, 30, 33, 36, 39; do

  setpci -s 0:19.2 98.l=$OFFSET
  echo 0:19.2 98.l: `setpci -s 0:19.2 98.l`
  echo 0:19.2 9C.l: `setpci -s 0:19.2 9C.l`

done

