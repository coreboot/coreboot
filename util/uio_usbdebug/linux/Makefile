
obj-m := uio_ehci_pci.o

all: uio_ehci_pci.c
	@$(MAKE) -C/lib/modules/`uname -r`/build M=$(CURDIR) modules

install:
	@$(MAKE) -C/lib/modules/`uname -r`/build M=$(CURDIR) modules_install

clean:
	-@$(MAKE) -C/lib/modules/`uname -r`/build M=$(CURDIR) clean

.PHONY: all install clean
