# Makefile to build lxdialog package
#

CONFIG_SHELL := sh

check-lxdialog  := ./check-lxdialog.sh

HOSTCC ?= gcc

# Use recursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
HOST_EXTRACFLAGS = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ccflags)
HOST_LOADLIBES   = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ldflags $(HOSTCC))

HOST_EXTRACFLAGS += -DLOCALE

PHONY += dochecklxdialog
dochecklxdialog:
	$(Q)$(CONFIG_SHELL) $(check-lxdialog) -check $(HOSTCC) $(HOST_LOADLIBES)

hostprogs-y	:= lxdialog
always		:= $(hostprogs-y) dochecklxdialog

lxdialog-objs := checklist.o menubox.o textbox.o yesno.o inputbox.o \
		 util.o lxdialog.o msgbox.o

lxdialog: dochecklxdialog $(lxdialog-objs)
	$(CC) $(HOST_LOADLIBES) -o lxdialog $(lxdialog-objs)

%.o: %.c
	$(Q)$(HOSTCC) $(HOST_EXTRACFLAGS) $^ -c -o $@

