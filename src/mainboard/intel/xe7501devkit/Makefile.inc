ROMCCFLAGS := -mcpu=p4 -O2
obj-$(CONFIG_GENERATE_ACPI_TABLES) += acpi_tables.o
obj-$(CONFIG_HAVE_HARD_RESET) += reset.o
include $(src)/mainboard/Makefile.romccboard.inc

