## SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for coreboot paper.
# hacked together by Stefan Reinauer <stepan@openbios.org>
#

PDFLATEX=pdflatex -t a4

FIGS=codeflow.pdf hypertransport.pdf

all: corebootPortingGuide.pdf

SVG2PDF=$(shell which svg2pdf)
INKSCAPE=$(shell which inkscape)
CONVERT=$(shell which convert)

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
