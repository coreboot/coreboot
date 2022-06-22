## SPDX-License-Identifier: BSD-3-Clause

ifneq ($(words $(CURDIR)),1)
    $(error Error: Path to the main directory cannot contain spaces)
endif
top := $(CURDIR)
src := src
srck := $(top)/util/kconfig
obj ?= build
override obj := $(subst $(top)/,,$(abspath $(obj)))
xcompile ?= $(obj)/xcompile
objutil ?= $(obj)/util
objk := $(objutil)/kconfig
absobj := $(abspath $(obj))

additional-dirs :=

VBOOT_HOST_BUILD ?= $(abspath $(objutil)/vboot_lib)

COREBOOT_EXPORTS := COREBOOT_EXPORTS
COREBOOT_EXPORTS += top src srck obj objutil objk

DOTCONFIG ?= $(top)/.config
KCONFIG_CONFIG = $(DOTCONFIG)
KCONFIG_AUTOADS := $(obj)/cb-config.ads
KCONFIG_AUTOHEADER := $(obj)/config.h
KCONFIG_AUTOCONFIG := $(obj)/auto.conf
KCONFIG_DEPENDENCIES := $(obj)/auto.conf.cmd
KCONFIG_SPLITCONFIG := $(obj)/config/
KCONFIG_TRISTATE := $(obj)/tristate.conf
KCONFIG_NEGATIVES := 1
KCONFIG_STRICT := 1
KCONFIG_PACKAGE := CB.Config

COREBOOT_EXPORTS += KCONFIG_CONFIG KCONFIG_AUTOHEADER KCONFIG_AUTOCONFIG
COREBOOT_EXPORTS += KCONFIG_DEPENDENCIES KCONFIG_SPLITCONFIG KCONFIG_TRISTATE
COREBOOT_EXPORTS += KCONFIG_NEGATIVES KCONFIG_STRICT
COREBOOT_EXPORTS += KCONFIG_AUTOADS KCONFIG_PACKAGE

# directory containing the toplevel Makefile.inc
TOPLEVEL := .

CONFIG_SHELL := sh
KBUILD_DEFCONFIG := configs/defconfig
UNAME_RELEASE := $(shell uname -r)
HAVE_DOTCONFIG := $(wildcard $(DOTCONFIG))
MAKEFLAGS += -rR --no-print-directory

# Make is silent per default, but 'make V=1' will show all compiler calls.
Q:=@
ifneq ($(V),1)
ifneq ($(Q),)
.SILENT:
MAKEFLAGS += -s
quiet_errors := 2>/dev/null
endif
endif

# Disable implicit/built-in rules to make Makefile errors fail fast.
.SUFFIXES:

HOSTCFLAGS := -g
HOSTCXXFLAGS := -g

PREPROCESS_ONLY := -E -P -x assembler-with-cpp -undef -I .

export $(COREBOOT_EXPORTS)

all: real-all

help_coreboot help::
	@echo  '*** coreboot platform targets ***'
	@echo  '  Use "make [target] V=1" for extra build debug information'
	@echo  '  all                   - Build coreboot'
	@echo  '  clean                 - Remove coreboot build artifacts'
	@echo  '  distclean             - Remove build artifacts and config files'
	@echo  '  sphinx                - Build sphinx documentation for coreboot'
	@echo  '  sphinx-lint           - Build sphinx documenttion for coreboot with warnings as errors'
	@echo  '  filelist              - Show files used in current build'
	@echo  '  printall              - print makefile info for debugging'
	@echo  '  gitconfig             - set up git to submit patches to coreboot'
	@echo  '  ctags / ctags-project - make ctags file for all of coreboot or current board'
	@echo  '  cscope / cscope-project - make cscope.out file for coreboot or current board'
	@echo

# This include must come _before_ the pattern rules below!
# Order _does_ matter for pattern rules.
include $(srck)/Makefile.inc

# Three cases where we don't need fully populated $(obj) lists:
# 1. when no .config exists
# 2. when make config (in any flavour) is run
# 3. when make distclean is run
# Don't waste time on reading all Makefile.incs in these cases
ifeq ($(strip $(HAVE_DOTCONFIG)),)
NOCOMPILE:=1
endif
ifneq ($(MAKECMDGOALS),)
ifneq ($(filter %config %clean cross% clang iasl lint% help% what-jenkins-does,$(MAKECMDGOALS)),)
NOCOMPILE:=1
endif
ifneq ($(filter %clean lint% help% what-jenkins-does,$(MAKECMDGOALS)),)
NOMKDIR:=1
UNIT_TEST:=1
endif
endif

ifneq ($(filter help%, $(MAKECMDGOALS)), )
NOCOMPILE:=1
UNIT_TEST:=1
else
ifneq ($(filter %-test %-tests %coverage-report, $(MAKECMDGOALS)),)
ifneq ($(filter-out %-test %-tests %coverage-report, $(MAKECMDGOALS)),)
$(error Cannot mix unit-tests targets with other targets)
endif
UNIT_TEST:=1
NOCOMPILE:=
endif
endif

$(xcompile): util/xcompile/xcompile
	rm -f $@
	$< $(XGCCPATH) > $@.tmp
	\mv -f $@.tmp $@ 2> /dev/null
	rm -f $@.tmp

ifeq ($(NOCOMPILE),1)
# We also don't use .xcompile in the no-compile situations, so
# provide some reasonable defaults.
HOSTCC ?= $(if $(shell type gcc 2>/dev/null),gcc,cc)
HOSTCXX ?= g++

include $(TOPLEVEL)/Makefile.inc
include $(TOPLEVEL)/payloads/Makefile.inc
include $(TOPLEVEL)/util/testing/Makefile.inc
-include $(TOPLEVEL)/site-local/Makefile.inc
include $(TOPLEVEL)/tests/Makefile.inc
real-all:
	@echo "Error: Trying to build, but NOCOMPILE is set." >&2
	@echo "  Please file a bug with the following information:"
	@echo "- MAKECMDGOALS: $(MAKECMDGOALS)" >&2
	@echo "- HAVE_DOTCONFIG: $(HAVE_DOTCONFIG)" >&2
	@echo "- HAVE_KCONFIG_MAKEFILE_REAL: $(HAVE_KCONFIG_MAKEFILE_REAL)" >&2
	@exit 1
else

ifneq ($(UNIT_TEST),1)
include $(DOTCONFIG)
endif

# The toolchain requires xcompile to determine the ARCH_SUPPORTED, so we can't
# wait for make to generate the file.
$(if $(wildcard $(xcompile)),, $(shell \
	mkdir -p $(dir $(xcompile)) && \
	util/xcompile/xcompile $(XGCCPATH) > $(xcompile) || rm -f $(xcompile)))

include $(xcompile)

ifneq ($(XCOMPILE_COMPLETE),1)
$(shell rm -f $(xcompile))
$(error $(xcompile) deleted because it's invalid. \
	Restarting the build should fix that, or explain the problem)
endif

# reproducible builds
LANG:=C
LC_ALL:=C
TZ:=UTC0
ifneq ($(NOCOMPILE),1)
SOURCE_DATE_EPOCH := $(shell $(top)/util/genbuild_h/genbuild_h.sh . | sed -n 's/^.define COREBOOT_BUILD_EPOCH\>.*"\(.*\)".*/\1/p')
endif
# don't use COREBOOT_EXPORTS to ensure build steps outside the coreboot build system
# are reproducible
export LANG LC_ALL TZ SOURCE_DATE_EPOCH

ifneq ($(CONFIG_MMX),y)
CFLAGS_x86_32 += -mno-mmx
endif

ifneq ($(UNIT_TEST),1)
include toolchain.inc
endif

strip_quotes = $(strip $(subst ",,$(subst \",,$(1))))
# fix makefile syntax highlighting after strip macro \" "))

# The primary target needs to be here before we include the
# other files

real-all: real-target

# must come rather early
.SECONDARY:
.SECONDEXPANSION:
.DELETE_ON_ERROR:

$(KCONFIG_AUTOHEADER): $(KCONFIG_CONFIG) $(objutil)/kconfig/conf
	$(MAKE) olddefconfig
	$(MAKE) syncconfig

$(KCONFIG_AUTOCONFIG): $(KCONFIG_AUTOHEADER)
	true

$(KCONFIG_AUTOADS): $(KCONFIG_CONFIG) $(KCONFIG_AUTOHEADER) $(objutil)/kconfig/toada
	$(objutil)/kconfig/toada CB.Config <$< >$@

$(obj)/%/$(notdir $(KCONFIG_AUTOADS)): $(KCONFIG_AUTOADS)
	cp $< $@

# Add a new class of source/object files to the build system
add-class= \
	$(eval $(1)-srcs:=) \
	$(eval $(1)-objs:=) \
	$(eval classes+=$(1))

# Special classes are managed types with special behaviour
# On parse time, for each entry in variable $(1)-y
# a handler $(1)-handler is executed with the arguments:
# * $(1): directory the parser is in
# * $(2): current entry
add-special-class= \
	$(eval $(1):=) \
	$(eval special-classes+=$(1))

# Converts one or more source file paths to their corresponding build/ paths.
# Only .ads, adb, .c and .S get converted to .o, other files (like .ld) keep
# their name.
# $1 stage name
# $2 file path (list)
src-to-obj=\
	$(patsubst $(obj)/%,$(obj)/$(1)/%,\
	$(patsubst $(obj)/$(1)/%,$(obj)/%,\
	$(patsubst 3rdparty/%,$(obj)/%,\
	$(patsubst src/%,$(obj)/%,\
	$(patsubst %.ads,%.o,\
	$(patsubst %.adb,%.o,\
	$(patsubst %.c,%.o,\
	$(patsubst %.S,%.o,\
	$(subst .$(1),,$(2))))))))))

# Converts one or more source file paths to the corresponding build/ paths
# of their Ada library information (.ali) files.
# $1 stage name
# $2 file path (list)
src-to-ali=\
	$(patsubst $(obj)/%,$(obj)/$(1)/%,\
	$(patsubst $(obj)/$(1)/%,$(obj)/%,\
	$(patsubst 3rdparty/%,$(obj)/%,\
	$(patsubst src/%,$(obj)/%,\
	$(patsubst %.ads,%.ali,\
	$(patsubst %.adb,%.ali,\
	$(subst .$(1),,\
	$(filter %.ads %.adb,$(2)))))))))

# Clean -y variables, include Makefile.inc
# Add paths to files in X-y to X-srcs
# Add subdirs-y to subdirs
includemakefiles= \
	$(foreach class,classes subdirs $(classes) $(special-classes), $(eval $(class)-y:=)) \
	$(eval -include $(1)) \
	$(foreach class,$(classes-y), $(call add-class,$(class))) \
	$(foreach special,$(special-classes), \
		$(foreach item,$($(special)-y), $(call $(special)-handler,$(dir $(1)),$(item)))) \
	$(foreach class,$(classes), \
		$(eval $(class)-srcs+= \
			$$(subst $(absobj)/,$(obj)/, \
			$$(subst $(top)/,, \
			$$(abspath $$(subst $(dir $(1))/,/,$$(addprefix $(dir $(1)),$$($(class)-y)))))))) \
	$(eval subdirs+=$$(subst $(CURDIR)/,,$$(wildcard $$(abspath $$(addprefix $(dir $(1)),$$(subdirs-y))))))

# For each path in $(subdirs) call includemakefiles
# Repeat until subdirs is empty
evaluate_subdirs= \
	$(eval cursubdirs:=$(subdirs)) \
	$(eval subdirs:=) \
	$(foreach dir,$(cursubdirs), \
		$(eval $(call includemakefiles,$(dir)/Makefile.inc))) \
	$(if $(subdirs),$(eval $(call evaluate_subdirs)))

# collect all object files eligible for building
subdirs:=$(TOPLEVEL)
postinclude-hooks :=

# Don't iterate through Makefile.incs under src/ when building tests
ifneq ($(UNIT_TEST),1)
$(eval $(call evaluate_subdirs))
else
include $(TOPLEVEL)/tests/Makefile.inc
endif

ifeq ($(FAILBUILD),1)
$(error cannot continue build)
endif

# Run hooks registered by subdirectories that need to be evaluated after all files have been parsed
$(eval $(postinclude-hooks))

# Eliminate duplicate mentions of source files in a class
$(foreach class,$(classes),$(eval $(class)-srcs:=$(sort $($(class)-srcs))))

# Build Kconfig .ads if necessary
ifeq ($(CONFIG_RAMSTAGE_ADA),y)
ramstage-srcs += $(obj)/ramstage/$(notdir $(KCONFIG_AUTOADS))
endif

# To track dependencies, we need all Ada specification (.ads) files in
# *-srcs. Extract / filter all specification files that have a matching
# body (.adb) file here (specifications without a body are valid sources
# in Ada).
$(foreach class,$(classes),$(eval $(class)-extra-specs := \
	$(filter \
		$(addprefix %/,$(patsubst %.adb,%.ads,$(notdir $(filter %.adb,$($(class)-srcs))))), \
		$(filter %.ads,$($(class)-srcs)))))
$(foreach class,$(classes),$(eval $(class)-srcs := \
	$(filter-out $($(class)-extra-specs),$($(class)-srcs))))

$(foreach class,$(classes),$(eval $(class)-objs:=$(call src-to-obj,$(class),$($(class)-srcs))))
$(foreach class,$(classes),$(eval $(class)-alis:=$(call src-to-ali,$(class),$($(class)-srcs))))

# For Ada includes
$(foreach class,$(classes),$(eval $(class)-ada-dirs:=$(sort $(dir $(filter %.ads %.adb,$($(class)-srcs)) $($(class)-extra-specs)))))

# Save all objs before processing them (for dependency inclusion)
originalobjs:=$(foreach var, $(addsuffix -objs,$(classes)), $($(var)))

# Call post-processors if they're defined
$(foreach class,$(classes),\
	$(if $(value $(class)-postprocess),$(eval $(call $(class)-postprocess,$($(class)-objs)))))

allsrcs:=$(foreach var, $(addsuffix -srcs,$(classes)), $($(var)))
allobjs:=$(foreach var, $(addsuffix -objs,$(classes)), $($(var)))
alldirs:=$(sort $(abspath $(dir $(allobjs))))

# Reads dependencies from an Ada library information (.ali) file
# Only basenames (with suffix) are preserved so we have to look the
# paths up in $($(stage)-srcs).
# $1 stage name
# $2 ali file
create_ada_deps=$$(if $(2),\
	gnat.adc \
	$$(filter \
		$$(addprefix %/,$$(shell sed -ne's/^D \([^\t]\+\).*$$$$/\1/p' $(2) 2>/dev/null)), \
		$$($(1)-srcs) $$($(1)-extra-specs)))

# macro to define template macros that are used by use_template macro
define create_cc_template
# $1 obj class
# $2 source suffix (c, S, ld, ...)
# $3 additional compiler flags
# $4 additional dependencies
ifn$(EMPTY)def $(1)-objs_$(2)_template
de$(EMPTY)fine $(1)-objs_$(2)_template
ifn$(EMPTY)eq ($(filter ads adb,$(2)),)
$$(call src-to-obj,$1,$$(1).$2): $$(1).$2 $$(call create_ada_deps,$1,$$(call src-to-ali,$1,$$(1).$2)) $(4)
	@printf "    GCC        $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$(GCC_$(1)) \
		$$$$(ADAFLAGS_$(1)) $$$$(addprefix -I,$$$$($(1)-ada-dirs)) \
		$(3) -c -o $$$$@ $$$$<
el$(EMPTY)se
$$(call src-to-obj,$1,$$(1).$2): $$(1).$2 $(KCONFIG_AUTOHEADER) $(4)
	@printf "    CC         $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$(CC_$(1)) \
		-MMD $$$$(CPPFLAGS_$(1)) $$$$(CFLAGS_$(1)) -MT $$$$(@) \
		$(3) -c -o $$$$@ $$$$<
end$(EMPTY)if
en$(EMPTY)def
end$(EMPTY)if
endef

filetypes-of-class=$(subst .,,$(sort $(suffix $($(1)-srcs))))
$(foreach class,$(classes), \
	$(foreach type,$(call filetypes-of-class,$(class)), \
		$(eval $(class)-$(type)-ccopts += $(generic-$(type)-ccopts) $($(class)-generic-ccopts)) \
		$(if $(generic-objs_$(type)_template_gen),$(eval $(call generic-objs_$(type)_template_gen,$(class))),\
		$(eval $(call create_cc_template,$(class),$(type),$($(class)-$(type)-ccopts),$($(class)-$(type)-deps))))))

foreach-src=$(foreach file,$($(1)-srcs),$(eval $(call $(1)-objs_$(subst .,,$(suffix $(file)))_template,$(basename $(file)))))
$(eval $(foreach class,$(classes),$(call foreach-src,$(class))))

# To supported complex package initializations, we need to call the
# emitted code explicitly. gnatbind gathers all the calls for us
# and exports them as a procedure $(stage)_adainit(). Every stage that
# uses Ada code has to call it!
define gnatbind_template
# $1 class
$$(obj)/$(1)/b__$(1).adb: $$$$(filter-out $$(obj)/$(1)/b__$(1).ali,$$$$($(1)-alis))
	@printf "    BIND       $$(subst $$(obj)/,,$$@)\n"
	# We have to give gnatbind a simple filename (without leading
	# path components) so just cd there.
	cd $$(dir $$@) && \
		$$(GNATBIND_$(1)) -a -n \
			--RTS=$$(absobj)/libgnat-$$(ARCH-$(1)-y)/ \
			-L$(1)_ada -o $$(notdir $$@) \
			$$(subst $$(dir $$@),,$$^)
$$(obj)/$(1)/b__$(1).o: $$(obj)/$(1)/b__$(1).adb
	@printf "    GCC        $$(subst $$(obj)/,,$$@)\n"
	$(GCC_$(1)) $$(ADAFLAGS_$(1)) -c -o $$@ $$<
$(1)-objs += $$(obj)/$(1)/b__$(1).o
$($(1)-alis): %.ali: %.o ;
endef

$(eval $(foreach class,$(filter-out libgnat-%,$(classes)), \
	$(if $($(class)-alis),$(call gnatbind_template,$(class)))))

DEPENDENCIES += $(addsuffix .d,$(basename $(allobjs)))
-include $(DEPENDENCIES)

printall:
	@$(foreach class,$(classes), echo $(class)-objs: $($(class)-objs) | tr ' ' '\n'; echo; )
	@echo alldirs: $(alldirs) | tr ' ' '\n'; echo
	@echo allsrcs: $(allsrcs) | tr ' ' '\n'; echo
	@echo DEPENDENCIES: $(DEPENDENCIES) | tr ' ' '\n'; echo
	@$(foreach class,$(special-classes),echo $(class):'$($(class))' | tr ' ' '\n'; echo; )
endif

ifndef NOMKDIR
$(shell mkdir -p $(KCONFIG_SPLITCONFIG) $(objk)/lxdialog $(additional-dirs) $(alldirs))
endif

$(obj)/project_filelist.txt:
	if [ -z "$(wildcard $(obj)/coreboot.rom)" ]; then \
		echo "*** Error: Project must be built before generating file list ***"; \
		exit 1; \
	fi
	find $(obj) -path "$(obj)/util" -prune -o -name "*.d" -exec cat {} \; | \
	  sed "s|$(top)/||" | sed 's/[:\\]/ /g' | sed 's/ /\n/g' | sort | uniq | \
	  grep -v '\.o$$' > $(obj)/project_filelist.txt

filelist: $(obj)/project_filelist.txt
	printf "\nFiles used in build:\n"
	cat $(obj)/project_filelist.txt

#works with either exuberant ctags or ctags.emacs
ctags-project: clean-ctags $(obj)/project_filelist.txt
	cat $(obj)/project_filelist.txt | \
	  xargs ctags -o tags

cscope-project: clean-cscope $(obj)/project_filelist.txt
	cat $(obj)/project_filelist.txt | xargs cscope -b

cscope:
	cscope -bR

sphinx:
	$(MAKE) -C Documentation -f Makefile.sphinx html

sphinx-lint:
	$(MAKE) SPHINXOPTS=-W -C Documentation -f Makefile.sphinx html

clean-for-update:
	rm -rf $(obj) .xcompile

clean: clean-for-update clean-utils clean-payloads
	rm -f .ccwrap

clean-cscope:
	rm -f cscope.out

clean-ctags:
	rm -f tags

clean-utils:
	$(foreach tool, $(TOOLLIST), \
		$(MAKE) -C util/$(tool) clean MFLAGS= MAKEFLAGS= ;)

distclean-utils:
	$(foreach tool, $(TOOLLIST), \
		$(MAKE) -C util/$(tool) distclean MFLAGS= MAKEFLAGS= ; \
		rm -f /util/$(tool)/junit.xml;)

distclean: clean clean-ctags clean-cscope distclean-payloads distclean-utils
	rm -f .config .config.old ..config.tmp* .kconfig.d .tmpconfig* .ccwrap .xcompile
	rm -rf coreboot-builds coreboot-builds-chromeos
	rm -f abuild*.xml junit.xml* util/lint/junit.xml

.PHONY: $(PHONY) clean clean-for-update clean-cscope cscope distclean sphinx sphinx-lint
.PHONY: ctags-project cscope-project clean-ctags
