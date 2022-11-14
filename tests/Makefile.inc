# SPDX-License-Identifier: GPL-2.0-only

# Place the build output in one of two places depending on COV, so that code
# built with code coverage never mixes with code built without code coverage.
ifeq ($(COV),1)
testobj := $(obj)/coverage
else
testobj := $(obj)/tests
endif

include $(top)/tests/Makefile.common

# Enable GDB debug build if requested
GDB_DEBUG ?= 0
ifneq ($(GDB_DEBUG),0)
TEST_CFLAGS += -g -Og
endif

# Enable code coverage if requested
ifeq ($(COV),1)
TEST_CFLAGS += --coverage
TEST_LDFLAGS += --coverage
endif

stages := decompressor bootblock romstage smm verstage
stages += ramstage rmodule postcar libagesa

alltests :=
subdirs := tests/arch tests/acpi tests/commonlib tests/console tests/cpu
subdirs += tests/device tests/drivers tests/ec tests/lib tests/mainboard
subdirs += tests/northbridge tests/security tests/soc tests/southbridge
subdirs += tests/superio tests/vendorcode

define tests-handler
alltests += $(1)$(2)
$(foreach attribute,$(attributes),
	$(eval $(1)$(2)-$(attribute) += $($(2)-$(attribute))))
$(foreach attribute,$(attributes),
	$(eval $(2)-$(attribute) := ))

# Sanity check for stage attribute value
$(eval $(1)$(2)-stage := $(if $($(1)$(2)-stage),$($(1)$(2)-stage),ramstage))
$(if $(findstring $($(1)$(2)-stage), $(stages)),,
	$(error Wrong $(1)$(2)-stage value $($(1)$(2)-stage). \
		Check your $(dir $(1)$(2))Makefile.inc))
endef

$(call add-special-class, tests)
$(call evaluate_subdirs)

$(foreach test, $(alltests), \
	$(eval $(test)-srcobjs := $(addprefix $(testobj)/$(test)/, \
		$(patsubst %.c,%.o,$(filter src/%,$($(test)-srcs))))) \
	$(eval $(test)-sysobjs := $(addprefix $(testobj)/$(test)/, \
		$(patsubst %.c,%.o,$($(test)-syssrcs)))) \
	$(eval $(test)-objs := $(addprefix $(testobj)/$(test)/, \
		$(patsubst %.c,%.o,$($(test)-srcs)))))
$(foreach test, $(alltests), \
	$(eval $(test)-bin := $(testobj)/$(test)/run))
$(foreach test, $(alltests), \
	$(eval $(call TEST_CC_template,$(test))))

$(foreach test, $(alltests), \
	$(eval all-test-objs += $($(test)-objs)))
$(foreach test, $(alltests), \
	$(eval test-bins += $($(test)-bin)))

DEPENDENCIES += $(addsuffix .d,$(basename $(all-test-objs)))
-include $(DEPENDENCIES)

.PHONY: $(alltests) $(addprefix clean-,$(alltests)) $(addprefix try-,$(alltests))
.PHONY: $(addprefix build-,$(alltests)) $(addprefix run-,$(alltests))
.PHONY: unit-tests build-unit-tests run-unit-tests clean-unit-tests
.PHONY: junit.xml-unit-tests clean-junit.xml-unit-tests

# %g in CMOCKA_XML_FILE will be replaced with "__TEST_NAME__(<test-group-name>)"
# by macro cb_run_group_tests(), which should be used for running tests.
# __TEST_NAME__ contains test name including path e.g. tests_lib_rtc-test
ifeq ($(JUNIT_OUTPUT),y)
$(addprefix run-,$(alltests)): export CMOCKA_MESSAGE_OUTPUT=xml
$(addprefix run-,$(alltests)): export CMOCKA_XML_FILE=$(testobj)/junit-%g.xml
endif

$(addprefix run-,$(alltests)): run-%: $$(%-bin)
	rm -f $(testobj)/junit-$(subst /,_,$(patsubst $(testobj)/%/,%,$(dir $^)))\(*\).xml
	rm -f $(testobj)/$(subst /,_,$^).failed
	-$^ || echo failed > $(testobj)/$(subst /,_,$^).failed

$(addprefix build-,$(alltests)): build-%: $$(%-bin)

$(alltests): run-$$(@)

$(addprefix try-,$(alltests)): try-%: clean-% $(TEST_COMMON_DEPENDENCIES)
	mkdir -p $(testobj)/$*
	echo "<testcase classname='coreboot_build_unit_test' name='$*'>" >> $(testobj)/$*.tmp; \
	$(MAKE) V=$(V) Q=$(Q) COV=$(COV) JUNIT_OUTPUT=y "build-$*" >> $(testobj)/$*.tmp.2 2>&1 \
		&& type="system-out" || type="failure"; \
	if [ $$type = "failure" ]; then \
		echo "<failure type='buildFailed'>" >> $(testobj)/$*.tmp; \
	else \
		echo "<$$type>" >> $(testobj)/$*.tmp; \
	fi; \
	echo '<![CDATA[' >> $(testobj)/$*.tmp; \
	cat $(testobj)/$*.tmp.2 >> $(testobj)/$*.tmp; \
	echo "]]></$$type>" >> $(testobj)/$*.tmp; \
	rm -f $(testobj)/$*.tmp.2; \
	echo "</testcase>" >> $(testobj)/$*.tmp; \
	if [ $$type != 'failure' ]; then \
		$(MAKE) V=$(V) Q=$(Q) COV=$(COV) JUNIT_OUTPUT=y "run-$*"; \
	fi


TESTS_BUILD_XML_FILE := $(testobj)/junit-tests-build.xml

$(TESTS_BUILD_XML_FILE): clean-junit.xml-unit-tests $(addprefix try-,$(alltests))
	mkdir -p $(dir $@)
	echo '<?xml version="1.0" encoding="utf-8"?><testsuite>' > $@
	for tst in $(alltests); do \
		cat $(testobj)/$$tst.tmp >> $@; \
	done
	echo "</testsuite>" >> $@

junit.xml-unit-tests: $(TESTS_BUILD_XML_FILE)

clean-junit.xml-unit-tests:
	rm -f $(TESTS_BUILD_XML_FILE)


# Build a code coverage report by collecting all the gcov files into a single
# report. If COV is not set, this might be a user error, and they're trying
# to generate a coverage report without first having built and run the code
# with code coverage. So instead of silently correcting it by adding COV=1,
# let's flag it to the user so they can be sure they're doing the thing they
# want to do.

.PHONY: coverage-report clean-coverage-report

ifeq ($(COV),1)
coverage-report:
	lcov -o $(testobj)/tests.info -c -d $(testobj) --exclude '$(testsrc)/*'
	genhtml -q -o $(testobj)/$(coverage_dir) -t "coreboot unit tests" \
	-s $(testobj)/tests.info

clean-coverage-report:
	rm -Rf $(testobj)/$(coverage_dir)
else
coverage-report:
	COV=1 V=$(V) $(MAKE) coverage-report

clean-coverage-report:
	COV=1 V=$(V) $(MAKE) clean-coverage-report
endif

unit-tests: build-unit-tests run-unit-tests

build-unit-tests: $(test-bins)

run-unit-tests: $(alltests)
	if [ `find $(testobj) -name '*.failed' | wc -l` -gt 0 ]; then \
		echo "**********************"; \
		echo "     TESTS FAILED"; \
		echo "**********************"; \
		exit 1; \
	else \
		echo "**********************"; \
		echo "   ALL TESTS PASSED"; \
		echo "**********************"; \
		exit 0; \
	fi

$(addprefix clean-,$(alltests)): clean-%:
	rm -rf $(testobj)/$*

clean-unit-tests:
	rm -rf $(testobj)

list-unit-tests:
	@echo "unit-tests:"
	for t in $(sort $(alltests)); do \
		echo "  $$t"; \
	done

help-unit-tests help::
	@echo  '*** coreboot unit-tests targets ***'
	@echo  '  Use "COV=1 make [target]" to enable code coverage for unit tests'
	@echo  '  Use "GDB_DEBUG=1 make [target]" to build with debug symbols'
	@echo  '  unit-tests            - Run all unit-tests from tests/'
	@echo  '  clean-unit-tests      - Remove unit-tests build artifacts'
	@echo  '  list-unit-tests       - List all unit-tests'
	@echo  '  <unit-test>           - Build and run single unit-test'
	@echo  '  clean-<unit-test>     - Remove single unit-test build artifacts'
	@echo  '  coverage-report       - Generate a code coverage report'
	@echo  '  clean-coverage-report - Remove the code coverage report'
	@echo
