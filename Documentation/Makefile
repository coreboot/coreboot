## SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for coreboot paper.
# hacked together by Stefan Reinauer <stepan@openbios.org>
#

PDFLATEX=pdflatex -t a4

FIGS=codeflow.pdf hypertransport.pdf

all: sphinx corebootPortingGuide.pdf

SVG2PDF=$(shell command -v svg2pdf)
INKSCAPE=$(shell command -v inkscape)
CONVERT=$(shell command -v convert)

codeflow.pdf: codeflow.svg
ifneq ($(strip $(SVG2PDF)),)
	svg2pdf $< $@
else ifneq ($(strip $(INKSCAPE)),)
	inkscape $< --export-pdf=$@
else ifneq ($(strip $(CONVERT)),)
	convert $< $@
endif

hypertransport.pdf: hypertransport.svg
ifneq ($(strip $(SVG2PDF)),)
	svg2pdf $< $@
else ifneq ($(strip $(INKSCAPE)),)
	inkscape $< --export-pdf=$@
else ifneq ($(strip $(CONVERT)),)
	convert $< $@
endif

corebootPortingGuide.toc: $(FIGS) corebootBuildingGuide.tex
	# 2 times to make sure we have a current toc.
	$(PDFLATEX) corebootBuildingGuide.tex
	$(PDFLATEX) corebootBuildingGuide.tex

corebootPortingGuide.pdf: $(FIGS) corebootBuildingGuide.tex corebootPortingGuide.toc
	$(PDFLATEX) corebootBuildingGuide.tex

sphinx:
	$(MAKE) -f Makefile.sphinx html

clean-sphinx:
	$(MAKE) -f Makefile.sphinx clean

clean: clean-sphinx
	rm -f *.aux *.idx *.log *.toc *.out $(FIGS)

distclean: clean
	rm -f corebootPortingGuide.pdf

livesphinx:
	$(MAKE) -f Makefile.sphinx livehtml SPHINXOPTS="$(SPHINXOPTS)"

help:
	@echo "all            - Builds coreboot porting guide PDF (outdated)"
	@echo "sphinx         - Builds html documentation in _build directory"
	@echo "clean          - Cleans intermediate files"
	@echo "clean-sphinx   - Removes sphinx output files"
	@echo "distclean      - Removes PDF files as well"
	@echo
	@echo "  Makefile.sphinx builds - run with $(MAKE) -f Makefile-sphinx [target]"
	@echo
	@$(MAKE) -s -f Makefile.sphinx help 2>/dev/null

.phony: help livesphinx sphinx test
.phony: distclean clean clean-sphinx
