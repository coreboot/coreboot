#
# Makefile for coreboot paper.
# hacked together by Stefan Reinauer <stepan@openbios.org>
#

PDFLATEX=pdflatex -t a4

FIGS=codeflow.pdf hypertransport.pdf

all: CorebootPortingGuide.pdf Kconfig.pdf

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

CorebootPortingGuide.toc: $(FIGS) CorebootBuildingGuide.tex
	# 2 times to make sure we have a current toc.
	$(PDFLATEX) CorebootBuildingGuide.tex
	$(PDFLATEX) CorebootBuildingGuide.tex

CorebootPortingGuide.pdf: $(FIGS) CorebootBuildingGuide.tex CorebootPortingGuide.toc
	$(PDFLATEX) CorebootBuildingGuide.tex

Kconfig.pdf: Kconfig.tex mainboardkconfig.tex cpukconfig.tex socketfkconfig.tex
	$(PDFLATEX) $<

# quick, somebody! make me a macro!
mainboardkconfig.tex: ../src/mainboard/Kconfig
	cat beginverbatim.tex  > $@
	grep '^config' $< | awk '{print $2}'  >>$@
	cat endverbatim.tex  >> $@

skconfig.tex: ../src/mainboard/amd/serengeti_cheetah/Kconfig
	cat beginverbatim.tex  > $@
	grep '^config' $< | awk '{print $2}'  >>$@
	cat endverbatim.tex  >> $@

cpukconfig.tex: ../src/cpu/Kconfig
	cat beginverbatim.tex  > $@
	grep '^config' $< | awk '{print $2}'  >>$@
	cat endverbatim.tex  >> $@

socketfkconfig.tex: ../src/cpu/amd/socket_F/Kconfig
	cat beginverbatim.tex  > $@
	grep '^config' $< | awk '{print $2}'  >>$@
	cat endverbatim.tex  >> $@

clean:
	rm -f *.aux *.idx *.log *.toc *.out $(FIGS) mainboardkconfig.tex skconfig.tex cpukconfig.tex socketfkconfig.tex

distclean: clean
	rm -f CorebootPortingGuide.pdf Kconfig.pdf
