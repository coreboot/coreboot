#*****************************************************************************
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Advanced Micro Devices, Inc. nor the names of
#       its contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

ifeq ($(CONFIG_SOC_AMD_STONEYRIDGE),y)

AGESA_CFLAGS += -march=amdfam10 -fno-strict-aliasing -D__LIBAGESA__

CC_bootblock := $(CC_bootblock) $(BINARY_PI_INC)
CC_romstage := $(CC_romstage) $(BINARY_PI_INC)
CC_postcar:= $(CC_postcar) -I$(AGESA_ROOT)/binaryPI
CC_ramstage := $(CC_ramstage) $(BINARY_PI_INC)

#######################################################################

define create_agesa_cp_template
# $1 AGESA source file

$(agesa_src_path)/$(notdir $1): $1
	@printf "    AGESA      Copying $$(notdir $1) => $$(@D)\n"
	if [ ! -r $(agesa_src_path)/$(notdir $1) ]; then                   \
		cp -f $1 $$(@D);                                           \
	fi

$(call src-to-obj,libagesa,$1): $(agesa_src_path)/$(notdir $1) $(obj)/config.h $(src)/include/kconfig.h
	@printf "    CC         $$(subst $(obj)/,,$$(@))\n"
	$(CC_libagesa) -c -MMD $(CFLAGS_libagesa) $(AGESA_CFLAGS)                     \
	               $(AGESA_INC)                                                   \
	               -include $(src)/include/kconfig.h                              \
	               -include $(src)/include/rules.h                                \
	               -include $(src)/commonlib/bsd/include/commonlib/bsd/compiler.h \
	               -o $$@                                                         \
	               $(agesa_src_path)/$(notdir $1)

endef

agesa_raw_files += $(wildcard $(AGESA_ROOT)/Lib/*.[cS])

agesa_raw_files += $(wildcard $(AGESA_ROOT)/binaryPI/*.[cS])

classes-y += libagesa

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)
$(eval $(call create_class_compiler,libagesa,x86_32))
else
$(eval $(call create_class_compiler,libagesa,x86_64))
endif

agesa_src_files := $(strip $(sort $(foreach file,$(strip $(agesa_raw_files)),$(call strip_quotes,$(file)))))
agesa_output_path := $(obj)/libagesa
agesa_src_path := $(agesa_output_path)
agesa_dirs := $(sort $(abspath $(dir $(call src-to-obj,libagesa,$(agesa_src_files)))))

additional-dirs += $(agesa_src_path) $(agesa_dirs)

$(foreach file,$(strip $(agesa_src_files)),$(eval $(call create_agesa_cp_template,$(file))))

$(agesa_output_path)/libagesa.a: $(call src-to-obj,libagesa,$(agesa_src_files))
	@printf "    AGESA      $(subst $(obj)/,,$(@))\n"
	$(AR_libagesa) rcs $@ $+

bootblock-libs += $(agesa_output_path)/libagesa.a
romstage-libs += $(agesa_output_path)/libagesa.a
ramstage-libs += $(agesa_output_path)/libagesa.a

#######################################################################

warn_no_agesa:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built with no AGESA support.  Successfully "
	printf "booting this image will be impossible.\n\n"

PHONY+=warn_no_agesa

ifeq ($(CONFIG_AGESA_SPLIT_MEMORY_FILES), y)

# convert input elf to rmodule
AGESA_POST_MEM_INPUT_ELF = $(call strip_quotes,$(CONFIG_AGESA_POST_MEMORY_BINARY_PI_FILE))

# If no post-mem file then also skip pre-mem file
ifeq ($(AGESA_POST_MEM_INPUT_ELF),)
files_added:: warn_no_agesa
else
AGESA_POST_MEM_ELF = $(objcbfs)/$(patsubst %.elf,%.debug,$(notdir $(AGESA_POST_MEM_INPUT_ELF)))
AGESA_POST_MEM_ELF_RMOD = $(AGESA_POST_MEM_ELF).rmod

$(AGESA_POST_MEM_ELF): $(AGESA_POST_MEM_INPUT_ELF)
	cp $< $@

$(AGESA_POST_MEM_ELF_RMOD): $(AGESA_POST_MEM_ELF)

cbfs-files-y += $(CONFIG_AGESA_PRE_MEMORY_CBFS_NAME)
$(CONFIG_AGESA_PRE_MEMORY_CBFS_NAME)-file := $(CONFIG_AGESA_PRE_MEMORY_BINARY_PI_FILE)
$(CONFIG_AGESA_PRE_MEMORY_CBFS_NAME)-type := stage
$(CONFIG_AGESA_PRE_MEMORY_CBFS_NAME)-options :=  --xip
# 4KiB alignment to handle any interior alignment. Current AGESA only has
# 64 byte alignment.
$(CONFIG_AGESA_PRE_MEMORY_CBFS_NAME)-align := 4096

cbfs-files-y += $(CONFIG_AGESA_POST_MEMORY_CBFS_NAME)
$(CONFIG_AGESA_POST_MEMORY_CBFS_NAME)-file := $(AGESA_POST_MEM_ELF_RMOD)
$(CONFIG_AGESA_POST_MEMORY_CBFS_NAME)-type := stage
$(CONFIG_AGESA_POST_MEMORY_CBFS_NAME)-compression := $(CBFS_COMPRESS_FLAG)

endif # AGESA_POST_MEM_INPUT_ELF == ""

else # CONFIG_AGESA_SPLIT_MEMORY_FILES

AGESA_BINARYPI_INPUT_FILE = $(call strip_quotes,$(CONFIG_AGESA_BINARY_PI_FILE))
ifeq ($(AGESA_BINARYPI_INPUT_FILE),)
files_added:: warn_no_agesa
else

cbfs-files-y += $(CONFIG_AGESA_CBFS_NAME)
$(CONFIG_AGESA_CBFS_NAME)-file := $(AGESA_BINARYPI_INPUT_FILE)

ifeq ($(CONFIG_AGESA_BINARY_PI_AS_STAGE),y)
$(CONFIG_AGESA_CBFS_NAME)-type := stage
$(CONFIG_AGESA_CBFS_NAME)-options :=  --xip
# 4KiB alignment to handle any interior alignment. Current AGESA only has
# 64 byte alignment.
$(CONFIG_AGESA_CBFS_NAME)-align := 4096
else
$(CONFIG_AGESA_CBFS_NAME)-type := raw
$(CONFIG_AGESA_CBFS_NAME)-position := $(CONFIG_AGESA_BINARY_PI_LOCATION)
regions-for-file-$(CONFIG_AGESA_CBFS_NAME) = COREBOOT
endif  # CONFIG_AGESA_BINARY_PI_AS_STAGE

endif  # AGESA_BINARYPI_INPUT_FILE == ""
endif  # CONFIG_AGESA_SPLIT_MEMORY_FILES

endif
