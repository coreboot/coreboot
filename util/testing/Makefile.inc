##
## This file is part of the coreboot project.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

junit.xml:
	echo "Building $(BLD)"
	echo '<?xml version="1.0" encoding="utf-8"?><testsuite>' > $@.tmp
	echo "<testcase classname='$(BLD)' name='$(BLD)'>" >> $@.tmp
	-$(MAKE) -j $(CPUS) -C "$(BLD_DIR)$(BLD)" clean distclean > $@.tmp.2 2>&1
	$(MAKE) -j $(CPUS) -C "$(BLD_DIR)$(BLD)" $(MAKETARGET) >> $@.tmp.2 2>&1 && type="system-out" || type="failure"; \
	cat $@.tmp.2; \
	if [ "$$type" = "failure" ]; then \
		echo "<failure type='buildFailed'>" >> $@.tmp; \
		echo "Building $(BLD) Failed"; \
	else \
		echo "<$$type>" >> $@.tmp; \
		echo "Building $(BLD) Succeeded"; \
	fi; \
	echo '<![CDATA[' >> $@.tmp; \
	cat $@.tmp.2 >> $@.tmp; \
	echo "]]></$$type>" >>$@.tmp
	rm -f $@.tmp.2
	echo "</testcase>" >> $@.tmp
	echo "</testsuite>" >> $@.tmp
	mv $@.tmp "$(BLD_DIR)$(BLD)/$@"
	echo

TOOLLIST= \
	cbmem \
	ectool \
	futility \
	inteltool \
	intelvbttool \
	nvramtool \
	superiotool \
	viatool
JENKINS_PAYLOAD?=none
CPUS?=4
what-jenkins-does:
	util/lint/lint lint-stable --junit
	util/abuild/abuild -B -J $(if $(JENKINS_NOCCACHE),,-y) -c $(CPUS) -z -p $(JENKINS_PAYLOAD) -x -X $(top)/abuild-chromeos.xml
	util/abuild/abuild -B -J $(if $(JENKINS_NOCCACHE),,-y) -c $(CPUS) -z -p $(JENKINS_PAYLOAD)
	(cd payloads/libpayload; unset COREBOOT_BUILD_DIR; $(MAKE) $(if $(JENKINS_NOCCACHE),,CONFIG_LP_CCACHE=y) V=$(V) Q=$(Q) junit.xml)
	$(foreach tool, $(TOOLLIST), $(MAKE) CPUS=$(CPUS) V=$(V) Q=$(Q) BLD_DIR=util/ BLD="$(tool)" MFLAGS= MAKEFLAGS= MAKETARGET= junit.xml; )
	unset COREBOOT_BUILD_DIR;$(MAKE) CPUS=$(CPUS) V=$(V) Q=$(Q) BLD_DIR=payloads/ BLD=nvramcui MFLAGS= MAKEFLAGS= MAKETARGET=all junit.xml
	unset COREBOOT_BUILD_DIR;$(MAKE) CPUS=$(CPUS) V=$(V) Q=$(Q) BLD_DIR=payloads/ BLD=coreinfo MFLAGS= MAKEFLAGS= MAKETARGET=defaultbuild junit.xml
	$(MAKE) CPUS=$(CPUS) V=$(V) Q=$(Q) BLD_DIR=util/ BLD=romcc MFLAGS= MAKEFLAGS= MAKETARGET=test junit.xml
