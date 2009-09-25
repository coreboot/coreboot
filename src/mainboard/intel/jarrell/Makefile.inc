obj-y += reset.o
ROMCCFLAGS := -mcpu=p4
include $(src)/mainboard/Makefile.romccboard.inc

