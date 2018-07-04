#!/usr/bin/env python2

#
# This file is part of the coreboot project.
#
# Copyright (c) 2015 MediaTek Inc.
# Author: Tristan Shieh <tristan.shieh@mediatek.com>
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


import struct
import sys
import hashlib

def read(path):
	with open(path, 'rb') as f:
		return f.read()

def write(path, data):
	with open(path, 'wb') as f:
		f.write(data)

def padding(data, size, pattern='\0'):
	return data + pattern * (size - len(data))

def align(data, size, pattern='\0'):
	return padding(data, (len(data) + (size - 1)) & ~(size - 1), pattern)

def gen_gfh_info(chip, data):
	entries = {
		'mt8173': 0x000C1000,
		'mt8183': 0x00201000
	}

	gfh_format = '<44I'
	gfh_size = struct.calcsize(gfh_format)
	load_addr = entries[chip] - gfh_size
	load_size = gfh_size + len(data) + hashlib.sha256().digest_size

	gfh = struct.pack(gfh_format,
		0x014d4d4d, 0x00000038, 0x454c4946, 0x464e495f,
		0x0000004f, 0x00000001, 0x01050001, load_addr,
		load_size,  0x00020000, 0x000000a8, 0x00000020,
		0x000000B0, 0x00000001, 0x014d4d4d, 0x0001000c,
		0x00000001, 0x034d4d4d, 0x00070064, 0x00001182,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00006400, 0x00001388,
		0x00000000, 0x00000000, 0x00000000, 0x00000000)

	return gfh

def gen_emmc_header(data):
	header = (padding(struct.pack('<12sII', 'EMMC_BOOT', 1, 512), 512, '\xff') +
		  padding(struct.pack('<8sIIIIIIII', 'BRLYT', 1, 2048, 2048 + len(data),
			  0x42424242, 0x00010005, 2048, 2048 + len(data), 1) + '\0' * 140, 512,
			  '\xff') +
		  '\0' * 1024)
	return header

def gen_sf_header(data):
	header = (padding(struct.pack('<12sII', 'SF_BOOT', 1, 512), 512, '\xff') +
		  padding(struct.pack('<8sIIIIIIII', 'BRLYT', 1, 2048, 2048 + len(data),
			  0x42424242, 0x00010007, 2048, 2048 + len(data), 1) + '\0' * 140, 512,
			  '\xff') +
		  '\0' * 1024)
	return header

gen_dev_header = {
	'emmc': gen_emmc_header,
	'sf': gen_sf_header
}

def gen_preloader(chip_ver, flash_type, data):
	gfh_info = gen_gfh_info(chip_ver, data)
	gfh_hash = hashlib.sha256(gfh_info + data).digest()

	data = align(gfh_info + data + gfh_hash, 512, '\xff')
	header = gen_dev_header[flash_type](data)
	return header + data

def main(argv):
	if len(argv) != 5:
		print 'Usage: %s <chip> <flash_type> <input_file> <output_file>' % argv[0]
		print '\t flash_type: emmc|sf'
		print '\t chip      : mt8173|mt8183'

		exit(1)
	write(argv[4], gen_preloader(argv[1], argv[2], read(argv[3])))

if __name__ == '__main__':
	main(sys.argv)
