# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile

PHONY += oldconfig xconfig gconfig menuconfig config silentoldconfig update-po-config

_OS=$(shell uname -s |cut -c-7)
regex-objs=
ifeq ($(_OS),MINGW32)
       regex-objs=regex.o
endif

Kconfig := Config.in

xconfig: prepare $(objk)/qconf
	$(Q)$(objk)/qconf $(Kconfig)

gconfig: prepare $(objk)/gconf
	$(Q)$(objk)/gconf $(Kconfig)

menuconfig: prepare $(objk)/mconf
	$(Q)$(objk)/mconf $(Kconfig)

config: prepare $(objk)/conf
	$(Q)$(objk)/conf $(Kconfig)

oldconfig: prepare $(objk)/conf
	$(Q)$(objk)/conf -o $(Kconfig)

silentoldconfig: prepare $(objk)/conf
	$(Q)$(objk)/conf -s $(Kconfig)

# --- UNUSED, ignore ----------------------------------------------------------
# Create new linux.pot file
# Adjust charset to UTF-8 in .po file to accept UTF-8 in Kconfig files
# The symlink is used to repair a deficiency in arch/um
update-po-config: $(obj)/kxgettext $(obj)/gconf.glade.h
	$(Q)echo "  GEN config"
	$(Q)xgettext --default-domain=linux              \
	    --add-comments --keyword=_ --keyword=N_      \
	    --from-code=UTF-8                            \
	    --files-from=scripts/kconfig/POTFILES.in     \
	    --output $(obj)/config.pot
	$(Q)sed -i s/CHARSET/UTF-8/ $(obj)/config.pot
	$(Q)ln -fs Kconfig.i386 arch/um/Kconfig.arch
	$(Q)(for i in `ls arch/`;                        \
	    do                                           \
		echo "  GEN $$i";                        \
		$(obj)/kxgettext arch/$$i/Kconfig        \
		     >> $(obj)/config.pot;               \
	    done )
	$(Q)msguniq --sort-by-file --to-code=UTF-8 $(obj)/config.pot \
	    --output $(obj)/linux.pot
	$(Q)rm -f arch/um/Kconfig.arch
	$(Q)rm -f $(obj)/config.pot
# --- UNUSED, ignore ----------------------------------------------------------

PHONY += randconfig allyesconfig allnoconfig allmodconfig defconfig

randconfig: prepare $(objk)/conf
	$(Q)$(objk)/conf -r $(Kconfig)

allyesconfig: prepare $(objk)/conf
	$(Q)$(objk)/conf -y $(Kconfig)

allnoconfig: prepare $(objk)/conf
	$(Q)$(objk)/conf -n $(Kconfig)

allmodconfig: prepare $(objk)/conf
	$(Q)$(objk)/conf -m $(Kconfig)

defconfig: prepare $(objk)/conf
ifeq ($(KBUILD_DEFCONFIG),)
	$(Q)$(objk)/conf -d $(Kconfig)
else
	@echo "*** Default configuration is based on '$(KBUILD_DEFCONFIG)'"
	$(Q)$(objk)/conf -D $(KBUILD_DEFCONFIG) $(Kconfig)
endif

%_defconfig: prepare $(objk)/conf
	$(Q)$(objk)/conf -D configs/$@ $(Kconfig)

# Help text used by make help
help:
	@echo  '  config	  - Update current config utilising a line-oriented program'
	@echo  '  menuconfig	  - Update current config utilising a menu based program'
	@echo  '  xconfig	  - Update current config utilising a QT based front-end'
	@echo  '  gconfig	  - Update current config utilising a GTK based front-end'
	@echo  '  oldconfig	  - Update current config utilising a provided .config as base'
	@echo  '  silentoldconfig - Same as oldconfig, but quietly'
	@echo  '  randconfig	  - New config with random answer to all options'
	@echo  '  defconfig	  - New config with default answer to all options'
	@echo  '  allmodconfig	  - New config selecting modules when possible'
	@echo  '  allyesconfig	  - New config where all options are accepted with yes'
	@echo  '  allnoconfig	  - New config where all options are answered with no'

# lxdialog stuff
check-lxdialog  := $(srck)/lxdialog/check-lxdialog.sh

# Use recursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
HOST_EXTRACFLAGS = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ccflags)
HOST_LOADLIBES   = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ldflags $(HOSTCC))

HOST_EXTRACFLAGS += -DLOCALE


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

lxdialog := lxdialog/checklist.o lxdialog/util.o lxdialog/inputbox.o
lxdialog += lxdialog/textbox.o lxdialog/yesno.o lxdialog/menubox.o

conf-objs	:= conf.o  zconf.tab.o $(regex-objs)
mconf-objs	:= mconf.o zconf.tab.o $(lxdialog) $(regex-objs)
kxgettext-objs	:= kxgettext.o zconf.tab.o

hostprogs-y := conf qconf gconf kxgettext

ifeq ($(MAKECMDGOALS),menuconfig)
	hostprogs-y += mconf
endif

ifeq ($(MAKECMDGOALS),xconfig)
	qconf-target := 1
endif
ifeq ($(MAKECMDGOALS),gconfig)
	gconf-target := 1
endif


ifeq ($(qconf-target),1)
qconf-cxxobjs	:= qconf.o
qconf-objs	:= kconfig_load.o zconf.tab.o $(regex-objs)
endif

ifeq ($(gconf-target),1)
gconf-objs	:= gconf.o kconfig_load.o zconf.tab.o $(regex-objs)
endif

clean-files	:= lkc_defs.h qconf.moc .tmp_qtcheck \
		   .tmp_gtkcheck zconf.tab.c lex.zconf.c zconf.hash.c gconf.glade.h
clean-files     += mconf qconf gconf
clean-files     += config.pot coreinfo.pot

# Check that we have the required ncurses stuff installed for lxdialog (menuconfig)
PHONY += $(objk)/dochecklxdialog
$(addprefix $(obj)/,$(lxdialog)): $(objk)/dochecklxdialog
$(objk)/dochecklxdialog:
	$(Q)$(CONFIG_SHELL) $(check-lxdialog) -check $(HOSTCC) $(HOST_EXTRACFLAGS) $(HOST_LOADLIBES)

always := dochecklxdialog

# Add environment specific flags
HOST_EXTRACFLAGS += $(shell $(CONFIG_SHELL) $(srck)/check.sh $(HOSTCC) $(HOSTCFLAGS))

# generated files seem to need this to find local include files
HOSTCFLAGS_lex.zconf.o	:= -I$(src)
HOSTCFLAGS_zconf.tab.o	:= -I$(src)

HOSTLOADLIBES_qconf	= $(KC_QT_LIBS) -ldl
HOSTCXXFLAGS_qconf.o	= $(KC_QT_CFLAGS) -D LKC_DIRECT_LINK

HOSTLOADLIBES_gconf	= `pkg-config --libs gtk+-2.0 gmodule-2.0 libglade-2.0`
HOSTCFLAGS_gconf.o	= `pkg-config --cflags gtk+-2.0 gmodule-2.0 libglade-2.0` \
                          -D LKC_DIRECT_LINK

$(objk)/qconf.o: $(objk)/.tmp_qtcheck

ifeq ($(qconf-target),1)
$(objk)/.tmp_qtcheck: $(srck)/Makefile
-include $(objk)/.tmp_qtcheck

# QT needs some extra effort...
$(objk)/.tmp_qtcheck: prepare
	@set -e; dir=""; pkg=""; \
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
	    echo "* Unable to find the QT3 installation. Please make sure that"; \
	    echo "* the QT3 development package is correctly installed and"; \
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
	echo "KC_QT_CFLAGS=$$cflags" > $@; \
	echo "KC_QT_LIBS=$$libs" >> $@; \
	echo "KC_QT_MOC=$$moc" >> $@
endif

$(objk)/gconf.o: $(objk)/.tmp_gtkcheck

ifeq ($(gconf-target),1)
-include $(objk)/.tmp_gtkcheck

# GTK needs some extra effort, too...
$(objk)/.tmp_gtkcheck: prepare
	@if `pkg-config --exists gtk+-2.0 gmodule-2.0 libglade-2.0`; then		\
		if `pkg-config --atleast-version=2.0.0 gtk+-2.0`; then			\
			touch $@;								\
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

# --- UNUSED, ignore ----------------------------------------------------------
ifdef UNUSED
$(obj)/zconf.tab.o: $(obj)/lex.zconf.c $(obj)/zconf.hash.c

$(obj)/kconfig_load.o: $(obj)/lkc_defs.h

$(obj)/qconf.o: $(obj)/qconf.moc $(obj)/lkc_defs.h

$(obj)/gconf.o: $(obj)/lkc_defs.h

$(obj)/%.moc: $(src)/%.h
	$(KC_QT_MOC) -i $< -o $@

$(obj)/lkc_defs.h: $(src)/lkc_proto.h
	sed < $< > $@ 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

# Extract gconf menu items for I18N support
$(obj)/gconf.glade.h: $(obj)/gconf.glade
	intltool-extract --type=gettext/glade $(obj)/gconf.glade
endif
# --- UNUSED, ignore ----------------------------------------------------------

###
# The following requires flex/bison/gperf
# By default we use the _shipped versions, uncomment the following line if
# you are modifying the flex/bison src.
# LKC_GENPARSER := 1

ifdef LKC_GENPARSER

# --- UNUSED, ignore ----------------------------------------------------------
$(obj)/zconf.tab.c: $(src)/zconf.y
$(obj)/lex.zconf.c: $(src)/zconf.l
$(obj)/zconf.hash.c: $(src)/zconf.gperf

%.tab.c: %.y
	bison -l -b $* -p $(notdir $*) $<
	cp $@ $@_shipped

lex.%.c: %.l
	flex -L -P$(notdir $*) -o$@ $<
	cp $@ $@_shipped

%.hash.c: %.gperf
	gperf < $< > $@
	cp $@ $@_shipped
# --- UNUSED, ignore ----------------------------------------------------------

endif

$(objk)/qconf: $(patsubst %,$(objk)/%,$(qconf-cxxobjs)) \
	       $(patsubst %,$(objk)/%,$(qconf-objs))
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) $(HOSTLOADLIBES_qconf) -o $@ $^
$(objk)/gconf: $(patsubst %,$(objk)/%,$(gconf-objs))
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(HOSTLOADLIBES_gconf) -o $@ $^
$(objk)/mconf: $(patsubst %,$(objk)/%,$(mconf-objs))
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) -o $@ $^ $(HOST_LOADLIBES)
$(objk)/conf: $(patsubst %,$(objk)/%,$(conf-objs))
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) -o $@ $^

$(objk)/mconf.o: $(srck)/mconf.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) -c -o $@ $<
$(objk)/conf.o: $(srck)/conf.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) -c -o $@ $<
$(objk)/regex.o: $(srck)/regex.c
	$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) -DHAVE_STRING_H -c -o $@ $<

$(objk)/zconf.tab.o: $(objk)/zconf.tab.c $(objk)/lex.zconf.c \
		     $(objk)/zconf.hash.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) -c -o $@ $(objk)/zconf.tab.c
$(objk)/kconfig_load.o: $(srck)/kconfig_load.c $(objk)/lkc_defs.h
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -c -o $@ $<
$(objk)/qconf.o: $(srck)/qconf.cc $(objk)/qconf.moc $(objk)/lkc_defs.h
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) $(HOSTCXXFLAGS_qconf.o) -c -o $@ $<
$(objk)/gconf.o: $(srck)/gconf.c $(objk)/lkc_defs.h
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(HOSTCFLAGS_gconf.o) -c -o $@ $<
$(objk)/%.moc: $(srck)/%.h
	$(Q)$(KC_QT_MOC) -i $< -o $@
$(objk)/lkc_defs.h: $(srck)/lkc_proto.h
	$(Q)sed < $< > $@ 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

$(objk)/lex.zconf.c: $(srck)/lex.zconf.c_shipped
	$(Q)cp $< $@
$(objk)/zconf.hash.c: $(srck)/zconf.hash.c_shipped
	$(Q)cp $< $@
$(objk)/zconf.tab.c: $(srck)/zconf.tab.c_shipped
	$(Q)cp $< $@

$(objk)/lxdialog/lxdialog: $(objk)/dochecklxdialog \
			   $(patsubst %,$(objk)/lxdialog/%,$(lxdialog))
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) \
		$(patsubst %,$(objk)/lxdialog/%,$(lxdialog)) $(HOST_LOADLIBES) -o $@
$(objk)/lxdialog/%.o: $(srck)/lxdialog/%.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) $^ -c -o $@

