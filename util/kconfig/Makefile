
# ===========================================================================
# Shared Makefile for the various kconfig executables:
# conf:	  Used for defconfig, oldconfig and related targets
# mconf:  Used for the mconfig target.
#         Utilizes the lxdialog package
# qconf:  Used for the xconfig target
#         Based on QT which needs to be installed to compile it
# gconf:  Used for the gconfig target
#         Based on GTK which needs to be installed to compile it
# object files used by all kconfig flavours


# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile

CONFIG_SHELL := sh
check-lxdialog  := $(src)/util/kconfig/lxdialog/check-lxdialog.sh
CURSESLIBS := $(shell $(CONFIG_SHELL) $(check-lxdialog) -ldflags $(HOSTCC))

.PHONY: oldconfig xconfig gconfig menuconfig config silentoldconfig update-po-config

xconfig: $(obj)/util/kconfig/.tmp_qtcheck $(obj)/util/kconfig/qconf
	$(Q)$(obj)/util/kconfig/qconf Kconfig

gconfig: prepare $(obj)/util/kconfig/.tmp_gtkcheck $(obj)/util/kconfig/gconf
	$(Q)$(obj)/util/kconfig/gconf Kconfig

menuconfig: prepare $(obj)/util/kconfig/lxdialog/lxdialog $(obj)/util/kconfig/mconf
	$(Q)$(obj)/util/kconfig/mconf Kconfig

config: prepare $(obj)/util/kconfig/conf
	$(Q)$(obj)/util/kconfig/conf Kconfig

oldconfig: prepare $(obj)/util/kconfig/conf
	$(Q)$(obj)/util/kconfig/conf -o Kconfig

silentoldconfig: prepare $(obj)/util/kconfig/conf
	$(Q)$(obj)/util/kconfig/conf -s Kconfig

.PHONY: randconfig allyesconfig allnoconfig allmodconfig defconfig

randconfig: $(obj)/util/kconfig/conf
	$(Q)$< -r Kconfig

allyesconfig: $(obj)/util/kconfig/conf
	$(Q)$< -y Kconfig

allnoconfig: $(obj)/util/kconfig/conf
	$(Q)$< -n Kconfig

allmodconfig: $(obj)/util/kconfig/conf
	$(Q)$< -m Kconfig

UNAME_RELEASE := $(shell uname -r)

#CLONECONFIG := $(firstword $(wildcard /proc/config.gz \
#				      /lib/modules/$(UNAME_RELEASE)/.config \
#				      /etc/kernel-config \
#				      /boot/config-$(UNAME_RELEASE)))

cloneconfig: $(obj)/util/kconfig/conf
	$(Q)case "$(CLONECONFIG)" in				\
	'')	echo -e "The configuration of the running"	\
			"firmware could not be determined\n";	\
		false ;;					\
	*.gz)	gzip -cd $(CLONECONFIG) > .config.running ;;	\
	*)	cat $(CLONECONFIG) > .config.running ;;		\
	esac &&							\
	echo -e "Cloning configuration file $(CLONECONFIG)\n"
	$(Q)$< -D .config.running Kconfig

defconfig: $(obj)/util/kconfig/conf
ifeq ($(KBUILD_DEFCONFIG),)
	$< -d Kconfig
else
	@echo *** Default configuration is based on '$(KBUILD_DEFCONFIG)'
	$(Q)$< -D configs/$(KBUILD_DEFCONFIG) Kconfig
endif

%_defconfig: $(obj)/util/kconfig/conf
	$(Q)$< -D configs/$@ Kconfig

# Help text used by make help
help:
	@echo  '  config	  - Update current config utilising a line-oriented program'
	@echo  '  menuconfig	  - Update current config utilising a menu based program'
	@echo  '  xconfig	  - Update current config utilising a QT based front-end'
	@echo  '  gconfig	  - Update current config utilising a GTK based front-end'
	@echo  '  oldconfig	  - Update current config utilising a provided .config as base'
	@echo  '  randconfig	  - New config with random answer to all options'
	@echo  '  defconfig	  - New config with default answer to all options'
	@echo  '  allmodconfig	  - New config selecting modules when possible'
	@echo  '  allyesconfig	  - New config where all options are accepted with yes'
	@echo  '  allnoconfig	  - New minimal config'

mconf-objects := mconf.o zconf.tab.o
conf-objects  := conf.o zconf.tab.o
qconf-objects := qconf.o kconfig_load.o zconf.tab.o
gconf-objects := gconf.o kconfig_load.o zconf.tab.o

$(obj)/util/kconfig/mconf: $(patsubst %,$(obj)/util/kconfig/%,$(mconf-objects))
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(CURSESLIBS) $(INTLLIBS) -o $@ $^

$(obj)/util/kconfig/conf: $(patsubst %,$(obj)/util/kconfig/%,$(conf-objects))
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(CURSESLIBS) -o $@ $^

$(obj)/util/kconfig/qconf: $(patsubst %,$(obj)/util/kconfig/%,$(qconf-objects))
	$(Q)printf "  HOSTCXX $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCXX) $(KC_QT_LIBS) -ldl -o $@ $^

$(obj)/util/kconfig/gconf: $(patsubst %,$(obj)/util/kconfig/%,$(gconf-objects))
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) `pkg-config --libs gtk+-2.0 gmodule-2.0 libglade-2.0` -o $@ $^



$(obj)/util/kconfig/mconf.o: $(src)/util/kconfig/mconf.c
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(INTLCFLAGS) -I$(src)/util/kconfig -c -o $@ $<

$(obj)/util/kconfig/conf.o: $(src)/util/kconfig/conf.c
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -I$(src)/util/kconfig -c -o $@ $<

$(obj)/util/kconfig/kconfig_load.o: $(src)/util/kconfig/kconfig_load.c
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -I$(obj)/util/kconfig -I$(src)/util/kconfig -c -o $@ $<

$(obj)/util/kconfig/qconf.o: $(src)/util/kconfig/qconf.cc $(obj)/util/kconfig/qconf.moc $(obj)/util/kconfig/lkc_defs.h
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)printf "  HOSTCXX $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCXX) $(KC_QT_CFLAGS) -D LKC_DIRECT_LINK \
		-I$(src)/util/kconfig -I$(obj)/util/kconfig -c -o $@ $<

$(obj)/util/kconfig/gconf.o: $(src)/util/kconfig/gconf.c $(obj)/util/kconfig/lkc_defs.h
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) `pkg-config --cflags gtk+-2.0 gmodule-2.0 libglade-2.0` \
		-D LKC_DIRECT_LINK -I$(obj) -I$(src)/util/kconfig -c -o $@ $<

$(obj)/util/kconfig/%.moc: $(src)/util/kconfig/%.h
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)printf "  MOC     $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(KC_QT_MOC) -i $< -o $@

$(obj)/util/kconfig/lkc_defs.h: $(src)/util/kconfig/lkc_proto.h
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)printf "  GEN     $(subst $(shell pwd)/,,$(@))\n"
	$(Q)sed < $< > $@ 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

$(obj)/util/kconfig/zconf.tab.o: $(obj)/util/kconfig/zconf.tab.c $(obj)/util/kconfig/zconf.hash.c $(obj)/util/kconfig/lex.zconf.c
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(INTLCFLAGS) -I$(src)/util/kconfig  -c -o $@ $(obj)/util/kconfig/zconf.tab.c

$(obj)/util/kconfig/lex.zconf.c: $(src)/util/kconfig/lex.zconf.c_shipped
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)cp $< $@

$(obj)/util/kconfig/zconf.hash.c: $(src)/util/kconfig/zconf.hash.c_shipped
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)cp $< $@

$(obj)/util/kconfig/zconf.tab.c: $(src)/util/kconfig/zconf.tab.c_shipped
	$(Q)mkdir -p $(obj)/util/kconfig
	$(Q)cp $< $@

include $(src)/util/kconfig/lxdialog/Makefile

# #####################################################################

ifeq ($(MAKECMDGOALS),xconfig)
	qconf-target := 1
endif
ifeq ($(MAKECMDGOALS),gconfig)
	gconf-target := 1
endif

ifdef KCONFIG_DEBUG

# Needed for systems without gettext
KBUILD_HAVE_NLS := $(shell \
     if echo "\#include <libintl.h>" | $(HOSTCC) $(HOSTCFLAGS) -E - > /dev/null 2>&1 ; \
     then echo yes ; \
     else echo no ; fi)
ifeq ($(KBUILD_HAVE_NLS),no)
HOSTCFLAGS	+= -DKBUILD_NO_NLS
endif

endif

# #####################################################################


ifeq ($(qconf-target),1)
$(obj)/util/kconfig/.tmp_qtcheck: $(src)/util/kconfig/Makefile

-include $(obj)/util/kconfig/.tmp_qtcheck

# QT needs some extra effort...
$(obj)/util/kconfig/.tmp_qtcheck:
	@set -e; echo "  CHECK   qt"; dir=""; pkg=""; \
	pkg-config --exists qt 2> /dev/null && pkg=qt; \
	pkg-config --exists qt-mt 2> /dev/null && pkg=qt-mt; \
	if [ -n "$$pkg" ]; then \
	  cflags="\$$(shell pkg-config $$pkg --cflags)"; \
	  libs="\$$(shell pkg-config $$pkg --libs)"; \
	  moc="\$$(shell pkg-config $$pkg --variable=prefix)/bin/moc"; \
	  dir="$$(pkg-config $$pkg --variable=prefix)"; \
	else \
	  for d in $$QTDIR /usr/share/qt* /usr/lib/qt*; do \
	    if [ -f $$d/include/qconfig.h ]; then dir=$$d; break; fi; \
	  done; \
	  if [ -z "$$dir" ]; then \
	    echo "*"; \
	    echo "* Unable to find the QT installation. Please make sure that"; \
	    echo "* the QT development package is correctly installed and"; \
	    echo "* either install pkg-config or set the QTDIR environment"; \
	    echo "* variable to the correct location."; \
	    echo "*"; \
	    false; \
	  fi; \
	  libpath=$$dir/lib; lib=qt; osdir=""; \
	  $(HOSTCXX) -print-multi-os-directory > /dev/null 2>&1 && \
	    osdir=x$$($(HOSTCXX) -print-multi-os-directory); \
	  test -d $$libpath/$$osdir && libpath=$$libpath/$$osdir; \
	  test -f $$libpath/libqt-mt.so && lib=qt-mt; \
	  cflags="-I$$dir/include"; \
	  libs="-L$$libpath -Wl,-rpath,$$libpath -l$$lib"; \
	  moc="$$dir/bin/moc"; \
	fi; \
	if [ ! -x $$dir/bin/moc -a -x /usr/bin/moc ]; then \
	  echo "*"; \
	  echo "* Unable to find $$dir/bin/moc, using /usr/bin/moc instead."; \
	  echo "*"; \
	  moc="/usr/bin/moc"; \
	fi; \
	mkdir -p $(obj)/util/kconfig; \
	echo "KC_QT_CFLAGS=$$cflags" > $@; \
	echo "KC_QT_LIBS=$$libs" >> $@; \
	echo "KC_QT_MOC=$$moc" >> $@
endif

ifeq ($(gconf-target),1)
$(obj)/util/kconfig/.tmp_gtkcheck: $(src)/util/kconfig/Makefile

-include $(obj)/util/kconfig/.tmp_gtkcheck

# GTK needs some extra effort, too...
$(obj)/util/kconfig/.tmp_gtkcheck:
	@if `pkg-config --exists gtk+-2.0 gmodule-2.0 libglade-2.0`; then		\
		if `pkg-config --atleast-version=2.0.0 gtk+-2.0`; then			\
			mkdir -p $(obj)/util/kconfig;					\
			touch $@;							\
		else									\
			echo "*"; 							\
			echo "* GTK+ is present but version >= 2.0.0 is required.";	\
			echo "*";							\
			false;								\
		fi									\
	else										\
		echo "*"; 								\
		echo "* Unable to find the GTK+ installation. Please make sure that"; 	\
		echo "* the GTK+ 2.0 development package is correctly installed..."; 	\
		echo "* You need gtk+-2.0, glib-2.0 and libglade-2.0."; 		\
		echo "*"; 								\
		false;									\
	fi
endif

###
# The following requires flex/bison/gperf
# By default we use the _shipped versions, uncomment the following line if
# you are modifying the flex/bison src.
# LKC_GENPARSER := 1

ifdef LKC_GENPARSER

$(obj)/zconf.tab.c: $(src)/util/kconfig/zconf.y
$(obj)/lex.zconf.c: $(src)/util/kconfig/zconf.l
$(obj)/zconf.hash.c: $(src)/util/kconfig/zconf.gperf

%.tab.c: %.y
	bison -l -b $* -p $(notdir $*) $<
	cp $@ $@_shipped

lex.%.c: %.l
	flex -L -P$(notdir $*) -o$@ $<
	cp $@ $@_shipped

%.hash.c: %.gperf
	gperf < $< > $@
	cp $@ $@_shipped

endif
