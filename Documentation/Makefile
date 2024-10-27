## SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for coreboot paper.
# hacked together by Stefan Reinauer <stepan@openbios.org>
#

BUILDDIR ?= _build
SPHINXOPTS ?= -j auto -W --keep-going

export SPHINXOPTS

all: sphinx

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

sphinx: $(BUILDDIR)
	$(MAKE) -f Makefile.sphinx html BUILDDIR="$(BUILDDIR)"

clean-sphinx:
	$(MAKE) -f Makefile.sphinx clean BUILDDIR="$(BUILDDIR)"

clean: clean-sphinx
	rm -f *.aux *.idx *.log *.toc *.out $(FIGS)

distclean: clean
	rm -f corebootPortingGuide.pdf

livesphinx: $(BUILDDIR)
	$(MAKE) -f Makefile.sphinx livehtml BUILDDIR="$(BUILDDIR)"

test:
	@echo "Test for logging purposes - Failing tests will not fail the build"
	-$(MAKE) -f Makefile.sphinx clean && $(MAKE) -k -f Makefile.sphinx html

help:
	@echo "all            - Builds all documentation targets"
	@echo "sphinx         - Builds html documentation in _build directory"
	@echo "clean          - Cleans intermediate files"
	@echo "clean-sphinx   - Removes sphinx output files"
	@echo "distclean      - Removes PDF files as well"
	@echo "test           - Runs documentation tests"
	@echo
	@echo "  Makefile.sphinx builds - run with $(MAKE) -f Makefile-sphinx [target]"
	@echo
	@$(MAKE) -s -f Makefile.sphinx help 2>/dev/null

.phony: help livesphinx sphinx test
.phony: distclean clean clean-sphinx
