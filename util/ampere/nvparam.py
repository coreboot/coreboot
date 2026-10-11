#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# @file
#
# Copyright (c) 2020, Ampere Computing LLC.
#
# SPDX-License-Identifier: ISC
#
# Altra NV-parameter generator
#
import argparse
import ast
import operator
import struct
import sys

# Evaluate the integer arithmetic of the offsets (e.g. "(17 * 8)") without eval()
def eval_offset(expr):
	ops = {
		ast.Add: operator.add,
		ast.Sub: operator.sub,
		ast.Mult: operator.mul,
	}

	def _eval(node):
		if isinstance(node, ast.Expression):
			return _eval(node.body)
		if isinstance(node, ast.Constant) and isinstance(node.value, int):
			return node.value
		if isinstance(node, ast.BinOp) and type(node.op) in ops:
			return ops[type(node.op)](_eval(node.left), _eval(node.right))
		raise ValueError("Invalid offset: " + expr)

	return _eval(ast.parse(expr.strip(), mode="eval"))

def nvp_gen_board_template(filename, template_filename):
	print("Generate NV-parameter template...")

	nvparam = []

	finput = open(filename)
	while True:
		line = finput.readline()
		if not line:
			break;
		line = line.lstrip()
		line = line.rstrip()
		subindex = line.find('NV_SI_RO_')
		if (subindex != 0):
			continue
		fields = line.split('=')
		pname = fields[0].strip()
		op = fields[1].split('+')
		offset = eval_offset(op[0])
		defaults = op[1].split('Default:')
		value = 0
		if len(defaults) > 1:
			value_str = defaults[1].strip()
			value_str = value_str.split(' ')
			if value_str[0].startswith("0x"):
				value = int(value_str[0], 16)
			else:
				value = int(value_str[0])

		item = []
		item.append(pname)
		item.append(offset)
		item.append(value)
		nvparam.append(item)

	finput.close()

	#
	# Validate the data to be contiguous
	next_value = 0
	for row in nvparam:
		if row[1] != next_value:
			print("Invalid row parameter " + row[0] + "\n")
			return -1
		next_value += 8

	foutput = open(template_filename, "w")
	foutput.write("# Sample board setting\n")
	foutput.write("# \n")
	foutput.write("# This is a sample board setting as used for the \n")
	foutput.write("# Ampere Altra reference design.\n")
	foutput.write("# \n")
	foutput.write("# Name, offset (hex), value\n")
	foutput.write("# value can be hex or decimal\n")
	foutput.write("#\n\n")
	for item in nvparam:
		offset = '0x{:04X}'.format(item[1])
		value = '0x{:08X}'.format(item[2])
		foutput.write(item[0] + ", " + offset + ", " + value + "\n")
	foutput.close()
	print("See file " + template_filename + "\n")

def compute_crc16(ptr, count):
	crc = 0

	for i in range(0, count):
		crc ^= ptr[i] << 8
		for j in range(0, 8):
			if (crc & 0x8000) != 0:
				crc = (crc << 1) ^ 0x1021
			else:
				crc = crc << 1
	return  crc & 0xffff;

def nvparam_write(foutput, item):
	data = bytearray(8)

	#
	# Write the value as binary 4 bytes
	byte1 = (item[2] >> 24) & 0xFF
	byte2 = (item[2] >> 16) & 0xFF
	byte3 = (item[2] >> 8) & 0xFF
	byte4 = item[2] & 0xFF

	struct.pack_into("BBBB", data, 0, byte4, byte3, byte2, byte1)


	# Write the permission as 0xFF for read-only and valid
	data[4] = 0xFF
	data[5] = 0x80

	# Write the crc16
	data[6] = 0x00
	data[7] = 0x00
	#
	# Compute the crc16
	crc16 = compute_crc16(data, 8)
	data[6] = crc16 & 0xff
	data[7] = crc16 >> 8
	#for val in data:
	#	print ''.join('{:02X}'.format(val)),
	#print ""
	foutput.write(data)

def nvp_gen_board_bin(filename, outfilename, padding):
	print("Generate NV-parameter image...")

	nvparam = []

	finput = open(filename)
	while True:
		line = finput.readline()
		if not line:
			break;
		line = line.lstrip()
		line = line.rstrip()
		subindex = line.find('NV_SI_RO_')
		if (subindex != 0):
			continue
		fields = line.split(',')
		pname = fields[0].strip()
		data = fields[1].strip()
		if data.startswith("0x"):
			offset = int(data, 16)
		else:
			offset = int(data)
		data = fields[2].strip()
		if data.startswith("0x"):
			value = int(data, 16)
		else:
			value = int(data)
		item = []
		item.append(pname)
		item.append(offset)
		item.append(value)
		nvparam.append(item)

	finput.close()

	# Check for non-sequential setting
	next_value = 0
	for row in nvparam:
		if row[1] != next_value:
			print("Invalid row parameter " + row[0] + "\n")
			return -1
		next_value += 8

	# Generate the binary image
	file_size = 0
	bin_filename = outfilename
	foutput = open(bin_filename, "wb")
	for item in nvparam:
		nvparam_write(foutput, item)
		file_size += 8
	#
	# Pad to 64KB
	if padding:
		while True:
			if file_size >= (64*1024):
				break
			data = bytearray([0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF])
			foutput.write(data)
			file_size += 8

	foutput.close()
	print(bin_filename)

print("Ampere Altra NV-parameter generator v0.1\n")

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--template", action="store_true", help="Generate template")
parser.add_argument("-f", "--filename", help="Input file name")
parser.add_argument("-o", "--output", help="Output file name")
parser.add_argument("-p", "--padded", action="store_true",
		    help="Also generate <output>.padded, padded to 64KB")
args = parser.parse_args()

if (args.filename is None):
	print("Please provide input file name\n")
	sys.exit(1)
else:
	filename = args.filename

if (args.output is None):
	outfilename = ""
else:
	outfilename = args.output

if args.template:
	if len(outfilename) <= 0:
		outfilename = "nvparam_template.txt"
	if nvp_gen_board_template(filename, outfilename) == -1:
		sys.exit(1)
	sys.exit(0)

if len(outfilename) <= 0:
	outfilename = filename + ".bin"

if nvp_gen_board_bin(filename, outfilename, 0) == -1:
	sys.exit(1)
if args.padded and nvp_gen_board_bin(filename, outfilename + ".padded", 1) == -1:
	sys.exit(1)
