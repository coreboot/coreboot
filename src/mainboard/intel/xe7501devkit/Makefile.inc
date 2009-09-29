ROMCCFLAGS := -mcpu=p4
obj-$(CONFIG_HAVE_ACPI_TABLES) += acpi_tables.o
ifeq ($(CONFIG_PCI_ROM_RUN),y)
	ifeq ($(CONFIG_CONSOLE_VGA),y)
		obj-y += vgarom.o
	else
		obj-y += no_vgarom.o
	endif
else
	obj-y += no_vgarom.o
endif
include $(src)/mainboard/Makefile.romccboard.inc

