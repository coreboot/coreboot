CPUFLAGS=-DL440GX -Di686 -Di586 -DINTEL_BRIDGE_CONFIG -DPIIX4E_NVRAM
CPUFLAGS += -D__KERNEL__
CPUFLAGS += -DINTEL_PPRO_MTRR -DPIIX4E_KEYBOARD
CPUFLAGS += -DSMP
#CPUFLAGS += -DHAVE_FRAMEBUFFER
CPUFLAGS += -DNEWPCI
CPUFLAGS += -DZKERNEL_START=0xfff40000
CPUFLAGS += -DZKERNEL_MASK=0x3ed 
CPUFLAGS += -DSERIAL_CONSOLE
CPUFLAGS += -DCMD_LINE='"ro root=/dev/hda1 console=ttyS0,115200 debug 3 single"'
CPUFLAGS += -DPIIX4_DEVFN=0x90
#CPUFLAGS += -DUPDATE_MICROCODE

phlash_floppy: phlash
	mcopy -o /tmp/$(PHLASH_BASE_NAME).bi? a:

crt0.s: crt0.S ../../chip/intel/intel_start32.S ../../chip/intel/intel_440ram-2.S 

include ../Makefile.common

# here begins stupid stuff for the phlash program. It's ugly. 

#PHLASH_BASE_NAME=p11-0105
PHLASH_BASE_NAME=p11-0102
#PHLASH_BASE_NAME=p12-0115
#PHLASH_BASE_NAME=p13-0125

phlash: vmlinux.bin.gz linuxbios.rom headers 
	rm -f xa?
	split -b 64k vmlinux.bin.gz
#       Now just touch them if we have a really 
# 	small kernel!
	touch xaa xab xac xad xae xaf xag xah
# this if starting at bank 4, and proceeding on. Unused banks are dups
# intel nvram is odd all of the banks are byte swapped
	cat $(PHLASH_BASE_NAME).bi1.header xaa > $(PHLASH_BASE_NAME).bi1
	cat $(PHLASH_BASE_NAME).bi3.header xab > $(PHLASH_BASE_NAME).bi3
	cat $(PHLASH_BASE_NAME).bi2.header xac > $(PHLASH_BASE_NAME).bi2
	cat $(PHLASH_BASE_NAME).bi4.header xad > $(PHLASH_BASE_NAME).bi4
	cat $(PHLASH_BASE_NAME).bi7.header xae > $(PHLASH_BASE_NAME).bi7
	cat $(PHLASH_BASE_NAME).bi6.header xaf > $(PHLASH_BASE_NAME).bi6
	cat $(PHLASH_BASE_NAME).bi9.header xag > $(PHLASH_BASE_NAME).bi9
	cat $(PHLASH_BASE_NAME).bi8.header xah > $(PHLASH_BASE_NAME).bi8
	cat $(PHLASH_BASE_NAME).bia.header linuxbios.rom > $(PHLASH_BASE_NAME).bia
	# Part o & 5 seem not to be written reliably for some reason... 
	cat $(PHLASH_BASE_NAME).bio.header /dev/null > $(PHLASH_BASE_NAME).bio
	cat $(PHLASH_BASE_NAME).bi5.header /dev/null > $(PHLASH_BASE_NAME).bi5
	sh -x BUILD_PHLASH_FILES $(PHLASH_BASE_NAME)

headers: \
	$(PHLASH_BASE_NAME).bi1.header	\
	$(PHLASH_BASE_NAME).bi2.header	\
	$(PHLASH_BASE_NAME).bi3.header	\
	$(PHLASH_BASE_NAME).bi4.header	\
	$(PHLASH_BASE_NAME).bi5.header	\
	$(PHLASH_BASE_NAME).bi6.header	\
	$(PHLASH_BASE_NAME).bi7.header	\
	$(PHLASH_BASE_NAME).bi8.header	\
	$(PHLASH_BASE_NAME).bi9.header	\
	$(PHLASH_BASE_NAME).bia.header	\
	$(PHLASH_BASE_NAME).bio.header

# This builds the headers from the intel flash disk. 
# we are not distributing this disk; you need to get it.
BUILDHEADER=dd if=$< of=$@ bs=1 count=160

$(PHLASH_BASE_NAME).bi1.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bi1
	$(BUILDHEADER)
$(PHLASH_BASE_NAME).bi2.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bi2
	$(BUILDHEADER)
$(PHLASH_BASE_NAME).bi3.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bi3
	$(BUILDHEADER)
$(PHLASH_BASE_NAME).bi4.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bi4
	$(BUILDHEADER)
$(PHLASH_BASE_NAME).bi5.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bi5
	$(BUILDHEADER)
$(PHLASH_BASE_NAME).bi6.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bi6
	$(BUILDHEADER)
$(PHLASH_BASE_NAME).bi7.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bi7
	$(BUILDHEADER)
$(PHLASH_BASE_NAME).bi8.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bi8
	$(BUILDHEADER)
$(PHLASH_BASE_NAME).bi9.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bi9
	$(BUILDHEADER)
$(PHLASH_BASE_NAME).bia.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bia
	$(BUILDHEADER)
$(PHLASH_BASE_NAME).bio.header: $(TOP)/../intel_flash_disk/$(PHLASH_BASE_NAME).bio
	$(BUILDHEADER)



clean::
	rm -f $(PHLASH_BASE_NAME)* 

