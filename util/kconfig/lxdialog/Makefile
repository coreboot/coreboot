# Makefile to build lxdialog package
#

CONFIG_SHELL:=sh

check-lxdialog  := $(src)/util/kconfig/lxdialog/check-lxdialog.sh

# Use reursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
HOST_EXTRACFLAGS = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ccflags)
HOST_LOADLIBES   = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ldflags $(HOSTCC))
 
HOST_EXTRACFLAGS += -DLOCALE 

.PHONY: dochecklxdialog
$(obj)/dochecklxdialog:
	$(Q)$(CONFIG_SHELL) $(check-lxdialog) -check $(HOSTCC) $(HOST_LOADLIBES)

hostprogs-y	:= lxdialog
always		:= $(hostprogs-y) dochecklxdialog

lxdialog-objs := checklist.o menubox.o textbox.o yesno.o inputbox.o \
		 util.o lxdialog.o msgbox.o


$(obj)/lxdialog: $(obj)/dochecklxdialog $(patsubst %,$(obj)/%,$(lxdialog-objs))
	$(HOSTCC) $(HOST_LOADLIBES) $(patsubst %,$(obj)/%,$(lxdialog-objs)) -o $@

$(obj)/checklist.o: $(src)/util/kconfig/lxdialog/checklist.c
	$(HOSTCC) $(HOST_EXTRACFLAGS) $^ -c -o $@

$(obj)/menubox.o: $(src)/util/kconfig/lxdialog/menubox.c
	$(HOSTCC) $(HOST_EXTRACFLAGS) $^ -c -o $@

$(obj)/textbox.o: $(src)/util/kconfig/lxdialog/textbox.c
	$(HOSTCC) $(HOST_EXTRACFLAGS) $^ -c -o $@

$(obj)/yesno.o: $(src)/util/kconfig/lxdialog/yesno.c
	$(HOSTCC) $(HOST_EXTRACFLAGS) $^ -c -o $@

$(obj)/inputbox.o: $(src)/util/kconfig/lxdialog/inputbox.c
	$(HOSTCC) $(HOST_EXTRACFLAGS) $^ -c -o $@

$(obj)/util.o: $(src)/util/kconfig/lxdialog/util.c
	$(HOSTCC) $(HOST_EXTRACFLAGS) $^ -c -o $@

$(obj)/lxdialog.o: $(src)/util/kconfig/lxdialog/lxdialog.c
	$(HOSTCC) $(HOST_EXTRACFLAGS) $^ -c -o $@

$(obj)/msgbox.o: $(src)/util/kconfig/lxdialog/msgbox.c
	$(HOSTCC) $(HOST_EXTRACFLAGS) $^ -c -o $@



