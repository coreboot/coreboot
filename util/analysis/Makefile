#  Coreboot codebase analysis tool
#
#  This makefile collects source usage information for all working targets.
#
#  Written 7/2006 by Josiah England <josiah@lanl.gov>
#
#  This file is subject to the terms and conditions of the GNU General
#  Public License. See the file COPYING in the main directory of this
#  archive for more details.

TOP := $(shell cd ../.. && pwd)
BUILD_BASE := $(TOP)/targets

IGNORE_ERRORS := 2>/dev/null # Comment out this line for some ugly verbosity
IGNORE_VENDORS := emulation momentum embeddedplanet motorola totalimpact
IGNORE_C := static.c# romcc.c

quote = "#"

VENDORS := $(shell ls -l $(TOP)/targets | grep ^d | grep -Eo [[:alnum:]_-]+$$$(foreach ignored, $(IGNORE_VENDORS), | grep -v $(ignored)))
#<VENDOR>_BOARDS assignments
$(foreach VENDOR, $(VENDORS), $(eval $(VENDOR)_BOARDS := $(shell ls $(TOP)/targets/$(VENDOR))))
TARGETS := $(foreach VENDOR, $(VENDORS), $(addprefix $(VENDOR)/, $($(VENDOR)_BOARDS)))

# The following delayed-evalutate variables are only to be used in rule commands.
MAINBOARD = $(TOP)/src/mainboard/$(shell grep ^mainboard $(dir $*)/Config.lb|grep -Eo [-[:alnum:]_/]+[[:space:]]?$$)
IMAGE_DIR = $(firstword $(shell grep -Eo ^romimage[[:space:]]+\"[[:alnum:]_-/]+ $(dir $*)/Config.lb|sed -r s/romimage[[:space:]]+\"//))

# Evaluate one assignment to variable "$1" from file "$2"
load_var = $(eval $(shell grep -E ^[[:space:]]*$1[[:space:]]*:*= $2 $(IGNORE_ERRORS)))

.PHONY: clean analysis

analysis: analysis.dat
	gnuplot -persist '$<'

# Generate gnuplot data file
analysis.dat: analysis.txt
	@ echo Writing gnuplot data file \($@\).
	@ echo -e > $@ "# gnuplot dataset auto-generated $(shell date)" \
	"\nset title \"Coreboot Codebase Analysis\"" \
	"\nset style data boxes" \
	"\nset style fill solid .5" \
	$(foreach target, $(TARGETS), "\n"set label \"$(target)\" at $(words $(labels))$(eval labels += $(target)),-145 rotate front) \
	"\nplot [-.5:] '-' t 'Source:' , '-' t 'Nested C:' , '-' t 'Headers:' , '-' t 'romcc Sources:' , '-' t 'romcc Headers:'"
	@ grep -F "C files" $< | grep -Eo [[:digit:]]+ >> $@
	@ echo e >> $@
	@ grep -F "Nested C" $< | grep -Eo [[:digit:]]+ >> $@
	@ echo e >> $@
	@ grep -F "Headers" $< | grep -Eo [[:digit:]]+ | sed -r s/\([[:digit:]]+\)/'-'\\1/>> $@
	@ echo e >> $@
	@ grep -F "romcc C" $< | grep -Eo [[:digit:]]+ >> $@
	@ echo e >> $@
	@ grep -F "romcc H" $< | grep -Eo [[:digit:]]+ | sed -r s/\([[:digit:]]+\)/'-'\\1/>> $@
	@ echo e >> $@

analysis.txt: $(foreach target, $(TARGETS), $(BUILD_BASE)/$(target)/$(shell grep ^target $(BUILD_BASE)/$(target)/Config.lb | grep -Eo [[:alnum:]_-]+[[:space:]]?$$)/analysis/info)
	@ echo -e "\n\n"Compiling individual target analysis info into $@.
	cat $? | tee -a $@

# Prevent automatic deletion of intermediate files
.SECONDARY: $(prepend $(foreach target, $(TARGETS), $(BUILD_BASE)/$(target)/$(shell grep ^target $(BUILD_BASE)/$(target)/Config.lb | grep -Eo [[:alnum:]_-]+$$)), /analysis, /analysis/c_files, /analysis/h_files, /analysis/info, /Makefile)

# FIXME: This rule is necessary even if the Makefile already exists.
%/Makefile:
	@ echo \*\*\* Building target: $(notdir $*) \*\*\*
	-@ cd $(TOP)/targets && ./buildtarget $(dir $*) 1>>build.log 2>>builderrors.log
	@ echo -e >> $*/$(IMAGE_DIR)/Makefile "depend:\n\t"'@ makedepend -v -f- -- $$(CPPFLAGS) -- $$(SOURCES)'

%/analysis/c_files: %/Makefile
	@ echo Analysis directory is $*/analysis
	-@ mkdir $*/analysis $(IGNORE_ERRORS)
	@ echo -n Finding C source files...
	@ grep -Eo \\$$+[\(][A-Z_]+[\)][/-_[:alnum:]]+'\.c\>' $*/$(IMAGE_DIR)/Makefile | grep -v $(IGNORE_C) | sort -u > $@
	@ echo " "Done.

# Grep for .c files #included within others (only one level deep).
# sed commands provide full pathname for included .c files, assuming two things:
# 1. If include statement has no directory component, the file is in same dir.
# 2. If included file has a directory component, it's base is from $(TOP)/src/.
%/analysis/nested_c_files: %/analysis/c_files
	@ echo -n Finding nested .c includes...
	$(eval c_files := $(shell cat $<))
	@ grep -Eo '\#'include[[:space:]\"]+[/-_[:alnum:]]+'\.c' $(c_files) | sed s/\#include[[:space:]]// > $@.tmp
	@ sed -r s/\([/-_[:alnum:]]+\\/\)\([-_[:alnum:]]+'.c'\):\"\([-_[:alnum:]]+'.c'$$\)/\\1\\2:' '\\1\\3/ $@.tmp | \
	sed -r s/\([/-_[:alnum:]]+\\/\)\([-_[:alnum:]]+'.c'\):\"\([/-_[:alnum:]]+'.c'$$\)/\\1\\2:' '\$$\(TOP\)\\/src\\/\\3/ > $@
	@ rm $@.tmp
	@ echo " "Done.

%/analysis/h_files: %/analysis/c_files %/analysis/nested_c_files
	@ echo -n Finding all included headers...
	$(call load_var,TARGET_DIR, $*/Makefile.settings)
	@ $(MAKE) -C $(TARGET_DIR)/$(IMAGE_DIR) depend $(IGNORE_ERRORS) | grep -v makedepend | grep -Eo [/-_[:alnum:]]+'\.h' | sort -u > $@ && \
	$(MAKE) -C $(TARGET_DIR)/$(IMAGE_DIR) "SOURCES := $(shell grep [/-_[:alnum:]]+'.c' $(word 2, $?))" depend $(IGNORE_ERRORS) | grep -v makedepend | grep -Eo [/-_[:alnum:]]+'\.h' | sort -u >> $@
	@ echo " "Done.

#%/auto.inc:

# Determine which sources use romcc by their inclusion in auto.inc #FIXME better
%/analysis/romcc_files: %/analysis/c_files %/analysis/nested_c_files
	$(call load_var,TARGET_DIR, $*/Makefile.settings)
	@ $(if $(findstring cache_as_ram, $(shell cat $<)), \
		echo none, \
		echo -n \* Uses romcc - making auto.inc... && \
		$(MAKE) -iC $(TARGET_DIR)/$(IMAGE_DIR) auto.inc $(IGNORE_ERRORS) 1>/dev/null && \
		echo " "to find sources that use romcc. && \
		grep -Eo [/-_[:alnum:]]+'\.c' $(TARGET_DIR)/$(IMAGE_DIR)/auto.inc | sort -u) \
	> $@

# Full pathnames of found files are gathered from  nested_c_files and c_files.
%/analysis/romcc_sources: %/analysis/c_files %/analysis/nested_c_files %/analysis/romcc_files
	@ echo -e $(foreach file, $(shell cat $(dir $@)/romcc_files), "\n"'$(firstword $(shell grep -Eho [/-_\$$\(\)[:alnum:]]+/'$(file)' $(dir $@)/nested_c_files $(dir $@)/c_files))') >$@

%/analysis/romcc_headers: %/analysis/romcc_sources
	@ echo -n Finding headers used by any romcc source...
	$(eval romcc_sources = $(shell cat $(dir $@)/romcc_sources))
	$(call load_var,TARGET_DIR, $*/Makefile.settings)
	@ $(MAKE) -C $(TARGET_DIR)/$(IMAGE_DIR) "SOURCES := $(romcc_sources)" depend $(IGNORE_ERRORS) | grep -v makedepend | grep -Eo [/-_[:alnum:]]+'\.'h | sort -u > $@
	@ echo " "Done.

$(BUILD_BASE)/%/analysis/info: $(BUILD_BASE)/%/analysis/h_files $(BUILD_BASE)/%/analysis/romcc_headers
	@ echo -e Target: $(subst /, , $(dir $*)) \
	"\n"Uses $(if $(findstring cache_as_ram, $(shell grep -F '.c' $(dir $@)/c_files)),CAR,romcc) \
	"\n"C files: $(shell grep -Eo [-_[:alnum:]]+'\.c' $(dir $@)/c_files $(dir $@)/nested_c_files $(dir $@)/romcc_sources | sort -u | grep -Fc '.c') \
	"\n"Nested C: $(shell grep -Eo [-_[:alnum:]]+'\.c' $(dir $@)/nested_c_files | sort -u | grep -Fc '.c') \
	"\n"Headers: $(shell grep -Eo [-_[:alnum:]]+'\.h' $(dir $@)/h_files $(dir $@)/romcc_headers | sort -u | grep -Fc '.h') \
	"\n"romcc C: $(shell grep -Ec [-_[:alnum:]]+'\.c' $(dir $@)/romcc_sources) \
	"\n"romcc H: $(shell grep -Ec [-_[:alnum:]]+'\.h' $(dir $@)/romcc_headers) \
	"\n">> $@

clean-builds:
	rm -rf $(foreach target, $(TARGETS), $(BUILD_BASE)/$(target)/$(shell grep ^target $(BUILD_BASE)/$(target)/Config.lb | grep -Eo [[:alnum:]_-]+[[:space:]]?$$))

clean:
	rm -rf $(foreach target, $(TARGETS), $(BUILD_BASE)/$(target)/$(shell grep ^target $(BUILD_BASE)/$(target)/Config.lb | grep -Eo [[:alnum:]_-]+[[:space:]]?$$)/analysis) analysis.txt analysis.dat
