##
##
## SPDX-License-Identifier: GPL-2.0-only

test-help help::
	@echo  '*** coreboot test targets ***'
	@echo  '  what-jenkins-does      - Run platform build tests with junit output'
	@echo  '  lint / lint-stable     - run coreboot lint tools (all / minimal)'
	@echo  '  test-basic             - Run stardard build tests. All expected to pass.'
	@echo  '  test-lint              - basic: Run stable and extended lint tests.'
	@echo  '  test-tools             - basic: Tests a basic list of tools.'
	@echo  '  test-abuild            - basic: Builds all platforms'
	@echo  '  test-payloads          - basic: Builds internal payloads'
	@echo  '  test-cleanup           - basic: Cleans coreboot directories'

# junit.xml is a helper target to wrap builds that don't create junit.xml output
# BLD = The name of the build
# BLD_DIR = Top path from coreboot to the build subdirectory
# MAKETARGET = target to build
junit.xml:
	echo "Building $(BLD)"
	echo '<?xml version="1.0" encoding="utf-8"?><testsuite>' > $@.tmp
	echo "<testcase classname='$(BLD)' name='$(BLD)'>" >> $@.tmp
	-if [ -z "$(BLD_DIR)" ]; then                                          \
		projdir="";                                                    \
		xmlfile=junit_$(BLD).xml;                                      \
	else                                                                   \
		projdir="-C $(BLD_DIR)";                                       \
		xmlfile="$(BLD_DIR)/$@";                                       \
	fi;                                                                    \
	if [ -z "$(SKIP_DISTCLEAN)" ]; then                                    \
		$(MAKE) -j $(CPUS) $$projdir clean distclean > $@.tmp.2 2>&1 ; \
	fi;                                                                    \
	if $(MAKE) -j $(CPUS) $$projdir $(MAKETARGET) >> $@.tmp.2 2>&1; then   \
		type="system-out";                                             \
		echo "<$$type>" >> $@.tmp;                                     \
		echo "Building $(BLD) Succeeded";                              \
	else                                                                   \
		type="failure";                                                \
		echo "<failure type='buildFailed'>" >> $@.tmp;                 \
		echo "Building $(BLD) Failed";                                 \
	fi;                                                                    \
	cat $@.tmp.2;                                                          \
	echo '<![CDATA[' >> $@.tmp;                                            \
	cat $@.tmp.2 >> $@.tmp;                                                \
	echo "]]></$$type>" >>$@.tmp;                                          \
	rm -f $@.tmp.2;                                                        \
	echo "</testcase>" >> $@.tmp;                                          \
	echo "</testsuite>" >> $@.tmp;                                         \
	mv $@.tmp "$$xmlfile"
	echo

TOOLLIST= \
cbmem \
ectool \
futility \
inteltool \
intelvbttool \
nvramtool \
superiotool

TEST_PAYLOADLIST_INTERNAL= \
coreinfo \
nvramcui

JENKINS_PAYLOAD?=none
TEST_PAYLOAD?=$(JENKINS_PAYLOAD)
CPUS?=4

lint lint-stable lint-extended:
	util/lint/lint $@

ABUILD_OPTIONS=-B -J -c $(CPUS) -Z -p $(JENKINS_PAYLOAD)
ABUILD_OPTIONS+=$(if $(V),-v,)
ABUILD_OPTIONS+=$(if $(JENKINS_NOCCACHE),,-y)
ABUILD_OPTIONS+=$(JENKINS_ABUILD_OPT)

COREBOOT_BUILD_DIR?=coreboot-builds

what-jenkins-does:
	rm -rf $(COREBOOT_BUILD_DIR)/chromeos $(COREBOOT_BUILD_DIR)/default
	util/lint/lint lint-stable --junit
	util/lint/lint lint-extended --junit
	util/abuild/abuild -o $(COREBOOT_BUILD_DIR)/chromeos $(ABUILD_OPTIONS) -x -X $(top)/abuild-chromeos.xml
	util/abuild/abuild -o $(COREBOOT_BUILD_DIR)/default $(ABUILD_OPTIONS)
	$(foreach tool, $(TOOLLIST), $(MAKE) CPUS=$(CPUS) V=$(V) Q=$(Q) BLD_DIR="util/$(tool)" BLD="$(tool)" MFLAGS= MAKEFLAGS= MAKETARGET= junit.xml; )
	unset COREBOOT_BUILD_DIR;$(MAKE) CPUS=$(CPUS) V=$(V) Q=$(Q) BLD_DIR=payloads/nvramcui BLD=nvramcui MFLAGS= MAKEFLAGS= MAKETARGET=all junit.xml
	unset COREBOOT_BUILD_DIR;$(MAKE) CPUS=$(CPUS) V=$(V) Q=$(Q) BLD_DIR=payloads/coreinfo BLD=coreinfo MFLAGS= MAKEFLAGS= MAKETARGET=defaultbuild junit.xml
	(cd payloads/libpayload; unset COREBOOT_BUILD_DIR; $(MAKE) $(if $(JENKINS_NOCCACHE),,CONFIG_LP_CCACHE=y) V=$(V) Q=$(Q) junit.xml)
	$(MAKE) CPUS=$(CPUS) V=$(V) Q=$(Q) BLD_DIR=src/soc/nvidia/tegra124/lp0 BLD=tegra124_lp0 MFLAGS= MAKEFLAGS= MAKETARGET=all junit.xml
	$(MAKE) CPUS=$(CPUS) V=$(V) Q=$(Q) BLD_DIR=src/soc/nvidia/tegra210/lp0 BLD=tegra120_lp0 MFLAGS= MAKEFLAGS= MAKETARGET=all junit.xml
	$(MAKE) unit-tests JUNIT_OUTPUT=y

test-basic: test-lint test-tools test-abuild test-payloads test-cleanup

test-lint:
	util/lint/lint lint-stable
	util/lint/lint lint-extended

test-abuild:
	rm -rf coreboot-builds-chromeos coreboot-builds
	export COREBOOT_BUILD_DIR=coreboot-builds-chromeos; util/abuild/abuild -B -e $(if $(TEST_NOCCACHE),,-y) -c $(CPUS) -p $(TEST_PAYLOAD) -x
	util/abuild/abuild -B -e $(if $(TEST_NOCCACHE),,-y) -c $(CPUS) -p $(TEST_PAYLOAD)

test-payloads:
	$(MAKE) -C payloads/libpayload test-configs -j $(CPUS) V=$(V) Q=$(Q) MFLAGS= MAKEFLAGS= $(if $(TEST_NOCCACHE),,CONFIG_LP_CCACHE=y)
	export MFLAGS=;export MAKEFLAGS=; \
	$(foreach payload, $(TEST_PAYLOADLIST_INTERNAL), \
		echo "*** Making payload $(payload) ***"; \
		$(MAKE) -C payloads/$(payload) distclean ;\
		$(MAKE) $(payload) -j $(CPUS) V=$(V) Q=$(Q)\
		|| exit 1; )

test-tools:
	@echo "Build testing $(TOOLLIST)"
	$(foreach tool, $(TOOLLIST), $(MAKE) -C util/$(tool) clean ; )
	$(foreach tool, $(TOOLLIST), echo "Building $(tool)";export MFLAGS= ;export MAKEFLAGS= ;$(MAKE) -C util/$(tool) all V=$(V) Q=$(Q) || exit 1; )
	@echo "Running gitconfig tests"
	@for test in $$(find util/gitconfig/test -maxdepth 1 \
		-type f -executable); do \
		echo "$${test}"; \
		"$${test}" || exit $${?}; \
	done

test-cleanup:
	rm -rf coreboot-builds coreboot-builds-chromeos
	$(MAKE) clean
	$(MAKE) distclean
	$(foreach tool, $(TOOLLIST), $(MAKE) -C util/$(tool) clean ; )

.PHONY: test-basic test-lint test-abuild test-payloads
.PHONY: test-tools test-cleanup test-help
.PHONY: lint lint-stable what-jenkins-does
