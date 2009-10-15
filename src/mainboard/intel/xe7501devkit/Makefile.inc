ROMCCFLAGS := -mcpu=p4 -O2
obj-$(CONFIG_GENERATE_ACPI_TABLES) += acpi_tables.o
include $(src)/mainboard/Makefile.romccboard.inc

