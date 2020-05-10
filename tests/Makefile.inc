# SPDX-License-Identifier: GPL-2.0-only

testobj = $(obj)/tests

TEST_DEFAULT_CONFIG = $(top)/configs/config.emulation_qemu_x86_i440fx
TEST_DOTCONFIG = $(testobj)/.config
TEST_KCONFIG_AUTOHEADER := $(testobj)/config.h
TEST_KCONFIG_AUTOCONFIG := $(testobj)/auto.conf
TEST_KCONFIG_DEPENDENCIES := $(testobj)/auto.conf.cmd
TEST_KCONFIG_SPLITCONFIG := $(testobj)/config
TEST_KCONFIG_TRISTATE := $(testobj)/tristate.conf

TEST_CFLAGS = -include$(src)/include/kconfig.h \
	-include$(src)/commonlib/bsd/include/commonlib/bsd/compiler.h \
	-include $(src)/include/rules.h \

# Include generic test mock headers, before original ones
TEST_CFLAGS += -Itests/include/mocks

TEST_CFLAGS += -I$(src)/include -I$(src)/commonlib/include \
	-I$(src)/commonlib/bsd/include -I$(src)/arch/x86/include \

# Path for Kconfig autoheader
TEST_CFLAGS += -I$(dir $(TEST_KCONFIG_AUTOHEADER))

TEST_CFLAGS += -std=gnu11 -Os -ffunction-sections -fdata-sections \
	-Wl,--gc-sections -fno-builtin

# Link against Cmocka
TEST_LDFLAGS = -lcmocka

# Extra attributes for unit tests, declared per test
attributes:= srcs cflags mocks stage

stages:= decompressor bootblock romstage smm verstage
stages+= ramstage rmodule postcar libagesa

alltests:=
subdirs:= tests/arch tests/commonlib tests/console tests/cpu tests/device
subdirs+= tests/drivers tests/ec tests/lib tests/mainboard
subdirs+= tests/northbridge tests/security tests/soc tests/southbridge
subdirs+= tests/superio tests/vendorcode

define tests-handler
alltests += $(1)$(2)
$(foreach attribute,$(attributes),
	$(eval $(1)$(2)-$(attribute) += $($(2)-$(attribute))))
$(foreach attribute,$(attributes),
	$(eval $(2)-$(attribute):=))

# Sanity check for stage attribute value
$(eval $(1)$(2)-stage:=$(if $($(1)$(2)-stage),$($(1)$(2)-stage),ramstage))
$(if $(findstring $($(1)$(2)-stage), $(stages)),,
	$(error Wrong $(1)$(2)-stage value $($(1)$(2)-stage). \
		Check your $(dir $(1)$(2))Makefile.inc))
endef

$(call add-special-class, tests)
$(call evaluate_subdirs)

# Create actual targets for unit test binaries
# $1 - test name
define TEST_CC_template
$($(1)-objs): TEST_CFLAGS+= \
	-D__$$(shell echo $$($(1)-stage) | tr '[:lower:]' '[:upper:]')__
$($(1)-objs): $(obj)/$(1)/%.o: $$$$*.c $(TEST_KCONFIG_AUTOHEADER)
	mkdir -p $$(dir $$@)
	$(HOSTCC) $(HOSTCFLAGS) $$(TEST_CFLAGS) $($(1)-cflags)  -MMD \
		-MT $$@ -c $$< -o $$@

$($(1)-bin): TEST_LDFLAGS+= $$(foreach mock,$$($(1)-mocks),-Wl,--wrap=$$(mock))
$($(1)-bin): $($(1)-objs)
	$(HOSTCC) $$^ $($(1)-cflags) $$(TEST_LDFLAGS) -o $$@

endef

$(foreach test, $(alltests), \
	$(eval $(test)-objs:=$(addprefix $(obj)/$(test)/, \
		$(patsubst %.c,%.o,$($(test)-srcs)))))
$(foreach test, $(alltests), \
	$(eval $(test)-bin:=$(obj)/$(test)/run))
$(foreach test, $(alltests), \
	$(eval $(call TEST_CC_template,$(test))))

$(foreach test, $(alltests), \
	$(eval all-test-objs+=$($(test)-objs)))
$(foreach test, $(alltests), \
	$(eval test-bins+=$($(test)-bin)))

DEPENDENCIES += $(addsuffix .d,$(basename $(all-test-objs)))
-include $(DEPENDENCIES)

# Kconfig targets
$(TEST_DOTCONFIG):
	mkdir -p $(dir $@)
	cp $(TEST_DEFAULT_CONFIG) $(TEST_DOTCONFIG)

# Don't override default Kconfig variables, since this will affect all
# Kconfig targets. Change them only when calling sub-make instead.
$(TEST_KCONFIG_AUTOHEADER): TEST_KCONFIG_FLAGS:= DOTCONFIG=$(TEST_DOTCONFIG) \
        KCONFIG_AUTOHEADER=$(TEST_KCONFIG_AUTOHEADER) \
        KCONFIG_AUTOCONFIG=$(TEST_KCONFIG_AUTOCONFIG) \
        KCONFIG_DEPENDENCIES=$(TEST_KCONFIG_DEPENDENCIES) \
        KCONFIG_SPLITCONFIG=$(TEST_KCONFIG_SPLITCONFIG) \
        KCONFIG_TRISTATE=$(TEST_KCONFIG_TRISTATE) \
        KBUILD_DEFCONFIG=$(TEST_DEFAULT_CONFIG)

$(TEST_KCONFIG_AUTOHEADER): $(TEST_DOTCONFIG) $(objutil)/kconfig/conf
	mkdir -p $(dir $@)
	+$(MAKE) $(TEST_KCONFIG_FLAGS) olddefconfig
	+$(MAKE) $(TEST_KCONFIG_FLAGS) silentoldconfig

$(TEST_KCONFIG_AUTOCONFIG): $(TEST_KCONFIG_AUTOHEADER)
	true

.PHONY: $(alltests) $(addprefix clean-,$(alltests))
.PHONY: unit-tests build-unit-tests run-unit-tests clean-unit-tests

$(alltests): $$($$(@)-bin)
	./$^

unit-tests: build-unit-tests run-unit-tests

build-unit-tests: $(test-bins)

run-unit-tests: $(alltests)
	echo "**********************"
	echo "   ALL TESTS PASSED"
	echo "**********************"

$(addprefix clean-,$(alltests)): clean-%:
	rm -rf $(obj)/$*

clean-unit-tests:
	rm -rf $(testobj)
