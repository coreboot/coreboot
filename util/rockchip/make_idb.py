#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause

import struct
import sys
from io import SEEK_SET, SEEK_END

class IDBTool:
  def __init__(self):
    print("Initialize IDBTool")

  def p_rc4(self, buf, length):
    key = (124,78,3,4,85,5,9,7,45,44,123,56,23,13,23,17)
    K = key * 16
    S = [i for i in range(256)]

    j = 0
    for i in range(256):
      j = (j + S[i] + K[i]) % 256
      temp = S[i]; S[i] = S[j]; S[j] = temp;

    i = j = k = 0
    for x in range(length):
      i = (i+1) % 256
      j = (j + S[i]) % 256
      temp = S[i]; S[i] = S[j]; S[j] = temp
      k = (S[i] + S[j]) % 256
      buf[x] = struct.pack('B', buf[x] ^ S[k])[0]

  def makeIDB(self, chip, from_file, to_file, rc4_flag = False, align_flag = False):
    try:
      fin = open(from_file, 'rb')
    except:
      sys.exit("Failed to open file : " + from_file)

    try:
      fin.seek(0, SEEK_END)
      if (fin.tell() > 4 * 1024 * 1024):
        sys.exit("Input file is more than 4MB")
      fin.seek(0)
      data = fin.read()
    finally:
      fin.close()

    data_len = len(data)
    SECTOR_SIZE = 512
    PAGE_ALIGN  = 4
    sectors = (data_len + 4 - 1) // SECTOR_SIZE + 1
    pages = (sectors - 1) // PAGE_ALIGN + 1
    sectors = pages * PAGE_ALIGN

    buf = bytearray(sectors * SECTOR_SIZE)
    assert len(chip) == 4
    buf[:4] = chip.encode('ascii')
    buf[4 : 4+data_len] = data

    idblock = bytearray(4 * SECTOR_SIZE)
    blank   = bytearray(4 * SECTOR_SIZE)
    idblock[:4] = b'\x55\xAA\xF0\x0F'

    if (not rc4_flag):
      idblock[8:12] = struct.pack("<I", 1)
    else:
      for i in range(sectors):
        list_tmp = buf[SECTOR_SIZE*i : SECTOR_SIZE*(i+1)]
        self.p_rc4(list_tmp, SECTOR_SIZE)
        buf[SECTOR_SIZE*i : SECTOR_SIZE*(i+1)] = list_tmp

    idblock[12:16] = struct.pack("<HH", 4, 4)
    idblock[506:510] = struct.pack("<HH", sectors, sectors)
    self.p_rc4(idblock, SECTOR_SIZE)

    try:
      fout = open(to_file, "wb+")
    except:
      sys.exit("Failed to open output file : " + to_file)

    try:
      if (align_flag):
        fout.write(idblock)
        fout.write(blank)

        for s in range(0, sectors * SECTOR_SIZE, PAGE_ALIGN * SECTOR_SIZE):
          fout.write(buf[s : s + PAGE_ALIGN * SECTOR_SIZE])
          fout.write(blank)
      else:
        fout.write(idblock)
        fout.write(buf)
      fout.flush()
    except:
      sys.exit("Failed to write data to : " + to_file)
    finally:
      fout.close()
    print("DONE")

def usage():
  print("Usage: make_idb.py [--chip=RKXX] [--enable-rc4] [--enable-align] [--to=out] --from=in")
  print("       --chip: default is RK32")

if __name__ == '__main__':
  rc4_flag = align_flag = False
  to_file = "IDBlock.bin"
  chip = "RK32"

  for para in sys.argv[1:]:
    if (para == "--enable-rc4"):
      rc4_flag = True
    elif (para == "--enable-align"):
      align_flag = True
    elif (para.startswith("--to=")):
      to_file = para.split('=')[1]
    elif (para.startswith("--from=")):
      from_file = para.split('=')[1]
    elif (para.startswith("--chip=")):
      chip = para.split('=')[1]
    elif (para == "--help" or para == "-h"):
      usage()
      sys.exit()
    else:
      usage()
      sys.exit()
  if ('from_file' not in vars() or to_file == ''):
    usage()
    sys.exit()

  idbtool = IDBTool()
  idbtool.makeIDB(chip, from_file, to_file, rc4_flag, align_flag)
