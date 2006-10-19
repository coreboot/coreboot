#!/usr/bin/python
#
# Copyright (C) 2005 Florian Zeitz <florian-zeitz@lycos.de>
# Copyright (C) 2005 Stefan Reinauer <stepan@coresystems.de>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
# 

def xmlString(string):
	for i in range(len(string)-1):
		if string[i] == "&":
			string = string[:i] + "&amp;" + string[i+1:]
		if string[i] == "<":
			string = string[:i] + "&lt;" + string[i+1:]
		if string[i] == ">":
			string = string[:i] + "&gt;" + string[i+1:]
	return string

def openInfile(filename):
	"getting the input from the inputfile (e.g. Options.lb)"
	infile = open(filename, "r")
	infile.seek(0)
	input = infile.readlines()
	infile.close()
	return input

def prepInput(input):
	"preparing the input for parsing (not really neccessary, but makes things simpler and doesnt take too long)"
	i = -1
	while True:
		i += 1
		if i >= len(input): break
		if input[i] == ("" or "\n"):
			input.pop(i)
		if input[i][0:1] == "\t":
			input[i] = input[i][1:]
			i = -1
	return input

def parseInput(input):
	"parse the output"
	output = ""
	for line in input:
		line = xmlString(line)
		if line[:6] == "define":
			output = output + '<option name="' + line[7:-1] + '">' + "\n"
		elif line[:3] == "end":
			output = output + '</option>' + "\n\n"
		elif line[:7] == "default":
			output = output + '<default>' + line[8:-1] + '</default>' + "\n"
		elif line[:6] == "format":
			output = output + '<format>' + line[7:-1] + '</format>' + "\n"
		elif line[:6] == "export":
			output = output + '<export>' + line[7:-1] + '</export>' + "\n"
		elif line[:7] == "comment":
			output = output + '<comment>' + line[8:-1] + '</comment>' + "\n"
	
	return output

def parseArgv():
	"parse the given arguments"
	import sys

	In = Out = False

	if len(sys.argv) >= 2:
		if sys.argv[1] == ("-h" or "--help"):
			print "Syntax: mkOptionList.py [infile] [outfile]"
		else:
			In = True
			inFilename = sys.argv[1]
	if len(sys.argv) >= 3:
		if sys.argv[2] == ("-h" or "--help"):
			print "Syntax: mkOptionList.py [infile] [outfile]"
		else:
			Out = True
			outFilename = sys.argv[2]

	if In and not Out:
		return inFilename
	elif In and Out:
		return inFilename, outFilename
	

def main():
	import time
	if not parseArgv():
		inFilename = "../../src/config/Options.lb"
		outFilename = "Options.xml"
	else:
		inFilename, outFilename = parseArgv()
	
	input = openInfile(inFilename)
	input = prepInput(input)
	output = parseInput(input)
	
	print "mkOptionList.py: LinuxBIOS option list generator"
	print " input file : ", inFilename
	print " output file: ", outFilename
	
	#opening the output file
	outfile = open(outFilename, "w", 0)

	#write the beginning of the XML to the output file
	outfile.write('<?xml version="1.0"?>')
	outfile.write("\n")
	outfile.write('<?xml-stylesheet type="text/xsl" href="Options.xsl"?>')
	outfile.write("\n")
	outfile.write('<options>')
	outfile.write("\n")
	outfile.write('<creationdate>')
	outfile.write(time.strftime('%Y/%m/%d %H:%M:%S'))
	outfile.write('</creationdate>')
	outfile.write("\n")

	
	#write the parsed file to the output file
	outfile.write(output)
	
	#write closing tags to the output file and close it
	outfile.write('</options>')
	outfile.write("\n")
	outfile.flush()
	outfile.close()

	print "Done!"

if __name__ == "__main__":
	main()
