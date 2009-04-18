#
# Makefile for coreboot paper.
# hacked together by Stefan Reinauer <stepan@openbios.org>
#

PDFLATEX=pdflatex -t a4

FIGS=codeflow.pdf hypertransport.pdf

all: LinuxBIOS-AMD64.pdf 


codeflow.pdf: codeflow.eps
	epstopdf $< -o=$@

hypertransport.pdf: hypertransport.eps
	epstopdf $< -o=$@

LinuxBIOS-AMD64.toc: $(FIGS) LinuxBIOS-AMD64.tex 
	# 2 times to make sure we have a current toc.
	$(PDFLATEX) LinuxBIOS-AMD64.tex
	$(PDFLATEX) LinuxBIOS-AMD64.tex

LinuxBIOS-AMD64.pdf: $(FIGS) LinuxBIOS-AMD64.tex LinuxBIOS-AMD64.toc
	$(PDFLATEX) LinuxBIOS-AMD64.tex

clean:
	rm -f *.aux *.idx *.log *.toc *.out $(FIGS)

distclean: clean
	rm -f LinuxBIOS-AMD64.pdf
	
	
