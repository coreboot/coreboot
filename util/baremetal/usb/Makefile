
CC = gcc
#INCLUDE = -I/usr/src/linux/include
ARCH = i386
CFLAGS = -I ../include -I ../include/$(ARCH) -O2 -DDEBUG -DZKERNEL_START=0xfff00000

#CFLAGS = -g -O2 $(INCLUDE)
KFLAGS = -g -O2 -D__KERNEL__ -DDEBUG -DKERNEL -DMODULE -I/usr/src/linux/include
LIBS = -lusb

DEPS = uhci.o usb_scsi_low.o debug.o scsi_cmds.o pci_util.o block_fill_inbuf.o ../lib/baremetal.a

LD = ld
KERN_LDFLAGS = -r --whole-archive

PREFIX = /usr
TARGETS = roothub kerntest test

all: usbboot.elf usb_stream.a

usbboot.elf: $(DEPS) main.o
	ld -defsym HEAPSIZE=0x8000 -T elfImage.lds main.o $(DEPS)  -o usbboot.elf

usb_stream.a: $(DEPS)
	ar -cr usb_stream.a $(DEPS)

test: test.c uhci.[ch] pci_util.o debug.[ch]
	$(CC) -g -DDEBUG -DUDEBUG -o test test.c uhci.c pci_util.o debug.c

kerntest: kernel_test.o

kernel_test.o: kernel_uhci.o pci_util.o kernel_debug.o kernel_pci_util.o kernel_usb_scsi_low.o kernel_scsi_cmds.o
	$(LD) $(KERN_LDFLAGS) -o kernel_test.o kernel_uhci.o kernel_pci_util.o kernel_debug.o kernel_usb_scsi_low.o kernel_scsi_cmds.o

kernel_uhci.o: uhci.c uhci.h
	$(CC) $(KFLAGS) -o kernel_uhci.o -c uhci.c

kernel_usb_scsi_low.o : usb_scsi_low.c
	$(CC) $(KFLAGS) -o kernel_usb_scsi_low.o -c usb_scsi_low.c

kernel_scsi_cmds.o : scsi_cmds.c
	$(CC) $(KFLAGS) -o kernel_scsi_cmds.o -c scsi_cmds.c

kernel_debug.o: debug.c debug.h
	$(CC) $(KFLAGS) -o kernel_debug.o -c debug.c

kernel_pci_util.o: pci_util.c pci_util.h
	$(CC) $(KFLAGS) -o kernel_pci_util.o -c pci_util.c

roothub: roothub.c pci_util.o
	$(CC) $(CFLAGS) -o roothub roothub.c pci_util.o

strip: $(TARGETS)
	strip $(TARGETS)

install: strip
	install -d $(PREFIX)/bin
	cp -a $(TARGETS) $(PREFIX)/bin

clean:
	rm -f $(TARGETS) *.o

distclean: clean
	rm -f roothub.tar.gz

tardist: distclean
	tar -C .. --exclude CVS -cvzf roothub.tar.gz thumb 

