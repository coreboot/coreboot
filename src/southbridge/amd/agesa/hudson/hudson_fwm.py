import sys, os, re
import struct
from Queue import Queue

def main(start_addr, file_name, xhci_name, imc_name, gec_name):
	fwm_sig		= 0x55AA55AA # Hudson-2/3/4 firmware signature
	fwm_header_len	= 0x10       # 55AA55AA, imc_off, gec_off, xhci_off

	if not os.path.exists(xhci_name):
		print "XHCI firmware %s does not exist\n" % xhci_name
		sys.exit(1)
	if not os.path.exists(imc_name):
		print "IMC firmware %s does not exist\n" % imc_name
		sys.exit(1)

	f = open(file_name, "w")
	print "write to file " + file_name

	imc_offset	= 0x10000 # 64K Bytes offset, hardcoded
	imc_addr	= start_addr + imc_offset; #startaddr + 0x10000
	gec_offset	= 0 #TODO
	gec_addr	= 0 #TODO
	xhci_addr	= start_addr + fwm_header_len #ROMSIG take 0x10 bytes

	format="I" # one unsigned integer
	data=struct.pack(format, fwm_sig)
	f.write(data)
	data=struct.pack(format, imc_addr)
	f.write(data)
	data=struct.pack(format, gec_addr)
	f.write(data)
	data=struct.pack(format, xhci_addr)
	f.write(data)

	fwm_content = open(xhci_name).read()
	f.write(fwm_content)

	imc_content = open(imc_name).read()
	f.seek(0)
	f.seek(imc_offset)
	f.write(imc_content)
#	if os.path.exists(gec_name):
#		gec_conent = open(gec_name).read()
#		f.seek(0)
#		f.seek(gec_offset)
#		f.write(gec_content)

	f.close()
	print "done\n"


if __name__ == '__main__':
	if (len(sys.argv) < 6):
		print "\nUsage: %s <rom_addr> <rom_file> <xhci_rom> <imc_rom> <gec_rom>\n" % sys.argv[0]
		print "Example: %s 0xFFF20000 hudson.bin xhci.bin imc.bin gec.bin\n" % sys.argv[0]
		sys.exit(1)
	rom_addr = int(sys.argv[1], 16)
	rom_file = sys.argv[2]
	xhci_file = sys.argv[3]
	imc_file = sys.argv[4]
	gec_file = sys.argv[5]
	print "%x %s %s %s %s" % (rom_addr, rom_file, xhci_file, imc_file, gec_file)

	main(rom_addr, rom_file, xhci_file, imc_file, gec_file)
