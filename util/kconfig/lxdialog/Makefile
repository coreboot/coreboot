# Makefile to build lxdialog package
#

CONFIG_SHELL:=sh

check-lxdialog  := $(src)/util/kconfig/lxdialog/check-lxdialog.sh

# Use recursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
HOST_EXTRACFLAGS = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ccflags)
HOST_LOADLIBS   = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ldflags $(HOSTCC))
 
HOST_EXTRACFLAGS += -DLOCALE 

.PHONY: dochecklxdialog
$(obj)/dochecklxdialog:
	$(Q)printf "  CHECK   $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CONFIG_SHELL) $(check-lxdialog) -check $(HOSTCC) $(HOST_LOADLIBS)

always		:= lxdialog dochecklxdialog

lxdialog-objs := checklist.o menubox.o textbox.o yesno.o inputbox.o \
		 util.o lxdialog.o msgbox.o

$(obj)/util/kconfig/lxdialog/lxdialog: $(obj)/dochecklxdialog $(patsubst %,$(obj)/util/kconfig/lxdialog/%,$(lxdialog-objs))
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) \
		$(patsubst %,$(obj)/util/kconfig/lxdialog/%,$(lxdialog-objs)) -o $@ $(HOST_LOADLIBS)

$(obj)/util/kconfig/lxdialog/%.o: $(src)/util/kconfig/lxdialog/%.c
	$(Q)mkdir -p $(obj)/util/kconfig/lxdialog/
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOST_EXTRACFLAGS) $^ -c -o $@


