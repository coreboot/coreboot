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
CPUFLAGS += -DUPDATE_MICROCODE
CPUFLAGS += -DCONFIGURE_L2_CACHE

LINUX=$(TOP)/../linux-2.4.0-test6.l440gx

TOP=../..
INCLUDES=-nostdinc -I $(TOP)/src/include 
CFLAGS=$(INCLUDES) -O2 $(CPUFLAGS) -Ilinux/include -Wall

OBJECTS=crt0.o hardwaremain.o linuxbiosmain.o 
OBJECTS += mainboard.o mtrr.o subr.o fill_inbuf.o params.o
OBJECTS += southbridge.o northbridge.o 
#OBJECTS += pci.o
OBJECTS += printk.o vsprintf.o
OBJECTS += newpci.o linuxpci.o
OBJECTS += cpuid.o 
OBJECTS += serial_subr.o
OBJECTS += mpspec.o
OBJECTS += microcode.o
OBJECTS += keyboard.o
OBJECTS += l2_cache.o

LINK = ld -T $(TOP)/src/mainboard/intel/l440gx/ldscript.ld -o $@ $(OBJECTS)
CC=cc $(CFLAGS)
CCASM=cc -I$(TOP)/chip/intel $(CFLAGS)

all: romimage
floppy: all 
	mcopy -o romimage a:
# here's the problem: we shouldn't assume we come up with more than 
# 64K of FLASH up. SO we need a working linuxbios at the tail, and it will
# enable all flash and then gunzip the linuxbios. As a result, 
# we need the vmlinux.bin.gz padded out and then cat the linuxbios.rom
# at then end. We always copy it to /tmp so that a waiting root shell 
# can put it on the floppy (see ROOTDOIT)
romimage: linuxbios.rom vmlinux.bin.gz.block 
	cat vmlinux.bin.gz.block linuxbios.rom > romimage
	cp romimage /tmp

linuxbios.rom: linuxbios.strip mkrom 
	./mkrom -s 64 -f -o linuxbios.rom linuxbios.strip

linuxbios.strip: linuxbios
	objcopy -O binary -R .note -R .comment -S linuxbios linuxbios.strip

linuxbios: $(OBJECTS) vmlinux.bin.gz
	@rm -f biosobject
	$(LINK) 
	nm -n linuxbios > linuxbios.map

# crt0 actually includes .inc files. 
# For self-documenting purposes, we put the FULL PATH of the 
# .inc files (relative to $TOP/src) in crt0.S. 
# So, for example, earlymtrr.inc is included as cpu/p6/earlymtrr.inc
# To make this work, add the extra -I $(TOP)/src here. 
crt0.s: $(TOP)/src/mainboard/intel/l440gx/crt0.S 
	$(CCASM) -I $(TOP)/src -E $< > crt0.s

crt0.o : crt0.s
	$(CCASM) -c crt0.s

mkrom: $(TOP)/mkrom/mkrom.c
	cc -o mkrom $<

linuxbiosmain.o: $(TOP)/src/lib/linuxbiosmain.c
	cc $(CFLAGS) -c $<

mainboard.o: $(TOP)/src/mainboard/intel/l440gx/mainboard.c
	cc $(CFLAGS) -c $<

fill_inbuf.o: $(TOP)/src/lib/fill_inbuf.c
	cc $(CFLAGS) -c $<

params.o: $(TOP)/src/lib/params.c
	cc $(CFLAGS) $(LINUXINCLUDE) -c $<

hardwaremain.o: $(TOP)/src/lib/hardwaremain.c
	cc $(CFLAGS) -c $<

southbridge.o: $(TOP)/src/southbridge/intel/piix4e/southbridge.c
	cc $(CFLAGS) -c $<

northbridge.o: $(TOP)/src/northbridge/intel/440gx/northbridge.c
	cc $(CFLAGS) -c $<

#superio.o: $(TOP)/src/superio/intel/950/superio.c
#	cc $(CFLAGS) -c $<

pci.o:  $(TOP)/src/lib/pci.c
	cc $(CFLAGS) -c $<


mtrr.o: $(TOP)/src/cpu/p6/mtrr.c
	cc $(CFLAGS) -c $<

subr.o: $(TOP)/src/lib/subr.c
	cc $(CFLAGS) -c $<

keyboard.o: $(TOP)/src/pc80/keyboard.c
	cc $(CFLAGS) -c $<

cpuid.o: $(TOP)/src/cpu/p5/cpuid.c
	cc $(CFLAGS) -c $<

mpspec.o: $(TOP)/src/cpu/p6/mpspec.c
	$(CC) $(CFLAGS) -c $<

microcode.o: $(TOP)/src/cpu/p6/microcode.c
	$(CC) $(CFLAGS) -c $<

l2_cache.o: $(TOP)/src/cpu/p6/l2_cache.c
	$(CC) $(CFLAGS) -c $<

serial_subr.o: $(TOP)/src/lib/serial_subr.c
	cc $(CFLAGS) -c $<

printk.o: $(TOP)/src/lib/printk.c
	cc $(CFLAGS) -c $<

vsprintf.o: $(TOP)/src/lib/vsprintf.c
	cc $(CFLAGS) -c $<

newpci.o: $(TOP)/src/lib/newpci.c
	cc $(CFLAGS) -c $<

linuxpci.o: $(TOP)/src/lib/linuxpci.c
	cc $(CFLAGS) -c $<

vmlinux.bin.gz.block: vmlinux.bin.gz
	dd conv=sync bs=640k if=vmlinux.bin.gz of=vmlinux.bin.gz.block
vmlinux.bin.gz: vmlinux.bin
	gzip -f -3 vmlinux.bin 

vmlinux.bin: $(LINUX)/vmlinux
	objcopy -O binary -R .note -R .comment -S $< vmlinux.bin

alltags: 
	gctags ../inflate/*.c ../../lib/*.c ../../chip/intel/*.c
	etags ../inflate/*.c ../../lib/*.c ../../chip/intel/*.c


clean:: 
	rm -f linuxbios.* vmlinux.* *.o mkrom xa? *~ linuxbios romimage crt0.s
	rm -f a.out *.s *.l
	rm -f TAGS tags
	rm -f docipl






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
	sh -x $(TOP)/src/mainboard/intel/l440gx/BUILD_PHLASH_FILES $(PHLASH_BASE_NAME)

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



