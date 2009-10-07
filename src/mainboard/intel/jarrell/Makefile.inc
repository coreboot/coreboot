obj-y += reset.o
ROMCCFLAGS := -mcpu=p4 -O2
include $(src)/mainboard/Makefile.romccboard.inc

